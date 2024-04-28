#include "gbConfig.h"
#include "io.h"
//#include <SDL/SDL.h>
#include "gbGlobals.h"
#include <string.h> //strlen
#include "gtp.h"
#include <stdio.h>
#include <Arduino.h> //delay
//#include "extarduino.h"

// I/O

#define MAX_INPUT_SIZE 256

//void jj_fast_putpixel(int x,int y,unsigned char c)
//{
// if ((x<0)||(x>639)||(y<0)||(y>399))
//  return;
// Uint8* p = (Uint8*)screen->pixels + (y * screen->pitch) + x;
// *p= c;
//} 

//*********************************************************************************
void draw_char(unsigned char karakter, unsigned short int pozX, unsigned char pozY)
{   
  unsigned char aux;
  unsigned char a0,a1,a2,a3;
  unsigned int a32;
  unsigned short int auxPosByte;

  if (!(karakter < 64))
  {
   if (karakter < 96)
   {
    karakter= karakter-64;
   }
   else
   {
    if (!(karakter < 128))
    {
     if (karakter < 192)
     {
      karakter= (karakter - 128 + 64);
     }
     else
     {
      karakter= (karakter - 192 + 64);
     }
    }    
   }
  } 

  auxPosByte= (karakter<<3)+(karakter<<2)+karakter; //karakter*13 = (karakter x 8)+(karakter x 4)+karakter;
  pozX= (pozX>>2); //DIV 4 32 bits 4 pixels  

  //for (unsigned char n=0;n<13; n++, pozY++)
  //gb_vga_alto_caracter= 12 o 13
  for (unsigned char n=0;n<gb_vga_alto_caracter; n++, pozY++) //correccion parpadeo
  {   
   aux= FONT8_1bit[auxPosByte];
   a0= gb_color_bw[(aux>>7)&0x01];
   a1= gb_color_bw[(aux>>6)&0x01];
   a2= gb_color_bw[(aux>>5)&0x01];
   a3= gb_color_bw[(aux>>4)&0x01];
   //a32= a0 | (a1<<8) | (a2<<16) | (a3<<24); //PC X86
   a32= a2 | (a3<<8) | (a0<<16) | (a1<<24); //ESP32
   gb_buffer_vga32[pozY][pozX]= a32;   
   
   a0= gb_color_bw[(aux>>3)&0x01];
   a1= gb_color_bw[(aux>>2)&0x01];
   a2= gb_color_bw[(aux>>1)&0x01];
   a3= gb_color_bw[(aux>>0)&0x01];
   //a32= a0 | (a1<<8) | (a2<<16) | (a3<<24); //PC X86
   a32= a2 | (a3<<8) | (a0<<16) | (a1<<24); //ESP32
   gb_buffer_vga32[pozY][pozX+1]= a32;
   
   auxPosByte++;   
  }  

/*
    unsigned char tmpX;
    unsigned char n;
    unsigned short int pozK;

    //   0 -  63 => karakter-0
    //  64 -  95 => karakter-64
    //  96 - 127 => karakter-0
    // 128 - ?
    if (karakter < 64)
    {
        pozK = (karakter * VISINA)<<3; //pozK = karakter * VISINA * SIRINA;
    }
    else if (karakter < 96)
    {
        pozK = ((karakter - 64) * VISINA)<<3; //pozK = (karakter - 64) * VISINA * SIRINA;
    }
    else if (karakter < 128)
    {
        pozK = (karakter * VISINA)<<3; //pozK = karakter * VISINA * SIRINA;
    }
    else if (karakter < 192)
    {
        pozK = ((karakter - 128 + 64) * VISINA)<<3; //pozK = (karakter - 128 + 64) * VISINA * SIRINA;
    }
    else
    {
        pozK = ((karakter - 192 + 64) * VISINA)<<3; //pozK = (karakter - 192 + 64) * VISINA * SIRINA;
    }

//JJ Revisar todo esto SDL2 a SDL1

    unsigned int jj_pozK=pozK>>3;

    for (n = 0; n < VISINA; n++, pozY++)
    {
        //JJ byte *ptr = screen->pixels + pozY * screen->pitch + pozX * screen->format->BytesPerPixel;
//        unsigned char *ptr = (unsigned char *) screen->pixels + pozY * screen->pitch + pozX * screen->format->BytesPerPixel;

        //for (tmpX = 0; tmpX < SIRINA; tmpX++)        
        {
            //JJ unsigned char auxDato= (unsigned char)FONT[pozK];
            //unsigned char auxDato= (FONT8[pozK]==0)?0:63; //FONT8 13312 bytes

//revisar            
//            unsigned int auxPosByte= pozK>>3; //Divido 3 optimizacion cutre revisar mas adelante
//            unsigned char auxModByte= pozK&0x07; //resto de 8            
//            unsigned char aux= FONT8_1bit[auxPosByte];
//            unsigned char auxBit= (aux>>(7-auxModByte))&0x01;
//
//            //unsigned char auxDato= (auxBit==0)?0:63; //FONT8_1bit 1664 bytes
//            unsigned char auxDato= (auxBit==0)?0:7; //FONT8_1bit 1664 bytes
//            
//            
//            pozK++;
        
            
//JJ            *(unsigned int *)ptr = FONT[pozK++]; //ya no se usa, directo bitluni
//JJ            ptr += screen->format->BytesPerPixel;
            

            //gb_buffer_vga[pozY][((pozX+tmpX)+gb_vga_display_offset_left)^2]= gb_const_colorNormal[auxDato]; //revisar            


            unsigned int a0,a1,a2,a3,a32;
            unsigned int auxPosByte= jj_pozK; //pozK>>3;
            unsigned char aux= FONT8_1bit[auxPosByte];            
            a0= gb_color_bw[(aux>>7)&0x01];
            a1= gb_color_bw[(aux>>6)&0x01];
            a2= gb_color_bw[(aux>>5)&0x01];
            a3= gb_color_bw[(aux>>4)&0x01];
            a32= a2 | (a3<<8) | (a0<<16) | (a1<<24);
            gb_buffer_vga32[pozY][(pozX>>2)]= a32;

            a0= gb_color_bw[(aux>>3)&0x01];
            a1= gb_color_bw[(aux>>2)&0x01];
            a2= gb_color_bw[(aux>>1)&0x01];
            a3= gb_color_bw[(aux>>0)&0x01];
            a32= a2 | (a3<<8) | (a0<<16) | (a1<<24);
            gb_buffer_vga32[pozY][((pozX+4)>>2)]= a32;

            jj_pozK++;           

        }
    }
    
        
//    for (int y=0;y<200;y++)
//    {     
//     for (int x=0;x<320;x++)
//     {
//      jj_fast_putpixel(x,y,x);
//     }
//    }
    
//    printf("char\n");
//    fflush(stdout);
*/
}

