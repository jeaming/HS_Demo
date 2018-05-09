#include "StdAfx.h"
#include "PCSC_Operator.h"

#ifndef WINSCARD_H
#include "WinSCard.h"
#define WINSCARD_H
#endif

#pragma comment(lib, "WinSCard.lib")

CPCSC_Operator::CPCSC_Operator(void)
	: m_isListReaders(0)
	, m_iAtrLen(0)
	, dwActiveProtocol(0)
{
	m_iReaderCount = 0;
	hContext = 0x00000000;
	lLastError = SCARD_S_SUCCESS;
	m_strError = "";
	for(int i = 0; i != 10; ++ i)
	{
		hThread[i] = NULL;
		memset(m_szCurrentState[i], 0, sizeof(m_szCurrentState[i]));
		memset(m_szPreviousState[i], 0, sizeof(m_szPreviousState[i]));
	}
	
	memset(m_szAtr, 0, sizeof(m_szAtr));
	m_chStatus = 0x00;
}


CPCSC_Operator::~CPCSC_Operator(void)
{
	long *lDev = NULL;
	int index = 0;

	for(int i = 0; i != m_iReaderCount; ++i)
	{
		index = i + 1;
		GetCardState(index);
		if(m_chStatus == CARD_INUSE)
		{
			SCardDisconnect(hCard[i+1], SCARD_EJECT_CARD);
		}
	}
	SCardReleaseContext(hContext);
	hContext = 0x00000000;
}

int CPCSC_Operator::HexNum2Dec(char* HexNum)
{
	int iLen = 0;
	int iNum[16];
	int iCount = 1;
	int iResult = 0;

	iLen = strlen(HexNum);
	for(int i = iLen - 1; i >= 0; i --)
	{
		if ((HexNum[i]>='0') && (HexNum[i]<='9'))  
			iNum[i]	= HexNum[i] - 48;//字符0的ASCII值为48
		else if ((HexNum[i] >= 'a') && (HexNum[i] <= 'f'))  
			iNum[i] = HexNum[i] - 'a' +10;  
		else if ((HexNum[i]>='A') && (HexNum[i]<='F'))  
			iNum[i] = HexNum[i] - 'A' + 10;  
		else
			iNum[i]=0;
		iResult = iResult + iNum[i] * iCount;  
		iCount = iCount * 16;//十六进制(如果是八进制就在这里乘以8)    
	}
	return iResult;
}

int CPCSC_Operator::Hex2Bin(BYTE* outBin, BYTE* inHex, int lPair_length)
{
	char src1,src2,factor1,factor2;
	long len;
	factor1 = '7';
	factor2 = '7';
    _strupr_s( (char *)inHex, strlen((char*)inHex) + 1 );

	for (len=0; len < lPair_length; len++) 
	{
		src1 = *(inHex+len*2);
		src2 = *(inHex+len*2+1);
		if ((src1>='0') && (src1<='9'))
		    factor1 = '0';
	    else if ((src1>='A') && (src1<='F'))
		    factor1 = '7';
	    else 
			return 1;
		if ((src2>='0') && (src2<='9'))
		    factor2 = '0';
	    else if ((src2>='A') && (src2<='F'))
		    factor2 = '7';
	    else 
			return 1;
		src1 = src1 - factor1;
		src2 = src2 - factor2;
		*outBin++ = (src1 << 4) | src2; 
	}
	return 0;
}

int CPCSC_Operator::Bin2Hex(BYTE* outHex, BYTE* inBin, int length)
{
	UCHAR hLowbit,hHighbit;
    long i;
	for(i=0;i<length*2;i=i+2)
	{
		hLowbit=inBin[i/2]&0x0f;
		hHighbit=inBin[i/2]/16;
		if(hHighbit>=10)
			outHex[i]=hHighbit+'7';
		else
			outHex[i]=hHighbit+'0';
		if(hLowbit>=10)
			outHex[i+1]=hLowbit+'7';
		else
			outHex[i+1]=hLowbit+'0';
	}
	outHex[length*2]='\0';

	return 0;
}

