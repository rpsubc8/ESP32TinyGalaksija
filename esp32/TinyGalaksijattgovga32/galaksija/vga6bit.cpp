// Ricardo Massaro mod library (Bitluni) mod by ackerman(convert C, resize)
//
// === Bare-bones 6-bit color VGA output for the ESP32 =================
//
// Most of this code was inspired by or stolen from bitluni's
// excellent ESP32Lib. I made this because ESP32Lib has a lot of extra
// stuff I don't need (like 16 bit output, drawing functions,
// etc.). My only addition an interrupt handler to count frames, which
// is used to wait for the right time to swap framebuffers.
//
// === Overview ========================================================
//
// This code outputs a VGA signal to 8 user-selected GPIO pins, with
// H-sync, V-sync and 6 color bits to be used as desired (usually 2
// red, 2 green and 2 blue with 3 2-bit DACs).
//
// It allocates 2 framebuffers in DMA-capable memory and sets I2S
// (port 1) to output one of them (together with other buffers set up
// as back/front porch and vsync/hsync) via DMA, triggering an
// interrupt at the end. Each byte of the framebuffer is sent to an
// output pin selected by the user, which should be connected to a VGA
// monitor using appropriate resistors and DACs. The two framebuffers
// can be swapped so while one of them is being sent out, the other is
// being drawn.
//
// Each byte of the framebuffer always contains the vsync and hsync
// signal at bits 7 and 6 (the high bits). The other 6 bits are set by
// the user: usually (msb)RRGGBB(lsb), but other configurations are
// possible (e.g. grayscale, RRGGGB, etc.). Just use appropriate DACs
// to convert the output pins to VGA voltage levels.
//
// One annoying aspect of this library is the wonky order of the
// pixels in the framebuffer: starting at each line, the address of
// the byte corresponding to each pixel is this (the pattern repeats
// every 4 pixels):
//
//   pixel x-coord:   0   1   2   3   4   5   6   7  ...
//    mem. address:  [2] [3] [0] [1] [6] [7] [4] [5] ...
//
// One easy way to adjust for that is to xor the X coordinate with 2,
// like this:
//
//   framebuffer[y][x^2] = vga_get_sync_bits() | color_bits;
//
// But note that writing one byte at a time on the ESP32 is very slow
// (for optimal results, write at least 4 bytes at a time).
//
// === To use the library ==============================================
//
// 1) Call vga_init() to set things up and start the VGA output.
// 
// 2) In your loop, call vga_get_framebuffer() to get a pointer to the
// lines of the current back framebuffer, where you'll write the
// pixels.  Don't forget to write the sync bits, which you can get
// with vga_get_sync_bits(). They depend only on the video mode so
// it's safe to store them in a variable at the start of the program.
//
// 3) When you're done drawing a frame, call vga_swap_buffers() to
// swap the front and back buffers. By default, this function waits
// for the current frame to end. It you don't want this, call it
// passing `false` (but if you do this, expect a wandering horizontal
// line on the screeb).
//
// =====================================================================

// definitions:
//   apll_clk = XTAL * (4 + sdm2 + sdm1 / 256 + sdm0 / 65536) / (2 * o_div + 4)
//     dividend = XTAL * (4 + sdm2 + sdm1 / 256 + sdm0 / 65536)
//     divisor  = (2 * o_div + 4)
//   freq = apll_clk / (2 + b / a)        => assumes  tx_bck_div_num = 1 and clkm_div_num = 2
// Other values range:
//   sdm0  0..255
//   sdm1  0..255
//   sdm2  0..63
//   o_div 0..31
// Assume xtal = FABGLIB_XTAL (40MHz)
// The dividend should be in the range of 350 - 500 MHz (350000000-500000000), so these are the
// actual parameters ranges (so the minimum apll_clk is 5303030 Hz and maximum is 125000000Hz):
//  MIN 87500000Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 0
//  MAX 125000000Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 0
//
//  MIN 58333333Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 1
//  MAX 83333333Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 1
//
//  MIN 43750000Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 2
//  MAX 62500000Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 2
//
//  MIN 35000000Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 3
//  MAX 50000000Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 3
//
//  MIN 29166666Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 4
//  MAX 41666666Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 4
//
//  MIN 25000000Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 5
//  MAX 35714285Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 5
//
//  MIN 21875000Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 6
//  MAX 31250000Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 6
//
//  MIN 19444444Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 7
//  MAX 27777777Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 7
//
//  MIN 17500000Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 8
//  MAX 25000000Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 8
//
//  MIN 15909090Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 9
//  MAX 22727272Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 9
//
//  MIN 14583333Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 10
//  MAX 20833333Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 10
//
//  MIN 13461538Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 11
//  MAX 19230769Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 11
//
//  MIN 12500000Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 12
//  MAX 17857142Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 12
//
//  MIN 11666666Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 13
//  MAX 16666666Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 13
//
//  MIN 10937500Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 14
//  MAX 15625000Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 14
//
//  MIN 10294117Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 15
//  MAX 14705882Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 15
//
//  MIN 9722222Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 16
//  MAX 13888888Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 16
//
//  MIN 9210526Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 17
//  MAX 13157894Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 17
//
//  MIN 8750000Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 18
//  MAX 12500000Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 18
//
//  MIN 8333333Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 19
//  MAX 11904761Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 19
//
//  MIN 7954545Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 20
//  MAX 11363636Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 20
//
//  MIN 7608695Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 21
//  MAX 10869565Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 21
//
//  MIN 7291666Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 22
//  MAX 10416666Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 22
//
//  MIN 7000000Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 23
//  MAX 10000000Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 23
//
//  MIN 6730769Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 24
//  MAX 9615384Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 24
//
//  MIN 6481481Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 25
//  MAX 9259259Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 25
//
//  MIN 6250000Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 26
//  MAX 8928571Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 26
//
//  MIN 6034482Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 27
//  MAX 8620689Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 27
//
//  MIN 5833333Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 28
//  MAX 8333333Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 28
//
//  MIN 5645161Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 29
//  MAX 8064516Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 29
//
//  MIN 5468750Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 30
//  MAX 7812500Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 30
//
//  MIN 5303030Hz - sdm0 = 0 sdm1 = 192 sdm2 = 4 o_div = 31
//  MAX 7575757Hz - sdm0 = 0 sdm1 = 128 sdm2 = 8 o_div = 31


#include "gbConfig.h"
#ifdef use_lib_tinybitluni_fast 

#include <esp_heap_caps.h>
#include <soc/rtc.h>
#include <soc/i2s_reg.h>
#include <soc/i2s_struct.h>
#include <soc/io_mux_reg.h>
#include <driver/rtc_io.h>
#include <driver/gpio.h>
#include <driver/periph_ctrl.h>

#if ARDUINO_ARCH_ESP32   // compiling under Arduino Core
#include <Arduino.h>
#include <rom/lldesc.h>
#define DELAY(n) delay(n)
#else                    // compiling under pure ESP-IDF
#include <esp32/rom/lldesc.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#define DELAY(n) vTaskDelay((n) / portTICK_PERIOD_MS)
#endif

#include "vga6bit.h"
#include "driver/i2c.h" // i2c_rtc_clk.h"

//BEGIN PLL
//#include <esp_heap_caps.h>
//#include <soc/rtc.h>
//#include <soc/i2s_reg.h>
//#include <soc/i2s_struct.h>
//#include <soc/io_mux_reg.h>
//#include <driver/rtc_io.h>
//#include <driver/gpio.h>
//#include <driver/periph_ctrl.h>
#include "regi2c_ctrl.h"
//END PLL


unsigned char pll_cte_force=0; //Forzar el uso de PLL constante
unsigned char pll_custom_force=0; //Forzar uso de custom_rtc_clk_apll_enable
unsigned int pll_cte_p0;
unsigned int pll_cte_p1;
unsigned int pll_cte_p2;
unsigned int pll_cte_p3;
unsigned char vga_is8colors=1;
//unsigned char vga_pinmap[8];
const unsigned char * vga_pinmap;
unsigned char vga_video_mode_id_cur;

