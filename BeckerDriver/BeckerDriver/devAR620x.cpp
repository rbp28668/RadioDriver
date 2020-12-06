/*
   LK8000 Tactical Flight Computer -  WWW.LK8000.IT
   Released under GNU/GPL License v.2
   See CREDITS.TXT file for authors and copyrights

   $Id$
*/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif


#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <iostream>

#include "devAR620x.h"

//#include "externs.h"
//#include "Globals.h"
//#include "device.h"

#define RoundFreq(a) ((int)((a)*1000.0+0.5)/1000.0)
#define Freq2Idx(a)  (int)(((a)-118.0) * 3040/(137.00-118.0)+0.5)
#define BIT(n) (1 << (n))

#define ACTIVE_STATION  1
#define PASSIVE_STATION 0


//#define RESEND_ON_NAK       /* switch for command retry on transmission fail  */
#define RADIO_VOLTAGE       /* read Radio Supply voltage and store it in BATT2 indicator */

constexpr uint8_t HEADER_ID = 0xA5;
#define PROTOKOL_ID     0x14
#define QUERY           BIT(7)
#define DAUL            BIT(8)
#define SQUELCH         BIT(7)

#ifdef TESTBENCH
int iAR620DebugLevel =1;
#else
int iAR620DebugLevel =0;
#endif
typedef union
{
  uint16_t intVal16;
  uint8_t  intVal8[2];
} IntConvertStruct;

IntConvertStruct CRC;
IntConvertStruct sFrequency;
IntConvertStruct sStatus;

#define MAX_CMD_LEN 128


#define  RESEND_DELAY 100

volatile bool bSending = false;
//volatile bool bReceiving = false;

//bool AR620x::Install(PDeviceDescriptor_t d){
//
//  _tcscpy(d->Name, TEXT("Dittel AR620x::"));
//  d->IsRadio        = AR620x::IsRadio;
//  d->PutVolume      = AR620x::PutVolume;
//  d->PutSquelch     = AR620x::PutSquelch;
//  d->PutFreqActive  = AR620x::PutFreqActive;
//  d->PutFreqStandby = AR620x::PutFreqStandby;
//  d->StationSwap    = AR620x::StationSwap;
//  d->ParseNMEA      = NULL;
//  d->ParseStream    = AR620x::ParseString;
//  d->PutRadioMode      = AR620x::RadioMode;
//  radioState.Enabled8_33 = true;
//  sStatus.intVal16 = SQUELCH; // BIT7 for Squelch enabled
//  return(true);
//
//}
//
//bool AR620x::Register(void){
//  return(devRegister(
//    TEXT("Becker AR620x::"),
//    (1l << dfRadio),
//    AR620x::Install
//  ));
//}
//
//
//bool AR620x::IsRadio(PDeviceDescriptor_t d){
//  (void)d;
//  return(true);
//}

int AR620x::sendCommand(uint8_t szTmp[], uint16_t len)
{

  bSending = true;
  serial->write(szTmp,len);
  delay(100);

  serial->write(szTmp,len);
  delay(100); // don't listen to old status in the pipe
  bSending = false;
  return true;
}


// Ok so 137 = 118 is 19
// Bottom 4 digits have fraction channel offset so factor of 16 (->304)
// Allow for single decimal digit (x10 -> 3040).
// Arithmetic messing about is basically - calculate channel number starting at 0 for 118.0, adding 1 per 100kHz spacing
// Then shift that left 4 bits and add the offset.
double AR620x::idx2Freq(uint16_t uFreqIdx)
{
double fFreq= 118.000 + (uFreqIdx& 0xFFF0) * (137.000-118.000)/3040.0;
switch(uFreqIdx & 0xF)
{
    case 0:  fFreq += 0.000; break;
    case 1:  fFreq += 0.005; break;
    case 2:  fFreq += 0.010; break;
    case 3:  fFreq += 0.015; break;
    case 4:  fFreq += 0.025; break;
    case 5:  fFreq += 0.030; break;
    case 6:  fFreq += 0.035; break;
    case 7:  fFreq += 0.040; break;
    case 8:  fFreq += 0.050; break;
    case 9:  fFreq += 0.055; break;
    case 10: fFreq += 0.060; break;
    case 11: fFreq += 0.065; break;
    case 12: fFreq += 0.075; break;
    case 13: fFreq += 0.080; break;
    case 14: fFreq += 0.085; break;
    case 15: fFreq += 0.090; break;   
}
return (fFreq);
}


