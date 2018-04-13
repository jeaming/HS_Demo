#include "SerialPort.h"

//////////////////////////////////////////////////////////////////////////  
/// COPYRIGHT NOTICE  
/// Copyright (c) 2009, ���пƼ���ѧtickTick Group  ����Ȩ������  
/// All rights reserved.  
///  
/// @file    SerialPort.cpp    
/// @brief   ����ͨ�����ʵ���ļ� 
///    
//////////////////////////////////////////////////////////////////////////  

#include "StdAfx.h"  
#include "SerialPort.h"  
#include <process.h>  
#include <iostream>  

/** �߳��˳���־ */
bool CSerialPort::s_bExit = false;
/** ������������ʱ,sleep���´β�ѯ�����ʱ��,��λ:�� */
const UINT SLEEP_TIME_INTERVAL = 5;

CSerialPort::CSerialPort(void)
: m_hListenThread(INVALID_HANDLE_VALUE)
{
	m_hComm = INVALID_HANDLE_VALUE;
	m_hListenThread = INVALID_HANDLE_VALUE;

	InitializeCriticalSection(&m_csCommunicationSync);

}

CSerialPort::~CSerialPort(void)
{
	CloseListenTread();
	ClosePort();
	DeleteCriticalSection(&m_csCommunicationSync);
}

