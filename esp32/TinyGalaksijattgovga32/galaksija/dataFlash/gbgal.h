#ifndef _GB_GAL_H
 #define _GB_GAL_H

 #include "gbCompileDataOpt.h"

 //#include <stddef.h>

 #ifdef gb_use_lib_compile_arduinodroid  
  #include "galFastMode.h"
  #include "galP.h"
  #include "galPomeri.h"
  #include "galPrint.h"
  #include "galProba.h"
  #include "galSat.h" 
 #else  
  #include "gal/galFastMode.h"
  #include "gal/galP.h"
  #include "gal/galPomeri.h"
  #include "gal/galPrint.h"
  #include "gal/galProba.h"
  #include "gal/galSat.h"
 #endif


 #define max_list_gal 6
 
 //GAL
 //Title
 static const char * gb_list_gal_title[max_list_gal]={  
  "Fast Mode",
  "P",
  "Pomeri",
  "Print",
  "Proba",
  "Sat"
 };
 
 //Size of bytes
 static const unsigned short int gb_list_gal_size[max_list_gal]={  
  8268,
  8268,
  8268,
  8268,
  8268,
  8268
 };
 
 
 //Data
 static const unsigned char * gb_list_gal_data[max_list_gal]={  
  gb_gal_data_FastMode,
  gb_gal_data_P,
  gb_gal_data_Pomeri,
  gb_gal_data_Print,
  gb_gal_data_Proba,
  gb_gal_data_Sat
 };


#endif