uint16_t Frq2Idx(double fFreq)
{
uint16_t  uFreIdx= Freq2Idx(fFreq);
uFreIdx &= 0xFFF0;
uint8_t uiFrac = ((int)(fFreq*1000.0+0.5)) - (((int)(fFreq *10.0))*100);
    
  switch(uiFrac )
  {
    case 0:   uFreIdx += 0;  break;
    case 5:   uFreIdx += 1;  break;
    case 10:  uFreIdx += 2;  break;
    case 15:  uFreIdx += 3;  break;
    case 25:  uFreIdx += 4;  break;
    case 30:  uFreIdx += 5;  break;
    case 35:  uFreIdx += 6;  break;
    case 40:  uFreIdx += 7;  break;
    case 50:  uFreIdx += 8;  break;
    case 55:  uFreIdx += 9;  break;
    case 60:  uFreIdx += 10; break;
    case 65:  uFreIdx += 11; break;
    case 75:  uFreIdx += 12; break;
    case 80:  uFreIdx += 13; break;
    case 85:  uFreIdx += 14; break;
    case 90:  uFreIdx += 15; break;
    case 100: uFreIdx += 0;  break;
    default:   //StartupStore(_T("undefined Frequency!  %u -> %u %s"),uiFrac, uFreIdx,NEWLINE);     
        break;
  }

return (uFreIdx);
}


static uint16_t CRCBitwise(uint8_t *data, size_t len)
{
  uint16_t crc = 0x0000;
  size_t j;
  int i;
  for (j=len; j>0; j--) {
    crc ^= (uint16_t)(*data++) << 8;
    for (i=0; i<8; i++) {
      if (crc & 0x8000) crc = (crc<<1) ^ 0x8005;
      else crc <<= 1;
    }
  }
  return (crc);
}







/*****************************************************************************
 * this function set the station name and frequency on the AR620x::
 *
 * ComPort        index of the comport the AR620x:: is attached
 * Active_Passive Active or passive station switch
 * fFrequency     station frequency
 * Station        station Name string
 *
 *****************************************************************************/
int  AR620x::setStation(uint8_t *Command ,int Active_Passive, double fFrequency, const char* Station)
{
  unsigned int len = 0;
  assert(Station != NULL);
  assert(Command != NULL);
  if(Command == NULL ) {
    return false;
  }

  IntConvertStruct ActiveFreqIdx;  ActiveFreqIdx.intVal16  = Frq2Idx(radioState.ActiveFrequency );
  IntConvertStruct PassiveFreqIdx; PassiveFreqIdx.intVal16 = Frq2Idx(radioState.PassiveFrequency);
  Command [len++] = HEADER_ID ;
  Command [len++] = PROTOKOL_ID ;
  Command [len++] = 5;

  switch (Active_Passive)
  {
    case ACTIVE_STATION:
         ActiveFreqIdx.intVal16 = Frq2Idx(fFrequency);
         //if(iAR620DebugLevel) StartupStore(_T(">AF:%u  %7.3f%s"), ActiveFreqIdx.intVal16, fFrequency, NEWLINE);
    break;
    default:
    case PASSIVE_STATION:
        PassiveFreqIdx.intVal16 =  Frq2Idx(fFrequency);
        //if(iAR620DebugLevel) StartupStore(_T(">PF:%u  %7.3f%s"), PassiveFreqIdx.intVal16, fFrequency,NEWLINE);
    break;
  }
  Command [len++] = 22;
  Command [len++] = ActiveFreqIdx.intVal8[1];
  Command [len++] = ActiveFreqIdx.intVal8[0];
  Command [len++] = PassiveFreqIdx.intVal8[1];
  Command [len++] = PassiveFreqIdx.intVal8[0];
  CRC.intVal16 =  CRCBitwise(Command, len);
  Command [len++] = CRC.intVal8[1];
  Command [len++] = CRC.intVal8[0];
  return len;
}





