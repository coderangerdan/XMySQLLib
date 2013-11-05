/*----------------------------------------------------------------------
Copyright (c) Dan Petitt, http://www.coderanger.com
All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	XMySQLLib.h
Owner:	danp@coderanger.com

Purpose:	Include this file to link the XMySQLLib wrapper classes into
					your project.

Usage:		There are two class objects:
					1. A CConnection class that creates a connection to a mySQL
					database but can also perform SQL commands that require no
					result set iteration.
					2. A CResultSet class that allows you to execute a SQL
					command and iterate over any results.

Example:
					try
					{
						XMySQL::CConnection myDB;
						myDB.Open( "localhost", "root", "abc123", 3306 );
						myDB.SelectDatabase( "Test );
						myDB.Executef( "DELETE FROM TableTest Where nID < %d", 10 );	// Perform a one off SQL statement

						// Iterate over a recordset
						XMySQL::CResultSet rs( myDB, XMySQL::CRecordset::Client );
						rs.Execute( "SELECT * FROM TableTest LIMIT 10" );
						while( !rs.IsEOF() )
						{
							const char * pszValue = rsSource.GetFieldString( 0 );
							rs.MoveNext();
						}

					}
					catch( XMySQL::CException *e )
					{
						_tprintf( _T("ERROR: (%d) %s\n"), e->GetErrorCode(), e->GetError() );
						delete e;
					}

----------------------------------------------------------------------*/
#ifndef XMYSQLLIB_H
#define XMYSQLLIB_H

#pragma once


typedef		unsigned __int64	uint64;
typedef		__int64						int64;



#ifdef _WIN64
	#include "C:\Program Files\MySQL\MySQL Server 5.1\include\mysql.h"

	#pragma message( "**** Linking with 64bit libmysql.lib" )
	#ifdef _DEBUG
		#pragma comment( lib, "C:\\Program Files\\MySQL\\MySQL Server 5.1\\lib\\debug\\libmysql.lib" )
	#else
		#pragma comment( lib, "C:\\Program Files\\MySQL\\MySQL Server 5.1\\lib\\opt\\libmysql.lib" )
	#endif	// _DEBUG
#else
	#include "C:\Program Files (x86)\MySQL\MySQL Server 5.1\include\mysql.h"

	#pragma message( "**** Linking with libmysql.lib" )
	#ifdef _DEBUG
		#pragma comment( lib, "C:\\Program Files (x86)\\MySQL\\MySQL Server 5.1\\lib\\debug\\libmysql.lib" )
	#else
		#pragma comment( lib, "C:\\Program Files (x86)\\MySQL\\MySQL Server 5.1\\lib\\opt\\libmysql.lib" )
	#endif	// _DEBUG
#endif	//	_WIN64



#ifndef BUILD_MYSQLLIB
	#define LIBRARY_NAME "XMySQLLib"

	#ifdef _WIN64
		#define LIBRARY_PLATFORM	"64"
	#else
		#define LIBRARY_PLATFORM	""
	#endif	//	 _WIN64

	#ifdef _UNICODE
		#define LIBRARY_UNICODE	"U"
	#else
		#define LIBRARY_UNICODE	""
	#endif	//	 _UNICODE

	#ifdef _DEBUG
		#define LIBRARY_DEBUG	"D"
	#else
		#define LIBRARY_DEBUG	""
	#endif	//	 _DEBUG


	#define LIBRARY_POSTFIX LIBRARY_PLATFORM LIBRARY_UNICODE LIBRARY_DEBUG

	#pragma comment( lib, LIBRARY_NAME LIBRARY_POSTFIX ".lib" )
	#pragma message( "**** Linking with " LIBRARY_NAME LIBRARY_POSTFIX ".lib" )

	#undef LIBRARY_POSTFIX
	#undef LIBRARY_PLATFORM
	#undef LIBRARY_UNICODE
	#undef LIBRARY_DEBUG
	#undef LIBRARY_NAME
#endif	//	BUILD_MYSQLLIB


#include <string>
#include <map>
#include <algorithm>	// Transform
#include <vector>			// arrays


#include "Exception.h"
#include "Connection.h"
#include "ResultSet.h"

#endif //XMYSQLLIB_H
