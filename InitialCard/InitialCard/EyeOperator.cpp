#include "StdAfx.h"
#include "EyeOperator.h"


////////////////////////////////////////////////////////////////DLL Operation
//设备操作
typedef INT32 (__stdcall *OPENRADER )(INT8* name, HANDLE *devNo, UINT32 param1, UINT32 param2);
typedef INT32 (__stdcall *CLOSEREADER )(HANDLE devNo);
typedef INT32 (__stdcall *DEVRESET) (HANDLE devNo);
typedef INT32 (__stdcall *LED) (HANDLE devNo, UINT8 ctrlInfo);
typedef INT32 (__stdcall *BEEP) (HANDLE devNo, UINT8 beepTimes, UINT8 interval, UINT8 time);
typedef INT32 (__stdcall *READVER )(HANDLE devNo, UINT8 *readerVersion);
typedef INT32 (__stdcall *READSN) (HANDLE devNo, UINT8 *readerSn);
typedef INT32 (__stdcall *WRITESN) (HANDLE devNo, UINT8 *readerSn);
//CPU卡操作
//	        INT32 __stdcall mw_cpu_reset(HANDLE devNo, UINT32 ivCardSeat, UINT8 *infoATR, UINT8 *atrLen);
typedef INT32 (__stdcall *RESET )(HANDLE devNo, UINT32 ivCardSeat, /*UINT16 *cardType,UINT8 *cardUid,*/ UINT8 *infoATR, UINT8* atrLen);
typedef INT32 (__stdcall *APDU )(HANDLE devNo, UINT32 ivCardSeat, UINT8 *apduData, UINT32 apduLen, UINT8 *respData, INT32 *respLen);
typedef INT32 (__stdcall *POWERDOWN )(HANDLE devNo, UINT32 ivCardSeat);
//typedef INT32 (__stdcall *RFRATS )(HANDLE devNo, UINT32 ivCardSeat, UINT8 *infoATR, uint *atrLen); 

//MIFARE卡操作
typedef INT32 (__stdcall *MIFAREOPENCARD )(HANDLE devNo, UINT8 openMode, UINT8 *cardUid);
typedef INT32 (__stdcall *MIFARECLOSECARD) (HANDLE devNo);
typedef INT32 (__stdcall *MIFARE_AUTH )(HANDLE devNo, UINT8 mode, UINT32 blockNo, UINT8 *key);
typedef INT32 (__stdcall *MIFARE_READ )(HANDLE devNo, UINT32 blockNo, UINT8 *blockData);
typedef INT32 (__stdcall *MIFARE_WRITE) (HANDLE devNo, UINT32 blockNo, UINT8 *blockData);
typedef INT32 (__stdcall *MIFAREREQUESTCARD) (HANDLE devNo, UINT8 openMode, UINT16 *cardType);
typedef INT32 (__stdcall *MIFAREANTICOLL) (HANDLE devNo, UINT8 *cardUid);
typedef INT32 (__stdcall *MIFARESELECT) (HANDLE devNo, UINT8 *cardUid, UINT32 *cardCapacity);
typedef INT32 (__stdcall *MIFAREHALT) (HANDLE devNo);
//typedef INT32 (__stdcall *MIFAREINITVAL) (HANDLE devNo, UINT32 blockNo, INT32 initValue);
//typedef INT32 (__stdcall *MIFAREREADVAL) (HANDLE devNo, UINT32 blockNo, INT32 *dstValue);
//typedef INT32 (__stdcall *MIFAREINCREMENT) (HANDLE devNo, UINT32 blockNo, INT32 value);
//typedef INT32 (__stdcall *MIFAREDECREMENT) (HANDLE devNo, UINT32 blockNo, INT32 value);
//typedef INT32 (__stdcall *MIFARERESTORE) (HANDLE devNo, UINT32 blockNo);
//typedef INT32 (__stdcall *MIFARETRANSFER) (HANDLE devNo, UINT32 blockNo);
////工具函数
//typedef INT32 (__stdcall *ASCTOHEX) (const UINT8 *srcAsc, UINT32 srcLen, UINT8 * dstHex);
//typedef INT32 (__stdcall *HEXTOASC )(const UINT8 *srcHex, UINT32 srcLen, UINT8 *dstAsc);
//typedef INT32 (__stdcall *RF_PCAS )(HANDLE devNo, UINT32 ivCardSeat, UINT8 *apduData, UINT32 apduLen, UINT8 *respData, INT32 *respLen);