AR620x::AR620x(BasicSerial* pbs)
    : serial(pbs)
{
    assert(pbs);
    radioState.ActiveFrequency = 121.280;
    radioState.PassiveFrequency = 121.5;
}

bool AR620x::putVolume(int Volume) {
uint8_t  szTmp[MAX_CMD_LEN];
int len;

    len = 0;
    szTmp [len++] = HEADER_ID ;
    szTmp [len++] = PROTOKOL_ID ;
    szTmp [len++] =2;
    szTmp [len++] =3;
    szTmp [len++]  =  50-Volume*5;
    CRC.intVal16 =  CRCBitwise(szTmp, len);
    szTmp [len++] = CRC.intVal8[1];
    szTmp [len++] = CRC.intVal8[0];
    sendCommand(szTmp,len);
    //if(iAR620DebugLevel) StartupStore(_T(". AR620x:: Volume  %i%s"), Volume,NEWLINE);
    radioState.Volume = Volume;
  return(true);
}




bool AR620x::putSquelch(int Squelch) {
uint8_t  szTmp[MAX_CMD_LEN];
uint8_t len;
    len = 0;
    szTmp [len++] = HEADER_ID ;
    szTmp [len++] = PROTOKOL_ID ;
    szTmp [len++] =2;
    szTmp [len++] =4;
    szTmp [len++] = 6 + (Squelch-1)*2;
    CRC.intVal16 =  CRCBitwise(szTmp, len);
    szTmp [len++] = CRC.intVal8[1];
    szTmp [len++] = CRC.intVal8[0];
    sendCommand(szTmp,len);
    //if(iAR620DebugLevel) StartupStore(_T(". AR620x:: Squelch  %i%s"), Squelch,NEWLINE);
    radioState.Squelch = Squelch;
  return(true);
}


bool AR620x::putFreqActive(double Freq, char const StationName[]) {
int len;
uint8_t  szTmp[MAX_CMD_LEN];
    //if(iAR620DebugLevel) StartupStore(_T(". AR620x:: Active Station %7.3fMHz %s%s"), Freq, StationName,NEWLINE);
    len = AR620x::setStation(szTmp ,ACTIVE_STATION, Freq, StationName);
    sendCommand(szTmp,len);
    radioState.ActiveFrequency=  Freq;
    if(StationName != NULL)
        _snprintf(radioState.ActiveName, NAME_SIZE,"%s",StationName) ;
  return(true);
}


bool AR620x::putFreqStandby(double Freq,  const char StationName[]) {
int len;
uint8_t  szTmp[MAX_CMD_LEN];
    len = AR620x::setStation(szTmp ,PASSIVE_STATION, Freq, StationName);
    sendCommand(szTmp,len);
    radioState.PassiveFrequency =  Freq;
    if(StationName != NULL)
        _snprintf(radioState.PassiveName, NAME_SIZE  ,"%s",StationName) ;
    //if(iAR620DebugLevel) StartupStore(_T(". AR620x:: Standby Station %7.3fMHz %s%s"), Freq, StationName,NEWLINE);
  return(true);
}


bool AR620x::stationSwap() {
uint8_t len=0;
uint8_t  szTmp[MAX_CMD_LEN];
    IntConvertStruct ActiveFreqIdx;  ActiveFreqIdx.intVal16  = Frq2Idx(radioState.ActiveFrequency );
    IntConvertStruct PassiveFreqIdx; PassiveFreqIdx.intVal16 = Frq2Idx(radioState.PassiveFrequency);
    szTmp [len++] = HEADER_ID ;
    szTmp [len++] = PROTOKOL_ID ;
    szTmp [len++] = 5;
    szTmp [len++] = 22;
    szTmp [len++] = PassiveFreqIdx.intVal8[1];
    szTmp [len++] = PassiveFreqIdx.intVal8[0];
    szTmp [len++] = ActiveFreqIdx.intVal8[1];
    szTmp [len++] = ActiveFreqIdx.intVal8[0];
    CRC.intVal16 =  CRCBitwise(szTmp, len);
    szTmp [len++] = CRC.intVal8[1];
    szTmp [len++] = CRC.intVal8[0];
    sendCommand(szTmp,len);
  return(true);
}


