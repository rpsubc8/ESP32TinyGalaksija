//Author: ackerman
//Galaksija tool
//Convert GTP Tape to .h
//        GAL SNAPSHOT to .h
//Max 64 chars file name
//Show 32 chars title osd
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#define maxObjects 255
//64 caracteres maximo fichero
#define max_cad_title 64
//Solo muestra 32 caracteres titulo
#define max_osd_real_title 32

#define max_char_file_name 64

#define tipo_gtp 0
#define tipo_gal 1


//unsigned char gb_bufferFile[524288]; //512 KB archivo maximo no deberia pasar de 384 KB

unsigned char gb_bufferFile[0xFFFF]; //64 KB archivo maximo no deberia pasar de 64 KB
unsigned char gb_global_tipo=0; //0 GTP   1 GAL


void listFilesRecursively(char *basePath);
void ShowHelp(void);
void WriteHeadGeneric_H(char *cadDefine);
void WriteFileHEX(unsigned char num,char *cadPath,char *cadFile, char *cadFileSource,char *tipoPath,char *tipo,char *defTipo);
void WriteSizeGeneric(char *cadTipo);
int GetSizeFile(char *cadFile);
void InitTitles(void);
void RemoveExt(char *cad);
void ProcesaFicherosGeneric(char *cadFileWrite, char *cadpath, char *cadHead, char *cadTipo);
void WriteArrayHexFile(char *cadFileOrigen, FILE *fileDestino);
void WriteHeadArduinoDroid_H(char *cadDefine);
void ProcesaArduinoDroid(char *cadFileWrite, char *cadHead);

FILE *gb_fileWrite = NULL;
char gb_titles[maxObjects][max_char_file_name];
char gb_nameFiles[maxObjects][max_char_file_name];
char gb_nameDir[maxObjects][max_char_file_name];
unsigned char gb_contRom=0;

//***********************************
void RemoveExt(char *cad)
{
 int total= strlen(cad);
 if (total > max_cad_title)
 {
  total= max_cad_title;
  cad[max_cad_title]='\0';
 }
 for (int i=0;i<total;i++)
 {
  if (cad[i]==' ')   
   cad[i] = '_';
  else
  {
   if (cad[i]=='.')
   {
    cad[i]='\0';
    return;
   }
  }
 }
}

//**********************************************
void InitTitles()
{
 for (unsigned char i=0;i<maxObjects;i++)
  gb_titles[i][0]='\0';
}

//**********************************************
void ShowHelp()
{
 printf("Galaksija DATA tool\n");
 printf("Author: ackerman\n\n");
}

//**********************************************
int GetSizeFile(char *cadFile)
{
 long aReturn=0;
 FILE* fp = fopen(cadFile,"rb");
 if(fp) 
 {
  fseek(fp, 0 , SEEK_END);
  aReturn = ftell(fp);
  fseek(fp, 0 , SEEK_SET);// needed for next read from beginning of file
  fclose(fp);
 }
 return aReturn;
}

//**********************************************
void WriteArrayHexFile(char *cadFileOrigen, FILE *fileDestino)
{
 unsigned char contLine=0;
 FILE *auxWrite = fileDestino;
 FILE *auxRead = NULL;
 long auxSize=0;
 
 auxRead = fopen(cadFileOrigen,"rb");
 printf("Read:%s\n",cadFileOrigen);
 if ((auxRead!=NULL)&&(auxWrite!=NULL))
 {
  auxSize = GetSizeFile(cadFileOrigen);
  printf("Size %d\n",auxSize);
  fread(gb_bufferFile,1,auxSize,auxRead);
  for (long i=0;i<auxSize;i++)
  {
   fprintf(auxWrite,"0x%02X",gb_bufferFile[i]);
   if (i<(auxSize-1)){ fprintf(auxWrite,","); }
   contLine++;
   if (contLine>15)
   {
    contLine=0;
    fprintf(auxWrite,"\n");
   }
  }
  fclose(auxRead);
 }
}

//**********************************************
void WriteFileHEX(unsigned char num,char *cadPath,char *cadFile, char *cadFileSource,char *tipoPath,char *tipo,char *defTipo)
{
 unsigned char contLine=0;
 FILE *auxWrite = NULL;
 FILE *auxRead = NULL;
 long auxSize=0;
 char cadDest[2048];
 char cadOri[2048];
 sprintf(cadOri,"%s\\%s",cadPath,cadFileSource);
 sprintf(cadDest,"output/dataFlash/%s/%s%s.h",tipoPath,tipo,cadFile);
 auxWrite = fopen(cadDest,"w+");
 if (auxWrite!= NULL)
 {
  fprintf(auxWrite,"#ifndef _%s_%s_H\n",defTipo,cadFile);
  fprintf(auxWrite," #define _%s_%s_H\n",defTipo,cadFile);
  //WriteHexData(auxWrite,cadFile);  

  auxRead = fopen(cadOri,"rb");
  if (auxRead!=NULL)
  {
   auxSize = GetSizeFile(cadOri);
   
   fprintf(auxWrite," //%s %s %d bytes\n\n",tipo,cadFile,auxSize);
   fprintf(auxWrite,"static const unsigned char gb_%s_data_%s[]={\n",tipo,cadFile);
      
   //printf ("Tam %d",auxSize);
   fread(gb_bufferFile,1,auxSize,auxRead);
   for (long i=0;i<auxSize;i++)
   {
    fprintf(auxWrite,"0x%02X",gb_bufferFile[i]);
    if (i<(auxSize-1))
     fprintf(auxWrite,",");
    contLine++;
    if (contLine>15)
    {
     contLine=0;
     fprintf(auxWrite,"\n");
    }
   }
   fclose(auxRead);
  }
  
  
  fprintf(auxWrite,"\n};\n");
  fprintf(auxWrite,"#endif\n");
  fclose(auxWrite);
 }
}


