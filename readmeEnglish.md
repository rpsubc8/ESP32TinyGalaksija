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


<br><br>
<h1>ArduinoDroid</h1>
The ArduinoDroid will allow us to compile and upload the <b>Galaksija</b> project to the ESP32 from a 64-bit Android device, that is, a mobile phone, a tablet or an Android box, as long as our device is supported.<br>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewWin.gif'></center>
If we are on Windows, the <b>makearduinodroidwin.bat</b> script must be executed just once, which leaves us with the entire data structure of the dataFlash directory, as well as the rest of the files, in the directory from which the script is launched.<br>
If we are on Android with <b>termux</b> or Linux, we must run <b>makearduinodroidlinux.sh</b>.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewTermux.gif'></center>
Basically, we are left with all the files on a single level, without possible subdirectories.<br>
The 'notdelete' directory is internal to the script and is used to copy the <b>gbCompileOpt.h</b>.<br>
At the end of the script itself, we can open the <b>galaksija.ino</b>, and recompile from the ArduinoDroid, without anything special. The script has a pause at the beginning, in case we want to stop the script from outside, as well as a stop at the end, to see the results of the process.<br>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewArduinoDroid.gif'></center>
If we notice, everything revolves around the pragma <b>gb_use_lib_compile_arduinodroid</b> of the <b>gbCompileOpt.h</b> header, which in normal mode is used with multi-level paths and in ArduinoDroid with the script, in a single level.<br>
This script searches for the files going up levels until it reaches <b>TinyGalaksijattgovga32/galaksija</b>, so it is required to download the complete project with the entire structure:
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
The ArduinoDroid (6.3.1) with ESP32 support only works with 64-bit processors. 


<br><br>
<h1>Arduino Web Editor</h1>
The makearduinodroidwin.bat script must be executed only once. Once finished, it is uploaded to the cloud as any project, whether compressed in zip or files.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewArduinoWebEditor.gif'></center>
It's basically the same thing, since you're trying to keep everything at a single directory level. It is recommended to import the project compressed in zip with normal compression, not maximum, since some cloud web editors do not support zip 100%.


<br><br>
<h1>PlatformIO</h1>
PLATFORMIO 2.2.1 must be installed from the Visual Studio extensions. Espressif32 v3.3.2 is also required.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewPlatformIOinstall.gif'></center>
The working directory <b>TinyGalaksijattgovga32</b> will then be selected.
We must modify the <b>platformio.ini</b> file and the <b>upload_port</b> option to select the COM port where we have our TTGO VGA32 board.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewPlatformIO.gif'></center>
Then we will proceed to compile and upload to the board. No partitions are used, so we must upload the entire compiled binary. Everything is ready to avoid having to install the bitluni or fabgl libraries.


<br><br>
<h1>Arduino IDE</h1>
The entire project is compatible with Arduino 1.8.11 framework. We just have to open <b>galaksija.ino</b> from the <b>galaksija</b> directory.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewArduinoIDEpreferences.gif'></center>
We must install the spressif extensions in the additional card url manager <b>https://dl.espressif.com/dl/package_esp32_index.json</b><br>
The project is already prepared, so no bitluni or fabgl library is needed. We must disable the PSRAM option, and if the binary exceeds 1 MB, select 4 MB partition when uploading. Although the code does not use PSRAM, if the option is active and our ESP32 does not have it, an exception will be generated and it will restart in loop mode.


<br><br>
<h1>Usability</h1>
The following actions are allowed from the menu (F1 key):
<ul>
 <li>Load or select a tape (GTP)</li>
 <li>Load or select a snapshot (GAL)</li>
 <li>Select language Spanish or English</li>
 <li>Select 16 video modes</li>
 <li>Change the color green, orange or black and white, like retro monitors</li>
 <li>Show CPU and video statistics</li>
 <li>Enable USB debugging output (serial)</li>
 <li>Help</li>
 <li>Reset the ESP32 or the emulator</li>
 <li>Intercept the tape routine when we type the OLD command from BASIC, and we have selected a GTP, instead of loading and executing it.</li>
 <li>Change polling milliseconds for video, keyboard</li>
 <li>View free RAM</li>
