EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:switches
LIBS:relays
LIBS:motors
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:dht-i2c-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "DHT11 / DHT22 to I2C converter"
Date ""
Rev ""
Comp "Copyright (c) 2018 Michael Buesch <m@bues.ch>"
Comment1 "https://bues.ch/"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ATTINY85-20PU U1
U 1 1 5AB3EEBA
P 5550 3750
F 0 "U1" H 4400 4150 50  0000 C CNN
F 1 "ATTINY85-20PU" H 6550 3350 50  0000 C CNN
F 2 "Housings_DIP:DIP-8_W7.62mm" H 6550 3750 50  0001 C CIN
F 3 "" H 5550 3750 50  0001 C CNN
	1    5550 3750
	1    0    0    -1  
$EndComp
$Comp
L CP_Small C2
U 1 1 5AB3EF48
P 7600 3750
F 0 "C2" H 7610 3820 50  0000 L CNN
F 1 "47 µF" H 7610 3670 50  0000 L CNN
F 2 "" H 7600 3750 50  0001 C CNN
F 3 "" H 7600 3750 50  0001 C CNN
	1    7600 3750
	1    0    0    -1  
$EndComp
$Comp
L C_Small C1
U 1 1 5AB3EFA9
P 7000 3750
F 0 "C1" H 7010 3820 50  0000 L CNN
F 1 "100 nF" H 7010 3670 50  0000 L CNN
F 2 "" H 7000 3750 50  0001 C CNN
F 3 "" H 7000 3750 50  0001 C CNN
	1    7000 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	6900 3500 8200 3500
Wire Wire Line
	8200 4000 6900 4000
Wire Wire Line
	7000 3650 7000 3500
Connection ~ 7000 3500
Wire Wire Line
	7000 3850 7000 4000
Connection ~ 7000 4000
Wire Wire Line
	7600 3650 7600 3500
Connection ~ 7600 3500
Wire Wire Line
	7600 3850 7600 4000
Connection ~ 7600 4000
$Comp
L R R1
U 1 1 5AB3F438
P 3450 4000
F 0 "R1" V 3530 4000 50  0000 C CNN
F 1 "10k" V 3450 4000 50  0000 C CNN
F 2 "" V 3380 4000 50  0001 C CNN
F 3 "" H 3450 4000 50  0001 C CNN
	1    3450 4000
	0    1    1    0   
$EndComp
Text GLabel 3200 4000 0    60   Input ~ 0
VCC
Wire Wire Line
	3600 4000 4200 4000
$Comp
L Conn_02x03_Odd_Even J1
U 1 1 5AB3F566
P 3200 3200
F 0 "J1" H 3250 3400 50  0000 C CNN
F 1 "ISP" H 3250 3000 50  0000 C CNN
F 2 "" H 3200 3200 50  0001 C CNN
F 3 "" H 3200 3200 50  0001 C CNN
	1    3200 3200
	-1   0    0    1   
$EndComp
Wire Wire Line
	3500 3300 3500 3600
Wire Wire Line
	3500 3600 4200 3600
Wire Wire Line
	3600 3200 3600 3700
Wire Wire Line
	3600 3700 4200 3700
Wire Wire Line
	3200 4000 3300 4000
Wire Wire Line
	3700 3100 3700 4000
Connection ~ 3700 4000
Wire Wire Line
	2400 3500 4200 3500
Wire Wire Line
	3500 3300 3400 3300
Wire Wire Line
	3600 3200 3400 3200
Wire Wire Line
	3700 3100 3400 3100
Wire Wire Line
	2400 3200 2900 3200
Text GLabel 2800 3100 0    60   Input ~ 0
GND
Text GLabel 2800 3300 0    60   Input ~ 0
VCC
Wire Wire Line
	2800 3100 2900 3100
Wire Wire Line
	2800 3300 2900 3300
Wire Wire Line
	2400 3200 2400 3500
$Comp
L Conn_01x02 J4
U 1 1 5AB3FC72
P 8450 3700
F 0 "J4" H 8450 3800 50  0000 C CNN
F 1 "POWER in" H 8450 3500 50  0000 C CNN
F 2 "" H 8450 3700 50  0001 C CNN
F 3 "" H 8450 3700 50  0001 C CNN
	1    8450 3700
	1    0    0    -1  
$EndComp
Text GLabel 8200 3400 1    60   Input ~ 0
VCC
Text GLabel 8200 4100 3    60   Input ~ 0
GND
Wire Wire Line
	8200 3400 8200 3700
