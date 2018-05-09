#pragma once

class CEyeOperator
{
public:
	CEyeOperator(void);
	CEyeOperator(char* pszFilePath);
	~CEyeOperator(void);
	enum CardSeat
	{
		USERCARD = 1,	//RF
		SAM1,			//С����
		SAM2,
		SAM3,
		SAM4,
		SAM5
	};
	enum DataMode
	{
		HEX2ASC = 11,
		ASC2HEX,
		HEX,
		ASC
	};

//���ñ���
public:
	int Dev_ReaderReset(void);
	int iErrorCode;
	UINT8 szCardUID[32];
	UINT8 szCardATR[128];
	UINT8 szSamUID[32];
	UINT8 szSamATR[128];

//��������
protected:
	BOOL m_isConnected;

//˽�б���
private:
	// ���Ӿ��
	HANDLE m_hDev;

//���к���
public:
	// p1:���ӷ�ʽ,COM1/USB1   P2:�����ʻ���1/��ռ2  P3:У��λE/N��USBΪ0
	int Dev_ConnectReader(char* pszConnType, int uiBaud, int uiCheckAcc);
	int Dev_DisconnReader(void);

	int Dev_LedControl(UINT8 chType);
	// 3�μ��100ms,ÿ��200msΪ Dev_Beep(3,1,2)
	int Dev_Beep(UINT8 chBeepTimes, UINT8 interval, UINT8 chLastTime);
	int Dev_CardReset(UINT32 uiCardSeat, UINT8* pszCardUid, UINT8* pszATR);
	// ����ֵ >= 0; 0��ʾ������Ϊ��
	int ICC_SendCommand(UINT32 uiCardSeat, UINT8* pszApduData, int uiApduLen, UINT8* pszRetSW, UINT8* pszRetData);
	int Dev_CardPowerDown(UINT32 uiCardSeat);
	int Hex2Asc(UINT8* inHex, UINT8* outAsc, LONG lPair_length);
	int Asc2Hex(UINT8* inAsc, UINT8* outHex, LONG length);
	int SwitchDataMode(UINT8* pszData, int* iDataLen, INT32 iSwitchMode);
	BOOL GetConnectStatus(void);
	int Hex2Dec(unsigned char* HexNum);
	int Dll_Initial(void);
	INT32 CEyeOperator::getAddress( CString fun_name , int &funHandle);
	INT32 CEyeOperator::releaseMWDll();
	INT32 CEyeOperator::loadMWDll( CString dll_name );

private:
	HINSTANCE m_hModule;
public:
	// Ѱ����ʽ��Ĭ��0���ɲ��ÿ���Ƭ�ظ�Ѱ��
	int m_iSelectCardMode;
	// m1 �򿪿�Ƭ
	int Dev_MifareOpenCard(unsigned char chOpenMode, unsigned char* pszCardUID);
	// m1 close card
	int Dev_MifareCloseCard(void);
	// KeyMode:0-keyA 1-keyB; sectorNO:0-15; hexKey:"FFFFFFFFFFFF"
	int Dev_MifareAuthenticate(unsigned char chKeyMode, unsigned char chSectorNO, unsigned char* pszHexKey);
	// block no.:0-63,S70 0-255;
	int Dev_MifareReadBlock(int iBlockNO, unsigned char* pszBlockData);
	int Dev_MifareWriteBlock(int iBlockNO, unsigned char* pszBlockData);
	int Dev_MifareRequest(unsigned char chOpenMode, int* iCardType);
	int Dev_MifareAnticoll(unsigned char* pszCardUID);
	int Dev_MifareSelect(unsigned char* pszCardUID, int* pszCardCapacity);
	// HALT
	int Dev_MifareHalt(void);
};


