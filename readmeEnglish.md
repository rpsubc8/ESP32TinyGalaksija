# Tiny Galaksija ESP32
Galaksija emulator port to TTGO VGA32 v1.x (1.0, 1.1, 1.2, 1.4) board with ESP32.<br>
Based on the x86 emulator by Miodrag Jevremovic, Tomaž Šolc and Peter Bakota.<br>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewBladeAlley.gif'></center>
I have made several modifications:
<ul>
 <li>Ported from x86 PC to ESP32</li>
 <li>No PSRAM used, running on ESP32 with 520 KB RAM (TTGO VGA32 v1.x)</li>
 <li>Use of a single core</li>
 <li>Low resource OSD</li>
 <li>Created project compatible with Arduino IDE and Platform IO</li>
 <li>Project compatible with Web Editor and ArduinoDroid (6.3.1) with tool <b>makearduinodroidwin.bat</b> and <b>makearduinodroidlinux.sh</b></li>
 <li>Support for mode 8 and 64 colors (reduced version by Ricardo Massaro).</li>
 <li>VGA 360x200, 320x200 and 320x240</li>
 <li>Startup video mode selection in build.</li>
 <li>Video mode selection at boot (500 ms) by pressing a key.</li>
 <li>Switching video mode on the fly, at any time and without rebooting.</li>
 <li>Crt b&w, green or orange color filter</li>
 <li>Reduced emulation from 64KB of RAM to just 8KB. Only Galaksija's 6 KB of RAM is emulated.</li>
 <li>Mapping ROMs to Flash, instead of RAM</li>
 <li>Various optimizations in video dump and CPU emulation.</li>
 <li>Reduced Z80 emulation code to only 2 files (Z80Z80.h and Z80Z80.cpp).</li>
 <li>Reduction and compaction of the project to be able to be taken to more limited platforms than the ESP32.</li>
 <li>CPU and Video Statistics in OSD.</li>
 <li>Free RAM statistics from OSD.</li> 
 <li>UART remote keyboard.</li>
</ul>


<br><br>
<h1>Video modes</h1>
There are 16 possible video modes, which are actually divided into 3 basic ones, but both with the possibility of 3 bpp (8 colors) or 6 bpp (64 colors):
<ul>
 <li>360x200</li>
 <li>320x200</li>
 <li>320x240</li>
</ul>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewVideoModes.gif'></center>
Since the Galaksija had a resolution of 256x208, it is ideal to use 320x240.<br>
Video modes can be selected during boot (500 ms) or reboot, and another time can be specified in <b>gbConfig.h</b> in the <b>use_lib_boot_time_select_vga</b> section , by simply pressing the same key, from '0' to 'F'.<br>
The startup video mode can also be set in the build, but it is most convenient to hot-switch it from the OSD at any time.<br>
Likewise, for each video mode, there is the possibility of using the Espressif pll adjustment function, or a custom one, which prevents the ESP32 from hanging. This special video mode has the addition of the <b>apll</b>.<br>
Different video settings are also allowed, with fabgl or bitluni modes.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewManic.gif'></center>
There is an option of CRT filters, in the classic style:
<ul>
 <li>black and white</li>
 <li>green match</li>
 <li>Orange</li>
</ul>
The color will vary depending on whether we have video mode:
<ul>
 <li>3 bpp (8 colors)</li>
 <li>6 bpp (64 colors)</li>
</ul>
The 64-color mode is recommended, and we will notice the snow effect on TV in blue tone (3 bpp) or gray scale (6 bpp).


<br><br>
<h1>Precompiled version</h1>
In the precompile folder there is an already compiled version (VGA 360x200) to be able to be recorded with the flash download tool 3.9.2. with demo games in FLASH.<br><br>
<a href='https://github.com/rpsubc8/ESP32TinyGalaksija/tree/main/esp32/precompile'>https://github.com/rpsubc8/ESP32TinyGalaksija/tree/main/esp32/precompile</a><br><br>
We must choose the ESP32 type:
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/flash00.gif'></center>
Subsequently, we will select the files as in the attached screenshot, with the same offset values:
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/flash01.gif'></center>
And we will hit start. If everything has been correct, we will only have to restart the ESP32.


<br><br>
<h1>Requirements</h1>
Is required:
 <ul>
  <li>TTGO VGA32 v1.x (1.0, 1.1, 1.2, 1.4)</li>
  <li>Visual Studio 1.48.1 PLATFORMIO 2.2.1 Espressif32 v3.3.2</li>
  <li>Arduino IDE 1.8.11 Espressif System 1.0.6</li>
  <li>ArduinoDroid (6.3.1)</li>  
 </ul>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/ttgovga32v12.jpg'></center>

