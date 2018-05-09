#include "stdafx.h"
#include "Windowsx.h"
#include "sqlext.h"
#include "SQL.H"

extern "C"
{
int __stdcall sql_connect(SQLCHAR * sqlServerName,SQLSMALLINT NameLen,
						  SQLCHAR * UserName,SQLSMALLINT UserNameLen,
						  SQLCHAR * AuthCode,SQLSMALLINT AuthCodeLen,
						  SQLHENV *hEnv,SQLHDBC *hDbc);
int __stdcall sql_disconnect(SQLHENV hEnv,SQLHDBC hDbc);
int __stdcall sql_get_stmt(SQLHENV hDbc,SQLHDBC *hStmt);
int __stdcall sql_free_stmt(SQLHDBC hStmt);
int __stdcall sql_execute_direct(SQLHSTMT hStmt,SQLCHAR *stmtText,
								SQLINTEGER len);
int __stdcall sql_get_data(SQLHSTMT hStmt,SQLUSMALLINT ColNo,
						    SQLSMALLINT TargetType,
						    SQLPOINTER TargetValuePtr,
							SQLINTEGER BufferLength,
							SQLINTEGER *StrLen_or_IndPtr);
int __stdcall sql_fetch(SQLHSTMT hStmt);
int __stdcall sql_closecursor(SQLHSTMT hStmt);
int __stdcall sql_prepare(SQLHSTMT hStmt,SQLCHAR *StmtText,SQLINTEGER TextLen);
int __stdcall sql_bindparameter(SQLHSTMT hStmt,
								SQLUSMALLINT ParameterNumber,
								SQLSMALLINT InputOutputType,
								SQLSMALLINT ValueType,
								SQLSMALLINT ParameterType,
								SQLUINTEGER ColumnSize,
								SQLSMALLINT DecimalDigits,
								SQLPOINTER ParameterValuePtr,
								SQLINTEGER BufferLength,
								SQLINTEGER *StrLen_or_IndPtr);
int __stdcall sql_execute(SQLHSTMT hStmt);
}

int __stdcall sql_connect(SQLCHAR * sqlServerName,SQLSMALLINT NameLen,
						  SQLCHAR * UserName,SQLSMALLINT UserNameLen,
						  SQLCHAR * AuthCode,SQLSMALLINT AuthCodeLen,
						  SQLHENV *hEnv,SQLHDBC *hDbc)
{
	SQLRETURN rtCode;

	//分配环境句柄
	rtCode=SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,hEnv);
	if(rtCode!=SQL_SUCCESS)
	{
		return 1;
	}
	//设置版本信息
	rtCode=SQLSetEnvAttr(*hEnv,SQL_ATTR_ODBC_VERSION,(void*)SQL_OV_ODBC2,0);
	if(rtCode!=SQL_SUCCESS)
	{
		SQLFreeHandle(SQL_HANDLE_ENV,*hEnv);
		return 2;
	}
	//分配连接句柄
	rtCode=SQLAllocHandle(SQL_HANDLE_DBC,*hEnv,hDbc);
	if(rtCode!=SQL_SUCCESS)
	{
		SQLFreeHandle(SQL_HANDLE_ENV,*hEnv);
		return 3;
	}
	//连接TIMEOUT为10秒
	/*
	int t=10;
	rtCode=SQLSetConnectAttr(*hDbc,SQL_LOGIN_TIMEOUT,(void *)&t,0);
	if(rtCode!=SQL_SUCCESS)
	{
		SQLFreeHandle(SQL_HANDLE_ENV,*hEnv);
		return 4;
	}
	*/
	//连接到数据源
	rtCode=SQLConnect(*hDbc,sqlServerName,NameLen,UserName,UserNameLen,AuthCode,AuthCodeLen);
	if(rtCode!=SQL_SUCCESS)
	{
		TRACE("connect ERR\n");
		SQLCHAR sqlstate[6];
		SQLINTEGER  NaticeError;
		SQLCHAR ErrMsg[SQL_MAX_MESSAGE_LENGTH];
		int i=1;
		while(SQLGetDiagRec(SQL_HANDLE_DBC,*hDbc,1,sqlstate,
			&NaticeError,ErrMsg,sizeof(ErrMsg),NULL))
			TRACE("Dial:%d,SQLSTATE:%s,NaticeErr:%d,ErrMsg:%s\n",
			i++,sqlstate,NaticeError,ErrMsg);
		SQLFreeHandle(SQL_HANDLE_DBC,hDbc);
		SQLFreeHandle(SQL_HANDLE_ENV,*hEnv);
		return 5;
	}
	return 0;
}

int __stdcall sql_disconnect(SQLHENV hEnv,SQLHDBC hDbc)
{
	if(SQLDisconnect(hDbc)!=SQL_SUCCESS)
		return 1;
	SQLFreeHandle(SQL_HANDLE_DBC,hDbc);
	SQLFreeHandle(SQL_HANDLE_ENV,hEnv);
	return 0;
}

int __stdcall sql_get_stmt(SQLHENV hDbc,SQLHDBC *hStmt)
{
	SQLRETURN rtCode;

	//分配语句句柄
	rtCode=SQLAllocHandle(SQL_HANDLE_STMT,hDbc,hStmt);
	if(rtCode!=SQL_SUCCESS)
	{
		return 1;
	}
	//SQLSetCursorName(*hStmt, (UCHAR *)"C1", SQL_NTS);


	/*
	rtCode=SQLSetStmtAttr(*hStmt,
SQLHSTMT StatementHandle,
SQLINTEGER Attribute,
SQLPOINTER ValuePtr,
SQLINTEGER StringLength);
*/

	return 0;
}