int CPCSC_Operator::SwitchDataMode(BYTE* pszData, int* iDataLen, int iSwitchMode)
{
	unsigned char szKeyHex[1024], szKeyAsc[1024];
	if(!pszData)
		return -1;

	memset(szKeyHex, 0, sizeof(szKeyHex));
	memset(szKeyAsc, 0, sizeof(szKeyAsc));
	if(iSwitchMode == HEX2BIN)
	{
		Hex2Bin(szKeyAsc, pszData, *iDataLen);
		memcpy(pszData, szKeyAsc, *iDataLen/2);
		pszData[*iDataLen/2] = '\0';
		if(iDataLen != NULL)
			*iDataLen = *iDataLen/2;
		return -1;
	}
	
	if(iSwitchMode == BIN2HEX)
	{
		Bin2Hex(szKeyHex, pszData, *iDataLen);
		memcpy(pszData, szKeyHex, *iDataLen*2);
		pszData[*iDataLen*2] = '\0';
		if(iDataLen != NULL)
			*iDataLen = *iDataLen *2;
		return -1;
	}
	return 0;
}

int CPCSC_Operator::TrackCardState(HANDLE* hThread)
{
	//LPTHREADSTRUCT ts;
	//ts = this;

	*hThread = CreateThread(NULL, 0, StartReaderStateCapture, this, 0, NULL);

	return 0;
}


DWORD CPCSC_Operator::StartReaderStateCapture(LPVOID param)
{
	//LPTHREADSTRUCT ts;
	CPCSC_Operator* _this;

	_this = (CPCSC_Operator*)param;

	while(1)
	{
		;
	}
	
	CloseHandle(_this->hThread);
	return 0;
}