APDU	Func_apdu;
DEVRESET	Func_devReset;
CLOSEREADER		Func_closeReader;
BEEP	Func_beep;
OPENRADER	Func_openReader;
POWERDOWN	Func_powerDown;
RESET	Func_cpuReset;
MIFAREOPENCARD	Func_mifareOpenCard;
MIFARECLOSECARD Func_mifareCloseCard;
MIFARE_AUTH		Func_mifareAuth;
MIFARE_READ		Func_mifareRead;
MIFARE_WRITE	Func_mifareWrite;
MIFAREREQUESTCARD	Func_mifareRequest;
MIFAREANTICOLL	Func_mifareAnticoll;
MIFARESELECT	Func_mifareSelect;
MIFAREHALT		Func_mifareHalt;

LED Func_led;
int addFuncApdu = 0;
int addFuncDevReset = 0;
int addFuncCloseReader = 0;
int addFuncBeep = 0;
int addFuncOpenReader = 0;
int addFuncPowerDown = 0;
int addFuncCpuReset = 0;
int addFuncLed = 0;
int addFuncMifareOpenCard = 0;
int addFuncMifareCloseCard = 0;
int addFuncMifareAuthenticate = 0;
int addFuncMifareRead = 0;
int addFuncMifareWrite = 0;
int addFuncMifareRequestCard = 0;
int addFuncMifareAnticoll = 0;
int addFuncMifareSelect = 0;
int addFuncMifareHalt = 0;



CEyeOperator::CEyeOperator(void)
	: m_isConnected(FALSE)
	, iErrorCode(0)
	, m_iSelectCardMode(0)
{
	m_hDev = NULL;

	////////////////////////////////////////////////////////////////////////
	int iRet = loadMWDll(".\\mwCardReader.dll");
	if (0 != iRet)
	{
		iErrorCode = -100;//装载动态库失败
	}
	if(getAddress("mw_cpu_apdu", addFuncApdu) == 0
		&& getAddress("mw_dev_reset", addFuncDevReset) == 0
		&& getAddress("mw_dev_closeReader", addFuncCloseReader) == 0
		&& getAddress("mw_dev_beep", addFuncBeep) == 0
		&& getAddress("mw_dev_openReader", addFuncOpenReader) == 0
		&& getAddress("mw_cpu_powerDown", addFuncPowerDown) == 0
		&& getAddress("mw_cpu_reset", addFuncCpuReset) == 0
		&& getAddress("mw_dev_led", addFuncLed) == 0
		&& getAddress("mw_mifare_openCard", addFuncMifareOpenCard) == 0
		&& getAddress("mw_mifare_closeCard", addFuncMifareCloseCard) == 0
		&& getAddress("mw_mifare_authentication", addFuncMifareAuthenticate) == 0
		&& getAddress("mw_mifare_read", addFuncMifareRead) == 0
		&& getAddress("mw_mifare_write", addFuncMifareWrite) == 0
		&& getAddress("mw_mifare_requestCard", addFuncMifareRequestCard) == 0
		&& getAddress("mw_mifare_anticoll", addFuncMifareAnticoll) == 0
		&& getAddress("mw_mifare_select", addFuncMifareSelect) == 0
		&& getAddress("mw_mifare_halt", addFuncMifareHalt) == 0
		)
	{
		Func_apdu = (APDU)addFuncApdu;
		Func_devReset = (DEVRESET)addFuncDevReset;
		Func_closeReader = (CLOSEREADER)addFuncCloseReader;
		Func_beep = (BEEP)addFuncBeep;
		Func_openReader = (OPENRADER)addFuncOpenReader;
		Func_powerDown = (POWERDOWN)addFuncPowerDown;
		Func_cpuReset = (RESET)addFuncCpuReset;
		Func_led = (LED)addFuncLed;
		Func_mifareOpenCard = (MIFAREOPENCARD)addFuncMifareOpenCard;
		Func_mifareCloseCard = (MIFARECLOSECARD)addFuncMifareCloseCard;
		Func_mifareAuth = (MIFARE_AUTH)addFuncMifareAuthenticate;
		Func_mifareRead = (MIFARE_READ)addFuncMifareRead;
		Func_mifareWrite = (MIFARE_WRITE)addFuncMifareWrite;
		Func_mifareRequest = (MIFAREREQUESTCARD)addFuncMifareRequestCard;
		Func_mifareAnticoll = (MIFAREANTICOLL)addFuncMifareAnticoll;
		Func_mifareSelect = (MIFARESELECT)addFuncMifareSelect;
		Func_mifareHalt = (MIFAREHALT)addFuncMifareHalt;
	}
	else
	{
		iErrorCode = -101;//取函数地址失败
	}

}

