#ifndef __ERRLOG_HPP
#define __ERRLOG_HPP


#include <windows.h>
#include <string>
#include <io.h>
#include <direct.h>
using namespace std;


//error infomation if failed
string strErr;
#define LOG_FILE		".\\ErrorLog"

#define LOG_APP_NAME  "TRADE"
const static string g_strDebugFlag("LOG");
const static string g_strFileFlag("FILE_NAME");
static string g_strLogFile("TraLog");
/**********************************************************/
BOOL bLog = 0;

#define ErrLog ErrorLog
#define GetCurDateTime GetCurTime
void GetCurTime(char* pszTime);
void GetDebugFlag()
{
	
	HANDLE hDllhandle = GetModuleHandle("SMK_Device.dll");//获取当前模块句柄
	if (hDllhandle == NULL)
	{
		return;
	}
	char buff[MAX_PATH];
	memset(buff, 0, MAX_PATH);
	GetModuleFileName((HMODULE)hDllhandle, buff, MAX_PATH);//获取当前模块句柄路径

	string strFullPath(buff);
	strFullPath.erase(strFullPath.find_last_of('\\') + 1);
	g_strLogFile = strFullPath;
	strFullPath += "Reader.ini";


	g_strLogFile += "TLog";
	//00 检测目录存在性 0x06
	int iRet = access(g_strLogFile.c_str(), 0x06);
	if(iRet == -1)
	{
		::mkdir(g_strLogFile.c_str());
	}
	g_strLogFile += "\\";

	char szFileName[256];
	memset(szFileName, 0, sizeof(szFileName));
	::GetPrivateProfileStringA(LOG_APP_NAME, g_strFileFlag.c_str(), "TraLog", szFileName, sizeof(szFileName), strFullPath.c_str());
	bLog = ::GetPrivateProfileIntA(LOG_APP_NAME, g_strDebugFlag.c_str(), 0, strFullPath.c_str());

	g_strLogFile += szFileName;
	memset(szFileName, 0, sizeof(szFileName));
	GetCurTime(szFileName);
	szFileName[8] = 0;
	g_strLogFile += szFileName;

}

void GetCurTime(char* pszTime)
{
	SYSTEMTIME sysTime;

	//GetSystemTime(&sysTime);
	::GetLocalTime(&sysTime);

	sprintf_s(pszTime, 30, "%d%02d%02d%02d%02d%02d", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

}


void ErrorLog(char* pszFormat, ...)
{

	GetDebugFlag();

	//log or not
	if(bLog == 0)
		return;

	va_list p;
	char szFormat[1024];
	char szTime[30];
	
	memset(szFormat, 0, sizeof(szFormat));

	va_start(p, pszFormat);
	vsprintf_s(szFormat, sizeof(szFormat), pszFormat, p);
	va_end(p);

	FILE *file;
	fopen_s(&file, g_strLogFile.c_str(), "a+");
	if(file == NULL)
	{
		return;
	}
	memset(szTime, 0, sizeof(szTime));
	GetCurTime(szTime);
	fwrite(szTime, strlen(szTime), 1, file);
	fwrite("\t", 1, 1, file);
	fwrite(szFormat, strlen(szFormat), 1, file);
	fwrite("\n", 1, 1, file);

	fclose(file);
	file = NULL;
}

void DebugLog(char* pszFormat, ...)
{
#ifdef _DEBUG
	GetDebugFlag();

	//log or not
	if(bLog == 0)
		return;

	va_list p;
	char szFormat[1024];
	char szTime[30];
	
	memset(szFormat, 0, sizeof(szFormat));

	va_start(p, pszFormat);
	vsprintf_s(szFormat, sizeof(szFormat), pszFormat, p);
	va_end(p);

	FILE *file;
	fopen_s(&file, g_strLogFile.c_str(), "a+");
	if(file == NULL)
	{
		return;
	}
	memset(szTime, 0, sizeof(szTime));
	GetCurTime(szTime);
	fwrite(szTime, strlen(szTime), 1, file);
	fwrite("\t", 1, 1, file);
	fwrite(szFormat, strlen(szFormat), 1, file);
	fwrite("\n", 1, 1, file);

	fclose(file);
	file = NULL;
#endif
}

#endif
///////////////////////////////////

