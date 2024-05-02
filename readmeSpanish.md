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
 <li>Reducción de emulación de 64KB de RAM a sólo 8 KB. Sólo se emulan los 6 KB de RAM de Galaksija.</li>
 <li>Mapeo de las ROMs en Flash, en lugar de RAM</li>
 <li>Varias optimizaciones en volcado de video y emulación CPU.</li>
 <li>Reducción de código de emulación Z80 a sólo 2 archivos (Z80Z80.h y Z80Z80.cpp).</li>
 <li>Reducción y compactación del proyecto para poder ser llevado a plataformas más limitadas que el ESP32.</li>
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
Dado que el Galaksija tenía una resolución de 256x208, lo ideal es usar 320x240.<br>
Los modos de video se pueden seleccionar durante el arranque (500 ms) o reinicio, pudiendo especificar otro tiempo en el <b>gbConfig.h</b> en la sección <b>use_lib_boot_time_select_vga</b>, con sólo pulsar la tecla del mismo, desde el '0' al 'F'.<br>
El modo de video de inicio también se puede fijar en la compilación, pero lo más cómodo es cambiarlo en caliente desde el OSD en cualquier instante.<br>
Así mismo, existen para cada modo de video, la posibilidad de poder usar la función del ajuste del pll de Espressif, o bien una custom, que evita que se quede colgado el ESP32. Dicho modo especial de video tiene el añadido del <b>apll</b>.<br>
También se permiten diferentes ajustes de video, con modos de fabgl o bitluni.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewManic.gif'></center>
Existe una opción de filtros CRT, al estilo clásico:
<ul>
 <li>Blanco y negro</li>
 <li>Fósforo verde</li>
 <li>Naranja</li>
</ul>
El color variará en función de que tengamos el modo de video:
<ul>
 <li>3 bpp (8 colores)</li>
 <li>6 bpp (64 colores)</li>
</ul>
Se recomienda el modo de 64 colores, y nos daremos cuenta por el efecto de nieve en TV en tonalidad azul (3 bpp) o escala de grises (6 bpp).


<br><br>
<h1>Versión precompilada</h1>
En la carpeta precompile se encuentra una versión ya compilada (VGA 360x200) para poder ser grabada con el flash download tool 3.9.2. con los juegos de demostración en FLASH.<br><br>
<a href='https://github.com/rpsubc8/ESP32TinyGalaksija/tree/main/esp32/precompile'>https://github.com/rpsubc8/ESP32TinyGalaksija/tree/main/esp32/precompile</a>
<br><br>
Debemos de elegir el tipo ESP32:
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/flash00.gif'></center>
Posteriormente, seleccionaremos los archivos tal y como la captura adjunta, con los mismos valores de offset:
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/flash01.gif'></center>
Y le daremos a start. Si todo ha sido correcto, sólo tendremos que reiniciar el ESP32.


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
Se debe ejecutar, sólo una vez, el script <b>makearduinodroidwin.bat</b>, que nos deja toda la estructura de datos del directorio dataFlash, así como el resto de archivos, en el directorio desde el que se lanza el script.<br>
Básicamente, nos quedan todos los archivos en un único nivel, sin subdirectorios posibles.<br>
El directorio 'notdelete' es interno del script y se utiliza para copiar el <b>gbCompileOpt.h</b>.<br>
Al finalizar, el propio script, podemos abrir el <b>galaksija.ino</b>, y recompilar desde el ArduinoDroid, sin nada especial. El script posee una pausa al inicio, por si queremos parar el script desde fuera, así como una parada al final, para ver los resultados del proceso.<br>
Si nos damos cuenta, todo gira en torno al pragma <b>gb_use_lib_compile_arduinodroid</b> de la cabecera <b>gbCompileOpt.h</b>, que en modo normal se utiliza con paths de múltiples niveles y en ArduinoDroid con el script, en un sólo nivel.<br>
Este script busca los archivos subiendo niveles hasta llegar al <b>TinyGalaksijattgovga32/galaksija</b>, así que se requiere que se baje el proyecto completo con toda la estructura:
<pre>
 TinyGalaksijattgovga32
  galaksija
   dataFlash
  include
  lib
  test
 tools
  arduinodroid
   galaksija
  data2h