CEyeOperator::CEyeOperator(char* pszFilePath)
	: m_isConnected(FALSE)
	, iErrorCode(0)
{
	//int iRet = loadMWDll("mwCardReader.dll");
	//if (0 != iRet)
	//{
	//	iErrorCode = -100;//装载动态库失败
	//}
	m_hDev = NULL;

	char szFilePath[200];

	memset(szFilePath, 0, 200);
	sprintf_s(szFilePath, 200, "%s\\mwCardReader.dll", pszFilePath);

	////////////////////////////////////////////////////////////////////////
	int iRet = loadMWDll(szFilePath);
	if (0 != iRet)
	{
		iErrorCode = -100;//装载动态库失败
	}
	if(getAddress("mw_cpu_apdu", addFuncApdu) == 0
		&& getAddress("mw_dev_reset", addFuncDevReset) == 0
		&& getAddress("mw_dev_closeReader", addFuncCloseReader) == 0
		&& getAddress("mw_dev_beep", addFuncBeep) == 0
		&& getAddress("mw_dev_openReader", addFuncOpenReader) == 0
		&& getAddress("mw_cpu_powerDown", addFuncPowerDown) == 0
		&& getAddress("mw_cpu_reset", addFuncCpuReset) == 0
		&& getAddress("mw_dev_led", addFuncLed) == 0
		)
	{
		Func_apdu = (APDU)addFuncApdu;
		Func_devReset = (DEVRESET)addFuncDevReset;
		Func_closeReader = (CLOSEREADER)addFuncCloseReader;
		Func_beep = (BEEP)addFuncBeep;
		Func_openReader = (OPENRADER)addFuncOpenReader;
		Func_powerDown = (POWERDOWN)addFuncPowerDown;
		Func_cpuReset = (RESET)addFuncCpuReset;
		Func_led = (LED)addFuncLed;
	}
	else
	{
		iErrorCode = -101;//取函数地址失败
	}

}

CEyeOperator::~CEyeOperator(void)
{
	Dev_DisconnReader();
	releaseMWDll();
}

// p1:连接方式,COM1/USB1   P2:波特率或共享1/独占2  P3:校验位E/N或USB为0
int CEyeOperator::Dev_ConnectReader(char* pszConnType, int uiBaud, int uiCheckAcc)
{
	//if(getAddress("mw_dev_openReader", addFuncOpenReader) == 0)
	//{
	//	Func_openReader = (OPENRADER)addFuncOpenReader;
	//}
	//else
	//{
	//	iErrorCode = -101;//取函数地址失败
	//}

	int iRet = 0;
	if(!m_isConnected)
	{
		iRet = Func_openReader((INT8*)pszConnType, &m_hDev, uiBaud, uiCheckAcc);
		if(!iRet)
			m_isConnected = TRUE;
	}
	
	return iRet;
}

