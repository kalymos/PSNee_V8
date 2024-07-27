# PSNee V8

The first stealth modchip supporting unlocking BIOS of Japanese versions Sony PlayStation 1

![Logo](images/PSNee_V8_logo.png)
Developed by **brill** & **postal2201**, based on PSNee V7 open source project.  
- http://www.emu-land.net/forum/index.php/topic,85934.0.html

## Supported platforms
PsNee V8.5 supports the following MCU's:  
- ATmega328(A/P/PA) @16Mhz  
- ATmega168(A/P/PA) @16Mhz
- Atmega32U4        @16Mhz
- ATtiny25/45/85    @8Mhz no BIOS patch!

## Installation
Place the cor folder in your ~arduino15/packages/

In the Arduino IDE choose tools->board->PSNee and the appropriate board

Choosing your ISP programmer

If you need to change fuses tool->burn bootloader (If you get an error message that it could not burn the bootloader, ignore it.)

To inject the code into the board, sketch->upload using a programmer


Use the programmer to flash MCU. 
**Flashing via COM port is not supported.**

### Fuses
Before flashing the MCU, you need to configure the fuses.  
- Fuses for JAP_FAT consoles: **H: DF, L: EE, E: FF**  
- Fuses for all other consoles: **H: DF, L: FF, E: FF**  

![Console](images/console.png)

## Installation diagram
![Board](images/PSNee_V8_pinout.png)

## model tested
![test](images/model_tested.png)
