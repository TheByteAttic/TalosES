Here you can find the Arduino sketch that constitutes Talos ES™'s basic firmware (a simple BIOS, monitor and assembler). But, before you can upload this sketch into Talos ES™, you will need to burn the appropriate bootloader in the ATmega1284P-PU I/O controller. This should be done prior to placing the ATmega1284P-PU in Talos ES™'s motherboard. To do so, you need to:
<UL>
  <LI>First, install the <a href="https://github.com/MCUdude/MightyCore">Arduino MightyCore library</a> in your Arduino IDE, as per the instructions in the library's repository.</LI>
  <LI>Set up the circuit illustrated in the photos in the directory "/Bootloader circuit photos" above, <i>but use a 18.432MHz strong 4-pin oscillator, NOT the 16MHz one shown in the photos</i> (these photos were taken from a very early prototype). You may need to slightly edit the MightyCore library's configuration files to create a 18.432MHz external oscillator entry from an existing 16MHz one.</LI>
  <LI>Using an Arduino Uno as programmer ("Arduino as ISP (MightyCore)"), burn the bootloader on the ATmega1284P-PU in the breadboard, as per the configuration shown in the figures below, <i>except that you should use a 18.432MHz external oscillator, NOT a 16MHz one</i> (the figures below were taken from an early prototype).</LI>
  <LI>Finally, to load the firmware into the ATmega1284P-PU, after having placed it into Talos ES™'s motherboard, you will need a standard FTDI adapter (USB-to-Serial) connected the Talos ES™'s FTDI port.</LI>
</UL>

![BurnBootloader1](https://github.com/TheByteAttic/TalosES/assets/69539226/14295e4b-fa40-4898-b9ec-f8741cb7693e)

![BurnBootloader2](https://github.com/TheByteAttic/TalosES/assets/69539226/5aadc621-fdaf-4eb9-ae8a-346e10df09f8)

![BurnBootloader3](https://github.com/TheByteAttic/TalosES/assets/69539226/a9f66a7a-db7e-4331-b613-80ede04c115c)
