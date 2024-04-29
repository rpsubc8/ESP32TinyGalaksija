//================================================================
// Port SDL1 and ESP32 by ackerman
//
// Resolution: 32x16  Font 8x16 Visible 8x13    256x208
// GALAKSIJA emulator
// DOS Verzija: Copyright (C) by Miodrag Jevremovic 1997.
// Win32 Verzija: copyright (C) by Tomaž Šolc 2002 (?).
// Linux (SDL2) Verzija: Copyright (C) by Peter Bakota 2017.
//
// Es Little Endian, poner define LSB_FIRST
// https://www.8bitchip.info/galaksija/galaksija/index1.html
//================================================================
#include "gbConfig.h"
#include "hardware.h"
#include "gbGlobals.h"
#include "PS2Kbd.h"
#include "osd.h"
#include "vga6bit.h"
#include <Arduino.h>
#include "galaksija.h"

#include "gtp.h"
#include "io.h"
#include "rom1bin.h"
#include "rom2bin.h"
#include "font8.h"
#include "gbCompileOpt.h"

#ifdef gb_use_lib_compile_arduinodroid
 #include "gbgtp.h"
#else
 #include "dataFlash/gbgtp.h"
#endif 

unsigned char gb_cont_error_mem=0;

unsigned int gb_ramfree_ini;
unsigned int gb_ramfree_setupEnd;

unsigned char gb_use_keyb_poll_ms= 10; //10 ms poll keyboard
unsigned int gb_keyb_poll_time_cur=0;
unsigned int gb_keyb_poll_time_prev=0;

unsigned char gb_use_vga_poll_ms= 20; //20 ms poll vga
unsigned int gb_vga_poll_time_cur= 0;
unsigned int gb_vga_poll_time_prev= 0;

#ifdef use_lib_vga8colors
 unsigned char gb_vga_8colors=1; //1 8 colores, 0 64 colores
#else
 unsigned char gb_vga_8colors=0;
#endif 
//unsigned char gb_color_bw[2]={0,7};
//unsigned char ESP32_WHITE=7;
//unsigned char ESP32_BLACK=0;

#ifdef use_lib_320x240
 unsigned char gb_vga_display_offset_left=((320-256)/2); //revisar quitar calcula OSDUpdateAnchoAltoVGA
 unsigned short int gb_vga_ancho= 320;
 unsigned short int gb_vga_alto= 240;
 unsigned short int gb_vga_yOffset= 16;
 unsigned char gb_vga_alto_caracter= 13;
#else 
 #ifdef use_lib_320x200
  unsigned char gb_vga_display_offset_left=((320-256)/2);
  unsigned short int gb_vga_ancho= 320;
  unsigned short int gb_vga_alto= 200;  
  unsigned short int gb_vga_yOffset= 2;
  unsigned char gb_vga_alto_caracter= 12;
 #else
  #ifdef use_lib_360x200
   unsigned char gb_vga_display_offset_left=((360-256)/2);
   unsigned short int gb_vga_ancho= 360;
   unsigned short int gb_vga_alto= 200;   
   unsigned short int gb_vga_yOffset= 2;
   unsigned char gb_vga_alto_caracter= 12;
  #endif
 #endif 
#endif

#ifdef use_lib_vga360x200x70hz_bitluni               
 unsigned char gb_vga_videomode_cur= video_mode_360x200x70hz_bitluni; //0
#else
 #ifdef use_lib_vga360x200x70hz_bitluni_apll_fix
  unsigned char gb_vga_videomode_cur= video_mode_360x200x70hz_bitluni_apll_fix; //1
 #else
  #ifdef use_lib_vga320x200x70hz_bitluni
   unsigned char gb_vga_videomode_cur= video_mode_vga320x200x70hz_bitluni; //2
  #else
   #ifdef use_lib_vga320x200x70hz_fabgl
    unsigned char gb_vga_videomode_cur= video_mode_vga320x200x70hz_fabgl; //3
   #else
    #ifdef use_lib_vga320x200x70hz_bitluni_apll_fix
     unsigned char gb_vga_videomode_cur= video_mode_vga320x200x70hz_bitluni_apll_fix; //4
    #else 
     #ifdef use_lib_vga320x240x60hz_bitluni
      unsigned char gb_vga_videomode_cur= video_mode_vga320x240x60hz_bitluni; //5
     #else
      #ifdef use_lib_vga320x240x60hz_fabgl
       unsigned char gb_vga_videomode_cur= video_mode_vga320x240x60hz_fabgl; //6
      #else 
       #ifdef use_lib_vga320x240x60hz_bitluni_apll_fix
        unsigned char gb_vga_videomode_cur= video_mode_vga320x240x60hz_bitluni_apll_fix; //7
       #endif
      #endif
     #endif
    #endif
   #endif
  #endif
 #endif
#endif 

unsigned char gb_vga_videomode_cur_menu=0; //El que seleccionas en menu OSD



unsigned char gb_use_interceptROMTape=1;
#ifdef use_lib_log_serial
 unsigned char gb_use_debug=1;
#else 
 unsigned char gb_use_debug=0;
#endif 

short int gb_osd_ventana_ini=0;
short int gb_osd_ventana_fin=15;

unsigned char gb_do_action_key_f1=0;
unsigned char gb_do_action_key_reset=0;
unsigned char gb_do_action_key_show_osd=0; //Muestra fps

unsigned char gb_setup_end=0;

#ifdef use_lib_360x200
 const unsigned int *gb_ptrVideo_cur= VgaMode_vga_mode_360x200;
#else
 #ifdef use_lib_320x200
  const unsigned int *gb_ptrVideo_cur= VgaMode_vga_mode_320x200;
 #else
  #ifdef use_lib_320x240
   const unsigned int *gb_ptrVideo_cur= VgaMode_vga_mode_320x240;
  #endif
 #endif
#endif

//#ifdef use_lib_vga8colors
// //Uso solo RGB 3 bits DAC
// //static const unsigned char pin_config[] = {
// const unsigned char pin_config[]= {
//  PIN_RED_HIGH,
//  PIN_GREEN_HIGH,  
//  PIN_BLUE_HIGH,
//  255,
//  255,
//  255,
//  PIN_HSYNC,
//  PIN_VSYNC      
// };
//
// unsigned char gb_vga_1bpp=0; //revisar no necesito
// unsigned char gb_vga_text=0;
// //unsigned char gb_vga_videomode= vgaMode_novideo;
// unsigned long precalcula_baseh_bit;
// unsigned long precalcula_base_bit;
// unsigned long precalcula_baseh_bitI;
// unsigned long precalcula_base_bitI;
// unsigned char precalcula_optimiceModehSync2;
// unsigned short int precalcula_optimiceModehRes2;
// unsigned short int precalcula_optimiceSumHfront;
//#endif


 const unsigned char pin_config8[] = {  
  PIN_RED_HIGH,
  PIN_GREEN_HIGH,  
  PIN_BLUE_HIGH,
  255,
  255,
  255,
  PIN_HSYNC,
  PIN_VSYNC
 };  
 const unsigned char pin_config64[] = {  
  PIN_RED_LOW,
  PIN_RED_HIGH,
  PIN_GREEN_LOW,
  PIN_GREEN_HIGH,
  PIN_BLUE_LOW,
  PIN_BLUE_HIGH,
  PIN_HSYNC,
  PIN_VSYNC
 };


unsigned char gb_const_colorNormal[64]; //8 y 64 colores
unsigned char gb_color_bw[2];

unsigned char gb_sync_bits;
unsigned char **gb_buffer_vga;
unsigned int **gb_buffer_vga32;



unsigned char gb_show_osd_main_menu=0;
unsigned char gb_language_en=1;
unsigned char gb_id_menu_cur=0;

unsigned char gb_gtp_id_sel_cur=0;
unsigned char gb_gtp_warmstart_cur=0;
unsigned char gb_gal_id_sel_cur=0;

unsigned int gb_fps_cur=0;
unsigned int gb_video_fps_cur=0;
unsigned int gb_fps_last=0;
unsigned int gb_fps_min=999999;
unsigned int gb_fps_max=0;
unsigned int gb_fps_med=0;
unsigned int gb_fps_time_cur=0;
unsigned int gb_fps_time_last=0;

unsigned int gb_cpu_time_cur=0;
unsigned int gb_cpu_time_last=0;
unsigned int gb_cpu_time_min=999999;
unsigned int gb_cpu_time_med=0;
unsigned int gb_cpu_time_max=0;

unsigned int gb_vga_time_cur=0;
unsigned int gb_vga_time_last=0; 

unsigned int gb_intercept_tape_cur=0;
unsigned int gb_intercept_tape_last=0;


unsigned char gb_cpu_delay=1;
unsigned int gb_vga_dump_time_dur=0;
//unsigned char gb_vga_display_offset_left=((320-256)/2);

unsigned int crna_color=0;
unsigned int pixel_color=0;
char workdir[256] = "";


