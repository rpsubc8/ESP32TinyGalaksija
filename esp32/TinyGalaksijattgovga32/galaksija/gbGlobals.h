#ifndef GB_GLOBALS_H
 #define GB_GLOBALS_H

 #include "gbConfig.h" 
 
 //#include <SDL/SDL.h>
 #include "Z80Z80.h"
 #include "stdio.h"

 //#define video_mode_360x200x70hz_bitluni 0 
 //#define video_mode_360x200x70hz_bitluni_apll_fix 1
 //#define video_mode_vga320x200x70hz_bitluni 2
 //#define video_mode_vga320x200x70hz_fabgl 3
 //#define video_mode_vga320x200x70hz_bitluni_apll_fix 4
 //#define video_mode_vga320x240x60hz_bitluni 5
 //#define video_mode_vga320x240x60hz_fabgl 6
 //#define video_mode_vga320x240x60hz_bitluni_apll_fix 7
 

 extern unsigned char gb_cont_error_mem;

 extern unsigned int gb_ramfree_ini;
 extern unsigned int gb_ramfree_setupEnd;
 extern volatile unsigned char gb_keymap[32];

 extern unsigned char gb_use_interceptROMTape;
 extern unsigned char gb_use_debug;

 //#ifdef use_lib_cont_call
 // extern unsigned int gb_cont_call;
 //#endif 


 #define video_mode_360x200 0 
 #define video_mode_320x200 1
 #define video_mode_320x240 2 

 extern unsigned char gb_do_action_key_f1;
 extern unsigned char gb_do_action_key_reset;
 extern unsigned char gb_do_action_key_show_osd;

 extern unsigned char gb_Kmap[54]; //Teclado de galaksija
 
 extern unsigned char gb_show_osd_main_menu;
 extern unsigned char gb_language_en;
 extern unsigned char gb_id_menu_cur;

 extern unsigned char gb_cont_vkeys;
 extern unsigned char gb_cont_pul_vkeys;
 extern unsigned char gb_send_vkeys;

 extern unsigned char gb_use_keyb_poll_ms;
 extern unsigned int gb_keyb_poll_time_cur;
 extern unsigned int gb_keyb_poll_time_prev;

 extern short int gb_osd_ventana_ini;
 extern short int gb_osd_ventana_fin;

 extern unsigned char gb_gtp_id_sel_cur;
 extern unsigned char gb_gtp_warmstart_cur;
 extern unsigned char gb_gal_id_sel_cur;

 extern unsigned char gb_cpu_delay; //No esperar cpu 20 ms
 extern unsigned char gb_vga_display_offset_left; //Pixels sumar izquierda


 extern unsigned int gb_vga_dump_time_dur; //duracion tiempo volcado vga

 extern unsigned int gb_vga_time_cur;
 extern unsigned int gb_vga_time_last; 

 extern unsigned char gb_use_vga_poll_ms;
 extern unsigned int gb_vga_poll_time_cur;
 extern unsigned int gb_vga_poll_time_prev;

 extern unsigned int gb_intercept_tape_cur;
 extern unsigned int gb_intercept_tape_last;

 extern unsigned int gb_fps_cur;
 extern unsigned int gb_video_fps_cur;
 extern unsigned int gb_fps_last;
 extern unsigned int gb_fps_min;
 extern unsigned int gb_fps_max;
 extern unsigned int gb_fps_med;
 extern unsigned int gb_fps_time_cur;
 extern unsigned int gb_fps_time_last;
 
 extern unsigned int gb_cpu_time_cur;
 extern unsigned int gb_cpu_time_last;
 extern unsigned int gb_cpu_time_min;
 extern unsigned int gb_cpu_time_med; 
 extern unsigned int gb_cpu_time_max;

 extern unsigned char gb_keep_alive;
 //extern SDL_Event gb_event;
 //extern SDL_Color gb_colors[256];

 #ifdef use_lib_keyboard_uart
  #define BUFFER_SIZE_UART 50
  extern char gb_buf_uart[BUFFER_SIZE_UART];
  extern unsigned char gb_rlen_uart;
  extern unsigned int gb_curTime_keyboard_before_uart;
  extern unsigned int gb_curTime_keyboard_uart;
 #endif
 
 extern unsigned char ** gb_buffer_vga;
 extern unsigned int **gb_buffer_vga32;
 extern unsigned char gb_const_colorNormal[64]; //8 y 64 colores
 extern unsigned char gb_color_bw[2];
 extern unsigned char gb_sync_bits;
 //extern const unsigned char pin_config[];
 extern const unsigned char pin_config8[];
 extern const unsigned char pin_config64[];
 extern const unsigned int *gb_ptrVideo_cur;
 
 extern unsigned char gb_vga_videomode_cur;
 extern unsigned char gb_vga_videomode_cur_menu; //El seleccionado en el OSD
 extern unsigned short int gb_vga_ancho;
 extern unsigned short int gb_vga_alto;
 extern unsigned short int gb_vga_yOffset; //Desplazamiento Y pantalla Galaksija BASIC
 extern unsigned char gb_vga_alto_caracter; //Alto caracter 13 o 12  
 extern unsigned char gb_vga_8colors; //1 8 colores, 0 64 colores
 //extern unsigned char gb_color_bw[2];
 extern unsigned char ESP32_WHITE;
 extern unsigned char ESP32_BLACK; 



 
 //extern FILE *gb_log;
 extern Z80 R;

 //JJ extern SDL_Renderer *renderer; //SDL2 a SDL1
 //JJ extern SDL_Window *window; //SDL2 a SDL1
 //extern SDL_Surface *screen;
 extern unsigned int crna_color;
 extern unsigned int pixel_color;
 
 extern unsigned char ExitLoop;


 #ifdef use_lib_ram_8KB
  extern unsigned char MEMORY[8192]; //8KB RAM(solo necesito 6KB)
 #else
  extern unsigned char MEMORY[65536]; // max 64k of MEMORY
 #endif 
 extern char workdir[256];
 
 
 

//main BEGIN
// Dakle 6k RAM-a
// Max 64k memorije, prvih 8k je ROM
#define WORK_SPACE 0x4000
#define KRAJ_RAMA 0x4000

// CPU_SPEED is the speed of the CPU in Hz. It is used together with
// FRAMES_PER_SECOND to calculate how much CPU cycles must pass between
// interrupts.
#define CPU_SPEED_NORMAL 3072000 // 6.144/2 Mhz
#define CPU_SPEED_FAST 25000000  // 25 Mhz?
#define FRAMES_PER_SECOND 50

#define VISINA 13
#define SIRINA 8
#define BROJ_ZNAKOVA 128

#define SCREEN_W 256
#define SCREEN_H 208

#define WINDOW_W 640
#define WINDOW_H 480



// extern unsigned int FONT[BROJ_ZNAKOVA * VISINA * SIRINA];
// extern const unsigned char FONT8[13312]; //128x13x8
 extern const unsigned char FONT8_1bit[1664];
 
//main END

 
 
 
 

#endif