</pre>
El ArduinoDroid (6.3.1) con soporte para ESP32 sólo funciona con procesadores de 64 bits.


<br><br>
<h1>Arduino Web Editor</h1>
Se debe ejecutar, sólo una vez, el script makearduinodroidwin.bat. Una vez finalizado, se sube a la nube como un proyecto cualquiera, ya sea comprimido en zip o por archivos.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewArduinoWebEditor.gif'></center>
Básicamente es lo mismo, dado que se intenta dejar todo en un único nivel de directorio. Se recomienda importar el proyecto comprimido en zip con una compresión normal, nada de máxima, dado que algunos editores web del cloud no soportan el zip al 100%.


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


<br><br>
<h1>Usabilidad</h1>
Se permiten las siguientes acciones desde el menú (tecla F1):
 <ul>
  <li>Cargar o seleccionar una cinta (GTP)</li>
  <li>Cargar o seleccionar un snapshot (GAL)</li>
  <li>Selecionar idioma Español o inglés</li>
  <li>Seleccionar 16 modos de video</li>
  <li>Cambiar el color verde, naranja o blanco y negro, como los monitores retro</li>  
  <li>Mostrar estadísticas de CPU y video</li>  
  <li>Activar la salida de depuración por usb (serie)</li>
  <li>Ayuda</li>
  <li>Resetear el ESP32 o el emulador</li>
  <li>Interceptar la rutina de cinta cuando escribimos el comando OLD desde BASIC, y hemos seleccionado un GTP, en lugar de cargarlo y ejecutarlo.</li>  
  <li>Cambiar los milisegundos de polling para video, teclado</li>
  <li>Ver la RAM libre</li>
 </ul>
 Se dispone de un OSD básico de bajos recursos, es decir, muy simple, que se visualiza pulsando la tecla <b>F1</b>.
 <center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewOSD.gif'></center>
 Algunos menús y submenús son de ejecución inmediata, es decir, al aceptar con la tecla ENTER, ya se cierra el OSD y se aplica la acción. Otros menús, por el contrario, vuelven al menu anterior.<br>
 Siempre se puede salir del OSD con la tecla ESC.<br>
 Los cursores Arriba y Abajo permiten navegar por cada acción. Si se mantienen durante un par de milisegundos pulsadas, permiten acción rápida.


<br><br>
<h1>GTP (cintas)</h1>
Siempre que carguemos un GTP (cinta) desde el OSD, nos permitirá elegir entre:
<ul>
 <li><b>Load:</b> Nos permite cargarlo.</li>
 <li><b>Select:</b> Sólo lo selecciona, pudiendo desde BASIC leerlo con el comando OLD.</li>
</ul>
Cuando se lanza el comando OLD, existe un pequeño truco no 100% correcto, pero eficiente en consumo de host, para interceptar la rutina de ROM, y proceder a cargarlo. Dicha intercepción se puede deshabilitar también desde el OSD.<br>
Si hemos elegido la carga, en lugar de la selección, cuando ya hemos elegido una cinta desde el OSD, nos aparecerá si queremos usar el <b>WARM START BASIC</b>.<br>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewWarmStart.gif'></center>
Posteriormente, nos dirá si queremos que haga un <b>RUN</b> automático desde BASIC o no.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewRUN.gif'></center>
En caso de que aceptemos, desde BASIC, se procederá a escribir RUN de manera automática.<br>
Es recomendable tener una ejecución limpia, siempre que carguemos juegos, es decir, reiniciar el emulador, para dejar todo lo mejor posible.<br>
Cuando ocurren errores de memoria, se deja traza por el puerto serie, y al cabo de un par de ellos, se autoreinicia el emulador (no el ESP32), dejándonos una traza.


<br><br>
<h1>Snapshot GAL</h1>
Siempre que carguemos un GAL, nos dejará la opción de ejecutar <b>RUN</b> o no desde BASIC.<br>
Los GAL deben ser de tamaño 8268 bytes.


