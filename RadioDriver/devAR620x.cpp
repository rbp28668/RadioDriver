
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif


#include <stdint.h>
#include <cstring>

#define assert(x)

#include "devAR620x.h"

#define BIT(n) (1 << (n))

#define ACTIVE_STATION  1
#define PASSIVE_STATION 0


#define RADIO_VOLTAGE       /* read Radio Supply voltage and store it in BATT2 indicator */

constexpr uint8_t HEADER_ID = 0xA5;
#define PROTOCOL_ID     0x14
#define QUERY           BIT(7)
#define DUAL            BIT(8)
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

FrequencyT AR620x::idx2Freq(uint16_t uFreqIdx)
{
  // Frequency is basically 118.00 + the number of 100kHz steps defined by the index.
  // However, index is shifted left 4 bits and the bottom 4 bits used to encode the bottom
  // 2 digits of the channel numbers.
FrequencyT freq = 118000;
freq += ((uFreqIdx & 0xFFF0) >> 4) * 100; // basically 100kHz increments

switch(uFreqIdx & 0xF)
{
    case 0:  freq += 0; break;
    case 1:  freq += 5; break;
    case 2:  freq += 10; break;
    case 3:  freq += 15; break;
    case 4:  freq += 25; break;
    case 5:  freq += 30; break;
    case 6:  freq += 35; break;
    case 7:  freq += 40; break;
    case 8:  freq += 50; break;
    case 9:  freq += 55; break;
    case 10: freq += 60; break;
    case 11: freq += 65; break;
    case 12: freq += 75; break;
    case 13: freq += 80; break;
    case 14: freq += 85; break;
    case 15: freq += 90; break;   
}
return (freq);
}