Wire Wire Line
	8200 3700 8250 3700
Wire Wire Line
	8250 3800 8200 3800
Wire Wire Line
	8200 3800 8200 4100
$Comp
L Conn_01x03 J2
U 1 1 5AB3FDEF
P 4000 1550
F 0 "J2" H 4000 1750 50  0000 C CNN
F 1 "I2C" H 4000 1350 50  0000 C CNN
F 2 "" H 4000 1550 50  0001 C CNN
F 3 "" H 4000 1550 50  0001 C CNN
	1    4000 1550
	0    -1   -1   0   
$EndComp
$Comp
L Conn_01x04 J3
U 1 1 5AB3FE6B
P 4000 5800
F 0 "J3" H 4000 6000 50  0000 C CNN
F 1 "DHT11/22" H 4000 5500 50  0000 C CNN
F 2 "" H 4000 5800 50  0001 C CNN
F 3 "" H 4000 5800 50  0001 C CNN
	1    4000 5800
	0    1    1    0   
$EndComp
Text GLabel 4100 1850 3    60   Input ~ 0
GND
Wire Wire Line
	4100 1850 4100 1750
Wire Wire Line
	3900 1750 3900 3700
Wire Wire Line
	4000 1750 4000 3500
Text Label 3900 2350 1    39   ~ 0
SCL
Text Label 4000 2350 1    39   ~ 0
SDA
Connection ~ 3900 3700
Connection ~ 4000 3500
$Comp
L R R2
U 1 1 5AB40304
P 5100 2500
F 0 "R2" V 5180 2500 50  0000 C CNN
F 1 "3.9k" V 5100 2500 50  0000 C CNN
F 2 "" V 5030 2500 50  0001 C CNN
F 3 "" H 5100 2500 50  0001 C CNN
	1    5100 2500
	0    1    1    0   
$EndComp
$Comp
L R R3
U 1 1 5AB4033D
P 5100 2900
F 0 "R3" V 5180 2900 50  0000 C CNN
F 1 "3.9k" V 5100 2900 50  0000 C CNN
F 2 "" V 5030 2900 50  0001 C CNN
F 3 "" H 5100 2900 50  0001 C CNN
	1    5100 2900
	0    1    1    0   
$EndComp
Wire Wire Line
	4200 2500 4000 2500
Connection ~ 4000 2500
Text GLabel 5450 2700 2    60   Input ~ 0
VCC
Wire Wire Line
	5250 2500 5350 2500
Wire Wire Line
	5350 2500 5350 2900
Wire Wire Line
	5350 2900 5250 2900
Text GLabel 4100 5500 1    60   Input ~ 0
VCC
Text GLabel 3800 5500 1    60   Input ~ 0
GND
NoConn ~ 3900 5600
Wire Wire Line
	3800 5500 3800 5600
Wire Wire Line
	4100 5500 4100 5600
Wire Wire Line
	4000 5600 4000 3900
Wire Wire Line
	4000 3900 4200 3900
NoConn ~ 4200 3800
Connection ~ 8200 3500
Connection ~ 8200 4000
Text Notes 3850 1150 0    118  ~ 0
I2C
Text Notes 3650 6550 0    118  ~ 0
DHT11\nDHT22
$Comp
L Jumper JP1
U 1 1 5AB4142B
P 4500 2500
F 0 "JP1" H 4500 2650 50  0000 C CNN
F 1 "Jumper" H 4500 2420 50  0000 C CNN
F 2 "" H 4500 2500 50  0001 C CNN
F 3 "" H 4500 2500 50  0001 C CNN
	1    4500 2500
	1    0    0    -1  
$EndComp
$Comp
L Jumper JP2
U 1 1 5AB4150D
P 4500 2900
F 0 "JP2" H 4500 3050 50  0000 C CNN
F 1 "Jumper" H 4500 2820 50  0000 C CNN
F 2 "" H 4500 2900 50  0001 C CNN
F 3 "" H 4500 2900 50  0001 C CNN
	1    4500 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 2900 3900 2900
Connection ~ 3900 2900
Wire Wire Line
	5350 2700 5450 2700
Connection ~ 5350 2700
Wire Wire Line
	4800 2500 4950 2500
Wire Wire Line
	4800 2900 4950 2900
Text Label 4000 5450 1    39   ~ 0
SIG
$EndSCHEMATC