<br><br>
<h1>Estadísticas</h1>
Hay una opción es <b>OSD Options</b> que nos permite ver lo que se llama el OSD, que son las estadísticas de consumos de CPU y video, por cada segundo (1000 milisegundos).
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewStatsCPU.gif'></center>
En una emulación de 50 fps (cuadros por segundo) existen 2 secciones:
<ul>
 <li>Tiempo de CPU (20 milisegundos)</li>
 <li>Tiempo de volcado de video (20 milisegundos)</li>
</ul>
Son 20 milisegundos, porque son 50 fps (1000 / 50 = 20). El tiempo de volcado de video, no tiene porque ser 50 fps reales, dado que lo importante, es el de CPU, que es el que va a marcar los 50 fps reales. Podemos pues, conseguir 50 fps de CPU, pero tener 24 fps en volcado de video, y sería totalmente funcional. No obstante, en este emulador, se consigue superar los 50 fps reales.<br>
Se han puesto las estadísticas a la derecha y en vertical, para no interferir en ningún momento en la pantalla emulada.
<ul>
 <li><b>C:</b> Los microsegundos del ESP32 que tarda en procesar el frame actual de 20 milisegundos.</li>
 <li><b>M:</b> Los microsegundos del ESP32 que tarda en procesar el frame más rápido de 20 milisegundos en 1000 milisegundos.</li>
 <li><b>MX:</b> Los microsegundos del ESP32 que tarda en procesar el frame más lento de 20 milisegundos en 1000 milisegundos.</li>
 <li><b>I:</b> Los microsegundos que queda libre el ESP32 para el frame actual de 20 milisegundos, teniendo en cuenta en el cálculo, el consumo del volcado de video.</li>
 <li><b>FPS:</b> FPS reales que procesa el emulador en 1000 milisegundos.</li>
 <li><b>FND:</b> FPS que nos daría si no lo limitaramos a 20 milisegundos por frame, es decir, que en el menú de la CPU tengamos 0 ms (rápido).</li>
 <li><b>V:</b> Los microsegundos del ESP32 que tarda en volcar un frame de 20 milisegundos.</li>
 <li><b>F:</b> Los FPS reales a los que estamos volcando el video realmente.</li>
</ul>


<br><br>
<h1>Teclado PS/2</h1>
Se requiere un teclado 100% PS/2, y que no requiera comando de inicialización.<br>
El Galaksija tiene un buffer para teclas pulsadas de 54 elementos.<br>
Están emuladas las siguiente teclas:

| Teclas emulador   | Teclas (teclado PS/2 real) | Buffer Galaksija |
| ----------------- | -------------------------- | ---------------- |
| 0 .. 9            | 0 .. 9                     | 32 .. 41         |
| A .. Z            | A .. Z                     |  1 .. 26         |
| Arriba            | Arriba                     | 27               |
| Abajo             | Abajo                      | 28               |
| Izquierda         | Izquierda                  | 29               |
| Derecha           | Derecha                    | 30               |
| Barra espaciadora | Barra espaciadora          | 31               |
| Borrar            | Borrar o Suprimir          | 29               |
| ;                 | ;                          | 42               |
| "                 | '                          | 43               |
| ,                 | ,                          | 44               |
| =                 | =                          | 45               |
| .                 | .                          | 46               |
| /                 | /                          | 47               |
| ENTER             | ENTER o RETURN             | 48               |
| CONTROL           | CONTROL                    | 50               |
| SCROLL            | SCROLL                     | 52               |
| SHIFT             | SHIFT izquierdo o derecho  | 53               |
  
La entrada 51 de Galaksija, que sería el DELETE no está mapeada en el emulador.