void CPCSC_Operator::GetErrMessage(long eError)
{
	//string strUnknownErr;

	lLastError = eError;
	   
	switch (eError)
	{
		  
	case SCARD_E_CANCELLED:
		m_strError = "SCARD_E_CANCELLED:\r\n   The action was canceled by an SCardCancel request.";
		break;
		   
	case SCARD_E_CANT_DISPOSE:
		m_strError = "SCARD_E_CANT_DISPOSE:\r\n   The system could not dispose of the media in the requested manner.";
		break;
		   
	case SCARD_E_CARD_UNSUPPORTED:
		m_strError = "SCARD_E_CARD_UNSUPPORTED:\r\n   The smart card does not meet minimal requirements for support.";
		break;
		   
	case SCARD_E_DUPLICATE_READER:
		m_strError = "SCARD_E_DUPLICATE_READER:\r\n   The reader driver didn't produce a unique reader name.";
		break;
		   
		   
	case SCARD_E_INSUFFICIENT_BUFFER:
		m_strError = "SCARD_E_INSUFFICIENT_BUFFER:\r\n   The data buffer for returned data is too small for the returned data.";
		break;
		   
	case SCARD_E_INVALID_ATR:
		m_strError = "SCARD_E_INVALID_ATR:\r\n   An ATR string obtained from the registry is not a valid ATR string.";
		break;
		   
		   
	case SCARD_E_INVALID_HANDLE:
		m_strError = "SCARD_E_INVALID_HANDLE:\r\n   The supplied handle was invalid.";
		break;
		   
	case SCARD_E_INVALID_PARAMETER:
		m_strError = "SCARD_E_INVALID_PARAMETER:\r\n   One or more of the supplied parameters could not be properly interpreted.";
		break;
		   
	case SCARD_E_INVALID_TARGET:
		m_strError = "SCARD_E_INVALID_TARGET:\r\n   Registry startup information is missing or invalid.";
		break;
		   
	case SCARD_E_INVALID_VALUE:
		m_strError = "SCARD_E_INVALID_VALUE:\r\n   One or more of the supplied parameter values could not be properly interpreted.";
		break;
		   
		   
	case SCARD_E_NO_MEMORY:
		m_strError = "SCARD_E_NO_MEMORY:\r\n   Not enough memory available to complete this command.";
		break;
		   
		   
	case SCARD_E_NO_SERVICE:
		m_strError = "SCARD_E_NO_SERVICE:\r\n   The smart card resource manager is not running.";
		break;
		   
	case SCARD_E_NO_SMARTCARD:
		m_strError = "SCARD_E_NO_SMARTCARD:\r\n   The operation requires a smart card,\r\n\tbut no smart card is currently in the device.";
		break;
		   
		   
	case SCARD_E_NOT_READY:
		m_strError = "SCARD_E_NOT_READY:\r\n   The reader or card is not ready to accept commands.";
		break;
		   
	case SCARD_E_NOT_TRANSACTED:
		m_strError = "SCARD_E_NOT_TRANSACTED:\r\n   An attempt was made to end a non-existent transaction.";
		break;
		   
	case SCARD_E_PCI_TOO_SMALL:
		m_strError = "SCARD_E_PCI_TOO_SMALL:\r\n   The PCI receive buffer was too small.";
		break;
		   
	case SCARD_E_PROTO_MISMATCH:
		m_strError = "SCARD_E_PROTO_MISMATCH:\r\n   The requested protocols are incompatible with the protocol\r\n\tcurrently in use with the card.";
		break;
		   
	case SCARD_E_READER_UNAVAILABLE:
		m_strError = "SCARD_E_READER_UNAVAILABLE:\r\n   The specified reader is not currently available for use.";
		break;
		   
	case SCARD_E_READER_UNSUPPORTED:
		m_strError = "SCARD_E_READER_UNSUPPORTED:\r\n   The reader driver does not meet minimal requirements for support.";
		break;
		   
	case SCARD_E_SERVICE_STOPPED:
		m_strError = "SCARD_E_SERVICE_STOPPED:\r\n   The smart card resource manager has shut down.";
		break;
		   
	case SCARD_E_SHARING_VIOLATION:
		m_strError = "SCARD_E_SHARING_VIOLATION:\r\n   The smart card cannot be accessed because of other outstanding connections.";
		break;
		   
	case SCARD_E_SYSTEM_CANCELLED:
		m_strError = "SCARD_E_SYSTEM_CANCELLED:\r\n   The action was canceled by the system, presumably to log off or shut down.";
		break;
		   
	case SCARD_E_TIMEOUT:
		m_strError = "SCARD_E_TIMEOUT:\r\n   The user-specified timeout value has expired.";
		break;
		   
		   
	case SCARD_E_UNKNOWN_CARD:
		m_strError = "SCARD_E_UNKNOWN_CARD:\r\n   The specified smart card name is not recognized.";
		break;
		   
	case SCARD_E_UNKNOWN_READER:
		m_strError = "SCARD_E_UNKNOWN_READER:\r\n   The specified reader name is not recognized.";
		break;
		   
		   
	case SCARD_F_COMM_ERROR:
		m_strError = "SCARD_F_COMM_ERROR:\r\n   An internal communications error has been detected.";
		break;
		   
	case SCARD_F_INTERNAL_ERROR:
		m_strError = "SCARD_F_INTERNAL_ERROR:\r\n   An internal consistency check failed.";
		break;
		   
	case SCARD_F_UNKNOWN_ERROR:
		m_strError = "SCARD_F_UNKNOWN_ERROR:\r\n   An internal error has been detected, but the source is unknown.";
		break;
		   
	case SCARD_F_WAITED_TOO_LONG:
		m_strError = "SCARD_F_WAITED_TOO_LONG:\r\n   An internal consistency timer has expired.";
		break;
		   
	case SCARD_P_SHUTDOWN:
		m_strError = "SCARD_P_SHUTDOWN:\r\n   The operation has been aborted to allow the server application to exit.";
		break;
		   
	case SCARD_S_SUCCESS:
		m_strError = "SCARD_S_SUCCESS:\r\n   No error was encountered.";
		break;
		   
		   
	case SCARD_W_REMOVED_CARD:
		m_strError = "SCARD_W_REMOVED_CARD:\r\n   The smart card has been removed, so that further communication is not possible.";
		break;
		   
	case SCARD_W_RESET_CARD:
		m_strError = "SCARD_W_RESET_CARD:\r\n   The smart card has been reset, so any shared state information is invalid.";
		break;
		   
		   
	case SCARD_W_UNPOWERED_CARD:
		m_strError = "SCARD_W_UNPOWERED_CARD:\r\n   Power has been removed from the smart card, so that further communication is not possible.";
		break;
		   
	case SCARD_W_UNRESPONSIVE_CARD:
		m_strError = "SCARD_W_UNRESPONSIVE_CARD:\r\n   The smart card is not responding to a reset.";
		break;
		   
	case SCARD_W_UNSUPPORTED_CARD:
		m_strError = "SCARD_W_UNSUPPORTED_CARD:\r\n   The reader cannot communicate with the card, due to ATR string configuration conflicts.";
		break;
		   
	default:
		{
			m_strError = "There was error in this operation, No error string available";//, ErrorCode = 0x%08X",eError;
			
		}
		break;
	}
	
}