//void draw_cursor(int pos)
//{
// revisar
//    SDL_Rect r = {pos * SIRINA, VISINA - 1, SIRINA, 1};
//    SDL_FillRect(screen, &r, SDL_MapRGBA(screen->format, 128, 0, 0, 255));
//    
//}

//SDL2 a SDL1 no implementado
//void SDL_StartTextInput()
//{
//}

//SDL2 a SDL1 no implementado
//revisar
//void SDL_SetTextInputRect(SDL_Rect * aux)
//{
//}


//SDL2 a SDL1 no implementado
//void SDL_StopTextInput()
//{
//}


/*
int input_text(const char *prompt, char *input_buffer)
{
    printf("input_text\n");
    //fflush(stdout);
    
    int input_finished = 0;
    int prompt_len = strlen(prompt), pos = strlen(input_buffer), max_len = 32 - prompt_len - 1;

//revisar    SDL_Rect input_rect = {0, 0, max_len * SIRINA, VISINA};

    SDL_StartTextInput();
//revisar    SDL_SetTextInputRect(&input_rect);

    while (input_finished == 0)
    {
        //JJ SDL_FillRect(screen, NULL, crna_color);
        //JJ SDL_LockSurface(screen);

        text_at(prompt, 0, 0);
        text_at(input_buffer, prompt_len, 0);

        //JJ SDL_UnlockSurface(screen);

        draw_cursor(prompt_len + pos);

//        revisar
//        SDL_Event event;
//        while (SDL_PollEvent(&event))
//        {
//            switch (event.type)
//            {
//            case SDL_QUIT:
//                ExitLoop = 1;
//                input_finished = 2;
//                break;
//            
//            //SDL2 a SDL1 no implementado    
//            //JJ case SDL_TEXTINPUT:
//            //JJ     if (strlen(input_buffer) < max_len)
//            //JJ     {
//            //JJ         int len = strlen(input_buffer);
//            //JJ         for (int i = len - 1; i >= pos; --i)
//            //JJ         {
//            //JJ             input_buffer[i + 1] = input_buffer[i];
//            //JJ         }
//            //JJ         input_buffer[pos++] = toupper(event.text.text[0]);
//            //JJ         input_buffer[len + 1] = 0;
//            //JJ     }
//            //JJ     break;
//            
//            case SDL_KEYDOWN:
//            {
//                int c;
//                switch (event.key.keysym.sym)
//                {
//                case SDLK_ESCAPE:
//                    c = 0x1b;
//                    break;
//                case SDLK_RETURN:
//                    c = 0x0a;
//                    break;
//                case SDLK_DELETE:
//                    c = 0x7f;
//                    break;
//                case SDLK_BACKSPACE:
//                    c = 0x08;
//                    break;
//                case SDLK_LEFT:
//                    c = 1;
//                    break;
//                case SDLK_RIGHT:
//                    c = 2;
//                    break;
//                //
//                default:
//                    c = 0;
//                    break; // unsupported key
//                }
//
//                switch (c)
//                {
//                case 0x1b: // ESCAPE
//                    input_finished = 2;
//                    break;
//                case 0x0a: // RETURN
//                    input_finished = 1;
//                    break;
//                case 0x7f: // DELETE
//                    if (pos >= 0 && pos < strlen(input_buffer))
//                    {
//                        //JJ memcpy((void *)input_buffer + pos, (void *)input_buffer + pos + 1, strlen(input_buffer) - pos);
//                        memcpy((unsigned char *)input_buffer + pos, (unsigned char *)input_buffer + pos + 1, strlen(input_buffer) - pos);
//                    }
//                    break;
//                case 0x08: // BACKSPACE
//                    if (pos > 0)
//                    {
//                        //JJ memcpy((void *)input_buffer + pos - 1, (void *)input_buffer + pos, strlen(input_buffer) - pos + 1);
//                        memcpy((unsigned char *)input_buffer + pos - 1, (unsigned char *)input_buffer + pos, strlen(input_buffer) - pos + 1);
//                        pos--;
//                    }
//                    break;
//                case 1:
//                    if (pos > 0)
//                        pos--;
//                    break;
//                case 2:
//                    if (pos < strlen(input_buffer))
//                        pos++;
//                    break;
//                }
//            }
//            break;
//            }
//        }
        

        //Select the color for drawing.
        //JJ SDL_SetRenderDrawColor(renderer, 0, 0, 128, 255); //SDL2 a SDL1 no implementado

        // Clear the window
        //JJ SDL_RenderClear(renderer); //SDL2 a SDL1 no implementado
        //JJ SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, screen); //SDL2 a SDL1 no implementado

        #ifdef use_lib_ram_8KB
         int xOff = (int)MEMORY[0x2BA8-8192] * 8;
        #else
         int xOff = (int)MEMORY[0x2BA8] * 8;
        #endif 

//        revisar
//        SDL_Rect src = {0, 0, SCREEN_W, SCREEN_H},
//                 dst = {xOff + (WINDOW_W - SCREEN_W * 2) / 2 - 88, (WINDOW_H - SCREEN_H * 2) / 2, 2 * SCREEN_W, 2 * SCREEN_H};
        

        //JJ SDL_RenderCopy(renderer, tex, &src, &dst); //SDL2 a SDL1 no implementado

        // Render the changes above ( which up until now had just happened behind the scenes )
        //JJ SDL_RenderPresent(renderer); //SDL2 a SDL1 no implementado
        //JJ SDL_DestroyTexture(tex); //SDL2 a SDL1 no implementado

        delay(1000/50); //revisar delay(1000/50); //SDL_Delay(1000 / 50);
    }

    SDL_StopTextInput(); 

    return input_finished - 1;
}
*/