Z80 R;
#ifdef use_lib_ram_8KB
 unsigned char MEMORY[8192]; //8KB RAM(solo necesito 6KB)
#else
 unsigned char MEMORY[65536]; // max 64k of MEMORY
#endif 
unsigned char Fassst;
unsigned char ExitLoop=0;

unsigned int last_time = 0;
//float sekunda = 0;
//float render_time = 0; //No lo necesito
//unsigned char active_help = 0;
//char *prg_to_load = NULL;
unsigned short int prg_to_jump = 0;
int cpu_speed = CPU_SPEED_NORMAL;



//ya viene en el gbConfig.h el little endian
//#define LSB_FIRST
typedef union
{
 #ifdef LSB_FIRST
   struct { unsigned char l,h,h2,h3; } B; //byte
   struct { unsigned short int l,h; } W; //word
   unsigned int D;
 #else
   struct { unsigned char h3,h2,h,l; } B;
   struct { unsigned short int h,l; } W;
   unsigned int D;
 #endif
} pairDOS;


typedef struct
{
  pairDOS AF,BC,DE,HL,IX,IY,PC,SP;
  pairDOS AF2,BC2,DE2,HL2;
  unsigned int IFF1,IFF2,HALT,IM,I,R,R2; //unsigned IFF1,IFF2,HALT,IM,I,R,R2;
} Z80_RegsDOS;

Z80_RegsDOS DOS_R;


/*revisar
unsigned int Kmap[54] =
{
 0,
 SDLK_a, SDLK_b, SDLK_c, SDLK_d, SDLK_e, SDLK_f,
 SDLK_g, SDLK_h, SDLK_i, SDLK_j, SDLK_k, SDLK_l,
 SDLK_m, SDLK_n, SDLK_o, SDLK_p, SDLK_q, SDLK_r,
 SDLK_s, SDLK_t, SDLK_u, SDLK_v, SDLK_w, SDLK_x,
 SDLK_y, SDLK_z,
 SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT,
 SDLK_SPACE,
 SDLK_0, SDLK_1, SDLK_2, SDLK_3, SDLK_4,
 SDLK_5, SDLK_6, SDLK_7, SDLK_8, SDLK_9,
 SDLK_SEMICOLON, SDLK_QUOTEDBL, SDLK_COMMA, SDLK_EQUALS,
 SDLK_PERIOD, SDLK_SLASH, SDLK_RETURN, SDLK_TAB, SDLK_LCTRL,
 SDLK_DELETE, SDLK_SCROLLOCK, SDLK_LSHIFT
};
*/

unsigned char gb_Kmap[54];


#ifdef use_lib_keyboard_uart
 unsigned int gb_curTime_keyboard_before_uart;
 unsigned int gb_curTime_keyboard_uart;
 void do_keyboard_uart(void);
#endif 


//BEGIN encabezados
void bitluni_SDL_FillRect(unsigned char auxC);
//END encabezados






#ifdef use_lib_keyboard_uart
 char gb_buf_uart[BUFFER_SIZE_UART];
 unsigned char gb_rlen_uart=0;

  //array de 54 teclas
  //0,
  //SDLK_a, SDLK_b, SDLK_c, SDLK_d, SDLK_e, SDLK_f,
  //SDLK_g, SDLK_h, SDLK_i, SDLK_j, SDLK_k, SDLK_l,
  //SDLK_m, SDLK_n, SDLK_o, SDLK_p, SDLK_q, SDLK_r,
  //SDLK_s, SDLK_t, SDLK_u, SDLK_v, SDLK_w, SDLK_x,
  //SDLK_y, SDLK_z,
  //SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT,
  //SDLK_SPACE,
  //SDLK_0, SDLK_1, SDLK_2, SDLK_3, SDLK_4,
  //SDLK_5, SDLK_6, SDLK_7, SDLK_8, SDLK_9,
  //SDLK_SEMICOLON, SDLK_QUOTEDBL, SDLK_COMMA, SDLK_EQUALS,
  //SDLK_PERIOD, SDLK_SLASH, SDLK_RETURN, SDLK_TAB, SDLK_LCTRL,
  //SDLK_DELETE, SDLK_SCROLLOCK, SDLK_LSHIFT

 void do_keyboard_uart()
 {
  unsigned int contBuf=0;
  unsigned short int auxAddr;
  
  unsigned char aux_Kmap[54];
  memset(aux_Kmap,0xFF,sizeof(aux_Kmap));

  if (gb_rlen_uart>0)
  {
   while (contBuf < gb_rlen_uart)
   {
    switch (gb_buf_uart[contBuf])
    {
     case 0x01: //F2   01 62 4F 51      
      if ((contBuf+3) < gb_rlen_uart)
      {       
       gb_do_action_key_f1= 1;
       contBuf+= 3;
      }
      break;

     case 0x09: //TAB saco menu tambien
      gb_do_action_key_f1= 1;
      break;      

     case 0x08: case 0x7F: //Borrar
      aux_Kmap[29]= 0;
      break;

     case 0x0D: case 0x0A: //0D 0A ENTER 
      //keyboard[6]&= 0xFE; //ENTER
      aux_Kmap[48]= 0; //SDLK_RETURN id en kmap 48
      break;

     case 0x1B: //Arriba 1B 5B 41
      if ((contBuf+2) < gb_rlen_uart)
      {
       contBuf++;
       if (gb_buf_uart[contBuf] == 0x5B)
       {
        contBuf++;
        switch (gb_buf_uart[contBuf])
        {
         case 0x41: aux_Kmap[27]= 0; break; //arriba 1B 5B 41
         case 0x42: aux_Kmap[28]= 0; break; //abajo 1B 5B 42
         case 0x44: aux_Kmap[29]= 0; break; //izquierda 1B 5B 44
         case 0x43: aux_Kmap[30]= 0; break; //derecha 1B 5B 43
         default: break;
        }
       }
      }
      break;      

     case 0x2D: aux_Kmap[53]= 0; break; //SHIFT LEFT -
     case 0x2E: aux_Kmap[46]= 0; break; //.

     case 0x61: case 0x41: aux_Kmap[1]= 0; gb_Kmap[1]= 0; break; //A
     case 0x62: case 0x42: aux_Kmap[2]= 0; gb_Kmap[2]= 0; break; //B
     case 0x63: case 0x43: aux_Kmap[3]= 0; gb_Kmap[3]= 0; break; //C
     case 0x64: case 0x44: aux_Kmap[4]= 0; gb_Kmap[4]= 0; break; //D
     case 0x65: case 0x45: aux_Kmap[5]= 0; gb_Kmap[5]= 0; break; //E
     case 0x66: case 0x46: aux_Kmap[6]= 0; gb_Kmap[6]= 0; break; //F
     case 0x67: case 0x47: aux_Kmap[7]= 0; break; //G
     case 0x68: case 0x48: aux_Kmap[8]= 0; break; //H
     case 0x69: case 0x49: aux_Kmap[9]= 0; break; //I
     case 0x6A: case 0x4A: aux_Kmap[10]= 0; break; //J
     case 0x6B: case 0x4B: aux_Kmap[11]= 0; break; //K
     case 0x6C: case 0x4C: aux_Kmap[12]= 0; break; //L
     case 0x6D: case 0x4D: aux_Kmap[13]= 0; break; //M
     case 0x6E: case 0x4E: aux_Kmap[14]= 0; break; //N
     case 0x6F: case 0x4F: aux_Kmap[15]= 0; break; //O
     case 0x70: case 0x50: aux_Kmap[16]= 0; break; //P
     case 0x71: case 0x51: aux_Kmap[17]= 0; break; //Q
     case 0x72: case 0x52: aux_Kmap[18]= 0; break; //R
     case 0x73: case 0x53: aux_Kmap[19]= 0; break; //S
     case 0x74: case 0x54: aux_Kmap[20]= 0; break; //T
     case 0x75: case 0x55: aux_Kmap[21]= 0; break; //U
     case 0x76: case 0x56: aux_Kmap[22]= 0; break; //V
     case 0x77: case 0x57: aux_Kmap[23]= 0; break; //W
     case 0x78: case 0x58: aux_Kmap[24]= 0; break; //X
     case 0x79: case 0x59: aux_Kmap[25]= 0; break; //Y
     case 0x7A: case 0x5A: aux_Kmap[26]= 0; break; //Z
     
     case 0x20: aux_Kmap[31]= 0; break; //SPACE

     case 0x30: aux_Kmap[32]= 0; gb_Kmap[32]=0; break; //0
     case 0x31: aux_Kmap[33]= 0; gb_Kmap[33]=0; break; //1
     case 0x32: aux_Kmap[34]= 0; gb_Kmap[34]=0; break; //2
     case 0x33: aux_Kmap[35]= 0; gb_Kmap[35]=0; break; //3
     case 0x34: aux_Kmap[36]= 0; gb_Kmap[36]=0; break; //4
     case 0x35: aux_Kmap[37]= 0; gb_Kmap[37]=0; break; //5               
     case 0x36: aux_Kmap[38]= 0; gb_Kmap[38]=0; break; //6
     case 0x37: aux_Kmap[39]= 0; gb_Kmap[39]=0; break; //7     
     case 0x38: aux_Kmap[40]= 0; gb_Kmap[40]=0; break; //8
     case 0x39: aux_Kmap[41]= 0; gb_Kmap[41]=0; break; //9

     default: break;
    }//fin switch gb_buf_uart
    contBuf++;
   }//fin while contBuf   
  }//fin if gb_rlen_uart
    
  #ifdef use_lib_ram_8KB
   auxAddr= 0x2000-8192;
  #else
   auxAddr= 0x2000;
  #endif

  for (unsigned char i=0;i<54;i++)
  {
   //MEMORY[(auxAddr + i)]= (aux_Kmap[i]==0) ? 0xFE: 0xFF;   
   if (aux_Kmap[i]==0)
   {
    MEMORY[(auxAddr + i)]= 0xFE;
   }
  }
 }