//**********************************************
void WriteHeadGeneric_H(char *cadDefine, char *cadTipo)
{//Los 64k
 char cadAuxTitle[max_cad_title];
 if (gb_fileWrite == NULL)
 {
  return;
 }
 fprintf(gb_fileWrite,"#ifndef %s\n",cadDefine);
 fprintf(gb_fileWrite," #define %s\n",cadDefine); 
 fprintf(gb_fileWrite,"\n");
 fprintf(gb_fileWrite," #include \"gbCompileDataOpt.h\"\n\n");
 fprintf(gb_fileWrite," //#include <stddef.h>\n\n"); 
 
 fprintf(gb_fileWrite," #ifdef gb_use_lib_compile_arduinodroid\n"); 
 for (unsigned char i=0;i<gb_contRom;i++)
 {
  if (strlen(gb_titles[i]) != 0)
  {
   fprintf(gb_fileWrite,"  #include \"%s%s.h\"\n",cadTipo,gb_titles[i]); //fprintf(gb_fileWrite," #include \"com/com%s.h\"\n",gb_titles[i]);
  }
 }
 fprintf(gb_fileWrite," #else\n");
 for (unsigned char i=0;i<gb_contRom;i++)
 {
  if (strlen(gb_titles[i]) != 0)
  {
   fprintf(gb_fileWrite,"  #include \"%s/%s%s.h\"\n",cadTipo,cadTipo,gb_titles[i]); //fprintf(gb_fileWrite," #include \"com/com%s.h\"\n",gb_titles[i]);
  }
 } 
 fprintf(gb_fileWrite," #endif\n");
 fprintf(gb_fileWrite,"\n");
 
 fprintf(gb_fileWrite," #define max_list_%s %d\n\n\n",cadTipo,gb_contRom); //fprintf(gb_fileWrite," #define max_list_com %d\n\n\n",gb_contRom);
 fprintf(gb_fileWrite," //%s\n //Title\n",cadTipo); //fprintf(gb_fileWrite," //coms\n //Titulos\n");
 fprintf(gb_fileWrite," static const char * gb_list_%s_title[max_list_%s]={\n",cadTipo,cadTipo); //fprintf(gb_fileWrite," static const char * gb_list_com_title[max_list_com]={\n");
 for (unsigned char i=0;i<gb_contRom;i++)
 {
  if (strlen(gb_titles[i]) == 0)
  {
   fprintf(gb_fileWrite,"  \"\"");
  }
  else 
  {
   strcpy(cadAuxTitle,gb_titles[i]);
   cadAuxTitle[max_osd_real_title-1]='\0'; //Limito a 16 caracteres
   fprintf(gb_fileWrite,"  \"%s\"",cadAuxTitle); //fprintf(gb_fileWrite,"  \"%s\"",gb_titles[i]);
  }
  if (i<(gb_contRom-1))
  {
   fprintf(gb_fileWrite,",\n");
  }
 }
 fprintf(gb_fileWrite,"\n };\n");
 fprintf(gb_fileWrite,"\n");
 
 //fprintf(gb_fileWrite," //SEGMEN 0  1\n");
 //fprintf(gb_fileWrite," static const unsigned char gb_list_seg_load[max_list_com]={\n");  
 //for (unsigned char i=0;i<gb_contRom;i++)
 //{
 // fprintf(gb_fileWrite,"  1");
 // if (i<(gb_contRom-1)) { fprintf(gb_fileWrite,",\n"); }
 //}
 //fprintf(gb_fileWrite,"\n };\n");
 //fprintf(gb_fileWrite,"\n"); 
 
  
 fprintf(gb_fileWrite," //Data %s\n",cadTipo); //fprintf(gb_fileWrite," //Datos com\n");
 fprintf(gb_fileWrite," static const unsigned char * gb_list_%s_data[max_list_%s]={\n",cadTipo,cadTipo); //fprintf(gb_fileWrite," static const unsigned char * gb_list_com_data[max_list_com]={\n");
 for (unsigned char i=0;i<gb_contRom;i++)
 {
  if (strlen(gb_titles[i]) == 0)
  {
   fprintf(gb_fileWrite,"  NULL");
  }
  else 
  {
   fprintf(gb_fileWrite,"  gb_%s_data_%s",cadTipo,gb_titles[i]); //fprintf(gb_fileWrite,"  gb_com_%s",gb_titles[i]);
  } 
  if (i<(gb_contRom-1))
  {
   fprintf(gb_fileWrite,",\n");   
  }
 }  
 fprintf(gb_fileWrite,"\n };\n");
 
 fprintf(gb_fileWrite,"\n");
 //fprintf(gb_fileWrite,"#endif\n");
}