</ul>
There is a basic low-resource OSD, that is, very simple, which is displayed by pressing the <b>F1</b> key.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewOSD.gif'></center>
Some menus and submenus are immediately executed, that is, when you accept with the ENTER key, the OSD is closed and the action is applied. Other menus, on the other hand, return to the previous menu.<br>
You can always exit the OSD with the ESC key.<br>
The Up and Down cursors allow you to navigate through each action. If they are held down for a couple of milliseconds, they allow quick action.


<br><br>
<h1>GTP (tapes)</h1>
Whenever we load a GTP (tape) from the OSD, it will allow us to choose between:
<ul>
 <li><b>Load:</b> Allows us to load it.</li>
 <li><b>Select:</b> It only selects it, being able to read it from BASIC with the OLD command.</li>
</ul>
When the OLD command is issued, there is a little trick that is not 100% correct, but efficient in host consumption, to intercept the ROM routine, and proceed to load it. This interception can also be disabled from the OSD.
If we have chosen loading, instead of selection, when we have already chosen a tape from the OSD, it will appear if we want to use <b>WARM START BASIC</b>.<br>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewWarmStart.gif'></center>
Later, it will tell us if we want it to do an automatic RUN from BASIC or not.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewRUN.gif'></center>
If we accept, from BASIC, RUN will be written automatically.<br>
It is advisable to have a clean execution, whenever we load games, that is, restart the emulator, to leave everything as good as possible.<br>
When memory errors occur, a trace is left through the serial port, and after a couple of them, the emulator (not the ESP32) restarts itself, leaving us a trace.


<br><br>
<h1>Snapshot GAL</h1>
Whenever we load a GAL, it will give us the option of executing <b>RUN</b> or not from BASIC.<br>
GALs must be 8268 bytes in size.


<br><br>
<h1>Statistics</h1>
There is an option is <b>OSD Options</b> that allows us to see what is called the OSD, which are the statistics of CPU and video consumption, for each second (1000 milliseconds).
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewStatsCPU.gif'></center>
In a 50 fps (frames per second) emulation there are 2 sections:
<ul>
 <li>CPU time (20 milliseconds)</li>
 <li>Video dump time (20 milliseconds)</li>
</ul>
It's 20 milliseconds, because it's 50 fps (1000 / 50 = 20). The video dump time does not have to be 50 real fps, since what is important is the CPU time, which is what will set the real 50 fps. We can then get 50 fps from the CPU, but have 24 fps in the video dump, and it would be totally functional. However, in this emulator, it is possible to exceed 50 real fps.<br>
The statistics have been placed on the right and vertically, so as not to interfere at any time on the emulated screen.

| Field | Description                                                                                      |
| ----- | ------------------------------------------------------------------------------------------------ |
| C     | ESP32 microseconds that it takes to process the current 20 milli frame                           |
| M     | Microseconds of the ESP32 that it takes to process the fastest frame of 20 millis in 1000 millis |
| MX    | Microseconds of the ESP32 that it takes to process the slowest frame of 20 millis in 1000 millis |
| I     | ESP32 free microseconds in a 20 milli frame, counting the video dump                             |
| FPS   | Real FPS processed by the emulator at 1000 millis                                                |
| FND   | FPS that would give us no limit at 20 millis per frame, setting the CPU menu to 0 ms (fast)      |
| V     | Microseconds of the ESP32 that it takes to dump a frame of 20 millis                             |
| F     | Real FPS we are actually dumping the video at                                                    |


<br><br>
<h1>PS/2 keyboard</h1>
A 100% PS/2 keyboard is required, and does not require an initialization command.<br>
The Galaksija has a 54-element keystroke buffer.<br>
The following keys are emulated:

| Emulator keys 	   | Keys (real PS/2 keyboard)  | Buffer Galaksija |
| ----------------- | -------------------------- | ---------------- |
| 0 .. 9            | 0 .. 9                     | 32 .. 41         |
| A .. Z            | A .. Z                     |  1 .. 26         |
| Above             | Above                      | 27               |
| Below             | Below                      | 28               |
| Left              | Left                       | 29               |
| Right             | Right                      | 30               |
| Space bar         | Space bar                  | 31               |
| Delete            | Delete                     | 29               |
| ;                 | ;                          | 42               |
| "                 | '                          | 43               |
| ,                 | ,                          | 44               |
| =                 | =                          | 45               |
| .                 | .                          | 46               |
| /                 | /                          | 47               |
| ENTER             | ENTER or RETURN            | 48               |
| CONTROL           | CONTROL                    | 50               |
| SCROLL            | SCROLL                     | 52               |
| SHIFT             | SHIFT left or right        | 53               |

