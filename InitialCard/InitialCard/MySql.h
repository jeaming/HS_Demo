#include "stdafx.h"

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