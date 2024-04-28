#ifndef _MAIN_H
 #define _MAIN_H
 
 #include "gbConfig.h"
 #include "gbGlobals.h"
 
 unsigned char RdZ80(unsigned short int addr);
 void WrZ80(unsigned short int addr, unsigned char val);
 unsigned char InZ80(unsigned short int port);
 void OutZ80(unsigned short int port, unsigned char val);
 void PatchZ80(Z80 *R);
 //void bitluni_dump_vga(void);
 void LoadGALFlash(const unsigned char *ptrData);
 void read_keyboard(void);
 //#ifdef use_lib_keyboard_uart
 // void read_keyboard_uart(void);
 //#endif 

 void interceptarROMTape(void);
 void PrepareColorsBitluniVGA(void);

#endif