#endif


//void bitluni_dump_vga()
//{
// //revisar quitar
//}

//***************************************************
void bitluni_SDL_FillRect(unsigned char auxC)
{
 unsigned int a0,aux;
 unsigned short int topeAncho= (gb_vga_ancho-16)>>2; //Quitamos 16 pixels OSD estadisticas 2 filas
 a0= gb_const_colorNormal[auxC];
 aux= (a0<<24)|(a0<<16)|(a0<<8)|a0;

 //for (unsigned short int y=0;y<200;y++)
 for (unsigned short int y=0;y<gb_vga_alto;y++)
 {  
  //for (int x=0;x<256;x++)
  //for (unsigned short int x=0;x<64;x++)
  for (unsigned short int x=0;x<topeAncho;x++)
  {  
   gb_buffer_vga32[y][x]= aux;
  }  
 }     
}

//*******************************
void PrepareColorsBitluniVGA()
{  
 //(color & RGBAXMask) | SBits;
 if (gb_vga_8colors == 1)
 {//8 colores
  if (gb_use_debug==1){ Serial.printf("PrepareColorsBitluniVGA 8 colores\r\n"); }
  for (unsigned char i=0;i<8;i++)
  {
   gb_const_colorNormal[i]= (i & 0x07) | gb_sync_bits;
  }

  gb_color_bw[0]= gb_const_colorNormal[0];
  gb_color_bw[1]= gb_const_colorNormal[7];
  ESP32_WHITE=7;
  ESP32_BLACK=0;  
 }
 else
 {//64 colores
  if (gb_use_debug==1){ Serial.printf("PrepareColorsBitluniVGA 64 colores\r\n"); }
  for (unsigned char i=0;i<64;i++)
  {
   gb_const_colorNormal[i]= (i & 0x3F) | gb_sync_bits;
  }

  gb_color_bw[0]= gb_const_colorNormal[0];
  gb_color_bw[1]= gb_const_colorNormal[63];
  ESP32_WHITE=63;
  ESP32_BLACK=0;  
 }
}




#ifdef use_lib_rom_flash
 unsigned char RdZ80(unsigned short int addr)
 {
  //rom1    0 - 4096  gb_rom1_bin (4096 bytes)
  //rom2 4096 - 8192  gb_rom2_bin (4096 bytes)          
  if (addr<4096)
  {
   return gb_rom1_bin[addr];
  }
  else
  {
   if (addr<8192)
   {
    return gb_rom2_bin[(addr-4096)];
   }
   else
   {
    if (addr<=0x4000)
    {
     #ifdef use_lib_ram_8KB
      unsigned short int auxAddr= addr-8192;
      return MEMORY[auxAddr & 0x1FFF]; //Posible and 0x1FFF
     #else
      return MEMORY[addr & 0xffff];
     #endif 
    }
    else
    {
     if (gb_use_debug==1){ Serial.printf("ERROR!! mem %04X\r\n",addr); }
     gb_cont_error_mem++;
     if (gb_cont_error_mem>10)
     {
      gb_cont_error_mem=0;

      gb_do_action_key_reset= 1;
      if (gb_use_debug==1){ Serial.printf("Force reset cont error mem\r\n"); }
     }
     //fflush(stdout);
     return 0xFF;
    }
   }
  }
 }
#else
 unsigned char RdZ80(unsigned short int addr)
 {//13819 por frame
  //gb_cont_call++;
  #ifdef use_lib_ram_8KB
   unsigned short int auxAddr= addr-8192;
   return MEMORY[auxAddr & & 0x1FFF]; //Posible and 0x1FFF
  #else
   return MEMORY[addr & 0xffff];
  #endif 
 }
#endif 

//JJ inline void WrZ80(word addr, byte val)
void WrZ80(unsigned short int addr, unsigned char val)
{//396 por frame
 //gb_cont_call++;
 //Va de 0x2800 a 0x4000    10240..16384= 6134 bytes  &KB
 if (addr >= 0x2800 && addr < KRAJ_RAMA)
 {
  #ifdef use_lib_ram_8KB
   unsigned short int auxAddr= (addr-8192) & 0x1FFF; //Posible and 0x1FFF
   MEMORY[auxAddr]= val;
  #else 
   MEMORY[addr] = val;
  #endif 
 }
}

//JJ inline byte InZ80(word port)
unsigned char InZ80(unsigned short int port)
{//0 por frame
    //gb_cont_call++;
    
    // UNUSED
    return 0xff;
}

//JJ inline void OutZ80(word port, byte val)
void OutZ80(unsigned short int port, unsigned char val)
{//0 por frame
    //gb_cont_call++;
    
    // UNUSED
}

//inline void PatchZ80(Z80 *R)
void PatchZ80(Z80 *R)
{
    // UNUSED
}


//=============
//           //
// TASTATURA //
//           //
//=============
//#ifdef use_lib_keyboard_uart
// void read_keyboard_uart()
// {
  //array de 54 teclas
  //0,
  //SDLK_a, SDLK_b, SDLK_c, SDLK_d, SDLK_e, SDLK_f,
  //SDLK_g, SDLK_h, SDLK_i, SDLK_j, SDLK_k, SDLK_l,
  //SDLK_m, SDLK_n, SDLK_o, SDLK_p, SDLK_q, SDLK_r,
  //SDLK_s, SDLK_t, SDLK_u, SDLK_v, SDLK_w, SDLK_x,
  //SDLK_y, SDLK_z,
  //SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT,
  //SDLK_SPACE,
  //SDLK_0, SDLK_1, SDLK_2, SDLK_3, SDLK_4,
  //SDLK_5, SDLK_6, SDLK_7, SDLK_8, SDLK_9,
  //SDLK_SEMICOLON, SDLK_QUOTEDBL, SDLK_COMMA, SDLK_EQUALS,
  //SDLK_PERIOD, SDLK_SLASH, SDLK_RETURN, SDLK_TAB, SDLK_LCTRL,
  //SDLK_DELETE, SDLK_SCROLLOCK, SDLK_LSHIFT
// }
//#endif
// Skanira tastaturu, i GALAKSIJI servira ono sto ona ocekuje.
unsigned char gb_cont_vkeys=0;
unsigned char gb_cont_pul_vkeys=0;
unsigned char gb_send_vkeys=0;
unsigned char salta=0;

void send_virtual_keys()
{
 unsigned char teclas[4]={18,21,14,48}; //RUN ENTER
 //18 R
 //21 U
 //14 N
 //48 ENTER
 salta++;
 if (salta<16)
 {
  return;
 }
 
 salta=0;

 if (gb_send_vkeys==1)
 {
  unsigned char id=teclas[gb_cont_vkeys]; //'R'
  unsigned char pulsar= (gb_cont_pul_vkeys==0)?0xFE: 0xFF;
  #ifdef use_lib_ram_8KB
   MEMORY[0x2000 + id -8192] = pulsar;
  #else
   MEMORY[0x2000 + id] = pulsar;
  #endif     
 
  gb_cont_pul_vkeys++;
  gb_cont_pul_vkeys= gb_cont_pul_vkeys&0x01;
 
  if (pulsar==0xFF)
  {
   gb_cont_vkeys++;
   if (gb_cont_vkeys>3)
   {
    gb_cont_vkeys=0;
    gb_send_vkeys=0;
   }
  }
 }  
}