Galaksija entry 51, which would be DELETE, is not mapped in the emulator.<br>
The F1 key on the PS/2 keyboard brings up the emulator's OSD menu.


<br><br>
<h1>Remote keyboard</h1>
Using the keyboard is allowed from the Visual Studio console or from a putty terminal.<br>
The remote keyboard does not have the same response as a real keyboard, since key commands are sent, and not a map of pressed characters.<br>
It cannot be used in the Arduino IDE console, because it always waits for ENTER to be sent to accept commands.<br>
We have to activate the <b>use_lib_log_keyboard_uart</b> pragma in <b>gbConfig.h</b>, compile and run.<br>
Not all the keys are mapped, and it is something basic, useful for testing, without failing to use the real PS/2 keyboard. 

| PC keys           | Function                   |
| ----------------- | -------------------------- |
| F2 or TAB         | Shows the OSD menu         |
| ESC               | Exit menu                  |
| Above             | Above                      |
| Below             | Below                      |
| Left              | Left                       |
| Right             | Right                      |
| Delete or DEL     | Delete                     |
| ENTER             | ENTER                      |
| 0 .. 9            | 0 .. 9                     |
| A .. Z            | A .. Z                     |
| .                 | .                          |
| -                 | Left shift                 |

If we press the '-' at the same time as another key, for example, the 'A' is the equivalent of pressing (SHIFT + A) in the emulator.


<br><br>
<h1>GPIO</h1>
The <b>hardware.h</b> file contains what is needed for the GPIO of:
<ul>
 <li>
  <b>Keyboard:</b>
  <ul>
   <li>KEYBOARD_DATA - (32)</li>
   <li>KEYBOARD_CLK - (33)</li>
  </ul> 
 </li> 
 <li>
  <b>VGA syncs:</b>
  <ul>
   <li>HSYNC_PIN - (23)</li>
   <li>VSYNC_PIN - (15)</li>
  </ul>
 </li>
 <li>
  <b>VGA red</b> 
  <ul>
   <li>PIN_RED_LOW - (21)</li>
   <li>PIN_RED_HIGH -(22)</li>
  </ul>
 </li>
 <li>
  <b>VGA green</b> 
  <ul>
   <li>PIN_GREEN_LOW - (18)</li>
   <li>PIN_GREEN_HIGH - (19)</li>
  </ul>
 <li>
  <b>VGA blue</b>
  <ul>
   <li>PIN_BLUE_LOW - (4)</li>
   <li>PIN_BLUE_HIGH - (5)</li>
  </ul>
</ul>
It is designed for the TTGO VGA v1.2 board, so if we have another type of board, this is where we must define the changes.<br>
When we use 6 bpp, we are using all the VGA GPIOs, but when we use 3 bpp, we only use:
<ul>
 <li>PIN_RED_HIGH - (22)</li>
 <li>PIN_GREEN_HIGH - (19)</li>
 <li>PIN_BLUE_HIGH - (5)</li>
</ul>


<br><br>
<h1>Options</h1>
The options are selected in the <b>gbConfig.h file</b>:
We can choose the time (milliseconds) to be able to select the video mode during boot:
<ul>
 <li><b>use_lib_boot_time_select_vga:</b> The higher the value, the longer it will take to boot. If during this time, we press the keys from 0 to 9 or the letters from A to F, we will be choosing a video mode. It is also valid for reboot.</li>
