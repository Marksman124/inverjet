/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "usart.h"

/* ----------------------- static functions ---------------------------------*/
void prvvUARTTxReadyISR( void );
void prvvUARTRxISR( void );

#if MODBUS_USART == 2
UART_HandleTypeDef* p_huart_mb = &huart2;		 //UART句柄
#elif MODBUS_USART == 3
UART_HandleTypeDef* p_huart_mb = &huart3;		 //UART句柄
#elif MODBUS_USART == 5
UART_HandleTypeDef* p_huart_mb = &huart5;		 //UART句柄
#endif
/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
	if (xRxEnable)															//将串口收发中断和modbus联系起来，下面的串口改为自己使能的串口
		{
			__HAL_UART_ENABLE_IT(p_huart_mb,UART_IT_RXNE);	//我用的是串口2，故为p_huart_mb
		}
	else
		{
			__HAL_UART_DISABLE_IT(p_huart_mb,UART_IT_RXNE);
		}
	if (xTxEnable)
		{
			__HAL_UART_ENABLE_IT(p_huart_mb,UART_IT_TXE);
		}
	else
		{
			__HAL_UART_DISABLE_IT(p_huart_mb,UART_IT_TXE);
		}
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
    if(HAL_UART_Transmit (p_huart_mb ,(uint8_t *)&ucByte,1,0x01) != HAL_OK )	//添加发送一位代码
			return FALSE ;
		else
			return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
    if(HAL_UART_Receive (p_huart_mb ,(uint8_t *)pucByte,1,0x01) != HAL_OK )//添加接收一位代码
			return FALSE ;
		else
		{
			return TRUE;
		}
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
void prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
void prvvUARTRxISR( void )
{
	pxMBFrameCBByteReceived(  );
}