//********************************************
void WriteSizeGeneric(char *cadTipo)
{
 char cadDestino[1024];
 int auxSize;
 if (gb_fileWrite == NULL)
  return;
 fprintf(gb_fileWrite," //Size of bytes\n");
 fprintf(gb_fileWrite," static const unsigned short int gb_list_%s_size[max_list_%s]={\n",cadTipo,cadTipo); //fprintf(gb_fileWrite," static const unsigned short int gb_list_com_size[max_list_com]={\n");
 for (unsigned char i=0;i<gb_contRom;i++)
 {
  if (strlen(gb_titles[i]) == 0)
   fprintf(gb_fileWrite,"  \"\"");
  else
  {
   sprintf(cadDestino,"input\\%s\\%s",cadTipo,gb_nameFiles[i]); //sprintf(cadDestino,"input\\com\\%s",gb_nameFiles[i]);
   auxSize = GetSizeFile(cadDestino);
   fprintf(gb_fileWrite,"  %d",auxSize);
  }
  if (i<(gb_contRom-1))
   fprintf(gb_fileWrite,",\n");
 }
 fprintf(gb_fileWrite,"\n };\n");
 fprintf(gb_fileWrite,"\n");  
}



//**********************************************
void listFilesRecursively(char *basePath)
{     
    char cadFileSource[1000];
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);
    if (!dir)
        return; // Unable to open directory stream
    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            printf("%s\n", dp->d_name);
            
            strcpy(cadFileSource,dp->d_name);
            strcpy(gb_nameFiles[gb_contRom],dp->d_name); //Nombre completo
            RemoveExt(dp->d_name);
            strcpy(gb_titles[gb_contRom],dp->d_name);

            switch (gb_global_tipo)
            {
             case tipo_gtp: WriteFileHEX(gb_contRom,basePath,dp->d_name,cadFileSource,"gtp","gtp","GTP"); break;
             case tipo_gal: WriteFileHEX(gb_contRom,basePath,dp->d_name,cadFileSource,"gal","gal","GAL"); break;
            }
            
                        
            gb_contRom++;
            if (gb_contRom > (maxObjects-1))
             return;
            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            listFilesRecursively(path);            
        }
    }
    closedir(dir);
}

//*************************************************
void ProcesaFicherosGeneric(char *cadFileWrite, char *cadpath, char *cadHead, char *cadTipo)
{
 gb_contRom=0;
 gb_fileWrite = fopen(cadFileWrite,"w+"); //gb_fileWrite = fopen("output/dataFlash/gbgtp.h","w+"); 
 if (gb_fileWrite!= NULL)
 {                      
  listFilesRecursively(cadpath); //listFilesRecursively("input/gtp");
  WriteHeadGeneric_H(cadHead,cadTipo); //WriteHeadCOM_H("_GB_GTP_H");
  WriteSizeGeneric(cadTipo);
  fprintf(gb_fileWrite,"#endif\n");
  fclose(gb_fileWrite);
 }     
}

//*************************************************
void ProcesaArduinoDroid(char *cadFileWrite, char *cadHead)
{
 gb_fileWrite = fopen(cadFileWrite,"w+");
 if (gb_fileWrite!= NULL)
 {                      
  fprintf(gb_fileWrite,"#ifndef %s\n",cadHead);
  fprintf(gb_fileWrite," #define %s\n\n",cadHead);

  fprintf(gb_fileWrite,"  //support arduinodroid project and cloud (one level path files)\n");
  fprintf(gb_fileWrite,"  //#ifndef gb_use_lib_compile_arduinodroid\n");
  fprintf(gb_fileWrite,"  // #define gb_use_lib_compile_arduinodroid\n\n");

  fprintf(gb_fileWrite,"  //#endif\n\n");

  fprintf(gb_fileWrite,"#endif\n\n");

  fclose(gb_fileWrite);
 }
}







//********
//* MAIN *
//********
int main(int argc, char**argv)
{
 char path[100];    //Directory path to list files
 ShowHelp();
 
 printf("GTP FILE PROCESS:\n"); //Procesar fichero GTP
 gb_global_tipo= tipo_gtp; //GTP tipo
 InitTitles(); 
 ProcesaFicherosGeneric("output/dataFlash/gbgtp.h","input/gtp","_GB_GTP_H","gtp");
 
 printf("GAL FILE PROCESS:\n"); //Procesar fichero GTP
 gb_global_tipo= tipo_gal; //GAL tipo
 InitTitles(); 
 ProcesaFicherosGeneric("output/dataFlash/gbgal.h","input/gal","_GB_GAL_H","gal"); 
 
 
 ProcesaArduinoDroid("output/dataFlash/gbCompileDataOpt.h","_GB_COMPILE_DATA_OPT_H"); //Crea cabecera proyecto ANDROID
 
 return 0;
}