//return value implies count of readers 
int CPCSC_Operator::GetReaders(void)
{
	DWORD dwRv = 0, dwLength = 1024;
	char szReaders[1024];
	int i = 0;
	unsigned long iLen = 0;

	if(hContext == 0x00000000)
	{
		dwRv = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &hContext);
		if(dwRv != SCARD_S_SUCCESS)
		{
			lLastError = dwRv;
			GetErrMessage(dwRv);
			return lLastError;//Establish context error
		}
	}

	m_iReaderCount = 0;
	for(int i = 0; i != 10; ++i)
	{//clear readers buffer
		memset(m_szReader[i], 0, sizeof(m_szReader[i]));
	}
	
	memset(szReaders, 0, sizeof(szReaders));
	dwRv = SCardListReadersA(hContext, NULL, szReaders, &dwLength);
	if(dwRv != SCARD_S_SUCCESS)
	{
		lLastError = dwRv;
		GetErrMessage(dwRv);
		return -1;//list readers failed
	}

	//if(dwLength <= 2)
	//{
	//	lLastError = dwRv;
	//	m_strError.Format ="No reader exist;";
	//	return -3;//list readers failed		
	//}

	i = 0;
	while(iLen = strlen(szReaders + i))
	{
		if(iLen > dwLength)
			break;
		memcpy(m_szReader[m_iReaderCount], szReaders + i, iLen);
		i += iLen + 1;
		++m_iReaderCount;

		//MAX 10 readers
		if(m_iReaderCount == 10)
			break;
	}

	m_isListReaders = 1;
	return 0;
}

// set reader and start track card state.
// return handle of reader for short.
// return value is between 0-9.
int CPCSC_Operator::ConnectReader(const char* pszReader)
{
	SCARD_READERSTATEA readerState;
	int iReaderIndex = 0;
	readerState.dwCurrentState = SCARD_STATE_EMPTY;
	readerState.dwEventState = SCARD_STATE_EMPTY;
	long lHandle = NULL;

	if(pszReader == NULL)
	{
		return -1;
	}
	if(m_isListReaders == 0)
		GetReaders();
	iReaderIndex = GetReaderIndex(pszReader);
	if(iReaderIndex >= 0 && iReaderIndex < 10)
	{
		//DWORD dwRv = 0;
		lHandle = (long)iReaderIndex + 1;
		return lHandle;
	}

	else
		m_strError.Format("Unkown device name:\r\n%s", pszReader);
	return -1;
}

int CPCSC_Operator::GetCardState(int iDevIndex)
{
	DWORD dwRv = 0;
	int index = iDevIndex - 1;

	//point to the reader
	m_readerState.szReader = m_szReader[index];
	m_readerState.dwCurrentState = SCARD_STATE_EMPTY;
	m_readerState.dwEventState = SCARD_STATE_EMPTY;

	dwRv = SCardGetStatusChangeA(hContext, 500, &m_readerState, 1);
	if(dwRv == SCARD_S_SUCCESS)
	{
		memcpy(m_szPreviousState[index], m_szCurrentState[index], 8);
		sprintf_s(m_szCurrentState[index], 10, "%08X", m_readerState.dwEventState);

		if(m_readerState.dwEventState & SCARD_STATE_UNPOWERED)
			m_chStatus = CARD_UNPOWERED;
		else if(m_readerState.dwEventState & SCARD_STATE_EMPTY)
		{
			m_chStatus = CARD_EMPTY;
		}
		else if(m_readerState.dwEventState & SCARD_STATE_INUSE)
		{
			m_chStatus = CARD_INUSE;
		}
		else if(m_readerState.dwEventState & SCARD_STATE_PRESENT)
		{
			m_chStatus = CARD_PRESENT;	//different card

			memset(m_szAtr, 0, sizeof(m_szAtr));
			Bin2Hex(m_szAtr, m_readerState.rgbAtr, m_readerState.cbAtr);
			m_iAtrLen = m_readerState.cbAtr *2;
			m_szAtr[m_iAtrLen] = 0;
		}
		else
		{
			m_chStatus = CARD_ABSENT;
		}

		return 0;
	}
	else
	{
		GetErrMessage(dwRv);
	}

	return -1;
}