//**********************************************************
void read_keyboard()
{//1 por frame
 //array de 54 teclas
 //0,
 //SDLK_a, SDLK_b, SDLK_c, SDLK_d, SDLK_e, SDLK_f,
 //SDLK_g, SDLK_h, SDLK_i, SDLK_j, SDLK_k, SDLK_l,
 //SDLK_m, SDLK_n, SDLK_o, SDLK_p, SDLK_q, SDLK_r,
 //SDLK_s, SDLK_t, SDLK_u, SDLK_v, SDLK_w, SDLK_x,
 //SDLK_y, SDLK_z,
 //SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT,
 //SDLK_SPACE,
 //SDLK_0, SDLK_1, SDLK_2, SDLK_3, SDLK_4,
 //SDLK_5, SDLK_6, SDLK_7, SDLK_8, SDLK_9,
 //SDLK_SEMICOLON, SDLK_QUOTEDBL, SDLK_COMMA, SDLK_EQUALS,
 //SDLK_PERIOD, SDLK_SLASH, SDLK_RETURN, SDLK_TAB, SDLK_LCTRL,
 //SDLK_DELETE, SDLK_SCROLLOCK, SDLK_LSHIFT

 //MEMORY[0x2000 + 2 -8192]=  0xFE;

/*revisar
    //gb_cont_call++;
     
    byte A;

    //JJ const byte *key = SDL_GetKeyboardState(NULL);
    //SDL_PollEvent (&gb_event); //falta ponerlo 
    Uint8 *key = SDL_GetKeyState(NULL);  //SDL2 a SDL1

    for (A = 1; A < 54; A++)
    {
        // Par modifikacija, radi udobnosti.
        // Shift je Shift, pa bio on levi ili desni. I tome slicno.

       switch (Kmap[A])
       {
        //JJ case SDL_SCANCODE_LEFT: //SDL2 a SDL1
        case SDLK_LEFT:
            #ifdef use_lib_ram_8KB
             MEMORY[0x2000 + A -8192] = (key[Kmap[A]] || key[SDLK_BACKSPACE]) ? 0xFE : 0xFF; //SDL2 a SDL1
            #else
             MEMORY[0x2000 + A] = (key[Kmap[A]] || key[SDLK_BACKSPACE]) ? 0xFE : 0xFF; //SDL2 a SDL1
            #endif 
            break;

        //JJ case SDL_SCANCODE_LSHIFT: //SDL2 a SDL1
         case SDLK_LSHIFT:
            #ifdef use_lib_ram_8KB
             MEMORY[0x2000 + A -8192] = (key[Kmap[A]] || key[SDLK_RSHIFT]) ? 0xFE : 0xFF; //SDL2 a SDL1
            #else
             MEMORY[0x2000 + A] = (key[Kmap[A]] || key[SDLK_RSHIFT]) ? 0xFE : 0xFF; //SDL2 a SDL1
            #endif 
            break;

        default:
            #ifdef use_lib_ram_8KB
             MEMORY[0x2000 + A -8192] = key[Kmap[A]] ? 0xFE : 0xFF;
            #else
             MEMORY[0x2000 + A] = key[Kmap[A]] ? 0xFE : 0xFF;
            #endif 
       }
    }
    */
  
  unsigned short int auxAddr;
  #ifdef use_lib_ram_8KB
   auxAddr= 0x2000-8192;
  #else
   auxAddr= 0x2000;
  #endif 

  //revisar
  //for (unsigned char i=0;i<54;i++)
  //{
  // MEMORY[auxAddr+i]= 0xFF;
  //}

  unsigned char aux_Kmap[54];
  memset(aux_Kmap,0xFF,sizeof(aux_Kmap));

  

  aux_Kmap[1]= gb_Kmap[1]= (~(checkKey(PS2_KC_A)))&0x01; //A
  aux_Kmap[2]= gb_Kmap[2]= (~(checkKey(PS2_KC_B)))&0x01; //B
  aux_Kmap[3]= gb_Kmap[3]= (~(checkKey(PS2_KC_C)))&0x01; //C
  aux_Kmap[4]= gb_Kmap[4]= (~(checkKey(PS2_KC_D)))&0x01; //D
  aux_Kmap[5]= gb_Kmap[5]= (~(checkKey(PS2_KC_E)))&0x01; //E
  aux_Kmap[6]= gb_Kmap[6]= (~(checkKey(PS2_KC_F)))&0x01; //F

  aux_Kmap[7]= (~(checkKey(PS2_KC_G)))&0x01; //G
  aux_Kmap[8]= (~(checkKey(PS2_KC_H)))&0x01; //H
  aux_Kmap[9]= (~(checkKey(PS2_KC_I)))&0x01; //I
  aux_Kmap[10]= (~(checkKey(PS2_KC_J)))&0x01; //J  
  aux_Kmap[11]= (~(checkKey(PS2_KC_K)))&0x01; //K
  aux_Kmap[12]= (~(checkKey(PS2_KC_L)))&0x01; //L

  aux_Kmap[13]= (~(checkKey(PS2_KC_M)))&0x01; //M
  aux_Kmap[14]= (~(checkKey(PS2_KC_N)))&0x01; //N
  aux_Kmap[15]= (~(checkKey(PS2_KC_O)))&0x01; //O
  aux_Kmap[16]= (~(checkKey(PS2_KC_P)))&0x01; //P
  aux_Kmap[17]= (~(checkKey(PS2_KC_Q)))&0x01; //Q
  aux_Kmap[18]= (~(checkKey(PS2_KC_R)))&0x01; //R

  aux_Kmap[19]= (~(checkKey(PS2_KC_S)))&0x01; //S
  aux_Kmap[20]= (~(checkKey(PS2_KC_T)))&0x01; //T
  aux_Kmap[21]= (~(checkKey(PS2_KC_U)))&0x01; //U
  aux_Kmap[22]= (~(checkKey(PS2_KC_V)))&0x01; //V
  aux_Kmap[23]= (~(checkKey(PS2_KC_W)))&0x01; //W
  aux_Kmap[24]= (~(checkKey(PS2_KC_X)))&0x01; //X

  aux_Kmap[25]= (~(checkKey(PS2_KC_Y)))&0x01; //Y
  aux_Kmap[26]= (~(checkKey(PS2_KC_Z)))&0x01; //Z

  aux_Kmap[27]= (~(checkKey(KEY_CURSOR_UP)))&0x01; //arriba
  aux_Kmap[28]= (~(checkKey(KEY_CURSOR_DOWN)))&0x01; //abajo
  aux_Kmap[29]= (~(checkKey(KEY_CURSOR_LEFT)))&0x01; //izquierda
  aux_Kmap[30]= (~(checkKey(KEY_CURSOR_RIGHT)))&0x01; //derecha  

  if ((checkKey(KEY_BACKSPACE)==1) || ((checkKey(KEY_DELETE)==1)))
  { //Borrar delete
   aux_Kmap[29]= 0;
  }

  
     
  aux_Kmap[31]= (~(checkKey(PS2_KC_SPACE)))&0x01; //SPACE

  aux_Kmap[32]= gb_Kmap[32]= (~(checkKey(PS2_KC_0)))&0x01; //0
  aux_Kmap[33]= gb_Kmap[33]= (~(checkKey(PS2_KC_1)))&0x01; //1
  aux_Kmap[34]= gb_Kmap[34]= (~(checkKey(PS2_KC_2)))&0x01; //2
  aux_Kmap[35]= gb_Kmap[35]= (~(checkKey(PS2_KC_3)))&0x01; //3
  aux_Kmap[36]= gb_Kmap[36]= (~(checkKey(PS2_KC_4)))&0x01; //4

  aux_Kmap[37]= gb_Kmap[37]= (~(checkKey(PS2_KC_5)))&0x01; //5
  aux_Kmap[38]= gb_Kmap[38]= (~(checkKey(PS2_KC_6)))&0x01; //6
  aux_Kmap[39]= gb_Kmap[39]= (~(checkKey(PS2_KC_7)))&0x01; //7
  aux_Kmap[40]= gb_Kmap[40]= (~(checkKey(PS2_KC_8)))&0x01; //8
  aux_Kmap[41]= gb_Kmap[41]= (~(checkKey(PS2_KC_9)))&0x01; //9

  aux_Kmap[42]= (~(checkKey(PS2_KC_SEMI)))&0x01; //;  SDLK_SEMICOLON
  aux_Kmap[43]= (~(checkKey(PS2_KC_APOS)))&0x01; //"  SDLK_QUOTEDBL '
  aux_Kmap[44]= (~(checkKey(PS2_KC_COMMA)))&0x01; //,  SDLK_COMMA
  aux_Kmap[45]= (~(checkKey(PS2_KC_EQUAL)))&0x01; //= SDLK_EQUALS

  aux_Kmap[46]= (~(checkKey(PS2_KC_DOT)))&0x01; //. SDLK_PERIOD
  aux_Kmap[47]= (~(checkKey(PS2_KEY_KP_DIV)))&0x01; // barra SDLK_SLASH
  
  aux_Kmap[48]= (~(checkKey(PS2_KC_ENTER)))&0x01; //RETURN
  aux_Kmap[49]= (~(checkKey(PS2_KC_TAB)))&0x01; //SDLK_TAB
  aux_Kmap[50]= (~(checkKey(PS2_KC_CTRL)))&0x01; //LEFT CONTROL  SDLK_LCTRL
    
  //aux_Kmap[51]= (~(checkKey(KEY_DELETE)))&0x01; //SDLK_DELETE borrar backspace
  //if (aux_Kmap[51]==0)
  //{
  // Serial.printf("del\r\n");
  //}
  aux_Kmap[52]= (~(checkKey(PS2_KC_SCROLL)))&0x01; //SDLK_SCROLLOCK 
  if ((checkKey(PS2_KC_L_SHIFT)==1) || ((checkKey(PS2_KC_R_SHIFT)==1)))
  {
   aux_Kmap[53]= 0;
  }
  //aux_Kmap[53]= (~(checkKey(PS2_KC_L_SHIFT)))&0x01; //SDLK_LSHIFT

  
  gb_do_action_key_f1= checkKey(PS2_KC_F1); //Sacar OSD




  for (unsigned char i=0;i<54;i++)
  {
   MEMORY[auxAddr+i]= aux_Kmap[i];
  }
}