uint16_t AR620x::Frq2Idx(FrequencyT fFreq)
{
  uint8_t uiFrac = fFreq % 100; // remember last 2 digits.
  fFreq -= 118000; // first possible frequency at 118.0 MHz maps to zero.
  uint16_t  uFreIdx = fFreq / 100;  // down to 100kHz steps.
  uFreIdx <<=4; // make space for fractional part
  
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
    default:     
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
int  AR620x::setStation(uint8_t *Command ,int Active_Passive, FrequencyT fFrequency, const char* Station)
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
  Command [len++] = PROTOCOL_ID ;
  Command [len++] = 5;

  switch (Active_Passive)
  {
    case ACTIVE_STATION:
         ActiveFreqIdx.intVal16 = Frq2Idx(fFrequency);
     break;
    default:
    case PASSIVE_STATION:
        PassiveFreqIdx.intVal16 =  Frq2Idx(fFrequency);
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
    radioState.ActiveFrequency = 131280L;
    radioState.PassiveFrequency = 121500L;
}


bool AR620x::putVolume(int Volume) {
uint8_t  szTmp[MAX_CMD_LEN];
int len;

    len = 0;
    szTmp [len++] = HEADER_ID ;
    szTmp [len++] = PROTOCOL_ID ;
    szTmp [len++] =2;
    szTmp [len++] =3;
    szTmp [len++]  =  50-Volume*5;
    CRC.intVal16 =  CRCBitwise(szTmp, len);
    szTmp [len++] = CRC.intVal8[1];
    szTmp [len++] = CRC.intVal8[0];
    sendCommand(szTmp,len);
    radioState.Volume = Volume;
  return(true);
}




bool AR620x::putSquelch(int Squelch) {
uint8_t  szTmp[MAX_CMD_LEN];
uint8_t len;
    len = 0;
    szTmp [len++] = HEADER_ID ;
    szTmp [len++] = PROTOCOL_ID ;
    szTmp [len++] =2;
    szTmp [len++] =4;
    szTmp [len++] = 6 + (Squelch-1)*2;
    CRC.intVal16 =  CRCBitwise(szTmp, len);
    szTmp [len++] = CRC.intVal8[1];
    szTmp [len++] = CRC.intVal8[0];
    sendCommand(szTmp,len);
    radioState.Squelch = Squelch;
  return(true);
}


bool AR620x::putFreqActive(FrequencyT Freq, char const StationName[]) {
int len;
uint8_t  szTmp[MAX_CMD_LEN];
    len = AR620x::setStation(szTmp ,ACTIVE_STATION, Freq, StationName);
    sendCommand(szTmp,len);
    radioState.ActiveFrequency=  Freq;
    if(StationName != NULL){
        strncpy(radioState.ActiveName, StationName, NAME_SIZE);
        radioState.ActiveName[NAME_SIZE-1] = 0;
    }
  return(true);
}


bool AR620x::putFreqStandby(FrequencyT Freq,  const char StationName[]) {
int len;
uint8_t  szTmp[MAX_CMD_LEN];
    len = AR620x::setStation(szTmp ,PASSIVE_STATION, Freq, StationName);
    sendCommand(szTmp,len);
    radioState.PassiveFrequency =  Freq;
    if(StationName != NULL){
      strncpy(radioState.PassiveName, StationName, NAME_SIZE) ;
      radioState.PassiveName[NAME_SIZE-1] = 0;
    }
   return(true);
}


bool AR620x::stationSwap() {
uint8_t len=0;
uint8_t  szTmp[MAX_CMD_LEN];
    IntConvertStruct ActiveFreqIdx;  ActiveFreqIdx.intVal16  = Frq2Idx(radioState.ActiveFrequency );
    IntConvertStruct PassiveFreqIdx; PassiveFreqIdx.intVal16 = Frq2Idx(radioState.PassiveFrequency);
    szTmp [len++] = HEADER_ID ;
    szTmp [len++] = PROTOCOL_ID ;
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
          sStatus.intVal16 |= DUAL;  // turn Dual Mode On
        }
        else
        {
          sStatus.intVal16 &= ~DUAL;   // turn Dual Mode Off
        }
        len = 0;
        szTmp [len++] = HEADER_ID ;
        szTmp [len++] = PROTOCOL_ID ;
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
    if(Command[Recbuflen-1] != PROTOCOL_ID)
      Recbuflen =0;

  if(Recbuflen >= 3)
  {
     CommandLength = Command[2];
     if(Recbuflen >= (CommandLength+5) ) // all received
     {
       CRC.intVal8[1] =  Command[CommandLength+3];
       CRC.intVal8[0] =  Command[CommandLength+4];
       CalCRC = CRCBitwise(Command, CommandLength+3);
       if(CalCRC  == CRC.intVal16)
       {
           if(!bSending)
           {
             convertAnswer(Command, CommandLength+5,CalCRC);
           }
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

//int Idx=0;
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
      }
    break;

    case 4:
      if(uiSquelchCRC!= CRC){
        uiSquelchCRC = CRC;
        radioState.Changed = true;
        radioState.Squelch = (int)(szCommand[4]-6)/2+1;  // 6 + (Squelch-1)*2
      }
    break;

    case 12:
      if(uiStatusCRC != CRC){
        uiStatusCRC = CRC;
        radioState.Changed = true;
        sStatus.intVal8[1] = szCommand[4] ;
        sStatus.intVal8[0] = szCommand[5] ;
        if(sStatus.intVal16 & DUAL)
         radioState.Dual = true;
        else
         radioState.Dual = false;
      }
    break;
#ifdef RADIO_VOLTAGE
    case 21:
      if(uiVoltageCRC != CRC) {
        uiVoltageCRC = CRC;
        radioState.BattVoltage =   8.5 + szCommand[4] *0.1;
        radioState.Changed = true;
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

        sFrequency.intVal8[1] = szCommand[6];
        sFrequency.intVal8[0] = szCommand[7] ;
        radioState.PassiveFrequency =  idx2Freq(sFrequency.intVal16);
 
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

      }
    break;
    default:
    break;
  }

  return processed;  /* return the number of converted characters */
}