/*
//JJ int input_number(const char *prompt, word *number)
int input_number(const char *prompt, unsigned short int *number)
{
//    revisar 
//    char number_buf[MAX_INPUT_SIZE];
//    snprintf(number_buf, sizeof(number_buf) - 1, "%04X", *number);
//
//    int result = input_text(prompt, &number_buf[0]);
//
//    if (result == 0)
//    {
//        //JJ *number = (word)strtol(&number_buf[0], NULL, 16);
//        *number = (unsigned short int)strtol(&number_buf[0], NULL, 16);
//    }
//
//    return result;
    
   return 0;
}
*/

//const char *build_path(const char *dirname, const char *filename)
//{
//    static char path_buf[MAX_INPUT_SIZE * 2] = {'\0'};
//
//    memcpy((void *)path_buf, (void *)dirname, strlen(dirname) + 1);
//
//#if defined(__WIN32__)
//    strcat(path_buf, "\\");
//#else
//    strcat(path_buf, "/");
//#endif
//    strcat(path_buf, filename);
//
//    return (const char *)&path_buf[0];
//}

/*
void save_memory()
{
    // By default save BASIC program
    unsigned short int start = 0x2c36;//JJ word start = 0x2c36;
    #ifdef use_lib_ram_8KB
     unsigned short int end = MEMORY[0x2c38-8192] + MEMORY[0x2c39-8192] * 256; //JJ word end = MEMORY[0x2c38] + MEMORY[0x2c39] * 256;
    #else
     unsigned short int end = MEMORY[0x2c38] + MEMORY[0x2c39] * 256; //JJ word end = MEMORY[0x2c38] + MEMORY[0x2c39] * 256;
    #endif 

    char filename[MAX_INPUT_SIZE / 2];

    if (input_number("START ADDR:", &start) != 0)
        return;
    if (input_number("END ADDR:", &end) != 0)
        return;

    if (input_text("SAVE FILE:", filename) != 0 || strlen(filename) == 0)
        return;

    const char *path = build_path(workdir, filename);
    printf("Saving memory to \"%s\", start=&%04X, end=&%04X\n", path, start, end);

    if (save_gtp_file(path, MEMORY, start, end) != 0)
    {
        fprintf(stderr, "Save error!\n");
    }
    else
    {
        fprintf(stderr, "Memory saved.\n");
    }
}
*/