//发送指令到机具//HEX//iCmdLen%2==0
int CPCSC_Operator::ICC_SendCommand(int iDevIndex, int iProtocol, unsigned char* pszCommand, int iCmdLen, unsigned char* pszRetSW, unsigned char* pszRetData)
{
	int index = iDevIndex -1;

	//data check
	if(iProtocol & SCARD_PROTOCOL_T0)
	{
		scIORequest.cbPciLength = SCARD_PCI_T0->cbPciLength;
		scIORequest.dwProtocol = SCARD_PCI_T0->dwProtocol;
	}
	else if(iProtocol & SCARD_PROTOCOL_T1)
	{
		scIORequest.cbPciLength = SCARD_PCI_T1->cbPciLength;
		scIORequest.dwProtocol = SCARD_PCI_T1->dwProtocol;
	}
	else
		return ERR_PROTOCOL;

	if(iCmdLen%2)
		return ERR_COMMAND_LENGTH;

	//check hCard
	if(hCard[index] == 0)
		return ERR_CARD_HANDLE;

	//check state
	unsigned char chCardState = 0x00;
	GetCardState(iDevIndex);
	if(chCardState == CARD_EMPTY)
		return ERR_NO_CARD;

	//define verable
	unsigned char szCommand[550];
	unsigned char szRetData[600];
	int iLen = 0;

	DWORD dwRv = 0, dwRecvLength = sizeof(szRetData);

	memset(szCommand, 0, sizeof(szCommand));
	memset(szRetData, 0, sizeof(szRetData));

	//turn to bytes
	memcpy(szCommand, pszCommand, iCmdLen);
	iLen = iCmdLen;
	SwitchDataMode(szCommand, &iLen, HEX2BIN);

	dwRv = SCardTransmit(hCard[index], &scIORequest, szCommand, iLen, NULL, szRetData, &dwRecvLength);
	if(dwRv == SCARD_S_SUCCESS)
	{
		if(dwRecvLength == 2)
		{
			Bin2Hex(pszRetSW, szRetData, dwRecvLength);
			pszRetSW[4] = 0;
			pszRetData[0] = 0;
			if(!memcmp(pszRetSW, "9000", 4))
				return SCARD_S_SUCCESS;
			else if(!memcmp(pszRetSW, "61", 2))
			{
				memset(szCommand, 0, sizeof(szCommand));
				memcpy(szCommand, "00C00000", 8);
				memcpy(szCommand + 8, pszRetSW + 2, 2);
				ICC_SendCommand(index, iProtocol, szCommand, 10, pszRetSW, pszRetData);
			}
			else
				return ERR_TRANSMIT;
		}
		else
		{
			Bin2Hex(pszRetData, szRetData, dwRecvLength);
			memcpy(pszRetSW, pszRetData + (dwRecvLength*2) - 4, 4);
			pszRetData[dwRecvLength*2-4] = 0;
			pszRetSW[4] = 0;
			return SCARD_S_SUCCESS;
		}
	}
	else
	{
		GetErrMessage(dwRv);
		//if(m_strError.GetLength() < 128)
		//{
		//	memcpy(pszRetData, m_strError.GetBuffer(), m_strError.GetLength());
		//	pszRetData[m_strError.GetLength()] = 0;
		//}
		return ERR_TRANSMIT;
	}

	return SCARD_S_SUCCESS;
}


long CPCSC_Operator::PowerOnCard(int iDevIndex)
{
	DWORD dwRv = 0;
	DWORD dwLength = 0, dwState = 0, dwAtrLen = 0;

	int index = iDevIndex - 1;

	dwRv = SCardConnectA(hContext, m_szReader[index], SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0|SCARD_PROTOCOL_T1, &hCard[index], &dwActiveProtocol);
	if(dwRv)
	{
		GetErrMessage(dwRv);
		return ERR_POWER_ON;
	}

	GetCardState(iDevIndex);

	return 0;
}

int CPCSC_Operator::PowerOffCard(int iDevIndex)
{
	DWORD dwRv = 0;
	int index = iDevIndex - 1;

	dwRv = SCardDisconnect(hCard[index], SCARD_UNPOWER_CARD);
	if(dwRv)
	{
		GetErrMessage(dwRv);
		return 1;
	}

	return 0;
}

int CPCSC_Operator::ResetCard(int iDevIndex)
{
	DWORD dwRv = 0;
	int index = iDevIndex - 1;

	dwRv = SCardReconnect(hCard[index], SCARD_SHARE_EXCLUSIVE, SCARD_PROTOCOL_T0|SCARD_PROTOCOL_T1, SCARD_RESET_CARD, &dwActiveProtocol);
	if(dwRv != SCARD_S_SUCCESS)
	{
		GetErrMessage(dwRv);
		return ERR_RESET_CARD;
	}

	GetCardState(iDevIndex);
	return 0;
}


//////////////////////////////////////////////////////////////////////
/////////////
/////////////			Private Functions
/////////////

// return reader index -- Private function
int CPCSC_Operator::GetReaderIndex(const char* pszReader)
{
	int iLen = strlen(pszReader);

	if(iLen > 126)
		return -1;//Error reader name;

	for(int i = 0; i != 10; ++i)
	{
		if(!memcmp(m_szReader[i], pszReader, iLen))
		{
			return i;
		}
	}
	return -2;
}

