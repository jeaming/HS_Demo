#ifndef WINSCARD_H
#include "WinSCard.h"
#define WINSCARD_H
#endif

#pragma once
class CPCSC_Operator
{
public:
	CPCSC_Operator(void);
	~CPCSC_Operator(void);

	static unsigned char const CARD_ABSENT = 0x00;
	static unsigned char const CARD_PRESENT = 0x01;
	static unsigned char const CARD_INUSE = 0x02;
	static unsigned char const CARD_EMPTY = 0x03;		//
	static unsigned char const CARD_CHANGED = 0x04;		//different card
	static unsigned char const CARD_SAME = 0x05;		//same card
	static unsigned char const CARD_UNPOWERED = 0x05;

private:
	SCARDCONTEXT hContext;//unsigned long
	SCARDHANDLE	hCard[10];//unsigned long
	HANDLE hThread[10];
	SCARD_IO_REQUEST scIORequest;
	//char m_szCardState[10];

	/////////////FUNCTIONS
	// return reader index
	int GetReaderIndex(const char* pszReader);
	int m_isListReaders;

private:
	int TrackCardState(HANDLE* hThread);
	typedef struct TREAD_STRUCTURE
	{
		CPCSC_Operator* _this;
	}THREADSTRUCT,*LPTHREADSTRUCT;

	static DWORD WINAPI StartReaderStateCapture(LPVOID param);

public:
	////////////////////VARABLE
	enum _DATA_TYPE
	{
		HEX2BIN = 0,
		PROTOCOL_T0,
		PROTOCOL_T1,
		BIN2HEX
	};

	char m_szReader[10][100];//10 readers can be saved
	CString m_strError;//error information
	long lLastError;//last error
	SCARD_READERSTATEA m_readerState;
	char m_szCurrentState[10][10];
	char m_szPreviousState[10][10];
	char m_chStatus;
	unsigned char m_szAtr[128];
	DWORD dwActiveProtocol;


public:
	///////////////功能性函数
        /////////////   Using before create a varable
	static int HexNum2Dec(char* HexNum);
	static int Hex2Bin(BYTE* outBin, BYTE* inHex, int lPair_length);
	static int Bin2Hex(BYTE* outHex, BYTE* inBin, int length);
	static int SwitchDataMode(BYTE* pszData, int* iDataLen, int iSwitchMode);//iDataLen could be NULL

	// set reader and start track card state
	int ConnectReader(const char* pszReader);
	int m_iReaderCount;//reader count
	int GetReaders(void);
	void GetErrMessage(long eError);
	int GetCardState(int iDevIndex);

public:
	int ICC_SendCommand(int iDevIndex, int iProtocol, unsigned char* pszCommand, int iCmdLen, unsigned char* pszRetSW, unsigned char* pszRetData);
	long PowerOnCard(int iDevIndex);
	int PowerOffCard(int iDevIndex);
	int m_iAtrLen;
	int ResetCard(int iDevIndex);
	int ICC_CardControl(int iDevIndex, unsigned char* pszSendBuf, DWORD dwSendLen, unsigned char* pszRetData, DWORD dwRetLen);
};


#define ERR_PROTOCOL	-159
#define	ERR_COMMAND_LENGTH	-160
#define ERR_NO_CARD		-161
#define ERR_CARD_HANDLE	-162
#define ERR_TRANSMIT	-163
#define ERR_POWER_ON	-164
#define ERR_RESET_CARD	-165

//typedef struct {
//    LPCSTR      szReader;       // reader name
//    LPVOID      pvUserData;     // user defined data
//    DWORD       dwCurrentState; // current state of reader at time of call
//    DWORD       dwEventState;   // state of reader after state change
//    DWORD       cbAtr;          // Number of bytes in the returned ATR.
//    BYTE        rgbAtr[36];     // Atr of inserted card, (extra alignment bytes)
//} SCARD_READERSTATEA, *PSCARD_READERSTATEA, *LPSCARD_READERSTATEA;