<br><br>
<h1>GPIO</h1>
El archivo <b>hardware.h</b> contiene lo necesario para el GPIO de:
<ul>
 <li>
  <b>Teclado:</b>
  <ul>
   <li>KEYBOARD_DATA - (32)</li>
   <li>KEYBOARD_CLK - (33)</li>
  </ul> 
 </li> 
 <li>
  <b>VGA sincronismos:</b>
  <ul>
   <li>HSYNC_PIN - (23)</li>
   <li>VSYNC_PIN - (15)</li>
  </ul>
 </li>
 <li>
  <b>VGA rojo</b> 
  <ul>
   <li>PIN_RED_LOW - (21)</li>
   <li>PIN_RED_HIGH -(22)</li>
  </ul>
 </li>
 <li>
  <b>VGA verde</b> 
  <ul>
   <li>PIN_GREEN_LOW - (18)</li>
   <li>PIN_GREEN_HIGH - (19)</li>
  </ul>
 <li>
  <b>VGA azul</b>
  <ul>
   <li>PIN_BLUE_LOW - (4)</li>
   <li>PIN_BLUE_HIGH - (5)</li>
  </ul>
</ul>
Está pensado para la placa TTGO VGA v1.2, así que si tenemos otro tipo de placa, aquí es donde debemos definir los cambios.<br>
Cuando usamos 6 bpp, estamos usando todos los GPIO de VGA, pero cuando usamos el 3 bpp, sólo usamos:
<ul>
 <li>PIN_RED_HIGH - (22)</li>
 <li>PIN_GREEN_HIGH - (19)</li>
 <li>PIN_BLUE_HIGH - (5)</li>
</ul>



<br><br>
<h1>Opciones</h1>
El archivo <b>gbConfig.h</b> se seleccionan las opciones:
Podemos elegir el tiempo (milisegundos) para poder seleccionar el modo de video durante el arranque:
<ul>
 <li><b>use_lib_boot_time_select_vga:</b> Cuanto más alto sea el valor, más tiempo tardará en arrancar. Si durante este tiempo, pulsamos las teclas del 0 al 9 o las letras de la A a la F, estaremos eligiendo un modo de video. Es válido también para el reinicio.</li>
</ul>
Tenemos la posibilidad de elegir uno de los 16 posibles modos de video por defecto, es decir, por compilación, y que sea el que se inicia por defecto al arrancar la placa (360x200):
<ul>
 <li><b>use_lib_boot_vga_360x200x70hz_bitluni_3bpp:</b> 360x200 8 colores</li>
 <li><b>use_lib_boot_vga_360x200x70hz_bitluni_apll_3bpp:</b> 360x200 8 colores corección apll, para ESP32 que se quedan colgados.</li>
 <li><b>use_lib_boot_vga_320x200x70hz_bitluni_3bpp:</b> 320x200 8 colores</li>
 <li><b>use_lib_boot_vga_320x200x70hz_fabgl_3bpp:</b> 320x200 8 colores con parámetros de fabgl</li>
 <li><b>use_lib_boot_vga_320x200x70hz_bitluni_apll_3bpp:</b> 320x200 8 colores corección apll, para ESP32 que se quedan colgados.</li>
 <li><b>use_lib_boot_vga_320x240x60hz_bitluni_3bpp:</b> 320x240 8 colores</li>
 <li><b>use_lib_boot_vga_320x240x60hz_fabgl_3bpp:</b> 320x240 8 colores con parámetros de fabgl</li>
 <li><b>use_lib_boot_vga_320x240x60hz_bitluni_apll_3bpp:</b> 320x240 8 colores corección apll, para ESP32 que se quedan colgados.</li>
 <li><b>use_lib_boot_vga_360x200x70hz_bitluni_6bpp:</b> 360x200 64 colores</li>
 <li><b>use_lib_boot_vga_360x200x70hz_bitluni_apll_6bpp:</b> 360x200 64 colores corección apll, para ESP32 que se quedan colgados.</li>
 <li><b>use_lib_boot_vga_320x200x70hz_bitluni_6bpp:</b> 320x200 64 colores</li>
 <li><b>use_lib_boot_vga_320x200x70hz_fabgl_6bpp:</b> 320x200 64 colores con parámetros de fabgl</li>
 <li><b>use_lib_boot_vga_320x200x70hz_bitluni_apll_6bpp:</b> 320x200 64 colores corección apll, para ESP32 que se quedan colgados.</li>
 <li><b>use_lib_boot_vga_320x240x60hz_bitluni_6bpp:</b> 320x240 64 colores</li>
 <li><b>use_lib_boot_vga_320x240x60hz_fabgl_6bpp:</b> 320x240 64 colores con parámetros de fabgl</li>
 <li><b>use_lib_boot_vga_320x240x60hz_bitluni_apll_6bpp:</b> 320x240 64 colores corección apll, para ESP32 que se quedan colgados.</li>
