#ifndef _GB_CONFIG_H
 #define _GB_CONFIG_H

 //Important, is little endian (do not modify)
 #define LSB_FIRST 
  
 //use bilutni custom fast (do not modify)
 #define use_lib_tinybitluni_fast

 //Not use double numbers (IEEE not fpu) calculate vga frequency
 //#define use_lib_fix_double_precision

 //Debug i2s bitluni vga frequency
 #define use_lib_debug_i2s

 //Video mode boot (select only one option)
 #define use_lib_boot_vga_360x200x70hz_bitluni_3bpp
 //#define use_lib_boot_vga_360x200x70hz_bitluni_apll_3bpp
 //#define use_lib_boot_vga_320x200x70hz_bitluni_3bpp
 //#define use_lib_boot_vga_320x200x70hz_fabgl_3bpp
 //#define use_lib_boot_vga_320x200x70hz_bitluni_apll_3bpp
 //#define use_lib_boot_vga_320x240x60hz_bitluni_3bpp
 //#define use_lib_boot_vga_320x240x60hz_fabgl_3bpp
 //#define use_lib_boot_vga_320x240x60hz_bitluni_apll_3bpp
 //#define use_lib_boot_vga_360x200x70hz_bitluni_6bpp
 //#define use_lib_boot_vga_360x200x70hz_bitluni_apll_6bpp
 //#define use_lib_boot_vga_320x200x70hz_bitluni_6bpp
 //#define use_lib_boot_vga_320x200x70hz_fabgl_6bpp
 //#define use_lib_boot_vga_320x200x70hz_bitluni_apll_6bpp
 //#define use_lib_boot_vga_320x240x60hz_bitluni_6bpp
 //#define use_lib_boot_vga_320x240x60hz_fabgl_6bpp
 //#define use_lib_boot_vga_320x240x60hz_bitluni_apll_6bpp 
 

 //Time to select video mode at startup (ms)
 #define use_lib_boot_time_select_vga 500

 //Important (do not modify)
 #define use_lib_skip_FD_error

 //Put ROM in flash (free 8192 bytes RAM)
 #define use_lib_rom_flash
 
 //MEMORY uses 8 KN instead of 64 KB RAM
 #define use_lib_ram_8KB


 //log trace
 #define use_lib_log_serial

 //keyboard
 //#define use_lib_keyboard_uart

 //timeout read millis
 #define use_lib_keyboard_uart_timeout 0

 //log keyboard serial
 //#define use_lib_log_keyboard_uart


 //Proyect ArduinoDroid
 //#gb_use_lib_compile_arduinodroid








 //Internal (do not modify)
 #ifdef use_lib_boot_vga_360x200x70hz_bitluni_3bpp
  #define use_lib_vga8colors
  #define use_lib_vga360x200x70hz_bitluni
 #else
  #ifdef use_lib_boot_vga_360x200x70hz_bitluni_apll_3bpp
   #define use_lib_vga8colors
   #define use_lib_vga360x200x70hz_bitluni_apll_fix
  #else
   #ifdef use_lib_boot_vga_320x200x70hz_bitluni_3bpp
    #define use_lib_vga8colors
    #define use_lib_vga320x200x70hz_bitluni
   #else
    #ifdef use_lib_boot_vga_320x200x70hz_fabgl_3bpp
     #define use_lib_vga8colors
     #define use_lib_vga320x200x70hz_fabgl
    #else
     #ifdef use_lib_boot_vga_320x200x70hz_bitluni_apll_3bpp
      #define use_lib_vga8colors
      #define use_lib_vga320x200x70hz_bitluni_apll_fix
     #else
      #ifdef use_lib_boot_vga_320x240x60hz_bitluni_3bpp
       #define use_lib_vga8colors
       #define use_lib_vga320x240x60hz_bitluni
      #else
       #ifdef use_lib_boot_vga_320x240x60hz_fabgl_3bpp
        #define use_lib_vga8colors
        #define use_lib_vga320x240x60hz_fabgl
       #else
        #ifdef use_lib_boot_vga_320x240x60hz_bitluni_apll_3bpp
         #define use_lib_vga8colors
         #define use_lib_vga320x240x60hz_bitluni_apll_fix
         #else
          #ifdef use_lib_boot_vga_360x200x70hz_bitluni_6bpp           
           #define use_lib_vga360x200x70hz_bitluni
          #else 
           #ifdef use_lib_boot_vga_360x200x70hz_bitluni_apll_6bpp
            #define use_lib_vga360x200x70hz_bitluni_apll_fix
           #else
            #ifdef use_lib_boot_vga_320x200x70hz_bitluni_6bpp
             #define use_lib_vga320x200x70hz_bitluni
            #else
             #ifdef use_lib_boot_vga_320x200x70hz_fabgl_6bpp
              #define use_lib_vga320x200x70hz_fabgl
             #else
              #ifdef use_lib_boot_vga_320x200x70hz_bitluni_apll_6bpp
               #define use_lib_vga320x200x70hz_bitluni_apll_fix
              #else
               #ifdef use_lib_boot_vga_320x240x60hz_bitluni_6bpp
                #define use_lib_vga320x240x60hz_bitluni
               #else
                #ifdef use_lib_boot_vga_320x240x60hz_fabgl_6bpp
                 #define use_lib_vga320x240x60hz_fabgl
                #else
                 #ifdef use_lib_boot_vga_320x240x60hz_bitluni_apll_6bpp
                  #define use_lib_vga320x240x60hz_bitluni_apll_fix
                 #endif
                #endif 
               #endif 
              #endif
             #endif 
            #endif
           #endif
          #endif
        #endif
       #endif 
      #endif 
     #endif
    #endif
   #endif
  #endif
 #endif







