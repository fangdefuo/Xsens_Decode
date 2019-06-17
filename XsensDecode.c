#include "XsensDecode.h"
#include "stm32f4xx.h"
#include "main.h"

/*********************Ý’ö˜’†—p“ž“I?—ÊC”?CŽw?—vÝŠO•”æ’è?****************************/
uint8_t     Packsize_XsensDecode=0;
uint8_t     XsensDecode_RX_Buff[53];
RingBuffer  XsensDecode_RX_RingBuff;
uint8_t     XsensDecode_RX_WorkingBuff[53];
uint8_t     XsensDecode_RX_WorkingBuff_DataCount = 0;
uint8_t     flagClear=0;
uint8_t     cur;
//uint8_t     XsensDecode_Connected=0;
//uint8_t isSDXsensDecode_Interruput_2Second_Cnt = 0;
d_Get_Data   Get_Data;
d_Get_Data  *xGet_Data=&Get_Data;  //’èyi jie gou ‘Ì bian —Ê

#define SYNC1 0xFA
#define SYNC2 0xFF
#define SYNC3 0x32
#define SYNC4 0x30

#define LEN_DATA     53
#define LEN_SYNC     3
//#define LEN_CHECKSUM 1

void XsensDecode_Configure(void)  /* ***********xsens‰Žn‰»”z’u************* */
{
  UART_XsensDecode_init();
  rbInitialize(&XsensDecode_RX_RingBuff, XsensDecode_RX_Buff, sizeof(XsensDecode_RX_Buff));//?”˜‰Žn‰»
}

uint8_t XsensDecode_IsReadyToRead(void)//”»’f—L–v—LkaiŽndu”˜
{
  return !rbIsEmpty(&XsensDecode_RX_RingBuff);//duŽÊ‘Š“™•Ô‰ñ0C”Û?•Ô‰ñ1
}

void XsensDecode_Updata(void)
{
   if(XsensDecode_RX_RingBuff.flagOverflow==1)
  {
    rbClear(&XsensDecode_RX_RingBuff);
  }// ˆÊ‰Žn‰»
  while( !rbIsEmpty(&XsensDecode_RX_RingBuff) ) 
  {
    //unsigned char Cur;
    //m_pSci->ReadElement(&Cur);
    uint8_t flagClear = ERROR;
    uint8_t cur = rbPop(&XsensDecode_RX_RingBuff);      //–v”˜•Ô‰ñ0,—L”˜«”˜fu gei cur  cur is data  ****&&(XsensDecode_RX_WorkingBuff[3]==SYNC4)   &&(XsensDecode_RX_WorkingBuff[3]==SYNC4)
    //uint8_t flagClear = ERROR;
    if(XsensDecode_RX_WorkingBuff_DataCount == 0 ) 
    //if (m_Rx.nDataCount == 0)
    {	
      XsensDecode_RX_WorkingBuff[0] = XsensDecode_RX_WorkingBuff[1];
      XsensDecode_RX_WorkingBuff[1] = XsensDecode_RX_WorkingBuff[2];
      XsensDecode_RX_WorkingBuff[2] = cur;             //”@‰Ê?”?0C?workingbuffer?s?â‹?—
//      XsensDecode_RX_WorkingBuff[2] = XsensDecode_RX_WorkingBuff[3]; 
//      XsensDecode_RX_WorkingBuff[3] = cur; 
      if((XsensDecode_RX_WorkingBuff[0]==SYNC1 )&&(XsensDecode_RX_WorkingBuff[1]==SYNC2)&&(XsensDecode_RX_WorkingBuff[2]==SYNC3)) //?”˜“I‘O™{ˆÊ?s”»’f
        {
          
            Packsize_XsensDecode=LEN_DATA ;
            XsensDecode_RX_WorkingBuff_DataCount =  LEN_SYNC; 
           // XsensDecode_Connected=1;
           // isSDXsensDecode_Interruput_2Second_Clear();
        }                                                                           
     }
    else  
     {
	    XsensDecode_RX_WorkingBuff[XsensDecode_RX_WorkingBuff_DataCount++] = cur; //”c”˜du“žworkingbuffer—¢
			
	     if(XsensDecode_RX_WorkingBuff_DataCount==Packsize_XsensDecode )
            // if( CheckSum_twoBytes(XsensDecode_RX_WorkingBuff+1,Packsize_XsensDecode-1)==0)
                {
		   // XsensDecode_DataDecode();
                   XsensDecode_DataDecode();
                   flagClear =SUCCESS;
                              	
		}
		 
	     if( flagClear||( XsensDecode_RX_WorkingBuff_DataCount >= sizeof(XsensDecode_RX_WorkingBuff)))
                {
			//memset(m_Rx.Data, 0, LEN_SYNC);//?????
			XsensDecode_RX_WorkingBuff_DataCount = 0;
		}
     }	
  }	
}