bool CSerialPort::InitPort(UINT portNo /*= 1*/, UINT baud /*= CBR_9600*/, char parity /*= 'N'*/,
	UINT databits /*= 8*/, UINT stopsbits /*= 1*/, DWORD dwCommEvents /*= EV_RXCHAR*/)
{

	/** ��ʱ����,���ƶ�����ת��Ϊ�ַ�����ʽ,�Թ���DCB�ṹ */
	char szDCBparam[50];
	sprintf_s(szDCBparam, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopsbits);

	/** ��ָ������,�ú����ڲ��Ѿ����ٽ�������,�����벻Ҫ�ӱ��� */
	if (!openPort(portNo))
	{
		return false;
	}

	/** �����ٽ�� */
	EnterCriticalSection(&m_csCommunicationSync);

	/** �Ƿ��д����� */
	BOOL bIsSuccess = TRUE;

	/** �ڴ˿���������������Ļ�������С,���������,��ϵͳ������Ĭ��ֵ.
	*  �Լ����û�������Сʱ,Ҫע�������Դ�һЩ,���⻺�������
	*/
	/*if (bIsSuccess )
	{
	bIsSuccess = SetupComm(m_hComm,10,10);
	}*/

	/** ���ô��ڵĳ�ʱʱ��,����Ϊ0,��ʾ��ʹ�ó�ʱ���� */
	COMMTIMEOUTS  CommTimeouts;
	CommTimeouts.ReadIntervalTimeout = 0;
	CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	CommTimeouts.ReadTotalTimeoutConstant = 0;
	CommTimeouts.WriteTotalTimeoutMultiplier = 0;
	CommTimeouts.WriteTotalTimeoutConstant = 0;
	if (bIsSuccess)
	{
		bIsSuccess = SetCommTimeouts(m_hComm, &CommTimeouts);
	}

	DCB  dcb;
	if (bIsSuccess)
	{
		// ��ANSI�ַ���ת��ΪUNICODE�ַ���  
		DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, NULL, 0);
		wchar_t *pwText = new wchar_t[dwNum];
		if (!MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, pwText, dwNum))
		{
			bIsSuccess = TRUE;
		}

		/** ��ȡ��ǰ�������ò���,���ҹ��촮��DCB���� */
		bIsSuccess = GetCommState(m_hComm, &dcb) && BuildCommDCBW(pwText, &dcb);
		/** ����RTS flow���� */
		dcb.fRtsControl = RTS_CONTROL_ENABLE;

		/** �ͷ��ڴ�ռ� */
		delete[] pwText;
	}

	if (bIsSuccess)
	{
		/** ʹ��DCB�������ô���״̬ */
		bIsSuccess = SetCommState(m_hComm, &dcb);
	}

	/**  ��մ��ڻ����� */
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	/** �뿪�ٽ�� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return bIsSuccess == TRUE;
}

bool CSerialPort::InitPort(UINT portNo /*= 1*/, char* pszDCBparam, int iTimeOut)
{

	/** ��ʱ����,���ƶ�����ת��Ϊ�ַ�����ʽ,�Թ���DCB�ṹ */
	char szDCBparam[50];
	int iBaud = 9600;
	int iDatabits = 8;
	int iStopsBits = 1;
	char chParity;
	int dcbLen = strlen(pszDCBparam);
	int iOffsetL = 0;

	//baud
	for(int i = 0; i != dcbLen; ++i)
	{
		if(pszDCBparam[i] == ';')
		{
			char temp[32];
			memset(temp, 0, sizeof(temp));

			memcpy(temp, pszDCBparam + iOffsetL, i - iOffsetL);
			iBaud = atoi(temp);
			iOffsetL = i;
			break;
		}
	}
	//iDataBits
	iOffsetL ++;
	for(int i = iOffsetL; i != dcbLen; ++i)
	{
		if(pszDCBparam[i] == ';')
		{
			char temp[32];
			memset(temp, 0, sizeof(temp));

			memcpy(temp, pszDCBparam + iOffsetL, i - iOffsetL);
			iDatabits = atoi(temp);
			iOffsetL = i;
			break;
		}
	}

	//parity
	iOffsetL ++;
	chParity  = pszDCBparam[iOffsetL];
	if(chParity == 'E')
		chParity = EVENPARITY;
	else if(chParity == 'O')
		chParity = ODDPARITY;
	else if(chParity == 'N')
		chParity = NOPARITY;
	else if(chParity == 'M')
		chParity = MARKPARITY;
	else
		chParity = NOPARITY;
	iOffsetL ++;

	//stopbits
	iOffsetL ++;
	for(int i = iOffsetL; i != dcbLen; ++i)
	{
		if(pszDCBparam[i] == ';')
		{
			char temp[32];
			memset(temp, 0, sizeof(temp));

			memcpy(temp, pszDCBparam + iOffsetL, i - iOffsetL);
			iStopsBits = atoi(temp);
			iOffsetL = i;

			switch(iStopsBits)
			{
				case 1:iStopsBits=ONESTOPBIT;break;
				case 2:iStopsBits = TWOSTOPBITS;break;
				default:iStopsBits=ONESTOPBIT;
			}

			break;
		}
	}

	sprintf_s(szDCBparam, "baud=%d parity=%c data=%d stop=%d", iBaud, chParity, iDatabits, iStopsBits);

	/** ��ָ������,�ú����ڲ��Ѿ����ٽ�������,�����벻Ҫ�ӱ��� */
	if (!openPort(portNo))
	{
		return false;
	}
	
	/** �����ٽ�� */
	EnterCriticalSection(&m_csCommunicationSync);

	/** �Ƿ��д����� */
	BOOL bIsSuccess = TRUE;

	DCB  dcb;
	GetCommState(m_hComm, &dcb);

    dcb.BaudRate= iBaud;
    dcb.ByteSize= iDatabits;
    dcb.Parity= chParity; //MARKPARITY;//��У��
    dcb.StopBits= ONESTOPBIT;
    //dcb.fOutX=0;
    //dcb.fInX=0;
    //dcb.fBinary=1;
	
	if (bIsSuccess)
	{
		/** ʹ��DCB�������ô���״̬ */
		bIsSuccess = SetCommState(m_hComm, &dcb);
	}


	/** �ڴ˿���������������Ļ�������С,���������,��ϵͳ������Ĭ��ֵ.
	*  �Լ����û�������Сʱ,Ҫע�������Դ�һЩ,���⻺�������
	*/
	if (bIsSuccess )
	{
		bIsSuccess = SetupComm(m_hComm,1024,1024);
	}

	/** ���ô��ڵĳ�ʱʱ��,����Ϊ0,��ʾ��ʹ�ó�ʱ���� */
	COMMTIMEOUTS  CommTimeouts;
	CommTimeouts.ReadIntervalTimeout = 0;
	CommTimeouts.ReadTotalTimeoutMultiplier = iTimeOut;
	CommTimeouts.ReadTotalTimeoutConstant = iTimeOut;
	CommTimeouts.WriteTotalTimeoutMultiplier = iTimeOut;
	CommTimeouts.WriteTotalTimeoutConstant = iTimeOut;
	if (bIsSuccess)
	{
		bIsSuccess = SetCommTimeouts(m_hComm, &CommTimeouts);
	}

	//if (bIsSuccess)
	//{
	//	// ��ANSI�ַ���ת��ΪUNICODE�ַ���  
	//	DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, NULL, 0);
	//	wchar_t *pwText = new wchar_t[dwNum];
	//	if (!MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, pwText, dwNum))
	//	{
	//		bIsSuccess = TRUE;
	//	}

	//	/** ��ȡ��ǰ�������ò���,���ҹ��촮��DCB���� */
	//	bIsSuccess = GetCommState(m_hComm, &dcb) && BuildCommDCBW(pwText, &dcb);

	//	/** ����RTS flow���� */
	//	dcb.fRtsControl = RTS_CONTROL_ENABLE;

	//	/** �ͷ��ڴ�ռ� */
	//	delete[] pwText;
	//}


	/**  ��մ��ڻ����� */
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	/** �뿪�ٽ�� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return bIsSuccess == TRUE;
}

bool CSerialPort::InitPort(UINT portNo, const LPDCB& plDCB)
{
	/** ��ָ������,�ú����ڲ��Ѿ����ٽ�������,�����벻Ҫ�ӱ��� */
	if (!openPort(portNo))
	{
		return false;
	}

	/** �����ٽ�� */
	EnterCriticalSection(&m_csCommunicationSync);

	/** ���ô��ڲ��� */
	if (!SetCommState(m_hComm, plDCB))
	{
		return false;
	}

	/**  ��մ��ڻ����� */
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	/** �뿪�ٽ�� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}

bool CSerialPort::openPort(UINT portNo)
{
	/** �����ٽ�� */
	EnterCriticalSection(&m_csCommunicationSync);

	/** �Ѵ��ڵı��ת��Ϊ�豸�� */
	char szPort[50];
	sprintf_s(szPort, "COM%d", portNo);

	/** ��ָ���Ĵ��� */
	m_hComm = CreateFileA(szPort,  /** �豸��,COM1,COM2�� */
		GENERIC_READ | GENERIC_WRITE, /** ����ģʽ,��ͬʱ��д */
		0,                            /** ����ģʽ,0��ʾ������ */
		NULL,                         /** ��ȫ������,һ��ʹ��NULL */
		OPEN_EXISTING,                /** �ò�����ʾ�豸�������,���򴴽�ʧ�� */
		0,
		0);

	/** �����ʧ�ܣ��ͷ���Դ������ */
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		LeaveCriticalSection(&m_csCommunicationSync);
		return false;
	}

	/** �˳��ٽ��� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}

void CSerialPort::ClosePort()
{
	/** ����д��ڱ��򿪣��ر��� */
	if (m_hComm != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
	}
}