bool AR620x::radioMode(int mode) {
uint8_t len;
uint8_t  szTmp[MAX_CMD_LEN];

        if( mode > 0  )
        {
          sStatus.intVal16 |= DAUL;  // turn Dual Mode On
          //if(iAR620DebugLevel) StartupStore(_T(". AR620x::  Dual on %s"), NEWLINE);
        }
        else
        {
          sStatus.intVal16 &= ~DAUL;   // turn Dual Mode Off
          //if(iAR620DebugLevel)StartupStore(_T(". AR620x::  Dual off %s"), NEWLINE);
        }
        len = 0;
        szTmp [len++] = HEADER_ID ;
        szTmp [len++] = PROTOKOL_ID ;
        szTmp [len++] =3;
        szTmp [len++] =12;
        szTmp [len++]  = sStatus.intVal8[1];
        szTmp [len++]  = sStatus.intVal8[0];
        CRC.intVal16 =  CRCBitwise(szTmp, len);
        szTmp [len++] = CRC.intVal8[1];
        szTmp [len++] = CRC.intVal8[0];
        sendCommand(szTmp,len);
  return(true);
}




bool AR620x::parseString(char *String, int len)
{
int cnt=0;
uint16_t CalCRC=0;
static  uint16_t Recbuflen=0;

 int CommandLength =0;
#define REC_BUFSIZE 127
static uint8_t  Command[REC_BUFSIZE];


if(String == NULL) return 0;
if(len == 0) return 0;


while (cnt < len)
{   
  if((uint8_t)String[cnt] == HEADER_ID)
    Recbuflen =0;

  if(Recbuflen >= REC_BUFSIZE)
    Recbuflen =0;

  assert(Recbuflen < REC_BUFSIZE);
  Command[Recbuflen++] =(uint8_t) String[cnt++];

  if(Recbuflen == 2)
    if(Command[Recbuflen-1] != PROTOKOL_ID)
      Recbuflen =0;

  if(Recbuflen >= 3)
  {
     CommandLength = Command[2];
     if(Recbuflen >= (CommandLength+5) ) // all received
     {
       //if(iAR620DebugLevel ==2) for(int i=0; i < (CommandLength+4);i++)   StartupStore(_T("AR620x::  Cmd: 0x%02X  %s") ,Command[i] ,NEWLINE);
       CRC.intVal8[1] =  Command[CommandLength+3];
       CRC.intVal8[0] =  Command[CommandLength+4];
       //if(iAR620DebugLevel ==2) StartupStore(_T("AR620x::  CRC 0x%04X %s") ,CRC.intVal16,NEWLINE);
       CalCRC =CRCBitwise(Command, CommandLength+3);
       if(CalCRC  == CRC.intVal16)
       {
           if(!bSending)
           {
             //if(iAR620DebugLevel ==2 )  StartupStore(_T("AR620x::  Process Command %u  %s") ,Command[3]  ,NEWLINE);
             convertAnswer(Command, CommandLength+5,CalCRC);
           }
           //else
             //if(iAR620DebugLevel)  StartupStore(_T("AR620x::  skip Command %u  %s") ,Command[3]  ,NEWLINE);

       }
       else
       {
    	   //if(iAR620DebugLevel)StartupStore(_T("AR620x::  CRC check fail! Command 0x%04X  0x%04X %s") ,CRC.intVal16,CalCRC ,NEWLINE);
       }
       Recbuflen = 0;

     }
  }


}

return  radioState.Changed;
}



/*****************************************************************************
 * this function converts a KRT answer sting to a NMEA answer
 *
 * szAnswer       NMEA Answer
 * Answerlen      number of valid characters in the NMEA answerstring
 * szCommand      AR620x:: binary code to be converted
 * len            length of the AR620x:: binary code to be converted
 ****************************************************************************/