/*
 ////Mode 3bits 8 colors
 //#define use_lib_vga8colors

 //Modo video 
 //Select one mode and deselect other modes
 //360x200 720x400 31.3 Khz 70.3 Hz freq:28322000 pixel_clock:14161000
 //#define use_lib_vga360x200x70hz_bitluni
 //360x200 720x400 31.3 Khz 70.3 Hz freq:28322000 pixel_clock:14161000 FIX PLL LOCK ESP32
 //#define use_lib_vga360x200x70hz_bitluni_apll_fix

 //320x200 720x400 31.4 Khz 70.0 Hz freq:25175000 pixel_clock:12587500
 //#define use_lib_vga320x200x70hz_bitluni
 //320x200 70Hz freq:12587500 funciona
 //#define use_lib_vga320x200x70hz_fabgl
 //320x200 720x400 31.4 Khz 70.0 Hz freq:25175000 pixel_clock:12587500 FIX PLL LOCK ESP32
 //#define use_lib_vga320x200x70hz_bitluni_apll_fix
 
 
 //320x240 640x480 31.4 Khz 60 Hz freq:25175000 pixel_clock:12587500 
 #define use_lib_vga320x240x60hz_bitluni
 //QVGA 320x240 60Hz freq:12600000 funciona
 //#define use_lib_vga320x240x60hz_fabgl
 //320x240 640x480 31.4 Khz 60 Hz freq:25175000 pixel_clock:12587500 FIX PLL LOCK ESP32
 //#define use_lib_vga320x240x60hz_bitluni_apll_fix
*/
 

 #if defined(use_lib_vga360x200x70hz_bitluni) || defined(use_lib_vga360x200x70hz_bitluni_apll_fix)
  #define use_lib_360x200
 #else
  #if defined(use_lib_vga320x200x70hz_bitluni) || defined(use_lib_vga320x200x70hz_fabgl) || defined(use_lib_vga320x200x70hz_bitluni_apll_fix)
   #define use_lib_320x200
  #else
   #if defined(use_lib_vga320x240x60hz_bitluni) || defined(use_lib_vga320x240x60hz_fabgl) || defined(use_lib_vga320x240x60hz_bitluni_apll_fix)
    #define use_lib_320x240
   #endif
  #endif
 #endif


 /*No se usa
 //Modo video
 //#define use_lib_360x200
 //#define use_lib_320x200
 #define use_lib_320x240
 */
 

 //#define use_lib_cont_call
 //#define use_lib_skip_fps
 

 

 
#endif
 
