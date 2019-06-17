#ifndef _XsensDecode_H   //#ifndef���ڱ�֤ͷ�ļ�һ�����ã���ֹͷ�ļ��Ķ���ظ����õ��µ��ض��塣
#define _XsensDecode_H

#ifdef __cplusplus             //���������CPP�Ĵ��룬__cplusplus��cpp�е��Զ���꣬��ˣ�
                               //��CPP���Ե���ʽ���룬��������Ĵ�����C���ԵĴ��룬
                              //��ô��Ҫ��extern "C" { ��˵��һ�£���������C���Ե�ģʽ���롣
extern "C" {                   //
#endif

#include "stm32f4xx.h"
#include "main.h"
#include "stdio.h"
#include "structural.h"
#include "stm32f4xx_it.h"
#include "control.h"
#include "Safe_Check.h"
#include "uart_init.h"
#include "Parameter_Setting_Frametype_Selection.h"
#include "Parameter_Setting_Spary_Configration.h"
#include "Controller_Param_PID_Setting.h"
#include "canAPDecode.h"
#include "ringbuffer.h"
#include "canAP.h"
#include "RTk_Novatel.h"
#include "imu.h"
#include "Communication_transmit.h"  


////#define RXBUFFSIZE  56

extern  RingBuffer  XsensDecode_RX_RingBuff;


uint8_t   XsensDecode_IsReadyToRead(void);
uint16_t  CheckSum_twoBytes(uint8_t *pData,uint8_t nLength);
int8_t    rbIsEmpty(const RingBuffer* pRingBuff);

void    XsensDecode_Configure(void);
void    XsensDecode_Updata(void);
void    XsensDecode_Data(void);
void    XsensDecode_DataDecode(void);
void    UART_XsensDecode_init(void);

extern void    rbInitialize(RingBuffer* pRingBuff, uint8_t* buff, uint16_t length);
extern double  Hex2Float(const unsigned char* pData);
extern void    rbClear(RingBuffer* pRingBuff);
//void isSDXsensDecode_Interruput_2Second_Clear(void);

#ifdef __cplusplus
}
#endif
#endif