</ul>
We have the possibility of choosing one of the 16 possible default video modes, that is, by compilation, and that is the one that starts by default when the board starts up (360x200):
<ul>
 <li><b>use_lib_boot_vga_360x200x70hz_bitluni_3bpp:</b> 360x200 8 colors.</li>
 <li><b>use_lib_boot_vga_360x200x70hz_bitluni_apll_3bpp:</b> 360x200 8 colors apll correction, for ESP32 that hang.</li>
 <li><b>use_lib_boot_vga_320x200x70hz_bitluni_3bpp:</b> 320x200 8 colors.</li>
 <li><b>use_lib_boot_vga_320x200x70hz_fabgl_3bpp:</b> 320x200 8 colors with fabgl parameters.</li>
 <li><b>use_lib_boot_vga_320x200x70hz_bitluni_apll_3bpp:</b> 320x200 8 colors apll correction, for ESP32 that hang.</li>
 <li><b>use_lib_boot_vga_320x240x60hz_bitluni_3bpp:</b> 320x240 8 colors.</li>
 <li><b>use_lib_boot_vga_320x240x60hz_fabgl_3bpp:</b> 320x240 8 colors with fabgl parameters.</li>
 <li><b>use_lib_boot_vga_320x240x60hz_bitluni_apll_3bpp:</b> 320x240 8 colors apll correction, for ESP32 that hang.</li>
 <li><b>use_lib_boot_vga_360x200x70hz_bitluni_6bpp:</b> 360x200 64 colors</li>
 <li><b>use_lib_boot_vga_360x200x70hz_bitluni_apll_6bpp:</b> 360x200 64 colors apll correction, for ESP32 that hang.</li>
 <li><b>use_lib_boot_vga_320x200x70hz_bitluni_6bpp:</b> 320x200 64 colors</li>
 <li><b>use_lib_boot_vga_320x200x70hz_fabgl_6bpp:</b> 320x200 64 colors with fabgl parameters.</li>
 <li><b>use_lib_boot_vga_320x200x70hz_bitluni_apll_6bpp:</b> 320x200 64 colors apll correction, for ESP32 that hang.</li>
 <li><b>use_lib_boot_vga_320x240x60hz_bitluni_6bpp:</b> 320x240 64 colors</li>
 <li><b>use_lib_boot_vga_320x240x60hz_fabgl_6bpp:</b> 320x240 64 colors with fabgl parameters.</li>
 <li><b>use_lib_boot_vga_320x240x60hz_bitluni_apll_6bpp:</b> 320x240 64 colors apll correction, for ESP32 that hang.</li>
</ul>
As expected, you cannot select all the modes at the same time, so you must uncomment the one you want, and comment on the rest.<br>
There are more options.
<ul> 
 <li><b>use_lib_log_serial:</b> Logs are sent via USB serial port.</li>
 <li><b>use_lib_fix_double_precision:</b> Do not use FPU for PLL calculation.</li>
 <li><b>use_lib_debug_i2s:</b> Detailed information of video mode initialization.</li>
 <li><b>use_lib_keyboard_uart:</b> Allows you to use a remote keyboard via UART via putty or the VStudio console.</li>
 <li><b>use_lib_log_keyboard_uart:</b> The remote keyboard trace.</li>
</ul>


<br><br>
<h1>Tool data2h</h1>
I have created a very basic tool (win32) to convert .GTP and .GAL files into .h in ready mode to be processed by the emulator.<br>
It is recommended to have short file names. Additionally, the tool cuts to 32 characters to display in the OSD.<br>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewWinData2h.gif'></center>
We just have to leave the .GAL and .GTP files in the <b>input/GAL</b> folder and <b>input/GTP</b> run the <b>data2h.exe</b> file , so that an output will be generated in the <b>output/dataFlash</b> directory.<a href='https://github.com/rpsubc8/ESP32TinyGalaksija/tree/main/esp32/tools/data2h'>Tool data2h</a><br><br>
<pre>
 input/
  gal/
  gtp/
 output/
  dataFlash/
   gal/
   gtp/ 
</pre>
Later we must copy the <b>dataFlash</b> directory to the <b>TinyGalaksijattgovga32\galaksija</b> project, overwriting the previous dataFlash folder. It is recommended to clean the project and compile again.<br>
This tool is very simple, and does not control errors, so it is recommended to leave the files with very simple names and as simple as possible.<br>
Since the source code is available, it can be compiled for Linux or for Android under Termux. In Termux we have to install the <b>gcc</b> or <b>clang</b> package and compile:<br>
<pre>
 gcc -s data2h.cpp -odata2h.a
</pre>
It must be done from the home data directory of the Android device, so if we do it in any other directory, the <b>data2h.a</b> created will never be able to be executed, unless the device is rooted.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyGalaksija/main/preview/previewTermuxData2h.gif'></center>
The project in PLATFORM.IO is prepared for 1 MB of Flash. If we need the 4MB of flash, we will have to modify the entry in the <b>platformio.ini</b> file
<pre>board_build.partitions = huge_app.csv</pre>
In the Arduino IDE, we must choose the <b>Partition Scheme (Huge APP)</b> option.


<br><br>
<h1>DIY circuit</h1>








