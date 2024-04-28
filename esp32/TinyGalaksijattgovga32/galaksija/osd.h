#ifndef OSDFILE_H
 #define OSDFILE_H

 #include "gbConfig.h"
 
 //void do_tinyOSD(void);
 //void SDLActivarOSDMainMenu();
 //void SDLEsperaTeclado(void);
 //void SDLClear(void);
 
  #define id_menu_main 0
  #define id_menu_load_gtp 1
  #define id_menu_load_gal 2
  #define id_menu_language 3
  #define id_menu_video_options 4
   #define id_menu_video_mode 5
   #define id_menu_video_poll 6
  #define id_menu_keyb_options 7
  #define id_menu_cpu_options 8
  #define id_menu_osd 9
  #define id_menu_debug 10
  #define id_menu_help 11
  #define id_menu_reset 12
  #define id_menu_intercepttape 13
  #define id_menu_ram 14
  #define id_menu_about 15
  



#define max_gb_main_menu 13
static const char * gb_main_menu_sp[max_gb_main_menu]={
 "Leer/Sel GTP (Cinta)",
 "Leer GAL (SNAPSHOT)",
 "Idioma",
 "Opciones de Video",
 "Opciones de Teclado",
 "Opciones de CPU",
 "Opciones de OSD",
 "Opciones de Debug",
 "Ayuda",
 "Reset",
 "Interceptar ROM Cinta",
 "RAM libre",
 "Acerca de"
};
static const char * gb_main_menu_en[max_gb_main_menu]={
 "Load/Select GTP (Tape)",
 "Load GAL (SNAPSHOT)",
 "Language",
 "Video options",
 "Keyboard options",
 "CPU options",
 "OSD options",
 "Debug options",
 "Help",
 "Reset",
 "Intercept ROM Tape",
 "RAM free",
 "About"      
};

#define max_gb_loadselect_menu 2
static const char * gb_loadselect_menu_sp[max_gb_loadselect_menu]={
 "Leer",
 "Seleccionar"
};
static const char * gb_loadselect_menu_en[max_gb_loadselect_menu]={
 "Load",
 "Select"
};

#define max_gb_bool_menu 2
static const char * gb_bool_menu_sp[max_gb_bool_menu]={
 "SI",
 "NO"
};
static const char * gb_bool_menu_en[max_gb_bool_menu]={
 "Yes",
 "No"
};



#define max_gb_language_menu 2
static const char * gb_language_menu_sp[max_gb_language_menu]={
 "Ingles",
 "Castellano"
};
static const char * gb_language_menu_en[max_gb_language_menu]={
 "English",
 "Spanish"  
};


#define max_gb_video_options_menu 2
static const char * gb_video_options_menu_sp[max_gb_video_options_menu]={
 "Modo de video",
 "Video velocidad sondeo"
};
static const char * gb_video_options_menu_en[max_gb_video_options_menu]={
 "Video mode",
 "Video poll" 
};


#define max_gb_video_mode_menu 16
static const char * gb_video_mode_menu[max_gb_video_mode_menu]={
 "0 360x200x70hz bitluni(3bpp)",
 "1 360x200x70hz bitluni apll(3bpp)",
 "2 320x200x70hz bitluni(3bpp)",
 "3 320x200x70hz fabgl(3bpp)",
 "4 320x200x70hz bitluni apll(3bpp)",
 "5 320x240x60hz bitluni(3bpp)",
 "6 320x240x60hz fabgl(3bpp)",
 "7 320x240x60hz bitluni apll(3bpp)",
 "8 360x200x70hz bitluni(6bpp)",
 "9 360x200x70hz bitluni apll(6bpp)",
 "A 320x200x70hz bitluni(6bpp)",
 "B 320x200x70hz fabgl(6bpp)",
 "C 320x200x70hz bitluni apll(6bpp)",
 "D 320x240x60hz bitluni(6bpp)",
 "E 320x240x60hz fabgl(6bpp)",
 "F 320x240x60hz bitluni apll(6bpp)" 
};


#define max_gb_video_poll_menu 5
static const char * gb_video_poll_menu_sp[max_gb_video_poll_menu]={
 "0 ms (sin limite FPS)",
 "20 ms (50 FPS)",
 "30 ms (33 FPS)",
 "40 ms (25 FPS)",
 "50 ms (20 FPS)"
};
static const char * gb_video_poll_menu_en[max_gb_video_poll_menu]={
 "0 ms (no limit FPS)",
 "20 ms (50 FPS)",
 "30 ms (33 FPS)",
 "40 ms (25 FPS)",
 "50 ms (20 FPS)"
};


#define max_gb_keyboard_poll_menu 5
static const char * gb_keyboard_poll_menu_sp[max_gb_keyboard_poll_menu]={
 "10 ms (100 CPS)",
 "20 ms (50 CPS)",
 "30 ms (33 CPS)",
 "40 ms (25 CPS)",
 "50 ms (20 CPS)"
};
static const char * gb_keyboard_poll_menu_en[max_gb_keyboard_poll_menu]={
 "10 ms (100 CPS)",
 "20 ms (50 CPS)",
 "30 ms (33 CPS)",
 "40 ms (25 CPS)",
 "50 ms (20 CPS)"
};


#define max_gb_cpu_menu 2
static const char * gb_cpu_menu_sp[max_gb_cpu_menu]={
 "20 ms (normal)",
 "0 ms (rapido)"
};
static const char * gb_cpu_menu_en[max_gb_cpu_menu]={
 "20 ms (normal)",
 "0 ms (fast)"
};