int AR620x::convertAnswer(uint8_t  *szCommand, int len, uint16_t CRC)
{
if(szCommand == NULL) return 0;
if(len == 0)          return 0;

static uint16_t uiLastChannelCRC =0;
static uint16_t uiVolumeCRC      =0;
static uint16_t uiVersionCRC     =0;
static uint16_t uiStatusCRC      =0;
static uint16_t uiSquelchCRC     =0;
static uint16_t uiRxStatusCRC    =0;
#ifdef RADIO_VOLTAGE
static uint16_t uiVoltageCRC     =0;
uint32_t ulState;
#endif
int processed=0;

int Idx=0;
assert(szCommand !=NULL);

  switch ((unsigned char)(szCommand[3] & 0x7F))
  {
    case 0:
      if(uiVersionCRC!= CRC)  {
         uiVersionCRC = CRC;
       }
    break;

    case 3:
      if(uiVolumeCRC != CRC){
        uiVolumeCRC = CRC;
        radioState.Changed = true;
        radioState.Volume = (50-(int)szCommand[4])/5;
        std::cout << "Volume " << radioState.Volume << std::endl;
      }
    break;

    case 4:
      if(uiSquelchCRC!= CRC){
        uiSquelchCRC = CRC;
        radioState.Changed = true;
        radioState.Squelch = (int)(szCommand[4]-6)/2+1;  // 6 + (Squelch-1)*2
        std::cout << "Squelch " << radioState.Squelch << std::endl;
      }
    break;

    case 12:
      if(uiStatusCRC != CRC){
        uiStatusCRC = CRC;
        radioState.Changed = true;
        sStatus.intVal8[1] = szCommand[4] ;
        sStatus.intVal8[0] = szCommand[5] ;
        if(sStatus.intVal16 & DAUL)
         radioState.Dual = true;
        else
         radioState.Dual = false;
        std::cout << "Dual " << radioState.Dual << std::endl;
      }
    break;
#ifdef RADIO_VOLTAGE
    case 21:
      if(uiVoltageCRC != CRC) {
        uiVoltageCRC = CRC;
        radioState.BattVoltage =   8.5 + szCommand[4] *0.1;
        radioState.Changed = true;
        std::cout << "Battery Voltage " << radioState.BattVoltage << std::endl;
      }
    break;
#endif           
    case 22:
      if(uiLastChannelCRC != CRC) {
        uiLastChannelCRC = CRC;
        radioState.Changed = true;
        sFrequency.intVal8[1] = szCommand[4] ;
        sFrequency.intVal8[0] = szCommand[5] ;
        radioState.ActiveFrequency =  idx2Freq(sFrequency.intVal16);

        /* 
        Idx = searchStation(radioState.ActiveFrequency);
        if(Idx != 0)   _snprintf(radioState.ActiveName, NAME_SIZE,"%s",WayPointList[Idx].Name);
        else _snprintf(radioState.PassiveName , NAME_SIZE,"  ???   ");
        */
        std::cout << "Active Frequency " << radioState.ActiveFrequency << std::endl;

        sFrequency.intVal8[1] = szCommand[6];
        sFrequency.intVal8[0] = szCommand[7] ;
        radioState.PassiveFrequency =  idx2Freq(sFrequency.intVal16);
        /*
        Idx = searchStation(radioState.PassiveFrequency);
        if(Idx != 0)  _snprintf(radioState.PassiveName, NAME_SIZE ,"%s",WayPointList[Idx].Name);
        else _snprintf(radioState.PassiveName , NAME_SIZE,"  ???   ");
        */
        std::cout << "Standby Frequency " << radioState.PassiveFrequency << std::endl;
        radioState.Changed = true;
      }
    break;
    case 64:
      if(uiRxStatusCRC != CRC) {
          uiRxStatusCRC = CRC;

          ulState = szCommand[4] << 24 | szCommand[5] << 16 | szCommand[6] << 8 | szCommand[7];
        radioState.TX        = ((ulState & (BIT(5)| BIT(6))) > 0) ? true : false;
        radioState.RX_active = ((ulState & BIT(7)) > 0)  ? true : false;
        radioState.RX_standy = ((ulState & BIT(8)) > 0)  ? true : false;
        radioState.RX        = (radioState.RX_active ||   radioState.RX_standy );
        radioState.Changed = true;

        if (radioState.RX)        std::cout << "RX" << std::endl;
        if(radioState.RX_active) std::cout << "Rx Active" << std::endl;
        if(radioState.RX_standy) std::cout << "Rx Passive" << std::endl;
        if(radioState.TX)        std::cout << "Tx" << std::endl;
      }
    break;
    default:
    break;
  }

  return processed;  /* return the number of converted characters */
}