//**************************************************************
void interceptarROMTape()
{//Escribe OLD en BASIC   Hack cutre pendiente de revisar
 unsigned short int auxPC= R.PC.W;
 if (auxPC==0x0EE2)
 {
  //Load GTP
  if (gb_use_debug==1){ Serial.printf("Intercept tape ROM\r\n"); }
  //fflush(stdout);
  LoadGTPFlash(gb_list_gtp_data[0], gb_list_gtp_size[0]);
  R.PC.W = 0x0317; // BASIC WARM START
 }     
}

//**************************************************************
void refresh_screen()
{          
 unsigned short int xDest,yDest;
 unsigned int iniTime= micros();

  //printf("refresh_screen\n");
  //fflush(stdout);
  
 unsigned char x, y; //byte x, y;
 unsigned char mem2BA8;
 //unsigned short int adresa; //word adresa;

 #ifdef use_lib_ram_8KB
  unsigned short int adresa= 0x2800-8192;
  mem2BA8= MEMORY[0x2BA8-8192]; //12 es el valor normal. Pero sale 11. Depende ROM
 #else
  unsigned short int adresa= 0x2800;
  mem2BA8= MEMORY[0x2BA8]; //12 es el valor normal. Pero sale 11. Depende ROM
 #endif 

 short int iniLeft= ((short int)mem2BA8-11);

    // 0x2800 - 0x2a00 (size=512) VIDEO MEMORIJA
//    adresa = 0x2800;

//JJ    SDL_FillRect(screen, NULL, crna_color); //No lo necesito
//JJ    SDL_LockSurface(screen); //No lo necesito
 
 yDest= gb_vga_yOffset; //yDest=0;
 for (y = 0; y < 16; y++)
 {
  xDest= gb_vga_display_offset_left;

     if (iniLeft<0)
     {//Nos vamos a la izquierda
      adresa+= -iniLeft;
      for (x = (-iniLeft); x <32 ; x++)
      {
       draw_char(MEMORY[adresa], xDest, yDest);
       adresa++;
       xDest+= 8;
      }
      
      for (x = 0; x < (-iniLeft); x++)
      {
       draw_char(32, xDest, yDest);
       xDest+= 8;
      }           
     }
     else
     {//Nos vamos a la derecha     
      for (x = 0; x < iniLeft; x++)
      {
       draw_char(32, xDest, yDest);
       xDest+= 8;          
      }
      for (x = iniLeft; x < 32; x++)
      {
       draw_char(MEMORY[adresa], xDest, yDest);
       xDest+= 8;
       adresa++;
      }
      adresa+=iniLeft;      
     }

//  for (x = 0; x < 32; x++)
//  {   
//   draw_char(MEMORY[adresa], xDest, yDest);
//   adresa++;
//   xDest+= 8;

   //#ifdef use_lib_ram_8KB
   // unsigned short int auxAdresa= adresa-8192;
   // draw_char(MEMORY[auxAdresa], x * SIRINA, y * VISINA);
   // adresa++;
   //#else
   // draw_char(MEMORY[adresa++], x * SIRINA, y * VISINA);
   //#endif 
//  }
  yDest+= gb_vga_alto_caracter; //yDest+= 13;
 }

//JJ     SDL_UnlockSurface(screen); //No lo necesito
    
    
    //jj_fast_putpixel(1,1,3);
    
    //gb_fps_cur++;
    gb_video_fps_cur++;
    
    /*revisar
    unsigned int gb_vga_time_cur= millis(); //SDL_GetTicks();
    unsigned int auxTime= gb_vga_time_cur - gb_vga_time_last;
    unsigned int iniTime= gb_vga_time_cur;
    if (auxTime>=19)
    {
     gb_vga_time_last= gb_vga_time_cur;
     bitluni_dump_vga();
    }
    unsigned int finTime= millis(); //SDL_GetTicks();
    gb_vga_dump_time_dur= finTime-iniTime;
    */
    
    //SDL_Flip(screen);
  
//  printf("refresh_screen\n");
//  fflush(stdout);    

 unsigned int finTime= micros(); //SDL_GetTicks();
 gb_vga_dump_time_dur= finTime-iniTime;
}


//*****************************************************************
//void load_prg(char *prg)
//{
// if (gb_use_debug==1){ Serial.printf("load_prg: %s\r\n",prg); }
// //fflush(stdout);
//    if (load_gtp_file(prg, MEMORY, 0) == 0)
//    {
//        R.PC.W = prg_to_jump ? prg_to_jump : 0x0317; // 0x0317 = BASIC WARM START
//    }
//}

//*************************************************************
void LoadGALFlash(const unsigned char *ptrData)
{
 if (gb_use_debug==1){ Serial.printf("OSDLoadGALFlash BEGIN\r\n"); }
 //fflush(stdout);
 
 memset((void *)MEMORY, 0, sizeof(MEMORY));

 unsigned char *ptrDOS_R= (unsigned char *)&DOS_R;
 memcpy(ptrDOS_R, ptrData, sizeof(Z80_RegsDOS));

 ptrData+= sizeof(Z80_RegsDOS);
 #ifdef use_lib_ram_8KB
  memcpy(&MEMORY[0x2000-8192], ptrData, (WORK_SPACE-0x2000));
 #else
  memcpy(&MEMORY[0x2000], ptrData, (WORK_SPACE-0x2000));
 #endif
 
 R.AF.W = DOS_R.AF.W.l;
 R.BC.W = DOS_R.BC.W.l;
 R.DE.W = DOS_R.DE.W.l;
 R.HL.W = DOS_R.HL.W.l;
 R.IX.W = DOS_R.IX.W.l;
 R.IY.W = DOS_R.IY.W.l;
 R.PC.W = DOS_R.PC.W.l;
 R.SP.W = DOS_R.SP.W.l;
 R.AF1.W = DOS_R.AF2.W.l;
 R.BC1.W = DOS_R.BC2.W.l;
 R.DE1.W = DOS_R.DE2.W.l;
 R.HL1.W = DOS_R.HL2.W.l;
 R.IFF = 0;
 if (DOS_R.IM == 1)
 {
  R.IFF+=2;
 }
 if (DOS_R.IM == 2)
 {
  R.IFF+=4;
 }
 if (DOS_R.IFF1)
 {
  R.IFF+=1;
 }
 R.I = DOS_R.I;
 R.R = (DOS_R.R&127)|(DOS_R.R2&128); 
 
 
 gb_id_menu_cur= id_menu_main;
 
 if (gb_use_debug==1){ Serial.printf("OSDLoadGALFlash END\r\n"); }
 //fflush(stdout); 
}


//*************************************
/*
void LeerGAL()
{//Si tiene tamanio 8268 bytes
 FILE *f;
 
 printf("LeerGAL\n");
 //fflush(stdout);
 
 //f= fopen("P.GAL","rb");
 f= fopen("PROBA.GAL","rb");
 if (f!=NULL)
 {
  printf("Abierto P.GAL\n");
  //fflush(stdout);             
  
  fread (&DOS_R, 1, sizeof(Z80_RegsDOS), f);
  #ifdef use_lib_ram_8KB
   fread (&MEMORY[0x2000-8192], 1, WORK_SPACE-0x2000, f);
  #else
   fread (&MEMORY[0x2000], 1, WORK_SPACE-0x2000, f);
  #endif 
  //Convert registers Now
  
						R.AF.W = DOS_R.AF.W.l;
						R.BC.W = DOS_R.BC.W.l;
						R.DE.W = DOS_R.DE.W.l;
						R.HL.W = DOS_R.HL.W.l;
						R.IX.W = DOS_R.IX.W.l;
						R.IY.W = DOS_R.IY.W.l;
						R.PC.W = DOS_R.PC.W.l;
						R.SP.W = DOS_R.SP.W.l;
						R.AF1.W = DOS_R.AF2.W.l;
						R.BC1.W = DOS_R.BC2.W.l;
						R.DE1.W = DOS_R.DE2.W.l;
						R.HL1.W = DOS_R.HL2.W.l;
						R.IFF = 0;
						if (DOS_R.IM == 1) R.IFF+=2;
						if (DOS_R.IM == 2) R.IFF+=4;
						if (DOS_R.IFF1) R.IFF+=1;
						R.I = DOS_R.I;
						R.R = (DOS_R.R&127)|(DOS_R.R2&128);  
  
    
  
  fclose(f);
 }
 else
 {
  printf("ERROR abrir P.GAL\n");
  //fflush(stdout);
 }
 
 printf("LeerGAL END\n");
 //fflush(stdout);
}
*/