int CEyeOperator::Dev_DisconnReader(void)
{
	//if(getAddress("mw_dev_closeReader", addFuncCloseReader) == 0		)
	//{
	//	Func_closeReader = (CLOSEREADER)addFuncCloseReader;
	//}
	//else
	//{
	//	iErrorCode = -101;//取函数地址失败
	//}

	UINT32 iRet;
	if(m_isConnected && m_hDev)
	{
		if(!(iRet = Func_closeReader(m_hDev)))
		{
			m_isConnected = FALSE;
			m_hDev = NULL;
			return iRet;
		}
		
	}
	
	
	return -1;//已关闭等其他原因
}

int CEyeOperator::Dev_ReaderReset(void)
{
	//if(getAddress("mw_dev_reset", addFuncDevReset) == 0)
	//{
	//	Func_devReset = (DEVRESET)addFuncDevReset;
	//}
	//else
	//{
	//	iErrorCode = -101;//取函数地址失败
	//}

	if(m_hDev)
	{
		return (Func_devReset(m_hDev));
	}
	return -1;//句柄错误
}

//0x80 绿灯 0x70 红灯
int CEyeOperator::Dev_LedControl(UINT8 chType)
{
	//if(getAddress("mw_dev_led", addFuncLed) == 0)
	//{
	//	Func_led = (LED)addFuncLed;
	//}
	//else
	//{
	//	iErrorCode = -101;//取函数地址失败
	//}
	return (Func_led(m_hDev, chType));
}

// 3次间隔100ms,每次200ms为 Dev_Beep(3,1,2)
int CEyeOperator::Dev_Beep(UINT8 chBeepTimes, UINT8 interval, UINT8 chLastTime)
{
	//if(getAddress("mw_dev_beep", addFuncBeep) == 0)
	//{
	//	Func_beep = (BEEP)addFuncBeep;
	//}
	//else
	//{
	//	iErrorCode = -101;//取函数地址失败
	//}	
	return Func_beep(m_hDev, chBeepTimes, interval, chLastTime);
}

int CEyeOperator::Dev_CardReset(UINT32 uiCardSeat, UINT8* pszCardUid, UINT8* pszATR)
{
	INT32 iRet = 0, iLen = 0;
	memset(szCardUID, 0, 32);
	memset(szCardATR, 0, 128);
	memset(szSamUID, 0, 32);
	memset(szSamATR, 0, 128);


	if(m_iSelectCardMode == 0)
	{
		iRet = Dev_CardPowerDown(uiCardSeat);
		Sleep(5);
	}

	if(uiCardSeat == USERCARD)
	{
		if((iRet = Func_cpuReset(m_hDev, uiCardSeat, szCardATR, szCardUID)))
		{//失败
			return iRet;
		}
		else
		{
			iLen = (int)szCardUID[0];
			SwitchDataMode(szCardATR, &iLen, ASC2HEX);
			sprintf_s((char*)pszCardUid, 32, "%s", szCardUID);
			sprintf_s((char*)pszATR, 128, "%s", szCardATR);
			//pszCardUid[32] = 0;
			//pszATR[128] = 0;
			return 0;
		}
	}
	else if(uiCardSeat == SAM1 ||
		uiCardSeat == SAM2 ||
		uiCardSeat == SAM3 ||
		uiCardSeat == SAM4 ||
		uiCardSeat == SAM5)
	{
		if((iRet = Func_cpuReset(m_hDev, uiCardSeat, szSamATR, szSamUID)))
		{//失败
			return iRet;
		}
		else
		{
			iLen = (int)szSamUID[0];
			SwitchDataMode(szSamATR, &iLen, ASC2HEX);
			sprintf_s((char*)pszCardUid, 32, "%s", szSamUID);
			sprintf_s((char*)pszATR, 128, "%s", szSamATR);
			return 0;
		}
	}
	else
		return -11;//卡座类型错误
	return 0;
}