/*
void load_memory()
{
 printf("load_memory BEGIN\n");
 //fflush(stdout);
    char filename[MAX_INPUT_SIZE];
    char yn[2] = {'Y', '\0'};
    int warmstart = 0;

    //fuerzo filename
    strcpy(filename,"ASTEROID.GTP");
    printf("Fuerzo programa:%s\n",filename);
    //fflush(stdout);
    
    //no lo necesito
    //JJ if (input_text("LOAD FILE:", filename) != 0 || strlen(filename) == 0)
    //JJ {
    //JJ return;
    //JJ}
        
    strcpy(yn,"Y");
    printf("Fuerzo WARM Y:%s\n",yn);
    //fflush(stdout);   
         
    //Lo necesito
    //JJ if (input_text("BASIC WARM START (Y/N)?", yn) != 0)
    //JJ {
    //JJ  return;
    //JJ }

    warmstart = (yn[0] == 'Y');
    
    printf("warmstart %d\n",warmstart);
    //(stdout);       

    const char *path = build_path(workdir, filename);
    if (load_gtp_file(path, MEMORY, 0) == 0)
    {
        if (warmstart)
        {
            //printf("Jump to basic warmstart ...\n");
            printf("Jump to basic warmstart ...\n");
            //fflush(stdout);
            R.PC.W = 0x0317; // BASIC WARM START
        }
    }
 printf("load_memory END\n");    
 //fflush(stdout);
}
*/





//void text_at(const char *str, unsigned short int x, unsigned char y)
//{
//    while (*str)
//    {
//        if (x > 31)
//        {
//            x -= 32;
//            y++;
//        }
//
//        draw_char(*str++, x++ * SIRINA, y * VISINA);
//    }
//}
