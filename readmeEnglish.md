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
 <li>Project compatible with Web Editor and ArduinoDroid (6.3.1) with tool <b>makearduinodroidwin.bat</b>b> and <b>makearduinodroidlinux.sh</b>b></li>
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