int __stdcall sql_free_stmt(SQLHDBC hStmt)
{
	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	return 0;
}

int __stdcall sql_execute_direct(SQLHSTMT hStmt,SQLCHAR *stmtText,
								SQLINTEGER len)
{
	SQLRETURN rtCode;
	rtCode=SQLExecDirect(hStmt,stmtText,len);
	if(rtCode==SQL_SUCCESS)
	{
		return 0;
	}
	else if(rtCode==SQL_STILL_EXECUTING)
		return 10;
	TRACE("connect ERR\n");
	SQLCHAR sqlstate[6];
	SQLINTEGER  NaticeError;
	SQLCHAR ErrMsg[SQL_MAX_MESSAGE_LENGTH];
	int i=1;
	while(SQLGetDiagRec(SQL_HANDLE_STMT,hStmt,1,sqlstate,
		&NaticeError,ErrMsg,sizeof(ErrMsg),NULL))
		TRACE("Dial:%d,SQLSTATE:%s,NaticeErr:%d,ErrMsg:%s\n",
			i++,sqlstate,NaticeError,ErrMsg);
	return 2;
}

int __stdcall sql_get_data(SQLHSTMT hStmt,SQLUSMALLINT ColNo,
						    SQLSMALLINT TargetType,
						    SQLPOINTER TargetValuePtr,
							SQLINTEGER BufferLength,
							SQLINTEGER *StrLen_or_IndPtr)
{

	SQLRETURN rtCode;
	rtCode=SQLGetData(hStmt,ColNo,TargetType,TargetValuePtr,
		BufferLength,StrLen_or_IndPtr);
	if(rtCode==SQL_SUCCESS)
		return 0;
	else if(rtCode==SQL_STILL_EXECUTING)
		return 10;
	SQLCHAR sqlstate[6];
	SQLINTEGER  NaticeError;
	SQLCHAR ErrMsg[SQL_MAX_MESSAGE_LENGTH];
	int i=1;
	while(SQLGetDiagRec(SQL_HANDLE_STMT,hStmt,1,sqlstate,
		&NaticeError,ErrMsg,sizeof(ErrMsg),NULL))
		TRACE("Dial:%d,SQLSTATE:%s,NaticeErr:%d,ErrMsg:%s\n",
			i++,sqlstate,NaticeError,ErrMsg);
	return 2;
}

int __stdcall sql_fetch(SQLHSTMT hStmt)
{
		SQLRETURN rtCode;

		rtCode = SQLFetch(hStmt);
		if(rtCode!=SQL_SUCCESS)
		{
			return 1;
		}
		return 0;
}

int __stdcall sql_closecursor(SQLHSTMT hStmt)
{
		SQLRETURN rtCode;

		rtCode = SQLCloseCursor(hStmt);
		if(rtCode!=SQL_SUCCESS)
		{
			return 1;
		}
		return 0;
}

int __stdcall sql_prepare(SQLHSTMT hStmt,SQLCHAR *StmtText,SQLINTEGER TextLen)
{
		SQLRETURN rtCode;

		rtCode = SQLPrepare(hStmt,StmtText,TextLen);
		if(rtCode!=SQL_SUCCESS)
		{
			return 1;
		}
		return 0;
}

int __stdcall sql_bindparameter(SQLHSTMT hStmt,
								SQLUSMALLINT ParameterNumber,
								SQLSMALLINT InputOutputType,
								SQLSMALLINT ValueType,
								SQLSMALLINT ParameterType,
								SQLUINTEGER ColumnSize,
								SQLSMALLINT DecimalDigits,
								SQLPOINTER ParameterValuePtr,
								SQLINTEGER BufferLength,
								SQLINTEGER *StrLen_or_IndPtr)

{
		SQLRETURN rtCode;

		rtCode = SQLBindParameter(hStmt,
								ParameterNumber,
								InputOutputType,
								ValueType,
								ParameterType,
								ColumnSize,
								DecimalDigits,
								ParameterValuePtr,
								BufferLength,
								StrLen_or_IndPtr);
		if(rtCode!=SQL_SUCCESS)
		{
			return 1;
		}
		return 0;
}

int __stdcall sql_execute(SQLHSTMT hStmt)
{
		SQLRETURN rtCode;

		//rtCode=SQLCloseCursor(hStmt);

		rtCode = SQLExecute(hStmt);
		if(rtCode!=SQL_SUCCESS)
		{			
			if(rtCode==-1)
			{
				SQLCHAR sqlstate[6];
				SQLINTEGER  NaticeError;
				SQLCHAR ErrMsg[SQL_MAX_MESSAGE_LENGTH];
				int i=1;
				while(SQLGetDiagRec(SQL_HANDLE_STMT,hStmt,1,sqlstate,
					&NaticeError,ErrMsg,sizeof(ErrMsg),NULL))
					TRACE("Dial:%d,SQLSTATE:%s,NaticeErr:%d,ErrMsg:%s\n",
						i++,sqlstate,NaticeError,ErrMsg);
			}
			return 1;
		}
		return 0;
}
