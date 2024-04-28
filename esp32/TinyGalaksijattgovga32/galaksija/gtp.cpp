#include "gbConfig.h"
//#include <SDL/SDL.h>
#include "gtp.h"
#include "stdio.h"
#include "gbGlobals.h"
#include <string.h> //memcpy
#include <Arduino.h>

typedef struct
{
    //JJ Uint8 type;
    //JJ Uint32 len;
 unsigned char type;
 unsigned int len;
} GTP_header;

typedef struct
{
 unsigned char magic;   //JJ Uint8 magic; // 0xa5
 unsigned short int start; //JJ  Uint16 start;
 unsigned short int end;//JJ   Uint16 end;
 unsigned char data[0]; //JJ   Uint8 data[0];
    // crc.w at the end
} GTP_datablock;

int load_gtp_file(const char *gtp_file, void *memory, int block)
{
/*    
 printf("load_gtp_file BEGIN\n");
 fflush(stdout);
 
    unsigned short int bcnt = 0;//JJ Uint16 bcnt = 0;
    //JJ SDL_RWops *file = SDL_RWFromFile(gtp_file, "rb"); //SDL2 a SDL1
    FILE * file = fopen(gtp_file, "rb");

    if (file == NULL)
    {
        //fprintf(stderr, "Unable to open GTP file \"%s\"\n", gtp_file);
        printf("Unable to open GTP file \"%s\"\n", gtp_file);
        fflush(stdout);
        return -1;
    }

    //Sint64 total_fsize = SDL_RWsize(file); //SDL2 a SDL1
    
    fseek(file,0, SEEK_END); //SDL2 a SDL1
    Sint64 total_fsize = ftell(file); //SDL2 a SDL1
    fseek(file,0, SEEK_SET); //SDL2 a SDL1
    
    //fprintf(stderr, "Load size=0x%08lx\n", total_fsize);
    printf("Load size=%d\n", total_fsize);
    fflush(stdout);

    //while (SDL_RWtell(file) < total_fsize)
    while (ftell(file) < total_fsize) //SDL2 a SDL1
    {
        GTP_header hdr;
        fread(&(hdr.type),1,1,file); //JJ hdr.type = SDL_ReadU8(file); //SDL2 a SDL1
        fread(&(hdr.len),4,1,file); //JJ hdr.len = SDL_ReadLE32(file); //SDL2 a SDL1

        Sint64 block_start_offset = ftell(file); //JJ Sint64 block_start_offset = SDL_RWtell(file); //SDL2 a SDL1

        //fprintf(stderr, "%d. Block type: 0x%02x, OFFSET=0x%08lx LEN=0x%04x\n", (int)bcnt, (int)hdr.type, block_start_offset, (int)hdr.len);
        printf("%d. Block type: 0x%02x, OFFSET=0x%08lx LEN=0x%04x\n", (int)bcnt, (int)hdr.type, block_start_offset, (int)hdr.len);
        fflush(stdout);
        if (block_start_offset == 0)
        {
            //fprintf(stderr, "Could not read file!\n");
            printf("Could not read file!\n");
            fflush(stdout);
            break;
        }

        switch (hdr.type)
        {
        case 0: // Normal block
        case 1: // Turbo block
        {
            GTP_datablock blk;
            fread(&(blk.magic),1,1,file); //blk.magic = SDL_ReadU8(file); //SDL2 a SDL1
            fread(&(blk.start),2,1,file); //blk.start = SDL_ReadLE16(file); //SDL2 a SDL1
            fread(&(blk.end),2,1,file); //blk.end = SDL_ReadLE16(file); //SDL2 a SDL1

            if (blk.magic != 0xa5)
            {
                //fprintf(stderr, "   ERROR: Sync 0xa5 was not found!\n");
                printf("   ERROR: Sync 0xa5 was not found!\n");
                fflush(stdout);
                break;
            }

            #ifdef use_lib_ram_8KB
             fread((unsigned char *)memory + blk.start - 8192,(size_t)(blk.end - blk.start),1,file);  //SDL_RWread(file, (unsigned char *)memory + blk.start, (size_t)(blk.end - blk.start), 1); //JJ SDL_RWread(file, memory + blk.start, (size_t)(blk.end - blk.start), 1); //SDL2 a SDL1
            #else
             fread((unsigned char *)memory + blk.start,(size_t)(blk.end - blk.start),1,file);  //SDL_RWread(file, (unsigned char *)memory + blk.start, (size_t)(blk.end - blk.start), 1); //JJ SDL_RWread(file, memory + blk.start, (size_t)(blk.end - blk.start), 1); //SDL2 a SDL1
            #endif 
            unsigned char crc=0;
            fread(&(crc),1,1,file);  //unsigned char crc = SDL_ReadU8(file); //JJ byte crc = SDL_ReadU8(file); //SDL2 a SDL1
            unsigned char otro=0;
            fread(&(otro),1,1,file); //SDL_ReadU8(file); // checksum + 1 garbage byte //SDL2 a SDL1

            unsigned char chk = 0xa5 + ((blk.start >> 8) & 0xff) + ((blk.start) & 0xff) + ((blk.end >> 8) & 0xff) + ((blk.end) & 0xff); //JJ byte chk = 0xa5 + ((blk.start >> 8) & 0xff) + ((blk.start) & 0xff) + ((blk.end >> 8) & 0xff) + ((blk.end) & 0xff);
            for (int i = blk.start; i < blk.end; ++i)
            {
             #ifdef use_lib_ram_8KB
              chk += *((unsigned char *)memory + i-8192); //JJ chk += *((byte *)memory + i);
             #else
              chk += *((unsigned char *)memory + i); //JJ chk += *((byte *)memory + i);
             #endif 
            }

            //fprintf(stderr, "   START=0x%04x END=0x%04x CRC=0x%02x POS=0x%08lx\n",(int)blk.start, (int)blk.end, (int)crc, ftell(file)); //fprintf(stderr, "   START=0x%04x END=0x%04x CRC=0x%02x POS=0x%08lx\n",(int)blk.start, (int)blk.end, (int)crc, SDL_RWtell(file)); //SDL2 a SDL1
            printf("   START=0x%04x END=0x%04x CRC=0x%02x POS=0x%08lx\n",(int)blk.start, (int)blk.end, (int)crc, ftell(file)); //fprintf(gb_log,"   START=0x%04x END=0x%04x CRC=0x%02x POS=0x%08lx\n",(int)blk.start, (int)blk.end, (int)crc, SDL_RWtell(file)); //SDL2 a SDL1
            fflush(stdout);

            if (((chk + crc) & 0xff) != 0xff)
            {
                //fprintf(stderr, "   LOAD ERROR: 0x%02x + 0x%02x != 0xff\n", (int)crc, (int)chk);
                printf("   LOAD ERROR: 0x%02x + 0x%02x != 0xff\n", (int)crc, (int)chk);
                fflush(stdout);
            }
        }
        break;
        case 16: // Name block
        {
            char buffer[256];
            fread(buffer, 1, hdr.len, file); //SDL_RWread(file, buffer, 1, hdr.len); //SDL2 a SDL1
            //fprintf(stderr, "   FILE=\"%s\" POS=0x%08lx\n", buffer, ftell(file)); //fprintf(stderr, "   FILE=\"%s\" POS=0x%08lx\n", buffer, SDL_RWtell(file)); //SDL2 a SDL1
            printf("   FILE=\"%s\" POS=0x%08lx\n", buffer, ftell(file)); //fprintf(gb_log,"   FILE=\"%s\" POS=0x%08lx\n", buffer, SDL_RWtell(file)); //SDL2 a SDL1
            fflush(stdout);
        }
        break;
        default:
            //fprintf(stderr, "WARNING: Unknown block type 0x%02x\n", (int)hdr.type);
            printf("WARNING: Unknown block type 0x%02x\n", (int)hdr.type);
            fflush(stdout);
            break;
        }

        // Skip to next block
        fseek(file,SEEK_SET,block_start_offset + hdr.len); //SDL_RWseek(file, block_start_offset + hdr.len, RW_SEEK_SET);
        ++bcnt;
    }

    fclose(file); //SDL_RWclose(file); //SDL2 a SDL1
    
 printf("load_gtp_file END\n");    
 fflush(stdout);
 */
    return 0;
}