// 返回值 >= 0; 0表示数据域为空 //传入是使用HEX //9000返回0，其他SW返回2，数据 > 2
int CEyeOperator::ICC_SendCommand(UINT32 uiCardSeat, UINT8* pszApduData, int uiApduLen, UINT8* pszRetSW, UINT8* pszRetData)
{
	int iRet = 0, iRetLen = 0;
	int iCmdLen = 0;
	unsigned char szAscCommand[310];
	UINT8 szRetData[1024];
	//if(getAddress("mw_cpu_apdu", addFuncApdu) == 0)
	//{
	//	Func_apdu = (APDU)addFuncApdu;
	//}
	//else
	//{
	//	iErrorCode = -101;//取函数地址失败
	//}

	memset(szAscCommand, 0, 256);

	iCmdLen = uiApduLen;
	Hex2Asc(pszApduData, szAscCommand, iCmdLen);
	iCmdLen = iCmdLen/2;


	//Send Command, iRetLen传出返回值长度
	memset(szRetData, 0, 1024);
	iRet = Func_apdu(m_hDev, uiCardSeat, szAscCommand, iCmdLen, szRetData, &iRetLen);
	if (0 == iRet)
	{
		if(iRetLen == 2)
		{
			//自动取响应
			if(!memcmp(szRetData, "\x61", 1))
			{
				memcpy(szAscCommand, "\x00\xC0\x00\x00", 4);
				memcpy(szAscCommand + 4, szRetData + 1, 1);
				iCmdLen = 5;
				SwitchDataMode(szAscCommand, &iCmdLen, ASC2HEX);
				if((iRet = ICC_SendCommand(uiCardSeat, szAscCommand, iCmdLen, pszRetSW, pszRetData)))
					return iRet;
			}
			else
			{//其他SW
				memcpy(pszRetSW, szRetData, 2);
				iCmdLen = 2;
				SwitchDataMode((UINT8*)pszRetSW, &iCmdLen, ASC2HEX);
				pszRetSW[4] = 0;
				pszRetData[0] = 0;
				if(!memcmp(szRetData, "\x90\x00", 2))
					return 0;
				else
					return -2;
			}
		}
		else
		{//带数据域返回
			memcpy(pszRetSW, szRetData + iRetLen - 2, 2);
			iCmdLen = 2;
			SwitchDataMode((UINT8*)pszRetSW, &iCmdLen, ASC2HEX);

			iCmdLen = iRetLen - 2;
			memcpy(pszRetData, szRetData, iCmdLen);
			SwitchDataMode((UINT8*)pszRetData, &iCmdLen, ASC2HEX);
			return 0;
		}
	} 
	else
	{//指令发送失败
		return iRet;
	}

	return 0;
}

int CEyeOperator::Dev_CardPowerDown(UINT32 uiCardSeat)
{
	//if(getAddress("mw_cpu_powerDown", addFuncPowerDown) == 0)
	//{
	//	
	//	Func_powerDown = (POWERDOWN)addFuncPowerDown;
	//}
	//else
	//{
	//	
	//	iErrorCode = -101;//取函数地址失败
	//}
	return Func_powerDown(m_hDev, uiCardSeat);
}

int CEyeOperator::Hex2Asc(UINT8* inHex, UINT8* outAsc, LONG lPair_length)
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
		*outAsc++ = (src1 << 4) | src2; 
	}
	return 0;
}

