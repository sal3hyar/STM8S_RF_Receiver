# STM8S EV1527 RF_Module Receiver
A small project using stm8s mcu to receive &amp; parse ev1527 433Mhz RF-module data

I used EIDE inside VSCode. Cosmic_stm8 as Compiler and STVP as Flasher


# Description
1-Connect data pin of RF-module to PC3 (Pin No.13)

2-Connect an LED to PD3 (Pin No.20)

3-Connect Rx pin of your USB-To-TTL to PD5 (Pin No.2)(Tx) 

4-Upload .hex file to microcontroller (stm8s003f3)

5-Run a Serial Monitor program to see output

That's it.


# Output
This is what you should see in serial monitor:

-----------------------------------------------
Sync-Bit Length = 2648(ticks)

Received Data Frame = 111000000011100000110001

Remote Code = 918403

Pressed Key Name = A

Pressed Key Code = 1

-----------------------------------------------



# License
Totally free :)
