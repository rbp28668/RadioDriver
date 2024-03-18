EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L teensy:Teensy4.0 U?
U 1 1 6027C267
P 6250 4350
F 0 "U?" H 6250 5965 50  0000 C CNN
F 1 "Teensy4.0" H 6250 5874 50  0000 C CNN
F 2 "" H 5850 4550 50  0001 C CNN
F 3 "" H 5850 4550 50  0001 C CNN
	1    6250 4350
	1    0    0    -1  
$EndComp
$Comp
L Interface_UART:MAX3232 U?
U 1 1 6027E090
P 3350 4650
F 0 "U?" H 3350 6031 50  0000 C CNN
F 1 "MAX3232" H 3350 5940 50  0000 C CNN
F 2 "" H 3400 3600 50  0001 L CNN
F 3 "https://datasheets.maximintegrated.com/en/ds/MAX3222-MAX3241.pdf" H 3350 4750 50  0001 C CNN
	1    3350 4650
	-1   0    0    -1  
$EndComp
$Comp
L Interface_UART:MAX490E U?
U 1 1 6027FCD2
P 3300 2000
F 0 "U?" H 3300 2681 50  0000 C CNN
F 1 "MAX490E" H 3300 2590 50  0000 C CNN
F 2 "" H 3300 1400 50  0001 C CNN
F 3 "https://datasheets.maximintegrated.com/en/ds/MAX1487E-MAX491E.pdf" H 3060 2450 50  0001 C CNN
	1    3300 2000
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Micro_SD_Card J?
U 1 1 60280B98
P 9550 5450
F 0 "J?" H 9500 6167 50  0000 C CNN
F 1 "Micro_SD_Card" H 9500 6076 50  0000 C CNN
F 2 "" H 10700 5750 50  0001 C CNN
F 3 "http://katalog.we-online.de/em/datasheet/693072010801.pdf" H 9550 5450 50  0001 C CNN
	1    9550 5450
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x06_Odd_Even J?
U 1 1 60283250
P 9550 2850
F 0 "J?" H 9600 3267 50  0000 C CNN
F 1 "Conn_02x06_Odd_Even" H 9600 3176 50  0000 C CNN
F 2 "" H 9550 2850 50  0001 C CNN
F 3 "~" H 9550 2850 50  0001 C CNN
	1    9550 2850
	1    0    0    -1  
$EndComp
$Comp
L Connector:DB9_Female_MountingHoles J?
U 1 1 602844FB
P 1150 1900
F 0 "J?" H 1068 2592 50  0000 C CNN
F 1 "DB9_Female_MountingHoles" H 1068 2501 50  0000 C CNN
F 2 "" H 1150 1900 50  0001 C CNN
F 3 " ~" H 1150 1900 50  0001 C CNN
	1    1150 1900
	-1   0    0    -1  
$EndComp
$Comp
L Connector:DB15_Female_MountingHoles J?
U 1 1 60285120
P 900 4600
F 0 "J?" H 806 5592 50  0000 C CNN
F 1 "DB15_Female_MountingHoles" H 806 5501 50  0000 C CNN
F 2 "" H 900 4600 50  0001 C CNN
F 3 " ~" H 900 4600 50  0001 C CNN
	1    900  4600
	-1   0    0    -1  
$EndComp
$Comp
L Device:Rotary_Encoder_Switch SW?
U 1 1 60287A04
P 5250 1650
F 0 "SW?" H 5250 2017 50  0000 C CNN
F 1 "Rotary_Encoder_Switch" H 5250 1926 50  0000 C CNN
F 2 "" H 5100 1810 50  0001 C CNN
F 3 "~" H 5250 1910 50  0001 C CNN
	1    5250 1650
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW?
U 1 1 6028888F
P 7350 1650
F 0 "SW?" H 7350 1935 50  0000 C CNN
F 1 "SW_Push" H 7350 1844 50  0000 C CNN
F 2 "" H 7350 1850 50  0001 C CNN
F 3 "~" H 7350 1850 50  0001 C CNN
	1    7350 1650
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_SPDT_MSM SW?
U 1 1 6028930E
P 6450 1700
F 0 "SW?" H 6450 1985 50  0000 C CNN
F 1 "SW_SPDT_MSM" H 6450 1894 50  0000 C CNN
F 2 "" H 6450 1700 50  0001 C CNN
F 3 "~" H 6450 1700 50  0001 C CNN
	1    6450 1700
	1    0    0    -1  
$EndComp
$EndSCHEMATC
