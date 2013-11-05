/*----------------------------------------------------------------------
Copyright (c) Dan Petitt, http://www.coderanger.com
All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	Connection.cpp
Owner:	danp@coderanger.com

Purpose:	Creates a connection to a mySQL database
					See XMySQLLib.h for more info and examples
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "XMySQLLib.h"
#include "Connection.h"
#include "Exception.h"



XMySQL::CConnection::CConnection(void)
	: m_myData( NULL )
	, m_bConnected( false )
{
}


XMySQL::CConnection::~CConnection(void)
{
	Close();
}


void XMySQL::CConnection::Open( LPCTSTR pcszServerHost, LPCTSTR pcszUser, LPCTSTR pcszPassword, UINT uPort /*= MYSQL_PORT*/ )
{
	Close();

	m_myData = mysql_init( (MYSQL*)0 );
	if( !m_myData )
	{
		THROW_LAST_MYSQL_ERROR( INITIALISATION_FAILURE );
	}


	CGetAnsi ansServer( pcszServerHost, static_cast<int>( _tcslen( pcszServerHost ) ) );
	CGetAnsi ansUser( pcszUser, static_cast<int>( _tcslen( pcszUser ) ) );
	CGetAnsi ansPass( pcszPassword, static_cast<int>( _tcslen( pcszPassword ) ) );
	if( !mysql_real_connect( m_myData, ansServer.GetAnsi(), ansUser.GetAnsi(), ansPass.GetAnsi(), NULL, uPort, NULL, 0 ) )
	{
		THROW_LAST_MYSQL_ERROR( CONNECTION_FAILURE );
	}

	m_bConnected = true;
}


void XMySQL::CConnection::Close()
{
	if( m_myData )
	{
		mysql_close( m_myData );
		m_myData = NULL;

		m_bConnected = false;
	}
}


bool XMySQL::CConnection::IsConnected()
{
	if( m_bConnected )
	{
		int ping_result = mysql_ping( m_myData );
		if( ping_result != 0 )
		{
//			Close();
//			m_bConnected = false;
			THROW_LAST_MYSQL_ERROR( CONNECTION_FAILURE );
		}
	}

	return m_bConnected;
}


void XMySQL::CConnection::SelectDatabase( LPCTSTR pcszDatabase )
{
	if( !IsConnected() )
	{
		THROW_MYSQL_GONE_AWAY
	}

	CGetAnsi ansDatabase( pcszDatabase, static_cast<int>( _tcslen( pcszDatabase ) ) );
	if( mysql_select_db( m_myData, ansDatabase.GetAnsi() ) != 0 ) 
	{
		THROW_LAST_MYSQL_ERROR( DATA_ERROR );
	}		

	m_pcszCurrentDatabase = pcszDatabase;
}


void XMySQL::CConnection::Execute( LPCTSTR pcszSQL )
{
	if( !IsConnected() )
	{
		THROW_MYSQL_GONE_AWAY
	}

	CGetAnsi ansSQL( pcszSQL, static_cast<int>( _tcslen( pcszSQL ) ) );
	if( mysql_real_query( m_myData, ansSQL.GetAnsi(), static_cast<unsigned long>( strlen( ansSQL.GetAnsi() ) ) ) != 0 ) 
	{
		THROW_LAST_MYSQL_ERROR( DATA_ERROR );
	}

	MYSQL_RES *pRes = mysql_use_result( m_myData );
	if( pRes )
	{
		mysql_free_result( pRes );
	}
}


void XMySQL::CConnection::Executef( LPCTSTR pcszSQL, ... )
{
	if( !IsConnected() )
	{
		THROW_MYSQL_GONE_AWAY
	}

	va_list argList;
	va_start( argList, pcszSQL );
	
	TCHAR szBuffer[ 2049 ];
	memset( szBuffer, 0, sizeof( szBuffer ) );

	int nAllocated = 2048, nCharsWritten = 0;
	do
	{
#ifdef _UNICODE
		nCharsWritten = _vsnwprintf_s( szBuffer, nAllocated - 1, 2048, (LPCWSTR)pcszSQL, argList );
#else
		nCharsWritten = _vsntprintf_s( szBuffer, nAllocated - 1, 2048, pcszSQL, argList );
#endif // _DEBUG
		nAllocated *= 2;
	} while( nCharsWritten < 0 );
	szBuffer[ nCharsWritten ] = _T('\000');	// Ensure our buffer is terminated at the right place

	Execute( (LPCTSTR)szBuffer );

	va_end( argList );
}


void XMySQL::CConnection::GetTableInfo( std::vector< Table > &arr, LPCTSTR pcszWildcard /*= _T("%")*/ )
{
	if( !IsConnected() )
	{
		THROW_MYSQL_GONE_AWAY
	}


	MYSQL_RES *pTablesResult = mysql_list_tables( m_myData, pcszWildcard );
	if( pTablesResult )
	{
		MYSQL_ROW ppTableRow = NULL;
		do
		{
			ppTableRow = mysql_fetch_row( pTablesResult );
			if( ppTableRow )
			{
				Table tbl;
				tbl.strName = std::string( ppTableRow[ 0 ] );


				MYSQL_RES *pFieldsResult = mysql_list_fields( m_myData, ppTableRow[ 0 ], _T("%") );
				if( pFieldsResult )
				{
					MYSQL_FIELD *pField = NULL;
					const UINT num_fields = mysql_num_fields( pFieldsResult );
					for( UINT i = 0; i < num_fields; i++ )
					{
						pField = mysql_fetch_field_direct( pFieldsResult, i );
						if( pField )
						{
							FieldInfo fld;
							fld.strName = std::string( pField->name );
							fld.uType = pField->type;
							fld.bIsPrimaryKey = ( pField->flags & PRI_KEY_FLAG ) == PRI_KEY_FLAG;

							tbl.arrFields.push_back( fld );
						}
						else
						{
							THROW_LAST_MYSQL_ERROR( DATA_ERROR );
						}
					}
					arr.push_back( tbl );

					mysql_free_result( pFieldsResult );
				}
			}

		}
		while( pTablesResult && ppTableRow );

		if( pTablesResult ) mysql_free_result( pTablesResult );
	}
	else
	{
		THROW_LAST_MYSQL_ERROR( DATA_ERROR );
	}
}


void XMySQL::CConnection::EscapeString( std::string &str )
{
	char *szNew = new char[ strlen( str.c_str() ) * 3 + 1 ];
	mysql_real_escape_string( m_myData, szNew, str.c_str(), static_cast< unsigned long >( strlen( str.c_str() ) ) );

	std::string sNew( szNew );
	str = sNew;
}