//******************************************************************************
void LoadGTPFlash(const unsigned char *ptrData, unsigned short int ptrSize)
{
 unsigned int a0,a1,a2,a3, aDword, adword;
 unsigned short int aword;
     
 unsigned short int fcont=0;
 if (gb_use_debug==1){ Serial.printf("LoadGTPFlash BEGIN\r\n"); }
 
 unsigned short int bcnt = 0;
 unsigned short int total_fsize = ptrSize; //revisar Sint64 total_fsize = ptrSize;

 if (gb_use_debug==1){ Serial.printf("Load size=%d\r\n", total_fsize); }
 //fflush(stdout); 
 
 
    while (fcont < total_fsize) //SDL2 a SDL1
    {
        GTP_header hdr;
        hdr.type= ptrData[fcont++];
                
        a0= ptrData[fcont++]&0xFF;
        a1= ptrData[fcont++]&0xFF;
        a2= ptrData[fcont++]&0xFF;
        a3= ptrData[fcont++]&0xFF;
        adword= (a3<<24)|(a2<<16)|(a1<<8)|a0;
        
        hdr.len= adword;        

        unsigned short int block_start_offset = fcont; //revisar Sint64 block_start_offset = fcont;

        //fprintf(stderr, "%d. Block type: 0x%02x, OFFSET=0x%08lx LEN=0x%04x\n", (int)bcnt, (int)hdr.type, block_start_offset, (int)hdr.len);
        if (gb_use_debug==1){ Serial.printf("%d. Block type: 0x%02x, OFFSET=0x%08lx LEN=0x%04x\r\n", (int)bcnt, (int)hdr.type, block_start_offset, (int)hdr.len); }
        //fflush(stdout);
        if (block_start_offset == 0)
        {
            //fprintf(stderr, "Could not read file!\n");
            if (gb_use_debug==1){ Serial.printf("Could not read file!\r\n"); }
            //fflush(stdout);
            break;
        }

        switch (hdr.type)
        {
        case 0: // Normal block
        case 1: // Turbo block
        {
            GTP_datablock blk;
            blk.magic= ptrData[fcont++];
            
            a0= ptrData[fcont++]&0xFF;
            a1= ptrData[fcont++]&0xFF;
            aword= (a1<<8)|a0;
            blk.start= aword;

            a0= ptrData[fcont++]&0xFF;
            a1= ptrData[fcont++]&0xFF;
            aword= (a1<<8)|a0;
            
            blk.end= aword;

            if (blk.magic != 0xa5)
            {
                //fprintf(stderr, "   ERROR: Sync 0xa5 was not found!\n");
                if (gb_use_debug==1){ Serial.printf("ERROR: Sync 0xa5 was not found!\r\n"); }
                //fflush(stdout);
                break;
            }

            #ifdef use_lib_ram_8KB
             memcpy((unsigned char *)MEMORY + blk.start - 8192, &ptrData[fcont], (blk.end - blk.start));
             fcont+= (blk.end - blk.start);
            #else
             memcpy((unsigned char *)MEMORY + blk.start, &ptrData[fcont], (blk.end - blk.start));
             fcont+= (blk.end - blk.start);
            #endif 
            unsigned char crc=0;
            crc= ptrData[fcont++];
            unsigned char otro=0;
            otro= ptrData[fcont++]; 

            unsigned char chk = 0xa5 + ((blk.start >> 8) & 0xff) + ((blk.start) & 0xff) + ((blk.end >> 8) & 0xff) + ((blk.end) & 0xff); //JJ byte chk = 0xa5 + ((blk.start >> 8) & 0xff) + ((blk.start) & 0xff) + ((blk.end >> 8) & 0xff) + ((blk.end) & 0xff);
            for (int i = blk.start; i < blk.end; ++i)
            {
             #ifdef use_lib_ram_8KB
              chk += *((unsigned char *)MEMORY + i-8192); //JJ chk += *((byte *)memory + i);
             #else
              chk += *((unsigned char *)MEMORY + i); //JJ chk += *((byte *)memory + i);
             #endif 
            }

            //fprintf(stderr, "   START=0x%04x END=0x%04x CRC=0x%02x POS=0x%08lx\n",(int)blk.start, (int)blk.end, (int)crc, ftell(file)); //fprintf(stderr, "   START=0x%04x END=0x%04x CRC=0x%02x POS=0x%08lx\n",(int)blk.start, (int)blk.end, (int)crc, SDL_RWtell(file)); //SDL2 a SDL1
            if (gb_use_debug==1){ Serial.printf("START=0x%04x END=0x%04x CRC=0x%02x POS=0x%08lx\r\n",(int)blk.start, (int)blk.end, (int)crc, fcont); } //fprintf(gb_log,"   START=0x%04x END=0x%04x CRC=0x%02x POS=0x%08lx\n",(int)blk.start, (int)blk.end, (int)crc, SDL_RWtell(file)); //SDL2 a SDL1
            //fflush(stdout);

            if (((chk + crc) & 0xff) != 0xff)
            {
                //fprintf(stderr, "   LOAD ERROR: 0x%02x + 0x%02x != 0xff\n", (int)crc, (int)chk);
                if (gb_use_debug==1){ Serial.printf("LOAD ERROR: 0x%02x + 0x%02x != 0xff\r\n", (int)crc, (int)chk); }
                //fflush(stdout);
            }
        }
        break;
        case 16: // Name block
        {
            char buffer[256];
            memcpy(buffer, &ptrData[fcont], hdr.len);
            fcont += hdr.len;
            //fprintf(stderr, "   FILE=\"%s\" POS=0x%08lx\n", buffer, ftell(file)); //fprintf(stderr, "   FILE=\"%s\" POS=0x%08lx\n", buffer, SDL_RWtell(file)); //SDL2 a SDL1
            if (gb_use_debug==1){ Serial.printf("FILE=\"%s\" POS=0x%08lx\r\n", buffer, fcont); } //fprintf(gb_log,"   FILE=\"%s\" POS=0x%08lx\n", buffer, SDL_RWtell(file)); //SDL2 a SDL1
            //fflush(stdout);
        }
        break;
        default:
            //fprintf(stderr, "WARNING: Unknown block type 0x%02x\n", (int)hdr.type);
            if (gb_use_debug==1){ Serial.printf("WARNING: Unknown block type 0x%02x\r\n", (int)hdr.type); }
            //fflush(stdout);
            break;
        }

        // Skip to next block
        fcont= block_start_offset + hdr.len;
        ++bcnt;
    } 
 
 
 //gb_id_menu_cur= id_menu_main;
 
 if (gb_use_debug==1){ Serial.printf("LoadGTPFlash END\r\n"); }
 //fflush(stdout);
}