bool CSerialPort::OpenListenThread()
{
	/** ����߳��Ƿ��Ѿ������� */
	if (m_hListenThread != INVALID_HANDLE_VALUE)
	{
		/** �߳��Ѿ����� */
		return false;
	}

	s_bExit = false;
	/** �߳�ID */
	UINT threadId;
	/** �����������ݼ����߳� */
	m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, ListenThread, this, 0, &threadId);
	if (!m_hListenThread)
	{
		return false;
	}

	/** �����̵߳����ȼ�,������ͨ�߳� */
	if (!SetThreadPriority(m_hListenThread, THREAD_PRIORITY_ABOVE_NORMAL))
	{
		return false;
	}

	return true;
}

bool CSerialPort::CloseListenTread()
{
	if (m_hListenThread != INVALID_HANDLE_VALUE)
	{
		/** ֪ͨ�߳��˳� */
		s_bExit = true;

		/** �ȴ��߳��˳� */
		Sleep(10);

		/** ���߳̾����Ч */
		CloseHandle(m_hListenThread);
		m_hListenThread = INVALID_HANDLE_VALUE;
	}
	return true;
}

UINT CSerialPort::GetBytesInCOM()
{
	DWORD dwError = 0;  /** ������ */
	COMSTAT  comstat;   /** COMSTAT�ṹ��,��¼ͨ���豸��״̬��Ϣ */
	memset(&comstat, 0, sizeof(COMSTAT));

	UINT BytesInQue = 0;
	/** �ڵ���ReadFile��WriteFile֮ǰ,ͨ�������������ǰ�����Ĵ����־ */
	if (ClearCommError(m_hComm, &dwError, &comstat))
	{
		BytesInQue = comstat.cbInQue; /** ��ȡ�����뻺�����е��ֽ��� */
	}

	return BytesInQue;
}

UINT WINAPI CSerialPort::ListenThread(void* pParam)
{
	/** �õ������ָ�� */
	CSerialPort *pSerialPort = reinterpret_cast<CSerialPort*>(pParam);

	// �߳�ѭ��,��ѯ��ʽ��ȡ��������  
	while (!pSerialPort->s_bExit)
	{
		UINT BytesInQue = pSerialPort->GetBytesInCOM();
		/** ����������뻺������������,����Ϣһ���ٲ�ѯ */
		if (BytesInQue == 0)
		{
			Sleep(SLEEP_TIME_INTERVAL);
			continue;
		}

		/** ��ȡ���뻺�����е����ݲ������ʾ */
		char cRecved = 0x00;
		do
		{
			cRecved = 0x00;
			if (pSerialPort->ReadChar(cRecved) == true)
			{
				std::cout << cRecved;
				continue;
			}
		} while (--BytesInQue);
	}

	return 0;
}