//*************************************
unsigned short int LoopZ80(Z80 *R)
{
 //fprintf(gb_log,"LoopZ80\n");
 //fprintf(gb_log,"LoopZ80 Fassst %d PC:%04X\n",Fassst,R->PC);
 //fflush(gb_log); 
 
 gb_fps_time_cur= millis(); //revisar millis(); //SDL_GetTicks();
 unsigned int auxTime= (gb_fps_time_cur - gb_fps_time_last);
 if (auxTime >= 999)
 {
  gb_fps_time_last= gb_fps_time_cur;    
  gb_fps_med= gb_fps_cur;
  if (gb_do_action_key_show_osd==1)
  {
   ShowFPSCall();
  }
 }

 gb_intercept_tape_cur= millis(); //Intercepta Cinta
 auxTime= (gb_intercept_tape_cur - gb_intercept_tape_last);
 if (auxTime>=99)
 {
  gb_intercept_tape_last= gb_intercept_tape_cur;
  interceptarROMTape();
 }     


    //JJ u32 current_time = SDL_GetTicks();
    unsigned int current_time = millis(); //SDL_GetTicks();
        
    
//    float dt = (float)(current_time - last_time) / 1000.0f;
    last_time = current_time;
    //    int frame_ready=0;

//    sekunda += dt;
//    if (sekunda >= 1.0)
//    {
////        char buffer[256];
////        sprintf(buffer, "FPS: %d, SPEED=%s", (int)(1.0f/dt), (cpu_speed == CPU_SPEED_NORMAL ? "NORMAL" : "FAST"));
//
//        //JJ SDL_SetWindowTitle(window, buffer); //SDL2 a SDL1
//
//        sekunda -= 1.0;
//        //frame_ready = 1;
//    }
    
    //SDL_Delay((int)(1000/50));  //Espera 20 ms    

    gb_cpu_time_cur= micros(); //millis(); //SDL_GetTicks();
    unsigned int auxTimeCPU= gb_cpu_time_cur - gb_cpu_time_last;
    unsigned int auxDelayCPU= (gb_cpu_delay==1)? (20000-auxTimeCPU): 0;
    gb_cpu_time_med= auxTimeCPU;
    if (auxDelayCPU>20000)
    {
     auxDelayCPU= 20000;
    }


    if (gb_cpu_time_med<gb_cpu_time_min)
    {
     gb_cpu_time_min= gb_cpu_time_med;
    }
    if (gb_cpu_time_med>gb_cpu_time_max)
    {
     gb_cpu_time_max= gb_cpu_time_med;
    }
    
    //printf("Delay CPU:%d aux:%d\n",auxDelayCPU,auxTimeCPU);
    //#ifdef use_lib_cont_call
    // printf("cont:%d\n",gb_cont_call);
    // gb_cont_call=0;
    //#endif    
    //fflush(stdout);
    #ifdef use_lib_skip_fps
    #else
     if (gb_cpu_delay==1)
     {
      delayMicroseconds(auxDelayCPU); //delay(auxDelayCPU); //SDL_Delay(auxDelayCPU);      
     }
    #endif 
    gb_cpu_time_last= micros(); //millis(); //SDL_GetTicks();




    //if (active_help)
    //{
    //    //show_help();        //revisar no se necesita
    //}
    //else //if(frame_ready)
    //{
        if (!(R->IFF & IFF_2))
        {
            Fassst++;
        }
        else
        {
            Fassst = 0;
        }

        // Ako je EI osvezavaj i ekran i tastaturu, kao i kod prave masine.        
        
        //Lo fuerzo
//        read_keyboard();
//        refresh_screen();
        //fin de lo fuerzo        
        

         gb_keyb_poll_time_cur= millis();
         if (!((gb_keyb_poll_time_cur - gb_keyb_poll_time_prev) < gb_use_keyb_poll_ms))
         {
          gb_keyb_poll_time_prev= gb_keyb_poll_time_cur;

          read_keyboard();
          #ifdef use_lib_keyboard_uart
           keyboard_uart_poll();
           ///read_keyboard_uart();
           do_keyboard_uart();
          #endif
         }

         if (gb_send_vkeys==1)
         {
          send_virtual_keys();
         }
         
         

        if (!Fassst)                    
        {
         gb_vga_poll_time_cur = millis();
         if (
             (!((gb_vga_poll_time_cur - gb_vga_poll_time_prev) < gb_use_vga_poll_ms))
             ||
             (gb_use_vga_poll_ms==20)
             ||
             (gb_use_vga_poll_ms==0)
            ) 
         {
          gb_vga_poll_time_prev= gb_vga_poll_time_cur;
          refresh_screen();
         }
         gb_fps_cur++;
        }
        else
        {
            switch (Fassst)
            {
            // Because screen is made under IRQ, there is no more further screen updates.
            case 1:
                // Need to clear it ?
                //JJ SDL_FillRect(screen, NULL, crna_color); //revisar alternativo
                bitluni_SDL_FillRect((crna_color & 0x07)); //Da igual la mascara es negro 0 siempre
                break;

            // Stay where You are.
            case 2:
                Fassst--;
                break;
            
            default: break; 
            }
                        
        }
    //}


    //Preocesar acciones
    if (gb_do_action_key_f1==1)
    {
     gb_do_action_key_f1=0;
     gb_show_osd_main_menu= 1;
     do_tinyOSD();     
    }

    if (gb_do_action_key_reset==1)
    {
     gb_do_action_key_reset= 0;
     return INT_QUIT;
    }


    /*revisar
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            ExitLoop = 1;
            break;
        case SDL_KEYDOWN:
        {
            //JJ u32 shift = event.key.keysym.mod & KMOD_SHIFT;
            unsigned int shift = event.key.keysym.mod & KMOD_SHIFT;
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                ExitLoop = 1;
                break;
            case SDLK_F8: // TOGGLE SPEED
                cpu_speed = (cpu_speed == CPU_SPEED_NORMAL) ? CPU_SPEED_FAST : CPU_SPEED_NORMAL;
                R->IPeriod = cpu_speed / FRAMES_PER_SECOND;
                break;
            case SDLK_F1: // TOGGLE HELP SCREEN
                //active_help = !active_help;

                gb_show_osd_main_menu= 1;
                do_tinyOSD();                
                break;
            case SDLK_F2: // SAVE/LOAD MEMORY
                if (shift)
                    save_memory();
                else
                    load_memory();
                break;
            
            case SDLK_F3:
             LeerGAL();
             break;    
                
                
            case SDLK_F10:
             gb_cpu_delay= 1;
             break;
                             
            case SDLK_F11:
             gb_cpu_delay= 0;
             break;
             
            case SDLK_F12: // HARD/NMI RESET
                if (shift)
                {
                    ResetZ80(R);
                    return INT_NONE;
                }
                else
                    return INT_NMI;
                break;
            }
        }
        break;
        }
    }
    */



    // Select the color for drawing. It is set to black here.
    //JJ SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //SDL2 a SDL1

    // Clear the window
    //JJ SDL_RenderClear(renderer); //SDL2 a SDL1

    // 0x2bb0 = BROJAC ZA POMERANJE SLIKE
    #ifdef use_lib_ram_8KB
     int yOff = (int)MEMORY[0x2BB0-8192] * 3;
    #else
     int yOff = (int)MEMORY[0x2BB0] * 3;
    #endif 

    // 0x2ba8 = HORIZONTALNA POZICIJA TEKSTA
    #ifdef use_lib_ram_8KB
     int xOff = (int)MEMORY[0x2BA8-8192] * 8;
    #else
     int xOff = (int)MEMORY[0x2BA8] * 8;
    #endif 

    //if (active_help)
    //{
    //    yOff = 0;
    //}

    //JJ SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, screen); //SDL2 a SDL1

    if (yOff)
    {
        // yOff = 9..6..3..0
        int o = 9;
        /*revisar
        SDL_Rect src = {0, o - yOff, SCREEN_W, SCREEN_H - yOff},
                 dst = {xOff + (WINDOW_W - SCREEN_W * 2) / 2 - 88, o + (WINDOW_H - SCREEN_H * 2) / 2, 2 * SCREEN_W, 2 * (SCREEN_H - yOff)};
        */

        //JJ SDL_RenderCopy(renderer, tex, &src, &dst); //SDL2 a SDL1
    }
    else
    {
        /*revisar
        SDL_Rect src = {0, 0, SCREEN_W, SCREEN_H},
                 dst = {xOff + (WINDOW_W - SCREEN_W * 2) / 2 - 88, (WINDOW_H - SCREEN_H * 2) / 2, 2 * SCREEN_W, 2 * SCREEN_H};
        */

        //JJ SDL_RenderCopy(renderer, tex, &src, &dst); //SDL2 a SDL1
    }

    // Render the changes above ( which up until now had just happened behind the scenes )
    //JJ SDL_RenderPresent(renderer); //SDL2 a SDL1
    //JJ SDL_DestroyTexture(tex); //SDL2 a SDL1

//    if (prg_to_load)
//    {
//        //fprintf(gb_log,"prg_to_load W:%04X BEGIN\n",R->PC.W);
//        // XXX: Is the machine in idle loop?
//        if (R->PC.W == 0x0ceb)
//        {
//            //printf("Loading prg \"%s\" ...\n", prg_to_load);
//            //printf("Loading prg\n");
//            if (gb_use_debug==1){ Serial.printf("Loading prg \"%s\" ...\r\n", prg_to_load); }
//            //fflush(stdout);
//            load_prg(prg_to_load);
//
//            prg_to_load = NULL;
//        }
//        //fprintf(gb_log,"prg_to_load END\n");
//    }

    if (ExitLoop)
    {
        return INT_QUIT;
    }

    //if (active_help)
    //{
    //    return INT_NONE;
    //}

    return INT_IRQ; //(frame_ready) ? INT_IRQ : INT_NONE;
}


