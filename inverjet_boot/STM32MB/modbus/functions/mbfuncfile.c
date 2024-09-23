/* 
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006-2018 Christian Walter <cwalter@embedded-solutions.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include "iap.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbconfig.h"


/* ----------------------- Defines ------------------------------------------*/
//#define MB_PDU_FUNC_WRITE_FILE_ADDR_OFF						( MB_PDU_DATA_OFF + 0 )
#define MB_PDU_FUNC_WRITE_FILE_DATA_LEN_OFF      	( MB_PDU_DATA_OFF + 0 )
#define MB_PDU_FUNC_WRITE_FILE_TYPE_OFF        	 	( MB_PDU_DATA_OFF + 1 )
#define MB_PDU_FUNC_WRITE_FILE_NUMBER_OFF        	( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_WRITE_FILE_RECORD_OFF        	( MB_PDU_DATA_OFF + 4 )
#define MB_PDU_FUNC_WRITE_FILE_LENTH_OFF         	( MB_PDU_DATA_OFF + 6 )
#define MB_PDU_FUNC_WRITE_FILE_WRITE_VALUES_OFF  	( MB_PDU_DATA_OFF + 8 )
#define MB_PDU_FUNC_WRITE_FILE_REF_TYPE          	( 6 )
#define MB_PDU_FUNC_WRITE_FILE_NUMBER        			( 0x0001 )
#define MB_PDU_FUNC_WRITE_FILE_REPLY_LEN        	( 9 )

/* ----------------------- Static functions ---------------------------------*/
eMBException    prveMBError2Exception( eMBErrorCode eErrorCode );

/* ----------------------- Start implementation -----------------------------*/
#if MB_FUNC_WRITE_SINGLE_FILE_ENABLED > 0

eMBException
eMBFuncWriteSingleFileRegister( UCHAR * pucFrame, USHORT * usLen )
{
	BOOL refType=0;								// 参考类型   06
	USHORT fileNumber=0;					// 文件号			01  
	USHORT recordNumber=0;				// 记录号			0开始
	USHORT recordLength=0;				// 记录长度		(16位长度)
	
  eMBException eStatus = MB_EX_NONE;
	eMBErrorCode    eRegStatus;
	
    //if( *usLen == ( MB_PDU_FUNC_WRITE_FILE_SIZE_MIN + MB_PDU_SIZE_MIN ) )
    {
			refType = ( USHORT )( pucFrame[MB_PDU_FUNC_WRITE_FILE_TYPE_OFF]);
			
			fileNumber = ( USHORT )( pucFrame[MB_PDU_FUNC_WRITE_FILE_NUMBER_OFF] << 8 );
      fileNumber |= ( USHORT )( pucFrame[MB_PDU_FUNC_WRITE_FILE_NUMBER_OFF + 1] );
			
			recordNumber = ( USHORT )( pucFrame[MB_PDU_FUNC_WRITE_FILE_RECORD_OFF] << 8 );
      recordNumber |= ( USHORT )( pucFrame[MB_PDU_FUNC_WRITE_FILE_RECORD_OFF + 1] );
			
			recordLength = ( USHORT )( pucFrame[MB_PDU_FUNC_WRITE_FILE_LENTH_OFF] << 8 );
      recordLength |= ( USHORT )( pucFrame[MB_PDU_FUNC_WRITE_FILE_LENTH_OFF + 1] );

			 
			if(refType != MB_PDU_FUNC_WRITE_FILE_REF_TYPE)
			{
				eStatus = MB_EX_ILLEGAL_DATA_VALUE;
				return eStatus;
			}
			if(fileNumber != MB_PDU_FUNC_WRITE_FILE_NUMBER)
			{
				eStatus = MB_EX_ILLEGAL_DATA_VALUE;
				return eStatus;
			}
			
			 if(( refType == MB_PDU_FUNC_WRITE_FILE_REF_TYPE ) && ( fileNumber == MB_PDU_FUNC_WRITE_FILE_NUMBER ))
        {
            /* Make callback to update the register values. */
            eRegStatus = eMBRegFileCB( &pucFrame[MB_PDU_FUNC_WRITE_FILE_WRITE_VALUES_OFF],
                                          recordNumber, recordLength, MB_REG_WRITE );

            if( eRegStatus == MB_ENOERR )
            {
                /* Set the current PDU data pointer to the beginning. */
                *usLen = MB_PDU_FUNC_WRITE_FILE_REPLY_LEN;
            }
            if( eRegStatus != MB_ENOERR )
            {
                eStatus = prveMBError2Exception( eRegStatus );
            }
        }
        else
        {
            eStatus = MB_EX_ILLEGAL_DATA_VALUE;
        }
    }
//    else
//    {
//        /* Can't be a valid request because the length is incorrect. */
//        eStatus = MB_EX_ILLEGAL_DATA_VALUE;
//    }
    return eStatus;
}
#endif

