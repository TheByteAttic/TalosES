Here you can find the Arduino sketch that constitutes Talos ES™'s basic firmware (a simple BIOS, monitor and assembler). To upload this sketch into Talos ES™ you will need the <a href="https://github.com/MCUdude/MightyCore">Arduino MightyCore library</a>. You will also need to burn the appropriate bootloader into the ATmega1284P-PU <i>before placing it in Talos ES™</i>, so to get the ATmega to use an external 18.432MHz strong (4-pin) oscillator (an <i>oscillator</i>, not a crystal). Below are the correct Arduino configurations to burn the boatloader. In the directory "/Bootloader circuit photos" above you can find photos of the appropriate breadboard circuit to burn the bootloader. Finally, to load the firmware into the ATmega1284P-PU after having placed it into Talos ES™, you will need a standard FTDI adapter (USB-to-Serial).

![BurnBootloader1](https://github.com/TheByteAttic/TalosES/assets/69539226/14295e4b-fa40-4898-b9ec-f8741cb7693e)

![BurnBootloader2](https://github.com/TheByteAttic/TalosES/assets/69539226/5aadc621-fdaf-4eb9-ae8a-346e10df09f8)

![BurnBootloader3](https://github.com/TheByteAttic/TalosES/assets/69539226/a9f66a7a-db7e-4331-b613-80ede04c115c)
