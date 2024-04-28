@echo off
echo Convert ARDUINODROID PROJECT DATA (WIN32)
pause
echo Copy *.cpp (Copiar *.cpp)
copy/Y ..\..\..\TinyGalaksijattgovga32\galaksija\*.cpp
echo Copy *.h (Copiar *.h)
copy/Y ..\..\..\TinyGalaksijattgovga32\galaksija\*.h
echo Copy galaksija.ino (Copiar galaksija.ino)
copy/Y ..\..\..\TinyGalaksijattgovga32\galaksija\galaksija.ino
echo Copy gbgal.h (Copiar gbgal.h)
copy/Y ..\..\..\TinyGalaksijattgovga32\galaksija\dataFlash\gbgal.h
echo Copy gbgtp.h (Copiar gbgtp.h)
copy/Y ..\..\..\TinyGalaksijattgovga32\galaksija\dataFlash\gbgtp.h
echo Copy gbCompileDataOpt.h (Copiar gbCompileDataOpt.h)
copy/Y ..\..\..\TinyGalaksijattgovga32\galaksija\dataFlash\gbCompileDataOpt.h
echo Copy GTPs (Copiar GTPs)
copy/Y ..\..\..\TinyGalaksijattgovga32\galaksija\dataFlash\gtp\*.h
echo Copy GALs (Copiar GALs)
copy/Y ..\..\..\TinyGalaksijattgovga32\galaksija\dataFlash\gal\*.h
echo Copy Options ARDUINODROID
copy/Y notdelete\gbCompileOpt.h
pause