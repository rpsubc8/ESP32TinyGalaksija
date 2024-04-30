# Tiny Galaksija ESP32
Port del emulador Galaksija a la placa TTGO VGA32 v1.x (1.0, 1.1, 1.2, 1.4) con ESP32.<br>
Basado en el emulador x86 de Miodrag Jevremovic, Tomaž Šolc y Peter Bakota.<br>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewBladeAlley.gif'></center>
He realizado varias modificaciones:
<ul>
 <li>Portado de x86 PC a ESP32</li>
 <li>No se usa PSRAM, funcionando en ESP32 de 520 KB de RAM (TTGO VGA32 v1.x)</li>
 <li>Uso de un sólo core</li>
 <li>OSD de bajos recursos</li>
 <li>Creado proyecto compatible con Arduino IDE y Platform IO</li>
 <li>Proyecto compatible con Web Editor y ArduinoDroid (6.3.1) con tool makearduinodroidwin.bat</li>
 <li>Soporte para modo 8 y 64 colores (versión reducida de Ricardo Massaro).</li>
 <li>VGA 360x200, 320x200 y 320x240</li>
 <li>Selección de modo de video de inicio en compilación.</li>
 <li>Selección de modo de video en el arranque (500 ms) pulsando una tecla.</li>
 <li>Cambio del modo de video en caliente, en cualquier momento y sin necesidad de reiniciar.</li>
 <li>Filtro color crt b&w, verde o naranja</li>
 <li>Reducción de emulación de 64KB de RAM a sólo 8 KB.</li>
 <li>Varias optimizaciones en volcado de video y emulación CPU.</li>
 <li>Reducción de código de emulación Z80 a sólo 2 archivos (Z80Z80.h y Z80Z80.cpp).</li>
 <li>Estadísticas de CPU y Video en OSD.</li>
 <li>Estadísticas de RAM libre desde OSD.</li> 
 <li>Teclado remoto UART.</li>
</ul>


<br><br>
<h1>Modos de video</h1>
Existen 16 posibles modos de video, que en realidad se dividen en 3 básicos, pero con posibilidad ambos de 3 bpp (8 colores) o 6 bpp (64 colores):
<ul>
 <li>360x200</li>
 <li>320x200</li>
 <li>320x240</li>
</ul>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewVideoModes.gif'></center>  
Dado que el Galaksija tenía una resolución de 256x208, lo ideal es usar 320x240.
Los modos de video se pueden seleccionar durante el arranque (500 ms) o reinicio, pudiendo especificar otro tiempo en el gbConfig.h en la sección use_lib_boot_time_select_vga, con sólo pulsar la tecla del mismo, desde el 0 al F.<br>
El modo de video también se puede fijar en la compilación, pero lo más cómodo es cambiarlo en caliente desde el OSD en cualquier instante.<br>
Así mismo, existen para cada modo de video, la posibilidad de poder usar la función del ajuste del pll de Espressif, o bien una custom, que evita que se quede colgado el ESP32.<br>
También se permiten diferentes ajustes de video, con modos de fabgl o bitluni.




<br><br>
<h1>Requerimientos</h1>
Se requiere:
 <ul>
  <li>TTGO VGA32 v1.x (1.0, 1.1, 1.2, 1.4)</li>
  <li>Visual Studio 1.48.1 PLATFORMIO 2.2.1 Espressif32 v3.3.2</li>
  <li>Arduino IDE 1.8.11 Espressif System 1.0.6</li>
  <li>ArduinoDroid (6.3.1)</li>  
 </ul>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/ttgovga32v12.jpg'></center>


<br><br>
<h1>ArduinoDroid</h1>
Se debe ejecutar, sólo una vez, el script makearduinodroidwin.bat, que nos deja toda la estructura de datos del directorio dataFlash, así como el resto de archivos, en el directorio desde el que se lanza el script.<br>
Básicamente, nos quedan todos los archivos en un único nivel, sin subdirectorios posibles.
El directorio 'notdelete' es interno del script y se utiliza para copiar el gbCompileOpt.h.<br>
Al finalizar, el propio script, podemos abrir el galaksija.ino.


<br><br>
<h1>Arduino Web Editor</h1>
Se debe ejecutar, sólo una vez, el script makearduinodroidwin.bat. Una vez finalizado, se sube a la nube como un proyecto cualquiera, ya sea comprimido en zip o por archivos.


<br><br>
<h1>PlatformIO</h1>
Se debe instalar el PLATFORMIO 2.2.1 desde las extensiones del Visual Studio. Se requiere también Espressif32 v3.3.2.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewPlatformIOinstall.gif'></center>
Luego se seleccionará el directorio de trabajo <b>TinyGalaksijattgovga32</b>.
Debemos modificar el fichero <b>platformio.ini</b> la opción <b>upload_port</b> para seleccionar el puerto COM donde tenemos nuestra placa TTGO VGA32.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewPlatformIO.gif'></center>
Luego procederemos a compilar y subir a la placa. No se usa particiones, así que debemos subir todo el binario compilado.
Está todo preparado para no tener que instalar las librerias de bitluni ni fabgl.


<br><br>
<h1>Arduino IDE</h1>
Todo el proyecto es compatible con la estructura de Arduino 1.8.11.
Tan sólo tenemos que abrir el <b>galaksija.ino</b> del directorio <b>galaksija</b>.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewArduinoIDEpreferences.gif'></center>
Debemos instalar las extensiones de spressif en el gestor de urls adicionales de tarjetas <b>https://dl.espressif.com/dl/package_esp32_index.json</b>
<br>
Ya está preparado el proyecto, de forma que no se necesita ninguna librería de bitluni ni fabgl.
Debemos desactivar la opción de PSRAM, y en caso de superar 1 MB de binario, seleccionar 4 MB de partición a la hora de subir. Aunque el código no use PSRAM, si la opción está activa y nuestro ESP32 no dispone de ella, se generará una excepción y reinicio del mismo en modo bucle.