</ul>
Como es de esperar, no se puede seleccionar todos los modos al mismo tiempo, así que se debe descomentar el que queremos, y comentar el resto.<br>
Existen más opciones.
<ul> 
 <li><b>use_lib_log_serial:</b> Se envian logs por puerto serie usb</li>
 <li><b>use_lib_fix_double_precision:</b> No usar FPU para el cálculo del PLL.</li>
 <li><b>use_lib_debug_i2s:</b> Información detallada de la inicialización del modo de video.</li>
 <li><b>use_lib_keyboard_uart:</b> Permite usar un teclado remoto via UART por putty o la consola de VStudio.</li>
 <li><b>use_lib_log_keyboard_uart:</b> La traza del teclado remoto.</li>
</ul>



<br><br>
<h1>Tool data2h</h1>
He creado una herramienta muy básica (win32), para convertir los archivos .GTP y .GAL en .h en modo lista para ser procesados por el emulador. Tan sólo tenemos que dejar los archivos .GAL y .GTP en la carpeta <b>input/GAL</b> e <b>input/GTP</b> ejecutar el archivo <b>data2h.exe</b>, de forma que se generará una salida en el directorio <b>output/dataFlash</b>.
<a href='https://github.com/rpsubc8/ESP32TinyGalaksija/tree/main/esp32/tools/data2h'>Tool data2h</a>
<br><br>
<pre>
 input/
  gal/
  gtp/
 output/
  dataFlash/
   gal/
   gtp/ 
</pre>
Posteriormente debemos copiar el directorio <b>dataFlash</b> en el proyecto <b>TinyGalaksijattgovga32\galaksija</b> sobreescribiendo la carpeta dataFlash previa. Se recomienda limpiar el proyecto y volver a compilar.<br>
Esta herramienta es muy simple, y no controla los errores, por lo que se recomienda dejarle los archivos con nombres muy simples y lo más sencillo posible.<br>
El proyecto en PLATFORM.IO está preparado para 1 MB de Flash. Si necesitamos los 4MB de flash, tendremos que modificar la entrada del archivo <b>platformio.ini</b>
<pre>board_build.partitions = huge_app.csv</pre>
En el Arduino IDE, debemos elegir la opción <b>Partition Scheme (Huge APP)</b>.


<br><br>
<h1>DIY circuito</h1>
Si no queremos usar una placa TTGO VGA32 v1.x, podemos construirla siguiendo el esquema de <b>fabgl</b>:
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/fabglcircuit.gif'></center>


<br><br>
<h1>Versión minimalista</h1>
Se puede realizar una versión minimalista, sin resistencias, condensadores, y demás, sólo con conectores y cables directos, al menos para el caso de 8 colores y un sólo teclado:
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/minimalfabglcircuit.gif'></center>
Sólo sirve para teclados PS/2 que pueden alimentarse a 3.3v, que aunque son unos cuantos, suele excluir sobre todo a los muy antigüos, así como a los modernos con lucecitas.
La salida de audio (no se requiere en el emulador), está pensada para auriculares, y será bastante bajo. Asegurarse de que si le conectamos un amplificador de audio de dudoso estado o algo similar, al no llevar resistencias, podemos llevar un susto.<br>
La salida de video VGA es para 8 colores (3 pins). Lo mismo, si le conectamos un monitor de dudoso estado, aunque el VGA es para salida, podriamos llevar un susto.<br><br>
<b>Aunque es muy simple, se requiere unos conocimientos mínimos de electrónica, por lo que si no se sabe lo que se está haciendo, mejor no hacer nada. Me desentiendo de lo que pueda ocurrir.</b><br>