//******************************************************************************
//JJ int save_gtp_file(const char *gtp_file, void *memory, word start, word end)
int save_gtp_file(const char *gtp_file, void *memory, unsigned short int start, unsigned short int end)
{
/*revisar    
    FILE *file= fopen(gtp_file,"wb"); //SDL_RWops *file = SDL_RWFromFile(gtp_file, "wb"); //SDL2 a SDL1

    if (file == NULL)
    {
        printf("Unable to write GTP file \"%s\"\n", gtp_file);
        //fflush(stdout);
        return -1;
    }

    GTP_header hdr = {0, end - start + sizeof(GTP_datablock) + 3};
    fwrite(&(hdr.type),1,1,file); //SDL_WriteU8(file, hdr.type); //SDL2 a SDL1
    fwrite(&(hdr.len),4,1,file); //SDL_WriteLE32(file, hdr.len); //SDL2 a SDL1

    unsigned char crc = 0xa5; //JJ byte crc = 0xa5;
    crc += (start & 0xff);
    crc += ((start >> 8) & 0xff);
    crc += (end & 0xff);
    crc += ((end >> 8) & 0xff);

    for (int i = start; i < end; ++i)
    {
     #ifdef use_lib_ram_8KB
      crc += *((unsigned char *)memory + i-8192); //JJ crc += *((byte *)memory + i);
     #else
      crc += *((unsigned char *)memory + i); //JJ crc += *((byte *)memory + i);
     #endif 
    }

    unsigned char dato=0xa5;
    fwrite(&dato,1,1,file); //SDL_WriteU8(file, 0xa5); // magic //SDL2 a SDL1
    fwrite(&start,2,1,file); //SDL_WriteLE16(file, start); //SDL2 a SDL1
    fwrite(&end,2,1,file); //SDL_WriteLE16(file, end); //SDL2 a SDL1

    #ifdef use_lib_ram_8KB
     fwrite((unsigned char*)memory + start-8192, end - start, 1, file); //SDL_RWwrite(file, (unsigned char*)memory + start, end - start, 1); //JJ SDL_RWwrite(file, memory + start, end - start, 1); //SDL2 a SDL1
    #else
     fwrite((unsigned char*)memory + start, end - start, 1, file); //SDL_RWwrite(file, (unsigned char*)memory + start, end - start, 1); //JJ SDL_RWwrite(file, memory + start, end - start, 1); //SDL2 a SDL1
    #endif 
    dato= (unsigned char)(0xff - crc);
    fwrite(&dato,1,1,file); //SDL_WriteU8(file, (unsigned char)(0xff - crc)); // crc //JJ SDL_WriteU8(file, (byte)(0xff - crc)); // crc //SDL2 a SDL1
    dato= 0xff;
    fwrite(&dato,1,1,file); //SDL_WriteU8(file, 0xff);               // garbage //SDL2 a SDL1

    fclose(file); //SDL_RWclose(file); //SDL2 a SDL1
*/    
    return 0;
}