unsigned char DebugZ80(Z80 *R)
{
    //printf("%04x\n", (unsigned int)R->PC.W);
    return 1;
}

unsigned short int start_machine()
{
  if (gb_use_debug==1){ Serial.printf("start_machine BEGIN\r\n"); }
  //fflush(stdout);
     
#if defined(DEBUG)
    R.Trap = 0xFFFF;
    R.Trace = 0;
#endif
    last_time = millis(); //SDL_GetTicks();

    R.IPeriod = cpu_speed / FRAMES_PER_SECOND;

    ResetZ80(&R);

    if (gb_use_debug==1){ Serial.printf("start_machine END\r\n"); }
    //fflush(stdout);
    return RunZ80(&R);
}

void init_memory()
{     
//    FILE *fp;
    int n, a, b, x, karakter;
    unsigned short int adresa;
    //unsigned char buffer[2048];
    
    if (gb_use_debug==1){ Serial.printf("init_memory BEGIN\r\n"); }
    //fflush(stdout);

    // Clear memory
    memset((void *)MEMORY, 0, sizeof(MEMORY)); //Nos da igual 8KB o 64KB use_lib_ram_8KB
    
    if (gb_use_debug==1){ Serial.printf("Memory clear\r\n"); }
    //fflush(stdout);

//JJ    if (!(fp = fopen("CHRGEN.BIN", "r")))
//JJ    {
//JJ        //printf(stderr, "Can't open CHRGEN.BIN\n");
//JJ        printf("Can't open CHRGEN.BIN\n");
//JJ        fflush(stdout);
//JJ        exit(-1);
//JJ    }
//JJ    fread(buffer, 1, 2048, fp);
//JJ    fclose(fp);

//JJ    printf("CHRGEN.BIN Loaded\n");
//JJ    fflush(stdout);
    
    if (gb_use_debug==1){ Serial.printf("FONT BROJ_ZNAKOVA(%d) * VISINA(%d) * SIRINA(%d) = %d\r\n",BROJ_ZNAKOVA,VISINA,SIRINA, 4*(BROJ_ZNAKOVA * VISINA * SIRINA)); }
    //printf("sizeof FONT %d\n",sizeof(FONT));
    //fflush(stdout);
    
    //Ya no lo necesito las fuentes, lo tengo empaquetado
    /*
    unsigned int *ptr = FONT;
    for (karakter = 0; karakter < BROJ_ZNAKOVA; karakter++)
    {
        for (n = 0; n < VISINA; n++)
        {
            adresa = n << 7; // 128*VISINA + karakter
            adresa |= karakter;

            a = 1;
            //JJ b = buffer[adresa];
            b = gb_chrgen_bin[adresa];

            for (x = 0; x < SIRINA; x++)
            {
                *ptr++ = (!(b & a)) ? pixel_color : crna_color;
                a <<= 1;
            }
        }
    }
    */
    
    /*Sacar FONT convertido de 32 bits a 8 bits 0 y 1
    printf("\nFONT\n");    
    unsigned char *aux= (unsigned char *)FONT;
    unsigned int cont=0;
    for (int k=0;k<128;k++)
    {
     for (int j=0;j<13;j++)
     {
      for (int i=0;i<8;i++)
      {    
       //printf("%02X",aux[cont]);
       printf("%d,",(aux[cont]==0)?0:1);
       cont+=4;
      }
      printf("\n");
     }
     printf("\n");
    }
    printf("\n");
    */

/*
 printf("const unsigned char FONT8_1bit[1664]={\n");
 unsigned short int cont=0;
 unsigned char dato=0;
 for (int k=0;k<128;k++)
 {
  for (int j=0;j<13;j++)
  {
   dato=0;
   for (int i=0;i<8;i++)
   {
    dato= dato | (FONT8[cont]<<(7-i));
    cont++;    
   }
   printf("0x%02X,",dato);
  }
  printf("\n");
 }
 printf("};\n");
 fflush(stdout); 
 */
    

/*No necesito fichero todo en .h
    if (!(fp = fopen("ROM1.BIN", "r")))
    {
        //printf(stderr, "Can't open ROM1.BIN\n");
        printf("Can't open ROM1.BIN\n");
        fflush(stdout);
        exit(-1);
    }
    fread(&MEMORY[0], 1, 4096, fp);
    fclose(fp);
    */
    
    #ifdef use_lib_ram_8KB
    #else
     memcpy(&MEMORY[0], gb_rom1_bin, 4096);
    #endif 
    
    if (gb_use_debug==1){ Serial.printf("gb_rom1_bin Loaded from memory .h\r\n"); }
    //printf("ROM1.BIN Loaded\n");
    //fflush(stdout);

/*No se necesito leo de .h memoria
    if (!(fp = fopen("ROM2.BIN", "r")))
    {
        //fprintf(stderr, "Can't open ROM2.BIN\n");
        printf("Can't open ROM2.BIN\n");
        fflush(stdout);
        exit(-1);
    }
    fread(&MEMORY[0x1000], 1, 4096, fp);
    fclose(fp);
*/    

    #ifdef use_lib_ram_8KB
    #else
     memcpy(&MEMORY[0x1000], gb_rom2_bin, 4096);
    #endif
    
    if (gb_use_debug==1){ Serial.printf("gb_rom2_bin Loaded from memory .h\r\n"); }
    //printf("ROM2.BIN Loaded\n");
    //fflush(stdout);

    // Tastatura
    for (n = 0x2000; n < 0x2800; n++)
    {
     #ifdef use_lib_ram_8KB
      MEMORY[n-8192] = 0xFF;
     #else
      MEMORY[n] = 0xFF;
     #endif 
    }

    // Ekran
    for (n = 0x2800; n < 0x2A00; n++)
    {
     #ifdef use_lib_ram_8KB
      MEMORY[n-8192] = ' ' + n;
     #else
      MEMORY[n] = ' ' + n;
     #endif 
    }

    // 6116-ice
    for (n = 0x2A00; n < KRAJ_RAMA; n++)
    {
     #ifdef use_lib_ram_8KB
      MEMORY[n-8192] = 0;
     #else
      MEMORY[n] = 0;
     #endif 
    }

    Fassst = 0;
    ExitLoop = 0;
    
    if (gb_use_debug==1){ Serial.printf("init_memory END\r\n"); }
    //fflush(stdout);
}

//void usage(char *arg0)
//{
//    //printf(stderr, "\nUsage: %s [<GTP file to load> [-j<address>]] [-c<font color>] [-w<work dir>]\n\n", arg0);
//    printf("\nUsage: %s [<GTP file to load> [-j<address>]] [-c<font color>] [-w<work dir>]\n\n", arg0);
//    //fflush(stdout);
//}