void  XsensDecode_DataDecode()
{
   if( CheckSum_twoBytes(XsensDecode_RX_WorkingBuff+1,Packsize_XsensDecode-1)==0) //Zyan˜a”»’f
   {  
     XsensDecode_Data();
   }
}

//
////*******************************************************************************
////	IMUƒf[ƒ^‚ÌƒfƒR[ƒh  MTi ?????Y?ŽÃZ???? ? ?????
////*******************************************************************************
void  XsensDecode_Data()
{ 
 
         
        xGet_Data->accX =Hex2Float (XsensDecode_RX_WorkingBuff+4); 
	xGet_Data->accY = Hex2Float (XsensDecode_RX_WorkingBuff+8); 
	xGet_Data->accZ =Hex2Float (XsensDecode_RX_WorkingBuff+12); 
	// Angular rate data (original data is rad/s)
	xGet_Data->gyrX = Hex2Float (XsensDecode_RX_WorkingBuff+16); 
	xGet_Data->gyrY= Hex2Float (XsensDecode_RX_WorkingBuff+20); 
        xGet_Data->gyrZ= Hex2Float (XsensDecode_RX_WorkingBuff+24); 
	
	// Magnetometer Data (original data is mGauss)
	 xGet_Data->magX  = Hex2Float (XsensDecode_RX_WorkingBuff+28); 
	 xGet_Data->magY  = Hex2Float (XsensDecode_RX_WorkingBuff+32); 
         xGet_Data->magZ  = Hex2Float (XsensDecode_RX_WorkingBuff+36); 
	// Attitude Data (original data is degree) --> transform to rad
         xGet_Data->Roll= Hex2Float (XsensDecode_RX_WorkingBuff+40)* PI / 180.0; 
         xGet_Data->Pitch= Hex2Float (XsensDecode_RX_WorkingBuff+44)* PI / 180.0; 
         xGet_Data->Yaw= Hex2Float (XsensDecode_RX_WorkingBuff+48)* PI / 180.0; 
//  
//        xGet_Data->accX =( XsensDecode_RX_WorkingBuff[4]<<24|XsensDecode_RX_WorkingBuff[5]<<16|XsensDecode_RX_WorkingBuff[6]<<8|XsensDecode_RX_WorkingBuff[7])/pow(2,27);  
//	xGet_Data->accY = (XsensDecode_RX_WorkingBuff[8]<<24|XsensDecode_RX_WorkingBuff[9]<<16|XsensDecode_RX_WorkingBuff[10]<<8|XsensDecode_RX_WorkingBuff[11])/pow(2,27); 
//	xGet_Data->accZ =( XsensDecode_RX_WorkingBuff[12]<<24|XsensDecode_RX_WorkingBuff[13]<<16|XsensDecode_RX_WorkingBuff[14]<<8|XsensDecode_RX_WorkingBuff[15])/pow(2,27); 
	
//	// Angular rate data (original data is rad/s)
//	xGet_Data->gyrX = (XsensDecode_RX_WorkingBuff[16]<<24|XsensDecode_RX_WorkingBuff[17]<<16|XsensDecode_RX_WorkingBuff[18]<<8|XsensDecode_RX_WorkingBuff[19])/pow(2,20); 
//	xGet_Data->gyrY=(XsensDecode_RX_WorkingBuff[20]<<24|XsensDecode_RX_WorkingBuff[21]<<16|XsensDecode_RX_WorkingBuff[22]<<8|XsensDecode_RX_WorkingBuff[23])/pow(2,20); 
//        xGet_Data->gyrZ= (XsensDecode_RX_WorkingBuff[24]<<24|XsensDecode_RX_WorkingBuff[25]<<16|XsensDecode_RX_WorkingBuff[26]<<8|XsensDecode_RX_WorkingBuff[27])/pow(2,20); 
//	
//	// Magnetometer Data (original data is mGauss)
//	xGet_Data->magX  =(XsensDecode_RX_WorkingBuff[28]<<24|XsensDecode_RX_WorkingBuff[29]<<16|XsensDecode_RX_WorkingBuff[30]<<8|XsensDecode_RX_WorkingBuff[31])/pow(2,20); 
//	xGet_Data->magY  =( XsensDecode_RX_WorkingBuff[32]<<24|XsensDecode_RX_WorkingBuff[33]<<16|XsensDecode_RX_WorkingBuff[34]<<8|XsensDecode_RX_WorkingBuff[35])/pow(2,20); 
//        xGet_Data->magZ  = (XsensDecode_RX_WorkingBuff[36]<<24|XsensDecode_RX_WorkingBuff[37]<<16|XsensDecode_RX_WorkingBuff[38]<<8|XsensDecode_RX_WorkingBuff[39])/pow(2,20); 
//	
//	// Attitude Data (original data is degree) --> transform to rad
//         xGet_Data->Roll=(XsensDecode_RX_WorkingBuff[40]<<24|XsensDecode_RX_WorkingBuff[41]<<16|XsensDecode_RX_WorkingBuff[42]<<8|XsensDecode_RX_WorkingBuff[43])/pow(2,20);
//         xGet_Data->Pitch=(XsensDecode_RX_WorkingBuff[44]<<24|XsensDecode_RX_WorkingBuff[45]<<16|XsensDecode_RX_WorkingBuff[46]<<8|XsensDecode_RX_WorkingBuff[47])/pow(2,20);
//        xGet_Data->Yaw= (XsensDecode_RX_WorkingBuff[48]<<24|XsensDecode_RX_WorkingBuff[49]<<16|XsensDecode_RX_WorkingBuff[50]<<8|XsensDecode_RX_WorkingBuff[51])/pow(2,20) ;
//	T = XsensDecode_RX_WorkingBuff[21]<<8|XsensDecode_RX_WorkingBuff[22] ;
//        if(T>=pow(2,15))  xGet_Data.temp=(-pow(2,16)+T)/256;
//        else              xGet_Data.temp=T/256;
//	xGet_Data.Pitch =Hex2(XsensDecode_RX_WorkingBuff+ 44) * PI / 180.0;
//       xGet_Data.Yaw   = Hex2(XsensDecode_RX_WorkingBuff + 48) * PI / 180.0;

	// ¿biaoŒntongMTi“IiYzhou˜aZzhou‹tj¨daoq¿biaoŒn
	xGet_Data->accY *= -1;
	xGet_Data->accZ *= -1;
	
	xGet_Data->gyrY*= -1;
        xGet_Data->gyrZ *= -1;
	
	xGet_Data->magY *= -1;
        xGet_Data->magZ *= -1;
	
	xGet_Data->Pitch *= -1; // ŽptaiŠpD—^Ÿzhuan‰»iŽ®’†›ßzheng–¾
        xGet_Data->Yaw   *= -1;
}


uint16_t CheckSum_twoBytes(uint8_t *pData,uint8_t nLength)
{
  uint16_t checksum = 0;
  uint8_t i=0;
 // for(i=0;i<nLength;i++) checksum += ((uint16_t)pData[i] & 0x00FF); 
  for(i=0;i<nLength;i++)
      checksum += pData[i] ;
      checksum&=0x00FF;
      return checksum;
  
}


//double Hex2Float(const unsigned char* pData)
//{
//  union {
//    float        d;
//    unsigned char b[4];
//  } tf;
//  
//#ifdef LITTLE_ENDIAN                    //•s´‘^
//  for( int i = 0; i < 4; i++ ) {
//    tf.b[i] = *(pData++);
//  }
//#endif
//  
//#ifdef BIG_ENDIAN                      //•s´‘^
//  for( int i = 3; i >= 0; i-- ) {
//    tf.b[i] = *(pData++);
//  }
//#endif
//  
//  return tf.d;
//}
//void isSDXsensDecode_Interruput_2Second_Clear()
//{
//    isSDXsensDecode_Interruput_2Second_Cnt = 0;
//}