//Conversion a C
int h_front;
int h_sync;
int h_back;
int h_pixels;
int v_front;
int v_sync;
int v_back;
int v_pixels;
int v_div;
int pixel_clock;
unsigned char h_polarity;
unsigned char v_polarity;



 #define APLL_SDM_STOP_VAL_2_REV1 0x49
 #define APLL_SDM_STOP_VAL_2_REV0 0x69
 #define APLL_SDM_STOP_VAL_1 0x09
 #define APLL_CAL_DELAY_1 0x0f
 #define APLL_CAL_DELAY_2 0x3f
 #define APLL_CAL_DELAY_3 0x1f
 #define EFUSE_BLK0_RDATA3_REG (DR_REG_EFUSE_BASE + 0x00c)
 #define I2C_APLL 0X6D
 #define I2C_APLL_DSDM2 7
 #define I2C_APLL_DSDM0 9
 #define I2C_APLL_DSDM1 8
 #define I2C_APLL_SDM_STOP 5
 #define I2C_APLL_OR_OUTPUT_DIV 4
 #define I2C_APLL_IR_CAL_DELAY 0
 #define I2C_APLL_OR_CAL_END 3

 #define I2C_WRITEREG_MASK_RTC(block, reg_add, indata) \
      rom_i2c_writeReg_Mask(block, block##_HOSTID,  reg_add,  reg_add##_MSB,  reg_add##_LSB,  indata)

 #define I2C_READREG_MASK_RTC(block, reg_add) \
      rom_i2c_readReg_Mask(block, block##_HOSTID,  reg_add,  reg_add##_MSB,  reg_add##_LSB)

 #define I2C_WRITEREG_RTC(block, reg_add, indata) \
      rom_i2c_writeReg(block, block##_HOSTID,  reg_add, indata)

 void custom_rtc_clk_apll_enable(bool enable, uint32_t sdm0, uint32_t sdm1, uint32_t sdm2, uint32_t o_div)
 {  
  REG_SET_FIELD(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PLLA_FORCE_PD, enable ? 0 : 1);
  REG_SET_FIELD(RTC_CNTL_ANA_CONF_REG, RTC_CNTL_PLLA_FORCE_PU, enable ? 1 : 0);
 
  if (!enable && REG_GET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_SOC_CLK_SEL) != RTC_CNTL_SOC_CLK_SEL_PLL)
  {
   REG_SET_BIT(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_I2C_FORCE_PD);
  }
  else
  {
   REG_CLR_BIT(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_I2C_FORCE_PD);
  }
 
  if (enable)
  {
   uint8_t sdm_stop_val_2 = APLL_SDM_STOP_VAL_2_REV1;
   uint32_t is_rev0 = (GET_PERI_REG_BITS2(EFUSE_BLK0_RDATA3_REG, 1, 15) == 0);
   if (is_rev0) 
   {
    sdm0 = 0;
    sdm1 = 0;
    sdm_stop_val_2 = APLL_SDM_STOP_VAL_2_REV0;
   }
   
   I2C_WRITEREG_MASK_RTC(I2C_APLL, I2C_APLL_DSDM2, sdm2);
   I2C_WRITEREG_MASK_RTC(I2C_APLL, I2C_APLL_DSDM0, sdm0);
   I2C_WRITEREG_MASK_RTC(I2C_APLL, I2C_APLL_DSDM1, sdm1);
   I2C_WRITEREG_RTC(I2C_APLL, I2C_APLL_SDM_STOP, APLL_SDM_STOP_VAL_1);
   I2C_WRITEREG_RTC(I2C_APLL, I2C_APLL_SDM_STOP, sdm_stop_val_2);
   I2C_WRITEREG_MASK_RTC(I2C_APLL, I2C_APLL_OR_OUTPUT_DIV, o_div);
   
   //Quito calibracion
   ////calibration   
   //I2C_WRITEREG_RTC(I2C_APLL, I2C_APLL_IR_CAL_DELAY, APLL_CAL_DELAY_1);
   //I2C_WRITEREG_RTC(I2C_APLL, I2C_APLL_IR_CAL_DELAY, APLL_CAL_DELAY_2);
   //I2C_WRITEREG_RTC(I2C_APLL, I2C_APLL_IR_CAL_DELAY, APLL_CAL_DELAY_3); 
 
   ////wait for calibration end   
   //while (!(I2C_READREG_MASK_RTC(I2C_APLL, I2C_APLL_OR_CAL_END)))
   //{
   // //use ets_delay_us so the RTC bus doesn't get flooded
   // ets_delay_us(1);
   //}      
  }
  //Serial.printf("Custom PLL call\r\n");
 }


void VgaMode_VgaMode(int hf, int hs, int hb, int hpix,
          int vf, int vs, int vb, int vpix,
          int vdiv, int pixclock, int vp, int hp)
{
 h_front= hf;
 h_sync= hs;
 h_back= hb;
 h_pixels= hpix;
 v_front=vf;
 v_sync=vs;
 v_back=vb;
 v_pixels= vpix;
 v_div= vdiv;
 pixel_clock= pixclock;
 h_polarity= hp;
 v_polarity= vp;
}


unsigned char vsync_inv_bit(){ return v_polarity<<7; }
unsigned char hsync_inv_bit(){ return h_polarity<<6; }
unsigned char vsync_bit(){ return (1-v_polarity)<<7; }
unsigned char hsync_bit(){ return (1-h_polarity)<<6; }
unsigned char sync_bits(){ return vsync_inv_bit() | hsync_inv_bit(); }
int x_res(){ return h_pixels; }
int y_res(){ return v_pixels / v_div; }


// DMA buffer descriptors
static int dma_buf_desc_count;
static lldesc_t *dma_buf_desc;

// DMA buffers for hblank/vblank
static uint8_t *dma_buf_hblank_vnorm;       // horizontal blank, outside vsync
static uint8_t *dma_buf_hblank_vsync;       // horizontal blank, inside vsync
static uint8_t *dma_buf_vblank_vnorm;       // blank pixel data, outside vsync
static uint8_t *dma_buf_vblank_vsync;       // blank pixel data, inside vsync

// DMA buffers for pixels (framebuffers)
//JJ static int num_framebuffers;                // number of framebuffers (1 or 2, user-selected)
//JJ static int active_framebuffer;              // index of current active (display) framebuffer
//JJ static int back_framebuffer;                // index of current back (drawing) framebuffer
static unsigned char num_framebuffers;                // number of framebuffers (1 or 2, user-selected)
static unsigned char active_framebuffer;              // index of current active (display) framebuffer
static unsigned char back_framebuffer;                // index of current back (drawing) framebuffer
static uint8_t **framebuffer[2];            // framebuffer (pixel data + sync bits)

// interrupt handler stuff
static intr_handle_t i2s_isr_handle;        // I2S interrupt handler (triggered at end of frame)
static volatile int DRAM_ATTR vga_frame_count = 0;    // incremented by I2S interrupt handler

//static const VgaMode *vga_mode;

void FreeInterrupt()
{
 if (i2s_isr_handle!=0)
 {
  esp_intr_free(i2s_isr_handle);
  i2s_isr_handle=0; //Deberia chequear return ESP_OK
 } 
}

//****************************
void SetVideoInterrupt(unsigned char auxState)
{
 if (auxState == 1)
 {
  if (i2s_isr_handle!=0)
  {
   esp_intr_enable(i2s_isr_handle);   
  }
 }
 else
 {
  if (i2s_isr_handle!=0)
  {
   esp_intr_disable(i2s_isr_handle);   
  }
 } 
}

//****************************
static void die()
{  
  while (true) {
    DELAY(100);
  }
}

static void check_alloc(void *alloc)
{
  if (! alloc) 
  {
    die();
  }
}

//Allocate an array of DMA buffer descriptors
static lldesc_t *alloc_dma_buf_desc_array(int num)
{
  lldesc_t *d = (lldesc_t *) heap_caps_malloc(sizeof(lldesc_t) * num, MALLOC_CAP_DMA);
  if (d) {
    for (int i = 0; i < num; i++) {
      d[i].length = 0;
      d[i].size = 0;
      d[i].owner = 1;
      d[i].sosf = 0;
      d[i].buf = (uint8_t *) 0;
      d[i].offset = 0;
      d[i].empty = 0;
      d[i].eof = 0;
      d[i].qe.stqe_next = (lldesc_t *) 0;
    }
  }
  return d;
}

//Set buffer and length of a DMA buffer descriptor
static void set_dma_buf_desc_buffer(lldesc_t *buf_desc, uint8_t *buffer, int len)
{
  buf_desc->length = len;
  buf_desc->size = len;
  buf_desc->buf = buffer;
}

//Allocate DMA buffers and descriptors for VGA output via I2S.
//This function allocates all descriptors, but only the buffers
//outside the framebuffer area (vblank/hblank). The framebuffer lines
//must be allocated later and filled in with
//set_vga_i2s_active_framebuffer().
static void allocate_vga_i2s_buffers()
{
  // prepare DMA buffers for hblank/vblank areas
  int hblank_len = (h_front + h_sync + h_back + 3) & 0xfffffffc;
  dma_buf_hblank_vnorm = (uint8_t *) heap_caps_malloc(hblank_len, MALLOC_CAP_DMA);
  dma_buf_hblank_vsync = (uint8_t *) heap_caps_malloc(hblank_len, MALLOC_CAP_DMA);
  dma_buf_vblank_vnorm = (uint8_t *) heap_caps_malloc((h_pixels+3) & 0xfffffffc, MALLOC_CAP_DMA);
  dma_buf_vblank_vsync = (uint8_t *) heap_caps_malloc((h_pixels+3) & 0xfffffffc, MALLOC_CAP_DMA);

  check_alloc(dma_buf_hblank_vnorm);
  check_alloc(dma_buf_hblank_vsync);
  check_alloc(dma_buf_vblank_vnorm);
  check_alloc(dma_buf_vblank_vsync);

  for (int i = 0; i < hblank_len; i++) {
    if (i < h_front || i >= h_front+h_sync) {
      dma_buf_hblank_vnorm[i^2] = hsync_inv_bit() | vsync_inv_bit();
      dma_buf_hblank_vsync[i^2] = hsync_inv_bit() | vsync_bit();
    } else {
      dma_buf_hblank_vnorm[i^2] = hsync_bit() | vsync_inv_bit();
      dma_buf_hblank_vsync[i^2] = hsync_bit() | vsync_bit();
    }
  }
  for (int i = 0; i < h_pixels; i++) {
    dma_buf_vblank_vnorm[i^2] = hsync_inv_bit() | vsync_inv_bit();
    dma_buf_vblank_vsync[i^2] = hsync_inv_bit() | vsync_bit();
  }

  // allocate DMA buffer descriptors
  dma_buf_desc_count = 2 * (v_front + v_sync + v_back + v_pixels);
  dma_buf_desc = alloc_dma_buf_desc_array(dma_buf_desc_count);
  //JJ check_alloc(dma_buf_desc, "not enough memory for DMA buffer descriptors");
  check_alloc(dma_buf_desc);
  for (int i = 0; i < dma_buf_desc_count; i++) {
    dma_buf_desc[i].qe.stqe_next = &dma_buf_desc[(i+1)%dma_buf_desc_count];
  }
  dma_buf_desc[dma_buf_desc_count-1].eof = true;  // trigger interrupt at the end of last buffer

  // stitch together the DMA buffers to compose a full VGA frame
  int d = 0;
  for (int i = 0; i < v_front; i++) {
    set_dma_buf_desc_buffer(&dma_buf_desc[d++], dma_buf_hblank_vnorm, hblank_len);
    set_dma_buf_desc_buffer(&dma_buf_desc[d++], dma_buf_vblank_vnorm, h_pixels);
  }
  for (int i = 0; i < v_sync; i++) {
    set_dma_buf_desc_buffer(&dma_buf_desc[d++], dma_buf_hblank_vsync, hblank_len);
    set_dma_buf_desc_buffer(&dma_buf_desc[d++], dma_buf_vblank_vsync, h_pixels);
  }
  for (int i = 0; i < v_back; i++) {
    set_dma_buf_desc_buffer(&dma_buf_desc[d++], dma_buf_hblank_vnorm, hblank_len);
    set_dma_buf_desc_buffer(&dma_buf_desc[d++], dma_buf_vblank_vnorm, h_pixels);
  }
  for (int i = 0; i < v_pixels; i++) {
    set_dma_buf_desc_buffer(&dma_buf_desc[d++], dma_buf_hblank_vnorm, hblank_len);
    set_dma_buf_desc_buffer(&dma_buf_desc[d++], (uint8_t *) 0, h_pixels);  // set later with set_vga_i2s_active_framebuffer()
  } 
}

//Set the framebuffer memory as the buffers for the DMA buffer
//descriptors for I2S output.
static void set_vga_i2s_active_framebuffer(unsigned char **fb)
{
  for (int i = 0; i < v_pixels; i++) {
    set_dma_buf_desc_buffer(&dma_buf_desc[2*(v_front+v_sync+v_back+i)+1], fb[i/v_div], h_pixels);
  }  
}


//==========================================================================
//===== FRAMEBUFFER ========================================================
//==========================================================================
static unsigned char **alloc_framebuffer()
{
  unsigned char **fb = (unsigned char **) malloc(sizeof(unsigned char *) * y_res());
  //JJ check_alloc(fb, "not enough memory for framebuffer");
  check_alloc(fb);
  for (int i = 0; i < y_res(); i++) {
    fb[i] = (uint8_t *) heap_caps_malloc((h_pixels+3) & 0xfffffffc, MALLOC_CAP_DMA);
    //JJ check_alloc(fb[i], "not enough DMA memory for framebuffer");
    check_alloc(fb[i]);
  }
  return fb;
}

static void clear_framebuffer(unsigned char **fb, uint8_t color)
{
 //#ifdef use_lib_vga8colors
 // unsigned int clear_byte = sync_bits() | (color & 0x07);
 //#else
 // unsigned int clear_byte = sync_bits() | (color & 0x3f);
 //#endif
 unsigned int clear_byte=0;
 if (vga_is8colors==1)
 {
  clear_byte = sync_bits() | (color & 0x07);
 }
 else
 {
  clear_byte = sync_bits() | (color & 0x3f);
 }

  unsigned int clear_data = ((clear_byte << 24) |
                             (clear_byte << 16) |
                             (clear_byte <<  8) |
                             (clear_byte <<  0));
  for (int y = 0; y < y_res(); y++) 
  {
    unsigned int *line = (unsigned int *) fb[y];
    //for (int x = 0; x < x_res()/4; x++) 
    for (int x = 0; x < (h_pixels>>2); x++)
    {
      line[x] = clear_data;
    }
  }
}


//==========================================================================
//=== I2S OUTPUT ===========================================================
//==========================================================================
static void reset_i2s_txrx()
{
   I2S1.conf.tx_reset = 1;
   I2S1.conf.tx_reset = 0;
   I2S1.conf.rx_reset = 1;
   I2S1.conf.rx_reset = 0;
}

static void reset_i2s_fifo()
{
   I2S1.conf.rx_fifo_reset = 1;
   I2S1.conf.rx_fifo_reset = 0;
   I2S1.conf.tx_fifo_reset = 1;
   I2S1.conf.tx_fifo_reset = 0;
}

static void reset_i2s_dma()
{
   I2S1.lc_conf.in_rst  = 1;
   I2S1.lc_conf.in_rst  = 0;
   I2S1.lc_conf.out_rst = 1;
   I2S1.lc_conf.out_rst = 0;  
}

//Interrupt handler function: just increment the frame count to let
// the main logic to know when it's safe to swap framebuffers.
static void IRAM_ATTR i2s_isr(void *arg)
{
  REG_WRITE(I2S_INT_CLR_REG(1), (REG_READ(I2S_INT_RAW_REG(1)) & 0xffffffc0) | 0x3f); // 1 means I2S1
  vga_frame_count++;
}

//Prepare I2S output to the selected pins (see comments on function vga_init())
//JJ static void setup_i2s_output(const int *pin_map)
static void setup_i2s_output(const unsigned char *pin_map)
{
  periph_module_enable(PERIPH_I2S1_MODULE);
  //for (int i = 0; i < 8; i++) 
  for (unsigned char i = 0; i < 8; i++) 
  {   
   unsigned char pin = pin_map[i];
   if (pin != 255)
   {//Si es 255 se salta. DAC 8 colores
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[pin], PIN_FUNC_GPIO);
    gpio_set_direction((gpio_num_t) pin, (gpio_mode_t) GPIO_MODE_DEF_OUTPUT);
    gpio_matrix_out(pin, I2S1O_DATA_OUT0_IDX + i, false, false);
   }
  }
  periph_module_enable(PERIPH_I2S1_MODULE);

  // reset
  reset_i2s_txrx();
  reset_i2s_fifo();
  reset_i2s_dma();

  // set parallel mode
  I2S1.conf2.val = 0;
  I2S1.conf2.lcd_en = 1;
  I2S1.conf2.lcd_tx_wrx2_en = 1;
  I2S1.conf2.lcd_tx_sdx2_en = 0;
  
  // data rate
  I2S1.sample_rate_conf.val = 0;
  I2S1.sample_rate_conf.tx_bits_mod = 8;
    
  // clock setup
  #ifdef use_lib_fix_double_precision     
   unsigned int p0, p1, p2, p3;
   Serial.printf("use_lib_fix_double_precision\r\n");
   switch (vga_video_mode_id_cur)
   {
    case video_mode_360x200x70hz_bitluni: case video_mode_360x200x70hz_bitluni_apll_fix:
     p0= 0x00B1; p1= 0x00BE; p2= 0x0008; p3= 0x0007;     
     break;

    case video_mode_vga320x200x70hz_bitluni: case video_mode_vga320x200x70hz_bitluni_apll_fix:
     p0= 0x00A3; p1= 0x00D8; p2= 0x0009; p3= 0x0009;
     break;
    
    case video_mode_vga320x240x60hz_bitluni: case video_mode_vga320x240x60hz_bitluni_apll_fix:
     p0= 0x00A3; p1= 0x00D8; p2= 0x0009; p3= 0x0009;    
     break;

    case video_mode_vga320x200x70hz_fabgl: 
     p0= 0x00AE; p1= 0x00CF; p2= 0x0004; p3= 0x0005;
     break;
    
    case video_mode_vga320x240x60hz_fabgl:
     p0= 0x000A; p1= 0x0057; p2= 0x0007; p3= 0x0007;
     break;

    default: break;
   }

   /*
   #ifdef use_lib_vga360x200x70hz_bitluni
    //sdm:0x8BEB1 odir:0x0007
    //(sdm & 0xff):0x00B1 (sdm >> 8):0x00BE (sdm >> 16):0x0008
    unsigned int p0= 0x00B1;
    unsigned int p1= 0x00BE;
    unsigned int p2= 0x0008;
    unsigned int p3= 0x0007;
   #else
    #ifdef use_lib_vga320x200x70hz_bitluni
     //sdm:0x9D8A3 odir:0x0009
     //(sdm & 0xff):0x00A3 (sdm >> 8):0x00D8 (sdm >> 16):0x0009    
     unsigned int p0= 0x00A3;
     unsigned int p1= 0x00D8;
     unsigned int p2= 0x0009;
     unsigned int p3= 0x0009;

     //Datos fabgl 320x200 75 Hz freq:12930000 Da fuera de rango
     //p0=0x000E;
     //p1=0x000D;
     //p2=0x0005;
     //p3=0x0005;
     //Datos fabgl 320x200 70Hz freq:12587500 funciona
     //p0=0x00AE;
     //p1=0x00CF;
     //p2=0x0004;
     //p3=0x0005;
     //Datos fabgl 320x200@60HzD 60Hz freq:25175000 fuera de rango
     //p0=0x00EB;
     //p1=0x0011;
     //p2=0x0006;
     //p3=0x0002;
    #else
     #ifdef use_lib_vga320x240x60hz_bitluni
      //sdm:0x9D8A3 odir:0x0009
      //(sdm & 0xff):0x00A3 (sdm >> 8):0x00D8 (sdm >> 16):0x0009
      unsigned int p0= 0x00A3;
      unsigned int p1= 0x00D8;
      unsigned int p2= 0x0009;
      unsigned int p3= 0x0009;

      //Datos fabgl QVGA 320x240@60Hz 60Hz freq:12600000 Funciona
      //sdm0:000A sdm1:0057 sdm2:0007 o_div:0007
      //p0=0x000A;
      //p1=0x0057;
      //p2=0x0007;
      //p3=0x0007;
     #else
      #ifdef use_lib_vga320x200x70hz_fabgl
       unsigned int p0= 0x00AE;
       unsigned int p1= 0x00CF;
       unsigned int p2= 0x0004;
       unsigned int p3= 0x0005;          
      #else
       #ifdef use_lib_vga320x240x60hz_fabgl
        unsigned int p0= 0x000A;
        unsigned int p1= 0x0057;
        unsigned int p2= 0x0007;
        unsigned int p3= 0x0007;       
       #endif
      #endif
     #endif
    #endif
   #endif
   */

   #ifdef use_lib_debug_i2s
    Serial.printf("bitluni pixel_clock:%d\r\n",pixel_clock);
    Serial.printf("bitluni p0:0x%04X p1:0x%04X p2:0x%04X p3:0x%04X \r\n",p0,p1,p2,p3);
   #endif

   if (pll_custom_force==0)
   {
    rtc_clk_apll_enable(true, p0, p1, p2, p3);
   }
   else
   {
    custom_rtc_clk_apll_enable(true, p0, p1, p2, p3);
   }
   //Hay que revisar el swgenerator.cpp - play - setupClock si se usa sonido
   //Prepara rtc_clk_apll_enable con precision doble, pero no se usa
  #else
   long freq = pixel_clock * 2;
   int sdm, sdmn;
   int odir = -1;

   if (pll_cte_force==0)
   {   
    do {	
     odir++;
     sdm  = long((double(freq) / (20000000. / (odir + 2    ))) * 0x10000) - 0x40000;
     sdmn = long((double(freq) / (20000000. / (odir + 2 + 1))) * 0x10000) - 0x40000;
    } while(sdm < 0x8c0ecL && odir < 31 && sdmn < 0xA1fff);
    if (sdm > 0xA1fff) sdm = 0xA1fff;
   

    #ifdef use_lib_debug_i2s
     Serial.printf("bitluni freq:%ld pixel_clock:%d\r\n",freq,pixel_clock);
     Serial.printf("bitluni sdm:0x%04X odir:0x%04X\r\n",sdm,odir);
     Serial.printf("bitluni (sdm & 0xff):0x%04X (sdm >> 8):0x%04X (sdm >> 16):0x%04X\r\n",sdm & 0xff, (sdm >> 8) & 0xff, sdm >> 16);      
    #endif
    pll_cte_p0= (sdm & 0xff);
    pll_cte_p1= ((sdm >> 8) & 0xff);
    pll_cte_p2= (sdm >> 16);
    pll_cte_p3= odir;
    if (pll_custom_force==0)
    {
     rtc_clk_apll_enable(true, sdm & 0xff, (sdm >> 8) & 0xff, sdm >> 16, odir);
    }
    else
    {
     custom_rtc_clk_apll_enable(true, sdm & 0xff, (sdm >> 8) & 0xff, sdm >> 16, odir);
    }
   } 
   else   
   {
    Serial.printf("Force PLL cte calcule\r\n");
    unsigned int p0= pll_cte_p0;
    unsigned int p1= pll_cte_p1;
    unsigned int p2= pll_cte_p2;
    unsigned int p3= pll_cte_p3;
    Serial.printf("bitluni p0:0x%04X p1:0x%04X p2:0x%04X p3:0x%04X \r\n",p0,p1,p2,p3);
    if (pll_custom_force==0)
    {
     rtc_clk_apll_enable(true, p0, p1, p2, p3);
    }
    else
    {
     custom_rtc_clk_apll_enable(true, p0, p1, p2, p3);
    }
   }
  #endif

   //320x200  720x400 31.4 Khz 70.0 Hz
   //freq:25175000 pixel_clock:12587500
   //sdm:0x9D8A3 odir:0x0009
   //(sdm & 0xff):0x00A3 (sdm >> 8):0x00D8 (sdm >> 16):0x0009
   //
   //320x240  640x480 31.4 Khz 60 Hz
   //freq:25175000 pixel_clock:12587500
   //sdm:0x9D8A3 odir:0x0009
   //(sdm & 0xff):0x00A3 (sdm >> 8):0x00D8 (sdm >> 16):0x0009   
   //
   //360x200 720x400 31.3 Khz 70.3 Hz
   //freq:28322000 pixel_clock:14161000
   //sdm:0x8BEB1 odir:0x0007
   //(sdm & 0xff):0x00B1 (sdm >> 8):0x00BE (sdm >> 16):0x0008
   
  //rtc_clk_apll_enable(true, sdm & 0xff, (sdm >> 8) & 0xff, sdm >> 16, odir);

  I2S1.clkm_conf.val = 0;
  I2S1.clkm_conf.clka_en = 1;
  I2S1.clkm_conf.clkm_div_num = 2;
  I2S1.clkm_conf.clkm_div_a = 1;
  I2S1.clkm_conf.clkm_div_b = 0;
  I2S1.sample_rate_conf.tx_bck_div_num = 1;

  I2S1.fifo_conf.val = 0;
  I2S1.fifo_conf.tx_fifo_mod_force_en = 1;
  I2S1.fifo_conf.tx_fifo_mod = 1;
  I2S1.fifo_conf.tx_data_num = 32;
  I2S1.fifo_conf.dscr_en = 1;

  I2S1.conf1.val = 0;
  I2S1.conf1.tx_stop_en = 0;
  I2S1.conf1.tx_pcm_bypass = 1;
  
  I2S1.conf_chan.val = 0;
  I2S1.conf_chan.tx_chan_mod = 1;
  
  I2S1.conf.tx_right_first = 1;

  I2S1.timing.val = 0;
  
  I2S1.conf.tx_msb_right = 0;
  I2S1.conf.tx_msb_shift = 0;
  I2S1.conf.tx_mono = 0;
  I2S1.conf.tx_short_sync = 0;

  i2s_isr_handle = 0;
  esp_intr_alloc(ETS_I2S1_INTR_SOURCE, ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_LEVEL3 | ESP_INTR_FLAG_IRAM, i2s_isr, NULL, &i2s_isr_handle);
}


//Start the I2S output
static void start_i2s_output()
{
  esp_intr_disable(i2s_isr_handle);

  // reset
  I2S1.lc_conf.val |=   I2S_IN_RST_M | I2S_OUT_RST_M | I2S_AHBM_RST_M | I2S_AHBM_FIFO_RST_M;
  I2S1.lc_conf.val &= ~(I2S_IN_RST_M | I2S_OUT_RST_M | I2S_AHBM_RST_M | I2S_AHBM_FIFO_RST_M);
  I2S1.conf.val |=   I2S_RX_RESET_M | I2S_RX_FIFO_RESET_M | I2S_TX_RESET_M | I2S_TX_FIFO_RESET_M;
  I2S1.conf.val &= ~(I2S_RX_RESET_M | I2S_RX_FIFO_RESET_M | I2S_TX_RESET_M | I2S_TX_FIFO_RESET_M);
  while (I2S1.state.rx_fifo_reset_back) {
    // wait
  }

  // setup
  I2S1.lc_conf.val |= I2S_OUT_DATA_BURST_EN | I2S_OUTDSCR_BURST_EN;
  I2S1.out_link.addr = (uint32_t) &dma_buf_desc[0];
  I2S1.out_link.start = 1;
  I2S1.int_clr.val = I2S1.int_raw.val;
  I2S1.int_ena.val = 0;

  // enable interrupt
  I2S1.int_ena.out_eof = 1;
  esp_intr_enable(i2s_isr_handle);

  // start output
  I2S1.conf.tx_start = 1; 
}


//==========================================================================
//=== API ==================================================================
//==========================================================================

// Initialize VGA output.
//
// `pin_map` must point to 8 ints selecting the gpio pins, in this order:
//   [0] red   bit 0 (lsb)
//   [1] red   bit 1 (msb)
//   [2] green bit 0 (lsb)
//   [3] green bit 1 (msb)
//   [4] blue  bit 0 (lsb)
//   [5] blue  bit 1 (msb)
//   [6] H-sync
//   [7] V-sync
//JJ void vga_init(const int *pin_map, const VgaMode &mode, bool double_buffered)
//void vga_init(const unsigned char *pin_map, const VgaMode &mode, bool double_buffered)
void vga_init(const unsigned char *pin_map, const unsigned int *mode, bool double_buffered,unsigned char pllcteforce, unsigned int p0,unsigned int p1,unsigned int p2,unsigned int p3,unsigned char pllcustomforce,unsigned char is8colors, unsigned char vgaVideoModeid)
{
  //vga_mode = &mode;
  #ifdef use_lib_log_serial
   Serial.printf("vga_init BEGIN\r\n");
   //Serial.printf("Sizeof framebuffer %d\r\n",sizeof(framebuffer)); //8 bytes
  #endif 

  vga_is8colors= is8colors;
  vga_video_mode_id_cur= vgaVideoModeid;

  Serial.printf("Id video:");
  switch (vga_video_mode_id_cur)
  {
   case video_mode_360x200x70hz_bitluni: Serial.printf("360x200x70hz_bitluni\r\n"); break;
   case video_mode_360x200x70hz_bitluni_apll_fix: Serial.printf("360x200x70hz_bitluni_apll_fix\r\n"); break;
   case video_mode_vga320x200x70hz_bitluni: Serial.printf("vga320x200x70hz_bitluni\r\n"); break;
   case video_mode_vga320x200x70hz_fabgl: Serial.printf("vga320x200x70hz_fabgl\r\n"); break;
   case video_mode_vga320x200x70hz_bitluni_apll_fix: Serial.printf("vga320x200x70hz_bitluni_apll_fix\r\n"); break;
   case video_mode_vga320x240x60hz_bitluni: Serial.printf("vga320x240x60hz_bitluni\r\n"); break;
   case video_mode_vga320x240x60hz_fabgl: Serial.printf("vga320x240x60hz_fabgl\r\n"); break;
   case video_mode_vga320x240x60hz_bitluni_apll_fix: Serial.printf("vga320x240x60hz_bitluni_apll_fix\r\n"); break;
   default: break;
  }  
  
  //for (unsigned char i=0;i<8;i++)
  //{
  // vga_pinmap[i]= pin_map[i];
  //}
  vga_pinmap= pin_map;

  VgaMode_VgaMode(mode[0],mode[1],mode[2],mode[3],mode[4],mode[5],mode[6],mode[7],mode[8],mode[9],mode[10],mode[11]);
  pll_cte_force= pllcteforce; //force not calculate PLL
  pll_custom_force= pllcustomforce; //fuerza uso de custom pll call function
  pll_cte_p0= p0;
  pll_cte_p1= p1;
  pll_cte_p2= p2;
  pll_cte_p3= p3;

  num_framebuffers = (double_buffered) ? 2 : 1;
  for (int i = 0; i < num_framebuffers; i++) {
    framebuffer[i] = alloc_framebuffer();
  }
  active_framebuffer = 0;
  back_framebuffer = (active_framebuffer+1) % num_framebuffers;
  clear_framebuffer(framebuffer[active_framebuffer], 0);
  if (num_framebuffers>1)
  {//Solo tengo un buffer
   clear_framebuffer(framebuffer[back_framebuffer], 0);
  }

  allocate_vga_i2s_buffers();
  set_vga_i2s_active_framebuffer(framebuffer[active_framebuffer]);

  setup_i2s_output(pin_map);  
  start_i2s_output();

#if 0
  print("sync_bits=-x%x", vga_mode->sync_bits());
  for (int i = 0; i < dma_buf_desc_count; i++) {
    printf("%4d: %4d 0x%08x { %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x ... }\n",
           i, dma_buf_desc[i].length, (int32_t) dma_buf_desc[i].buf,
           dma_buf_desc[i].buf[ 0], dma_buf_desc[i].buf[ 1], dma_buf_desc[i].buf[ 2], dma_buf_desc[i].buf[ 3],
           dma_buf_desc[i].buf[ 4], dma_buf_desc[i].buf[ 5], dma_buf_desc[i].buf[ 6], dma_buf_desc[i].buf[ 7],
           dma_buf_desc[i].buf[ 8], dma_buf_desc[i].buf[ 9], dma_buf_desc[i].buf[10], dma_buf_desc[i].buf[11]);
  }
#endif

 #ifdef use_lib_log_serial
  Serial.printf("vga_init END\r\n");
 #endif 
}

//Swap the back (drawing) and active (display) framebuffers.
//If `wait_vsync` is true, wait for a vertical sync before
//swapping. Use false only for measure your drawing code performance,
//since it will result in an annoying horizontal line on the screen.
void vga_swap_buffers(bool wait_vsync)
{
  if (wait_vsync) {
    // we might wait up to 1s/60 = 16.7 milliseconds here
    vga_frame_count = 0;
    while (vga_frame_count == 0) {
      vTaskDelay(1);
    }
  }
  active_framebuffer = back_framebuffer;
  back_framebuffer = (active_framebuffer+1) % num_framebuffers;
  set_vga_i2s_active_framebuffer(framebuffer[active_framebuffer]);
}


//Clear the back (drawing) framebufer to the given color (0b00BBGGRR).
void vga_clear_screen(uint8_t color)
{
  clear_framebuffer(vga_get_framebuffer(), color);
}


//Return the sync bits to use in every framebuffer pixel
unsigned char vga_get_sync_bits()
{
  return sync_bits();
}

unsigned char vga_get_vsync_inv_bit()
{
  return vsync_inv_bit();
}

unsigned char vga_get_hsync_inv_bit(void)
{
 return hsync_inv_bit();
}


//Return the horizontal resolution of the screen (in pixels)
int vga_get_xres()
{
  return x_res();
}


//Return the vertical resolution of the screen (in pixels)
int vga_get_yres()
{
  return y_res();
}

//Return a pointer to the back (drawing) framebuffer lines.
unsigned char **vga_get_framebuffer()
{
  return framebuffer[back_framebuffer];
}

int vga_get_y_res()
{
 return v_pixels / v_div;
} 

void vga_free()
{
 unsigned int auxY= vga_get_y_res();
 Serial.printf("Free %d scanlines\r\n",auxY);
 if (framebuffer[0] != NULL)
 {
  for (unsigned int i = 0; i < auxY; i++)
  {
   free(framebuffer[0][i]);
   framebuffer[0][i]= NULL;
  }  
  free(framebuffer[0]);
  framebuffer[0]= NULL;
 }
 
 free(dma_buf_hblank_vnorm);
 dma_buf_hblank_vnorm= NULL;
 free(dma_buf_hblank_vsync);
 dma_buf_hblank_vsync= NULL;
 free(dma_buf_vblank_vnorm);
 dma_buf_vblank_vnorm= NULL;
 free(dma_buf_vblank_vsync);
 dma_buf_vblank_vsync= NULL;

 free(dma_buf_desc);
 dma_buf_desc= NULL;

 FreeInterrupt(); //Liberamos interrupcion
 Serial.printf("FreeInterrupt vga6bit\r\n");

 periph_module_disable(PERIPH_I2S1_MODULE); 
 Serial.printf("Free module I2S1\r\n");
 
 rtc_clk_apll_enable(false,0,0,0,0);
 Serial.printf("Disable rtc_clk_apll\r\n");

 vga_freeGPIO();

 back_framebuffer = active_framebuffer = 0;  
}

//**************************
void vga_freeGPIO()
{
 for (unsigned char i=0;i<8;i++)
 {
  if (vga_pinmap[i]!=255)
  {
   gpio_reset_pin((gpio_num_t)vga_pinmap[i]);
  }
 }
 Serial.printf("Reset gpio\r\n"); //Necesario cambiar 6pp a 3bpp correctamente
}

unsigned int vga_get_pll_cte_p0()
{
 return pll_cte_p0;
}

unsigned int vga_get_pll_cte_p1()
{
 return pll_cte_p1;
}

unsigned int vga_get_pll_cte_p2()
{
 return pll_cte_p2;
}

unsigned int vga_get_pll_cte_p3()
{
 return pll_cte_p3;
}

//void WaitVsync()
//{
// vga_frame_count = 0;
// while (vga_frame_count == 0)
// {
//  vTaskDelay(1);
// }
//}

#endif








/* 
//Clase C++ modificada custom
class VgaMode {
public:
  int h_front;
  int h_sync;
  int h_back;
  int h_pixels;
  int v_front;
  int v_sync;
  int v_back;
  int v_pixels;
  int v_div;
  int pixel_clock;
  //JJ int h_polarity;
  //JJ int v_polarity;
  unsigned char h_polarity;
  unsigned char v_polarity;

  VgaMode(int hf, int hs, int hb, int hpix,
          int vf, int vs, int vb, int vpix,
          int vdiv, int pixclock, int vp, int hp)
    : h_front(hf), h_sync(hs), h_back(hb), h_pixels(hpix),
      v_front(vf), v_sync(vs), v_back(vb), v_pixels(vpix),
      v_div(vdiv), pixel_clock(pixclock),
      h_polarity(hp), v_polarity(vp) {
      //v_polarity(vp), h_polarity(hp) {
  }
  
  uint8_t vsync_inv_bit() const { return v_polarity<<7; }
  uint8_t hsync_inv_bit() const { return h_polarity<<6; }
  uint8_t vsync_bit() const { return (1-v_polarity)<<7; }
  uint8_t hsync_bit() const { return (1-h_polarity)<<6; }
  uint8_t sync_bits() const { return vsync_inv_bit() | hsync_inv_bit(); }
  int x_res() const { return h_pixels; }
  int y_res() const { return v_pixels / v_div; }
  
};

// --------------------------- hf  hs  hb  hpix  vf  vs  vb  vpix  vdiv pixclock  vp hp
const VgaMode vga_mode_320x240(8,  48, 24, 320,  11, 2,  31, 480,  2,   12587500, 1, 1);
const VgaMode vga_mode_288x240(24, 48, 40, 288,  11, 2,  31, 480,  2,   12587500, 1, 1);
const VgaMode vga_mode_240x240(48, 48, 64, 240,  11, 2,  31, 480,  2,   12587500, 1, 1);

//JJ Extra esta cambiado vp y vh
const VgaMode vga_mode_320x200(8,48,24,320,12,2,35,400,2,12587500,0,1); //funciona con 0,1
//const VgaMode vga_mode_320x200(8, 48, 24, 320, 12, 2, 35, 400, 2, 12587500, 1, 1); //Se ve con 1,1
//hfront hsync hback pixels vfront vsync vback lines divy pixelclock hpolaritynegative vpolaritynegative
const VgaMode vga_mode_200x150(6,18,32,200, 1,2,22,600,4,9000000, 0,0);




// We use 2 DMA buffer descriptors per VGA line, their buffers are:
//
//                  |--hsync--|       |------------h_pixels------------|
//
//           +------+---------+-------+--------------------------------+
//           |      |         |       |                                |
//           |  dma_buf_hblank_vnorm  |   dma_buf_vblank_vnorm         |
//           |      |         |       |                                |
//     ---   +------+---------+-------+--------------------------------+
//      |    |      |         |       |                                |
//    vsync  |  dma_buf_hblank_vsync  |   dma_buf_vblank_vsync         |
//     |    |      |         |       |                                |
//     ---   +------+---------+-------+--------------------------------+
//           |      |         |       |                                |
//           |  dma_buf_hblank_vnorm  |   dma_buf_vblank_vnorm         |
//           |      |         |       |                                |
//     ---   +------+---------+-------+--------------------------------+
//      |    |      |         |       |                                |
//  v_pixels |  dma_buf_hblank_vnorm  |   framebuffer (pixels)         |
//      |    |      |         |       |                                |
//     ---   +------+---------+-------+--------------------------------+

// DMA buffer descriptors
static int dma_buf_desc_count;
static lldesc_t *dma_buf_desc;

// DMA buffers for hblank/vblank
static uint8_t *dma_buf_hblank_vnorm;       // horizontal blank, outside vsync
static uint8_t *dma_buf_hblank_vsync;       // horizontal blank, inside vsync
static uint8_t *dma_buf_vblank_vnorm;       // blank pixel data, outside vsync
static uint8_t *dma_buf_vblank_vsync;       // blank pixel data, inside vsync

// DMA buffers for pixels (framebuffers)
//JJ static int num_framebuffers;                // number of framebuffers (1 or 2, user-selected)
//JJ static int active_framebuffer;              // index of current active (display) framebuffer
//JJ static int back_framebuffer;                // index of current back (drawing) framebuffer
static unsigned char num_framebuffers;                // number of framebuffers (1 or 2, user-selected)
static unsigned char active_framebuffer;              // index of current active (display) framebuffer
static unsigned char back_framebuffer;                // index of current back (drawing) framebuffer
static uint8_t **framebuffer[2];            // framebuffer (pixel data + sync bits)

// interrupt handler stuff
static intr_handle_t i2s_isr_handle;        // I2S interrupt handler (triggered at end of frame)
static volatile int DRAM_ATTR vga_frame_count = 0;    // incremented by I2S interrupt handler

static const VgaMode *vga_mode;


//==========================================================================
//===== MISC FUNCTIONS =====================================================
//==========================================================================

//JJ static void die(const char *msg)
static void die()
{
  //JJ printf("\n\n**** ERROR: %s\n", msg);
  while (true) {
    DELAY(100);
  }
}

//JJ static void check_alloc(void *alloc, const char *msg)
static void check_alloc(void *alloc)
{
  if (! alloc) 
  {
    //JJ die(msg);
    die();
  }
}

//==========================================================================
//===== I2S BUFFERS ========================================================
//==========================================================================

//Allocate an array of DMA buffer descriptors
static lldesc_t *alloc_dma_buf_desc_array(int num)
{
  lldesc_t *d = (lldesc_t *) heap_caps_malloc(sizeof(lldesc_t) * num, MALLOC_CAP_DMA);
  if (d) {
    for (int i = 0; i < num; i++) {
      d[i].length = 0;
      d[i].size = 0;
      d[i].owner = 1;
      d[i].sosf = 0;
      d[i].buf = (uint8_t *) 0;
      d[i].offset = 0;
      d[i].empty = 0;
      d[i].eof = 0;
      d[i].qe.stqe_next = (lldesc_t *) 0;
    }
  }
  return d;
}

//Set buffer and length of a DMA buffer descriptor
static void set_dma_buf_desc_buffer(lldesc_t *buf_desc, uint8_t *buffer, int len)
{
  buf_desc->length = len;
  buf_desc->size = len;
  buf_desc->buf = buffer;
}

//Allocate DMA buffers and descriptors for VGA output via I2S.
//This function allocates all descriptors, but only the buffers
//outside the framebuffer area (vblank/hblank). The framebuffer lines
//must be allocated later and filled in with
//set_vga_i2s_active_framebuffer().
static void allocate_vga_i2s_buffers()
{
  // prepare DMA buffers for hblank/vblank areas
  int hblank_len = (vga_mode->h_front + vga_mode->h_sync + vga_mode->h_back + 3) & 0xfffffffc;
  dma_buf_hblank_vnorm = (uint8_t *) heap_caps_malloc(hblank_len, MALLOC_CAP_DMA);
  dma_buf_hblank_vsync = (uint8_t *) heap_caps_malloc(hblank_len, MALLOC_CAP_DMA);
  dma_buf_vblank_vnorm = (uint8_t *) heap_caps_malloc((vga_mode->h_pixels+3) & 0xfffffffc, MALLOC_CAP_DMA);
  dma_buf_vblank_vsync = (uint8_t *) heap_caps_malloc((vga_mode->h_pixels+3) & 0xfffffffc, MALLOC_CAP_DMA);

  //JJ check_alloc(dma_buf_hblank_vnorm, "not enough DMA memory for I2S buffers");
  //JJ check_alloc(dma_buf_hblank_vsync, "not enough DMA memory for I2S buffers");
  //JJ check_alloc(dma_buf_vblank_vnorm, "not enough DMA memory for I2S buffers");
  //JJ check_alloc(dma_buf_vblank_vsync, "not enough DMA memory for I2S buffers");

  check_alloc(dma_buf_hblank_vnorm);
  check_alloc(dma_buf_hblank_vsync);
  check_alloc(dma_buf_vblank_vnorm);
  check_alloc(dma_buf_vblank_vsync);

  for (int i = 0; i < hblank_len; i++) {
    if (i < vga_mode->h_front || i >= vga_mode->h_front+vga_mode->h_sync) {
      dma_buf_hblank_vnorm[i^2] = vga_mode->hsync_inv_bit() | vga_mode->vsync_inv_bit();
      dma_buf_hblank_vsync[i^2] = vga_mode->hsync_inv_bit() | vga_mode->vsync_bit();
    } else {
      dma_buf_hblank_vnorm[i^2] = vga_mode->hsync_bit() | vga_mode->vsync_inv_bit();
      dma_buf_hblank_vsync[i^2] = vga_mode->hsync_bit() | vga_mode->vsync_bit();
    }
  }
  for (int i = 0; i < vga_mode->h_pixels; i++) {
    dma_buf_vblank_vnorm[i^2] = vga_mode->hsync_inv_bit() | vga_mode->vsync_inv_bit();
    dma_buf_vblank_vsync[i^2] = vga_mode->hsync_inv_bit() | vga_mode->vsync_bit();
  }

  // allocate DMA buffer descriptors
  dma_buf_desc_count = 2 * (vga_mode->v_front + vga_mode->v_sync + vga_mode->v_back + vga_mode->v_pixels);
  dma_buf_desc = alloc_dma_buf_desc_array(dma_buf_desc_count);
  //JJ check_alloc(dma_buf_desc, "not enough memory for DMA buffer descriptors");
  check_alloc(dma_buf_desc);
  for (int i = 0; i < dma_buf_desc_count; i++) {
    dma_buf_desc[i].qe.stqe_next = &dma_buf_desc[(i+1)%dma_buf_desc_count];
  }
  dma_buf_desc[dma_buf_desc_count-1].eof = true;  // trigger interrupt at the end of last buffer

  // stitch together the DMA buffers to compose a full VGA frame
  int d = 0;
  for (int i = 0; i < vga_mode->v_front; i++) {
    set_dma_buf_desc_buffer(&dma_buf_desc[d++], dma_buf_hblank_vnorm, hblank_len);
    set_dma_buf_desc_buffer(&dma_buf_desc[d++], dma_buf_vblank_vnorm, vga_mode->h_pixels);
  }
  for (int i = 0; i < vga_mode->v_sync; i++) {
    set_dma_buf_desc_buffer(&dma_buf_desc[d++], dma_buf_hblank_vsync, hblank_len);
    set_dma_buf_desc_buffer(&dma_buf_desc[d++], dma_buf_vblank_vsync, vga_mode->h_pixels);
  }
  for (int i = 0; i < vga_mode->v_back; i++) {
    set_dma_buf_desc_buffer(&dma_buf_desc[d++], dma_buf_hblank_vnorm, hblank_len);
    set_dma_buf_desc_buffer(&dma_buf_desc[d++], dma_buf_vblank_vnorm, vga_mode->h_pixels);
  }
  for (int i = 0; i < vga_mode->v_pixels; i++) {
    set_dma_buf_desc_buffer(&dma_buf_desc[d++], dma_buf_hblank_vnorm, hblank_len);
    set_dma_buf_desc_buffer(&dma_buf_desc[d++], (uint8_t *) 0, vga_mode->h_pixels);  // set later with set_vga_i2s_active_framebuffer()
  }
}

//Set the framebuffer memory as the buffers for the DMA buffer
//descriptors for I2S output.
static void set_vga_i2s_active_framebuffer(unsigned char **fb)
{
  for (int i = 0; i < vga_mode->v_pixels; i++) {
    set_dma_buf_desc_buffer(&dma_buf_desc[2*(vga_mode->v_front+vga_mode->v_sync+vga_mode->v_back+i)+1], fb[i/vga_mode->v_div], vga_mode->h_pixels);
  }
}


//==========================================================================
//===== FRAMEBUFFER ========================================================
//==========================================================================
static unsigned char **alloc_framebuffer()
{
  unsigned char **fb = (unsigned char **) malloc(sizeof(unsigned char *) * vga_mode->y_res());
  //JJ check_alloc(fb, "not enough memory for framebuffer");
  check_alloc(fb);
  for (int i = 0; i < vga_mode->y_res(); i++) {
    fb[i] = (uint8_t *) heap_caps_malloc((vga_mode->h_pixels+3) & 0xfffffffc, MALLOC_CAP_DMA);
    //JJ check_alloc(fb[i], "not enough DMA memory for framebuffer");
    check_alloc(fb[i]);
  }
  return fb;
}

static void clear_framebuffer(unsigned char **fb, uint8_t color)
{
  unsigned int clear_byte = vga_mode->sync_bits() | (color & 0x3f);
  unsigned int clear_data = ((clear_byte << 24) |
                             (clear_byte << 16) |
                             (clear_byte <<  8) |
                             (clear_byte <<  0));
  for (int y = 0; y < vga_mode->y_res(); y++) {
    unsigned int *line = (unsigned int *) fb[y];
    for (int x = 0; x < vga_mode->x_res()/4; x++) {
      line[x] = clear_data;
    }
  }
}


//==========================================================================
//=== I2S OUTPUT ===========================================================
//==========================================================================
static void reset_i2s_txrx()
{
   I2S1.conf.tx_reset = 1;
   I2S1.conf.tx_reset = 0;
   I2S1.conf.rx_reset = 1;
   I2S1.conf.rx_reset = 0;
}

static void reset_i2s_fifo()
{
   I2S1.conf.rx_fifo_reset = 1;
   I2S1.conf.rx_fifo_reset = 0;
   I2S1.conf.tx_fifo_reset = 1;
   I2S1.conf.tx_fifo_reset = 0;
}

static void reset_i2s_dma()
{
   I2S1.lc_conf.in_rst  = 1;
   I2S1.lc_conf.in_rst  = 0;
   I2S1.lc_conf.out_rst = 1;
   I2S1.lc_conf.out_rst = 0;  
}

//Interrupt handler function: just increment the frame count to let
// the main logic to know when it's safe to swap framebuffers.
static void IRAM_ATTR i2s_isr(void *arg)
{
  REG_WRITE(I2S_INT_CLR_REG(1), (REG_READ(I2S_INT_RAW_REG(1)) & 0xffffffc0) | 0x3f); // 1 means I2S1
  vga_frame_count++;
}

//Prepare I2S output to the selected pins (see comments on function vga_init())
//JJ static void setup_i2s_output(const int *pin_map)
static void setup_i2s_output(const unsigned char *pin_map)
{
  periph_module_enable(PERIPH_I2S1_MODULE);
  for (int i = 0; i < 8; i++) {
    int pin = pin_map[i];
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[pin], PIN_FUNC_GPIO);
    gpio_set_direction((gpio_num_t) pin, (gpio_mode_t) GPIO_MODE_DEF_OUTPUT);
    gpio_matrix_out(pin, I2S1O_DATA_OUT0_IDX + i, false, false);
  }
  periph_module_enable(PERIPH_I2S1_MODULE);

  // reset
  reset_i2s_txrx();
  reset_i2s_fifo();
  reset_i2s_dma();

  // set parallel mode
  I2S1.conf2.val = 0;
  I2S1.conf2.lcd_en = 1;
  I2S1.conf2.lcd_tx_wrx2_en = 1;
  I2S1.conf2.lcd_tx_sdx2_en = 0;
  
  // data rate
  I2S1.sample_rate_conf.val = 0;
  I2S1.sample_rate_conf.tx_bits_mod = 8;
  
  // clock setup
  long freq = vga_mode->pixel_clock * 2;
  int sdm, sdmn;
  int odir = -1;
  do {	
    odir++;
    sdm  = long((double(freq) / (20000000. / (odir + 2    ))) * 0x10000) - 0x40000;
    sdmn = long((double(freq) / (20000000. / (odir + 2 + 1))) * 0x10000) - 0x40000;
  } while(sdm < 0x8c0ecL && odir < 31 && sdmn < 0xA1fff);
  if (sdm > 0xA1fff) sdm = 0xA1fff;
  rtc_clk_apll_enable(true, sdm & 0xff, (sdm >> 8) & 0xff, sdm >> 16, odir);

  I2S1.clkm_conf.val = 0;
  I2S1.clkm_conf.clka_en = 1;
  I2S1.clkm_conf.clkm_div_num = 2;
  I2S1.clkm_conf.clkm_div_a = 1;
  I2S1.clkm_conf.clkm_div_b = 0;
  I2S1.sample_rate_conf.tx_bck_div_num = 1;

  I2S1.fifo_conf.val = 0;
  I2S1.fifo_conf.tx_fifo_mod_force_en = 1;
  I2S1.fifo_conf.tx_fifo_mod = 1;
  I2S1.fifo_conf.tx_data_num = 32;
  I2S1.fifo_conf.dscr_en = 1;

  I2S1.conf1.val = 0;
  I2S1.conf1.tx_stop_en = 0;
  I2S1.conf1.tx_pcm_bypass = 1;
  
  I2S1.conf_chan.val = 0;
  I2S1.conf_chan.tx_chan_mod = 1;
  
  I2S1.conf.tx_right_first = 1;

  I2S1.timing.val = 0;
  
  I2S1.conf.tx_msb_right = 0;
  I2S1.conf.tx_msb_shift = 0;
  I2S1.conf.tx_mono = 0;
  I2S1.conf.tx_short_sync = 0;

  i2s_isr_handle = 0;
  esp_intr_alloc(ETS_I2S1_INTR_SOURCE, ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_LEVEL3 | ESP_INTR_FLAG_IRAM, i2s_isr, NULL, &i2s_isr_handle);
}


//Start the I2S output
static void start_i2s_output()
{
  esp_intr_disable(i2s_isr_handle);

  // reset
  I2S1.lc_conf.val |=   I2S_IN_RST_M | I2S_OUT_RST_M | I2S_AHBM_RST_M | I2S_AHBM_FIFO_RST_M;
  I2S1.lc_conf.val &= ~(I2S_IN_RST_M | I2S_OUT_RST_M | I2S_AHBM_RST_M | I2S_AHBM_FIFO_RST_M);
  I2S1.conf.val |=   I2S_RX_RESET_M | I2S_RX_FIFO_RESET_M | I2S_TX_RESET_M | I2S_TX_FIFO_RESET_M;
  I2S1.conf.val &= ~(I2S_RX_RESET_M | I2S_RX_FIFO_RESET_M | I2S_TX_RESET_M | I2S_TX_FIFO_RESET_M);
  while (I2S1.state.rx_fifo_reset_back) {
    // wait
  }

  // setup
  I2S1.lc_conf.val |= I2S_OUT_DATA_BURST_EN | I2S_OUTDSCR_BURST_EN;
  I2S1.out_link.addr = (uint32_t) &dma_buf_desc[0];
  I2S1.out_link.start = 1;
  I2S1.int_clr.val = I2S1.int_raw.val;
  I2S1.int_ena.val = 0;

  // enable interrupt
  I2S1.int_ena.out_eof = 1;
  esp_intr_enable(i2s_isr_handle);

  // start output
  I2S1.conf.tx_start = 1;
}


//==========================================================================
//=== API ==================================================================
//==========================================================================

// Initialize VGA output.
//
// `pin_map` must point to 8 ints selecting the gpio pins, in this order:
//   [0] red   bit 0 (lsb)
//   [1] red   bit 1 (msb)
//   [2] green bit 0 (lsb)
//   [3] green bit 1 (msb)
//   [4] blue  bit 0 (lsb)
//   [5] blue  bit 1 (msb)
//   [6] H-sync
//   [7] V-sync
//JJ void vga_init(const int *pin_map, const VgaMode &mode, bool double_buffered)
void vga_init(const unsigned char *pin_map, const VgaMode &mode, bool double_buffered)
{
  vga_mode = &mode;
  num_framebuffers = (double_buffered) ? 2 : 1;
  for (int i = 0; i < num_framebuffers; i++) {
    framebuffer[i] = alloc_framebuffer();
  }
  active_framebuffer = 0;
  back_framebuffer = (active_framebuffer+1) % num_framebuffers;
  clear_framebuffer(framebuffer[active_framebuffer], 0);
  clear_framebuffer(framebuffer[back_framebuffer], 0);

  allocate_vga_i2s_buffers();
  set_vga_i2s_active_framebuffer(framebuffer[active_framebuffer]);

  setup_i2s_output(pin_map);  
  start_i2s_output();

#if 0
  print("sync_bits=-x%x", vga_mode->sync_bits());
  for (int i = 0; i < dma_buf_desc_count; i++) {
    printf("%4d: %4d 0x%08x { %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x ... }\n",
           i, dma_buf_desc[i].length, (int32_t) dma_buf_desc[i].buf,
           dma_buf_desc[i].buf[ 0], dma_buf_desc[i].buf[ 1], dma_buf_desc[i].buf[ 2], dma_buf_desc[i].buf[ 3],
           dma_buf_desc[i].buf[ 4], dma_buf_desc[i].buf[ 5], dma_buf_desc[i].buf[ 6], dma_buf_desc[i].buf[ 7],
           dma_buf_desc[i].buf[ 8], dma_buf_desc[i].buf[ 9], dma_buf_desc[i].buf[10], dma_buf_desc[i].buf[11]);
  }
#endif
}

//Swap the back (drawing) and active (display) framebuffers.
//If `wait_vsync` is true, wait for a vertical sync before
//swapping. Use false only for measure your drawing code performance,
//since it will result in an annoying horizontal line on the screen.
void vga_swap_buffers(bool wait_vsync)
{
  if (wait_vsync) {
    // we might wait up to 1s/60 = 16.7 milliseconds here
    vga_frame_count = 0;
    while (vga_frame_count == 0) {
      vTaskDelay(1);
    }
  }
  active_framebuffer = back_framebuffer;
  back_framebuffer = (active_framebuffer+1) % num_framebuffers;
  set_vga_i2s_active_framebuffer(framebuffer[active_framebuffer]);
}


//Clear the back (drawing) framebufer to the given color (0b00BBGGRR).
void vga_clear_screen(uint8_t color)
{
  clear_framebuffer(vga_get_framebuffer(), color);
}


//Return the sync bits to use in every framebuffer pixel
uint8_t vga_get_sync_bits()
{
  return vga_mode->sync_bits();
}

uint8_t vga_get_vsync_inv_bit()
{
  return vga_mode->vsync_inv_bit();
}

uint8_t vga_get_hsync_inv_bit(void)
{
 return vga_mode->hsync_inv_bit();
}


//Return the horizontal resolution of the screen (in pixels)
int vga_get_xres()
{
  return vga_mode->x_res();
}


//Return the vertical resolution of the screen (in pixels)
int vga_get_yres()
{
  return vga_mode->y_res();
}

//Return a pointer to the back (drawing) framebuffer lines.
uint8_t **vga_get_framebuffer()
{
  return framebuffer[back_framebuffer];
}

*/

