#define max_gb_reset_menu 2
static const char * gb_reset_menu_sp[max_gb_reset_menu]={
 "Soft",
 "Hard"
};
static const char * gb_reset_menu_en[max_gb_reset_menu]={
 "Soft",
 "Hard"
};


#define max_gb_help_menu 18
static const char * gb_help_menu_sp[max_gb_help_menu]={
 "F1     - muestra OSD",
 "ESC    - salir OSD",
 "Arriba - seleccionar",
 "Abajo  - seleccionar",
 "Enter  - Aceptar",
 "",
 "GTP (Selecciona o carga cinta)",
 " Permite WARM START y auto RUN",
 "GAL (Carga Snapshot)",
 " Permite auto RUN",
 "Lenguaje (castellano, ingles)",
 "Modos de video (3 bpp y 6 bpp)",
 " milisegundos de polling",
 "Teclado (ms de polling)",
 "CPU velocidad (20 ms 50 fps,0 ms sin freno)",
 "OSD (mostrar consumos)",
 "Debug (activar trazas)",
 "Reset (software o hardware)"
};
static const char * gb_help_menu_en[max_gb_help_menu]={
 "F1     - show OSD",
 "ESC    - exit OSD",
 "Arriba - select",
 "Abajo  - select",
 "Enter  - Accept",
 "",
 "GTP (Select or load tape)",
 " Enables WARM START and auto RUN",
 "GAL (Snapshot upload)",
 " Enables auto RUN",
 "Language (Spanish, English)",
 "Video modes (3 bpp and 6 bpp)",
 " milliseconds of polling",
 "Keyboard (ms of polling)",
 "CPU speed (20 ms 50 fps,0 ms without delay)",
 "OSD (show cpu consumption)",
 "Debug (activate traces)",
 "Reset (software or hardware)"
};


#define max_gb_about_menu 11
static const char * gb_about_menu_sp[max_gb_about_menu]={
 "Fecha de publicado: 2024/04/28",
 "",
 "Port del emulador galaksija realizado por",
 "ackerman, basado en x86 pbakota's Miodrag",
 "Jevremovic, Tomaz Solc y Marat Fayzullin.",
 "",
 "ESP32 TTGO VGA32 v1.x sin PSRAM",
 "VGA bitluni (mod Ricardo Massaro)",
 "VGA 3 bpp y 6 bpp",
 "Cambio modo video en inicio o en caliente",
 "Teclado PS/2"
};
static const char * gb_about_menu_en[max_gb_about_menu]={
 "Date Published: 2024/04/28",
 "",
 "Port of the galaksija emulator by",
 "ackerman, based on x86 pbakota's Miodrag",
 "Jevremovic, Tomaz Solc and Marat Fayzullin.",
 "",
 "ESP32 TTGO VGA32 v1.x without  PSRAM",
 "VGA bitluni (mod Ricardo Massaro)",
 "VGA 3 bpp and 6 bpp",
 "Switching video mode on start-up or hot-switching",
 "PS/2 keyboard"
};



 void OSDprintChar(char car,unsigned short int x,unsigned short int y,unsigned char color,unsigned char backcolor,unsigned char isDouble,unsigned char isVertical);
 void OSDprintText(char *cad,unsigned char x, unsigned char y, unsigned char color,unsigned char backcolor,unsigned char isDouble, unsigned char isVertical);
 void OSDprintTextFPS(char *cad,unsigned char x, unsigned char y, unsigned char color,unsigned char backcolor);
 //unsigned char ShowTinyMenu(char *cadTitle,const char **ptrValue,unsigned char idSel,unsigned char aMax);
 unsigned char ShowTinyMenu(char *cadTitle,char **ptrValue,unsigned char idSel,unsigned char aMax);
 void do_tinyOSD(void);
 //void DrawNewOSDMenu(const char **ptr_cad, unsigned char idSel, unsigned char maximo);
 void DrawNewOSDMenu(char **ptr_cad, unsigned char idSel, unsigned char maximo);
 void OSDdrawRectFill(unsigned short int x,unsigned short int y,unsigned short int aWidth, unsigned short int aHeight, unsigned char aColor);
 void WaitFreeKeys(unsigned char isEffect);
 
 void ShowTinyMainMenu(void);
 void ShowTinyLoadGTPMenu(void);
 void ShowTinyLoadGALMenu(void);
 void ShowTinyLanguageMenu(void);
 void ShowTinyVideoOptionsMenu(void);
 void ShowTinyVideoModeMenu(void);
 void ShowTinyVideoPollMenu(void);
 void ShowTinyKeyboardPollMenu(void);
 void ShowTinyCPUMenu(void);
 void ShowTinyOSDMenu(void);
 void ShowTinyDebugMenu(void);
 
 void OSDLoadGALFlash(unsigned short int id);
 void OSDLoadGTPFlash(unsigned short int id);
 void OSDClear(void);
 //void OSDKeepAlive(void);
 void OSDUpdateAnchoAltoVGA(unsigned char modeVideo, unsigned char is8colors);
 //void OSDClearPantallaReal(void);
 void ShowTinyResetMenu(void);
 void ShowTinyAboutMenu(void);
 void ShowTinyHelpMenu(void);
 void ShowTinyInterceptTapeMenu(void); 
 void ShowTinyRAM(void);
 void OSDSubir(unsigned char idSel);
 void OSDBajar(unsigned char idSel);

 #ifdef use_lib_keyboard_uart
  void keyboard_uart_poll(void);  
 #endif
 
#endif

 
 