//**************************************************
void ActivarVideoPorTeclas()
{
 //unsigned char is8colors=1;
 unsigned char is8colors= gb_vga_8colors;
 unsigned char usepllcteforce=0;
 unsigned char usecustompll=0;
 unsigned int p0=0;
 unsigned int p1=0;
 unsigned int p2=0;
 unsigned int p3=0; 

 unsigned int tiempo_ahora;
 unsigned int tiempo_antes;
 unsigned char video=255;
 #ifdef use_lib_ram_8KB
  unsigned short int auxAddr= 0x2000-8192;
 #else
  unsigned short int auxAddr= 0x2000;
 #endif
 
 tiempo_antes= tiempo_ahora= millis();
 
 while ((tiempo_ahora-tiempo_antes)<use_lib_boot_time_select_vga)
 {
  tiempo_ahora= millis();

  read_keyboard();
  #ifdef use_lib_keyboard_uart
   keyboard_uart_poll();
   ///read_keyboard_uart();
   do_keyboard_uart();
  #endif

  if (gb_Kmap[32]== 0) {video=0;} //0
  if (gb_Kmap[33]== 0) {video=1;} //1
  if (gb_Kmap[34]== 0) {video=2;} //2
  if (gb_Kmap[35]== 0) {video=3;} //3
  if (gb_Kmap[36]== 0) {video=4;} //4
  if (gb_Kmap[37]== 0) {video=5;} //5               
  if (gb_Kmap[38]== 0) {video=6;} //6
  if (gb_Kmap[39]== 0) {video=7;} //7 //fin modos 3 bpp

  if (gb_Kmap[40]== 0) {video=8;} //8
  if (gb_Kmap[41]== 0) {video=9;} //9
  if (gb_Kmap[1]== 0) {video=10;} //A
  if (gb_Kmap[2]== 0) {video=11;} //B
  if (gb_Kmap[3]== 0) {video=12;} //C
  if (gb_Kmap[4]== 0) {video=13;} //D
  if (gb_Kmap[5]== 0) {video=14;} //E
  if (gb_Kmap[6]== 0) {video=15;} //F
 }

 if (video!=255)
 {//Cambio modo video
  if (gb_use_debug==1){ Serial.printf("Video mode change\r\n"); }
  switch (video)
  {
   case 0: gb_vga_videomode_cur= 0; is8colors=1; gb_ptrVideo_cur= VgaMode_vga_mode_360x200; usepllcteforce=0; usecustompll=0; break; //360x200x70hz bitluni(3bpp)
   case 1: gb_vga_videomode_cur= 1; is8colors=1; gb_ptrVideo_cur= VgaMode_vga_mode_360x200; usepllcteforce=0; usecustompll=1; break; //360x200x70hz bitluni apll(3bpp)
   case 2: gb_vga_videomode_cur= 2; is8colors=1; gb_ptrVideo_cur= VgaMode_vga_mode_320x200; usepllcteforce=0; usecustompll=0; break; //320x200x70hz bitluni(3bpp)
   case 3: gb_vga_videomode_cur= 3; is8colors=1; gb_ptrVideo_cur= VgaMode_vga_mode_320x200; usepllcteforce=1; p0=0x00AE; p1=0x00CF; p2=0x0004; p3=0x0005; usecustompll=0; break; //320x200x70hz fabgl(3bpp)
   case 4: gb_vga_videomode_cur= 4; is8colors=1; gb_ptrVideo_cur= VgaMode_vga_mode_320x200; usepllcteforce=0; usecustompll=1; break; //320x200x70hz bitluni apll(3bpp)
   case 5: gb_vga_videomode_cur= 5; is8colors=1; gb_ptrVideo_cur= VgaMode_vga_mode_320x240; usepllcteforce=0; usecustompll=0; break; //320x240x60hz bitluni(3bpp)
   case 6: gb_vga_videomode_cur= 6; is8colors=1; gb_ptrVideo_cur= VgaMode_vga_mode_320x240; usepllcteforce=1; p0=0x000A; p1=0x0057; p2=0x0007; p3=0x0007; usecustompll=0; break; //320x240x60hz fabgl(3bpp)
   case 7: gb_vga_videomode_cur= 7; is8colors=1; gb_ptrVideo_cur= VgaMode_vga_mode_320x240; usepllcteforce=0; usecustompll=1; break; //320x240x60hz bitluni apll(3bpp)

   case 8: gb_vga_videomode_cur= 0; is8colors=0; gb_ptrVideo_cur= VgaMode_vga_mode_360x200; usepllcteforce=0; usecustompll=0; break;
   case 9: gb_vga_videomode_cur= 1; is8colors=0; gb_ptrVideo_cur= VgaMode_vga_mode_360x200; usepllcteforce=0; usecustompll=1; break;
   case 10:gb_vga_videomode_cur= 2; is8colors=0; gb_ptrVideo_cur= VgaMode_vga_mode_320x200; usepllcteforce=0; usecustompll=0; break;
   case 11:gb_vga_videomode_cur= 3; is8colors=0; gb_ptrVideo_cur= VgaMode_vga_mode_320x200; usepllcteforce=1; p0=0x00AE; p1=0x00CF; p2=0x0004; p3=0x0005; usecustompll=0; break;
   case 12:gb_vga_videomode_cur= 4; is8colors=0; gb_ptrVideo_cur= VgaMode_vga_mode_320x200; usepllcteforce=0; usecustompll=1; break;
   case 13:gb_vga_videomode_cur= 5; is8colors=0; gb_ptrVideo_cur= VgaMode_vga_mode_320x240; usepllcteforce=0; usecustompll=0; break;
   case 14:gb_vga_videomode_cur= 6; is8colors=0; gb_ptrVideo_cur= VgaMode_vga_mode_320x240; usepllcteforce=1; p0=0x000A; p1=0x0057; p2=0x0007; p3=0x0007; usecustompll=0; break;
   case 15:gb_vga_videomode_cur= 7; is8colors=0; gb_ptrVideo_cur= VgaMode_vga_mode_320x240; usepllcteforce=0; usecustompll=1; break;   
   default: break;
  }

 }
 if (gb_use_debug==1){ Serial.printf("Video mode set %d\r\n",video); }

   OSDUpdateAnchoAltoVGA(gb_vga_videomode_cur,is8colors);//Set is8colors gb_vga_8colors

   //vga_init(pin_config,VgaMode_vga_mode_320x240,false,0,0,0,0,0,0);
   vga_init((is8colors==1)?pin_config8:pin_config64,gb_ptrVideo_cur,false,usepllcteforce,p0,p1,p2,p3,usecustompll,is8colors, gb_vga_videomode_cur);
   gb_sync_bits= vga_get_sync_bits();
   gb_buffer_vga = vga_get_framebuffer();
   gb_buffer_vga32=(unsigned int **)gb_buffer_vga;
   PrepareColorsBitluniVGA(); //Llamar despues de tener gb_sync_bits  
}





//*********
//* SETUP *
//*********
void setup() 
{
 gb_ramfree_ini= ESP.getFreeHeap();
 #if defined(use_lib_log_serial) || defined(use_lib_keyboard_uart)
  Serial.begin(115200);         
  if (gb_use_debug==1){ Serial.printf("HEAP BEGIN %d\r\n", gb_ramfree_ini); }
 #endif

 #ifdef use_lib_keyboard_uart
  Serial.setTimeout(use_lib_keyboard_uart_timeout);
 #endif

 //OSDUpdateAnchoAltoVGA(gb_vga_videomode_cur,gb_vga_8colors);

 memset(gb_Kmap,0xFF,sizeof(gb_Kmap)); //Necesito teclas iniciar modo video
 kb_begin(); //Cargo teclado antes seleccion modo video
 delay(100);
 ActivarVideoPorTeclas();

 //vga_init(pin_config,VgaMode_vga_mode_320x240,false,0,0,0,0,0,0);
 //gb_sync_bits= vga_get_sync_bits();
 //gb_buffer_vga = vga_get_framebuffer();
 //gb_buffer_vga32=(unsigned int **)gb_buffer_vga;
 //PrepareColorsBitluniVGA(); //Llamar despues de tener gb_sync_bits 
 //SDLClear();  //revisar

 #ifdef use_lib_log_serial
  if (gb_use_debug==1){ Serial.printf("VGA %d\r\n", ESP.getFreeHeap()); }
 #endif

 //memset(gb_Kmap,0xFF,sizeof(gb_Kmap));
 //kb_begin(); 

 #ifdef use_lib_log_serial  
  if (gb_use_debug==1){ Serial.printf("KEYBOARD %d\r\n", ESP.getFreeHeap()); }
 #endif

 init_memory();
 //#ifdef use_lib_log_serial  
 // Serial.printf("INIT MEMORY %d\r\n", ESP.getFreeHeap()); 
 //#endif

 gb_ramfree_setupEnd= ESP.getFreeHeap();
 #ifdef use_lib_log_serial  
  if (gb_use_debug==1){ Serial.printf("END SETUP %d\r\n", gb_ramfree_setupEnd); }
 #endif


 gb_cpu_time_cur= gb_cpu_time_last= gb_vga_time_cur= gb_vga_time_last= micros();
 gb_keyb_poll_time_cur= gb_keyb_poll_time_prev = millis();
 gb_vga_poll_time_cur= gb_vga_poll_time_prev= gb_keyb_poll_time_cur;
 gb_fps_time_cur= gb_fps_time_last= gb_keyb_poll_time_cur;

 gb_setup_end= 1;
}





//*******************************************
void loop() 
{
 if (gb_setup_end==1)
 {
  gb_cont_error_mem=0;
  unsigned short int pc = start_machine();
  //fprintf(stderr, "Leaving emulator PC=$%04x\n", pc);
  #ifdef use_lib_log_serial  
   if (gb_use_debug==1){ Serial.printf("Leaving emulator PC=$%04x\r\n", pc); }
  #endif
 }
}