int CEyeOperator::Asc2Hex(UINT8* inAsc, UINT8* outHex, LONG length)
{
	UCHAR hLowbit,hHighbit;
    long i;
	for(i=0;i<length*2;i=i+2)
	{
		hLowbit=inAsc[i/2]&0x0f;
		hHighbit=inAsc[i/2]/16;
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

int CEyeOperator::SwitchDataMode(UINT8* pszData, int* iDataLen, INT32 iSwitchMode)
{
	unsigned char szKeyHex[512], szKeyAsc[256];
	if(!pszData)
		return -1;

	if(iSwitchMode == HEX2ASC)
	{
		Hex2Asc(pszData, szKeyAsc, *iDataLen);
		memcpy(pszData, szKeyAsc, *iDataLen/2);
		pszData[*iDataLen/2] = '\0';
		*iDataLen = *iDataLen/2;
		return -1;
	}
	
	if(iSwitchMode == ASC2HEX)
	{
		Asc2Hex(pszData, szKeyHex, *iDataLen);
		memcpy(pszData, szKeyHex, *iDataLen*2);
		pszData[*iDataLen*2] = '\0';
		*iDataLen = *iDataLen *2;
		return -1;
	}
	return 0;
}

BOOL CEyeOperator::GetConnectStatus(void)
{
	return m_isConnected;
}

// 十六进制数据转换成10进制
int CEyeOperator::Hex2Dec(unsigned char* HexNum)
{
	int iLen = 0;
	int iNum[16];
	int iCount = 1;
	int iResult = 0;

	iLen = strlen((char*)HexNum);
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

INT32 CEyeOperator::loadMWDll( CString dll_name )
{
	INT32 st = 0;
	CString str_dll( dll_name );
	HMODULE dll_module = LoadLibrary(str_dll);
	if (NULL == dll_module)
	{
		st = -101;
	} 
	else
	{
		st = 0;
		m_hModule = dll_module;
	}
	return st;
}

INT32 CEyeOperator::releaseMWDll()
{
	INT32 st = 0;
	if (INVALID_HANDLE_VALUE == m_hModule)
	{
		st = -102;
	} 
	if (FreeLibrary(m_hModule))
	{
		st = 0;
	} 
	else
	{
		st = -102;
	}
	return st;
}

INT32 CEyeOperator::getAddress( CString fun_name , int &funHandle)
{
	INT32 st = 0;
	int address = 0;
	if (INVALID_HANDLE_VALUE == m_hModule)
	{
		st = -101;
	} 
	else
	{
		if (address = (INT32) GetProcAddress(m_hModule, fun_name))
		{
			st = 0;
			funHandle = address;
		} 
		else
		{
			st = -101;
		}
	}
	return st;
}


// m1 打开卡片 openMode: 0x00 STD,只能寻到空闲卡; 0x01 ALL,所有卡
int CEyeOperator::Dev_MifareOpenCard(unsigned char chOpenMode, unsigned char* pszCardUID)
{
	//if(m_iSelectCardMode == 0)
	//	Dev_CardPowerDown(USERCARD);
	return Func_mifareOpenCard(m_hDev, chOpenMode, pszCardUID);
}


// m1 close card
int CEyeOperator::Dev_MifareCloseCard(void)
{
	return Func_mifareCloseCard(m_hDev);
}


// KeyMode:0-keyA 1-keyB; sectorNO:0-15; hexKey:"FFFFFFFFFFFF"
int CEyeOperator::Dev_MifareAuthenticate(unsigned char chKeyMode, unsigned char chSectorNO, unsigned char* pszHexKey)
{
	return Func_mifareAuth(m_hDev, chKeyMode, chSectorNO, pszHexKey);
}


// block no.:0-63,S70 0-255;
int CEyeOperator::Dev_MifareReadBlock(int iBlockNO, unsigned char* pszBlockData)
{
	return Func_mifareRead(m_hDev, iBlockNO, pszBlockData);
}


int CEyeOperator::Dev_MifareWriteBlock(int iBlockNO, unsigned char* pszBlockData)
{
	return Func_mifareWrite(m_hDev, iBlockNO, pszBlockData);
}


int CEyeOperator::Dev_MifareRequest(unsigned char chOpenMode, int* iCardType)
{
	return Func_mifareRequest(m_hDev, chOpenMode, (UINT16*)iCardType);
}


//output pszCardUID
int CEyeOperator::Dev_MifareAnticoll(unsigned char* pszCardUID)
{
	return Func_mifareAnticoll(m_hDev, pszCardUID);
}


int CEyeOperator::Dev_MifareSelect(unsigned char* pszCardUID, int* pszCardCapacity)
{
	return Func_mifareSelect(m_hDev, pszCardUID, (UINT32*)pszCardCapacity);
}


// HALT
int CEyeOperator::Dev_MifareHalt(void)
{
	return Func_mifareHalt(m_hDev);
}