bool CSerialPort::ReadChar(char &cRecved)
{
	BOOL  bResult = TRUE;
	DWORD BytesRead = 0;
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	/** �ٽ������� */
	EnterCriticalSection(&m_csCommunicationSync);

	/** �ӻ�������ȡһ���ֽڵ����� */
	bResult = ReadFile(m_hComm, &cRecved, 1, &BytesRead, NULL);
	if ((!bResult))
	{
		/** ��ȡ������,���Ը��ݸô�����������ԭ�� */
		DWORD dwError = GetLastError();

		/** ��մ��ڻ����� */
		PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&m_csCommunicationSync);

		return false;
	}

	/** �뿪�ٽ��� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return (BytesRead == 1);

}

bool CSerialPort::WriteData(unsigned char* pData, unsigned int length)
{
	BOOL   bResult = TRUE;
	DWORD  BytesToSend = 0;
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	/** �ٽ������� */
	EnterCriticalSection(&m_csCommunicationSync);

	/** �򻺳���д��ָ���������� */
	bResult = WriteFile(m_hComm, pData, length, &BytesToSend, NULL);
	if (!bResult)
	{
		DWORD dwError = GetLastError();
		/** ��մ��ڻ����� */
		PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&m_csCommunicationSync);

		return false;
	}

	/** �뿪�ٽ��� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}

int CSerialPort::ComTransmit(unsigned char* pszSendBuf,unsigned int iSendLen, unsigned char* pszRecvBuf, int* iRecvLen, int iTimeOut)
{
	BOOL bIsSuccess = TRUE;

	//д����
	if(!WriteData(pszSendBuf, iSendLen))
		return ERR_WRITE_BUFFER;

	/** ��մ��ڻ����� */
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);

	if(iTimeOut != 0)
	{
		/** ���ô��ڵĳ�ʱʱ��,����Ϊ0,��ʾ��ʹ�ó�ʱ���� */
		COMMTIMEOUTS  CommTimeouts;
		CommTimeouts.ReadIntervalTimeout = 0;
		CommTimeouts.ReadTotalTimeoutMultiplier = iTimeOut;
		CommTimeouts.ReadTotalTimeoutConstant = iTimeOut;
		CommTimeouts.WriteTotalTimeoutMultiplier = iTimeOut;
		CommTimeouts.WriteTotalTimeoutConstant = iTimeOut;
		if (bIsSuccess)
		{
			bIsSuccess = SetCommTimeouts(m_hComm, &CommTimeouts);
		}
	}
	
	//׼����ȡ����
	unsigned char szReadBuf[1024];
	unsigned char szTemp[64];
	BOOL  bResult = TRUE;
	int iBufferLen = 0;
	DWORD BytesRead = 0;

	/** �ٽ������� */
	EnterCriticalSection(&m_csCommunicationSync);

	/** �ӻ�������ȡ7���ֽ�(Э��ͷ)������ */
	memset(szTemp, 0, sizeof(szTemp));
	bResult = ReadFile(m_hComm, szTemp, 7, &BytesRead, NULL);
	if ((!bResult))
	{
		/** ��ȡ������,���Ը��ݸô�����������ԭ�� */
		DWORD dwError = GetLastError();

		/** ��մ��ڻ����� */
		PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&m_csCommunicationSync);

		return ERR_READ_BUFFER;
	}

	iBufferLen = atoi((char*)szTemp + 3)  + 2;
	memcpy(szReadBuf, szTemp, 7);
	
	/** �ӻ�������ȡʣ���ֽڵ����� */
	bResult = ReadFile(m_hComm, szReadBuf + 7, iBufferLen, &BytesRead, NULL);
	if ((!bResult))
	{
		/** ��ȡ������,���Ը��ݸô�����������ԭ�� */
		DWORD dwError = GetLastError();

		/** ��մ��ڻ����� */
		PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&m_csCommunicationSync);

		return ERR_READ_BUFFER;
	}

	*iRecvLen = (int)BytesRead + 7;
	memcpy(pszRecvBuf, szReadBuf, *iRecvLen);

	/** �뿪�ٽ��� */
	LeaveCriticalSection(&m_csCommunicationSync);

	return 0;
}