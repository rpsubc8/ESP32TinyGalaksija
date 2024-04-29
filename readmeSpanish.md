# Tiny Galaksija ESP32
Port del emulador Galaksija a la placa TTGO VGA32 v1.x (1.0, 1.1, 1.2, 1.4) con ESP32.<br>
Basado en el emulador x86 de Miodrag Jevremovic, Tomaž Šolc y Peter Bakota.<br>
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
</ul>

<br>
<h1>Requerimientos</h1>
Se requiere:
 <ul>
  <li>TTGO VGA32 v1.x (1.0, 1.1, 1.2, 1.4)</li>
  <li>Visual Studio 1.48.1 PLATFORMIO 2.2.1 Espressif32 v3.3.2</li>
  <li>Arduino IDE 1.8.11 Espressif System 1.0.6</li>
  <li>ArduinoDroid (6.3.1)</li>  
 </ul>


<br><br>
<h1>ArduinoDroid</h1>
Se debe ejecutar, sólo una vez, el script makearduinodroidwin.bat, que nos deja toda la estructura de datos del directorio dataFlash, así como el resto de archivos, en el directorio desde el que se lanza el script.<br>
Básicamente, nos quedan todos los archivos en un único nivel, sin subdirectorios posibles.
El directorio 'notdelete' es interno del script y se utiliza para copiar el gbCompileOpt.h.<br>
Al finalizar, el propio script, podemos abrir el galaksija.ino.
