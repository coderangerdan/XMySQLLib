/*----------------------------------------------------------------------
Copyright (c) Dan Petitt, http://www.coderanger.com
All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	ResultSet.cpp
Owner:	danp@coderanger.com

Purpose:	Allows you to execute a SQL command on a mySQL database
					connection and iterate over any results
					See XMySQLLib.h for more info and examples
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "XMySQLLib.h"
#include "Connection.h"
#include "ResultSet.h"
#include "Exception.h"

static const char * emptyString = "";

XMySQL::CResultSet::CResultSet( XMySQL::CConnection &conn, CursorType cursor /*= Server*/ )
	: m_conn( conn )
	, m_cursor( cursor )

	, m_pResult( NULL )
	, m_ppRow( NULL )
	, m_uRowCount( 0 )
	, m_uCurrentRow( 0 )

	, m_uNumFields( 0 )
	, m_pRowFields( NULL )
{
}

XMySQL::CResultSet::~CResultSet( void )
{
	if( m_pResult /*&& m_cursor == Client*/ )
	{
		mysql_free_result( m_pResult );
		m_pResult = NULL;
		m_ppRow = NULL;

		m_uRowCount = 0;
		m_uCurrentRow = 0;

		m_uNumFields = 0;
		m_pRowFields = NULL;

		m_mapFieldNames.clear();
	}
}


void XMySQL::CResultSet::Execute( LPCTSTR pcszSQL )
{
	if( m_pResult )
	{
		THROW_MYSQL_QUERY_BUSY
	}

	CGetAnsi ansSQL( pcszSQL, static_cast<int>( _tcslen( pcszSQL ) ) );
	if( mysql_real_query( m_conn.m_myData, ansSQL.GetAnsi(), static_cast<unsigned long>( strlen( ansSQL.GetAnsi() ) ) ) != 0 ) 
	{
		THROW_LAST_MYSQL_QUERY_ERROR( DATA_ERROR )
	}

//	mysql_use_result() - http://dev.mysql.com/doc/refman/4.1/en/mysql-use-result.html
	m_pResult = ( m_cursor == Client ? mysql_store_result( m_conn.m_myData ) : mysql_use_result( m_conn.m_myData ) );
	if( m_pResult )
	{
		m_uRowCount = mysql_num_rows( m_pResult );
		m_uNumFields = mysql_num_fields( m_pResult );
		m_pRowFields = mysql_fetch_fields( m_pResult );
		for( UINT u = 0; u < m_uNumFields; u++ )
		{
			const char *pcsName = m_pRowFields[ u ].name;
			#ifdef _UNICODE
				CGetUnicode uniName( pcsName, static_cast<int>( strlen( pcsName ) ) );
				std::wstring sName( uniName.GetUnicode() );
			#else
				std::string sName( pcsName );
			#endif	// _UNICODE

			std::transform( sName.begin(), sName.end(), sName.begin(), tolower );
			m_mapFieldNames[ sName ] = u;
		}

		RetrieveRowData();
	}
}


void XMySQL::CResultSet::Executef( LPCTSTR pcszSQL, ... )
{
	if( m_pResult )
	{
		THROW_MYSQL_QUERY_BUSY
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


inline void XMySQL::CResultSet::RetrieveRowData()
{
	m_ppRow = mysql_fetch_row( m_pResult );
	m_uCurrentRow++;
}

bool XMySQL::CResultSet::IsEOF() const
{
	return m_pResult == NULL || m_ppRow == NULL ? true : false;
}

void XMySQL::CResultSet::MoveNext()
{
	RetrieveRowData();
}

uint64 XMySQL::CResultSet::GetRowCount() const
{
	return m_uRowCount;
}

uint64 XMySQL::CResultSet::GetCurrentRowNum() const
{
	return m_uCurrentRow;
}


UINT XMySQL::CResultSet::GetFieldCount() const
{
	return m_uNumFields;
}

UINT XMySQL::CResultSet::GetFieldType( UINT uColumn ) const
{
	return m_pRowFields[ uColumn ].type;
}

const char * XMySQL::CResultSet::GetFieldName( UINT uColumn ) const
{
	return m_pRowFields[ uColumn ].name;
}

const char * XMySQL::CResultSet::GetFieldValue( UINT uColumn ) const
{
	return m_ppRow[ uColumn ];
}

bool XMySQL::CResultSet::IsFieldUnsigned( UINT uColumn ) const
{
	return ( m_pRowFields[ uColumn ].flags & UNSIGNED_FLAG ) == UNSIGNED_FLAG ? true : false;
}

bool XMySQL::CResultSet::IsFieldPrimaryKey( UINT uColumn ) const
{
	return ( m_pRowFields[ uColumn ].flags & PRI_KEY_FLAG ) == PRI_KEY_FLAG ? true : false;
}

bool XMySQL::CResultSet::IsFieldUniqueKey( UINT uColumn ) const
{
	return ( m_pRowFields[ uColumn ].flags & UNIQUE_KEY_FLAG ) == UNIQUE_KEY_FLAG ? true : false;
}



UINT XMySQL::CResultSet::GetFieldIndex( LPCTSTR pcszColumnName )
{
#ifdef _UNICODE
// 	CUnicodeToAnsi ansName( (LPCWSTR)pcszColumnName, static_cast<int>( _tcslen( pcszColumnName ) ) );
// 	std::string sColumn( ansName );
	std::wstring sColumn( pcszColumnName );
#else
	std::string sColumn( pcszColumnName );
#endif

	std::transform( sColumn.begin(), sColumn.end(), sColumn.begin(), tolower);
	if( m_mapFieldNames.find( sColumn ) == m_mapFieldNames.end() )
	{
		THROW_ERROR( OUT_OF_RANGE, _T("Field column name is unknown") );
	}

	return m_mapFieldNames[ sColumn ];
}


bool XMySQL::CResultSet::IsNull( UINT uColumn ) const
{
	if( uColumn < 0 || uColumn > m_uNumFields )
	{
		THROW_ERROR( OUT_OF_RANGE, _T("Field column value is out of range") );
	}

	return m_ppRow[ uColumn ] == NULL ? true : false;
}

bool XMySQL::CResultSet::IsNull( LPCTSTR pcszColumnName )
{
	return IsNull( GetFieldIndex( pcszColumnName ) );
}



bool XMySQL::CResultSet::GetBool( UINT uColumn ) const
{
	if( uColumn < 0 || uColumn > m_uNumFields )
	{
		THROW_ERROR( OUT_OF_RANGE, _T("Field column value is out of range") );
	}

	if( m_pRowFields[ uColumn ].type == MYSQL_TYPE_TINY )
	{
		if( m_ppRow[ uColumn ] == NULL )
		{
			return false;
		}

		return atoi( m_ppRow[ uColumn ] ) == 0 ? false : true;
	}
	else
	{
		THROW_ERROR( INVALID_TYPE, _T("Field is not a BOOLEAN or TINYINT field") );
	}
}

bool XMySQL::CResultSet::GetBoolByName( LPCTSTR pcszColumnName )
{
	return GetBool( GetFieldIndex( pcszColumnName ) );
}


char XMySQL::CResultSet::GetTinyInt( UINT uColumn ) const
{
	if( uColumn < 0 || uColumn > m_uNumFields )
	{
		THROW_ERROR( OUT_OF_RANGE, _T("Field column value is out of range") );
	}

	if( m_pRowFields[ uColumn ].type == MYSQL_TYPE_TINY )
	{
		if( m_ppRow[ uColumn ] == NULL )
		{
			return 0;
		}

		return (char)atoi( m_ppRow[ uColumn ] );
	}
	else
	{
		THROW_ERROR( INVALID_TYPE, _T("Field is not a TINYINT numeric field") );
	}
}

char XMySQL::CResultSet::GetTinyIntByName( LPCTSTR pcszColumnName )
{
	return GetTinyInt( GetFieldIndex( pcszColumnName ) );
}


int64 XMySQL::CResultSet::GetBigInt( UINT uColumn ) const
{
	if( uColumn < 0 || uColumn > m_uNumFields )
	{
		THROW_ERROR( OUT_OF_RANGE, _T("Field column value is out of range") );
	}

	if( m_pRowFields[ uColumn ].type == MYSQL_TYPE_LONGLONG )
	{
		if( IsFieldUnsigned( uColumn ) )
		{
			THROW_ERROR( INVALID_TYPE, _T("Field is an UNSIGNED BIGINT numeric field, use GetUBigInt") );
		}

		if( m_ppRow[ uColumn ] == NULL )
		{
			return 0;
		}

		return _atoi64( m_ppRow[ uColumn ] );
	}
	else
	{
		THROW_ERROR( INVALID_TYPE, _T("Field is not a BIGINT numeric field") );
	}
}

int64 XMySQL::CResultSet::GetBigIntByName( LPCTSTR pcszColumnName )
{
	return GetBigInt( GetFieldIndex( pcszColumnName ) );
}


uint64 XMySQL::CResultSet::GetUBigInt( UINT uColumn ) const
{
	if( uColumn < 0 || uColumn > m_uNumFields )
	{
		THROW_ERROR( OUT_OF_RANGE, _T("Field column value is out of range") );
	}

	if( m_pRowFields[ uColumn ].type == MYSQL_TYPE_LONGLONG )
	{
		if( !IsFieldUnsigned( uColumn ) )
		{
			THROW_ERROR( INVALID_TYPE, _T("Field is an BIGINT numeric field, use GetBigInt") );
		}

		if( m_ppRow[ uColumn ] == NULL )
		{
			return 0;
		}

		uint64 val = 0;
		std::string str( m_ppRow[ uColumn ] );
		for (size_t i = 0; i < str.size(); i++)
		{
			val = val * 10 + (str[i] - 48);
		}
		return val;
	}
	else
	{
		THROW_ERROR( INVALID_TYPE, _T("Field is not a BIGINT numeric field") );
	}
}

uint64 XMySQL::CResultSet::GetUBigIntByName( LPCTSTR pcszColumnName )
{
	return GetUBigInt( GetFieldIndex( pcszColumnName ) );
}


int XMySQL::CResultSet::GetInt( UINT uColumn ) const
{
	if( uColumn < 0 || uColumn > m_uNumFields )
	{
		THROW_ERROR( OUT_OF_RANGE, _T("Field column value is out of range") );
	}

	if( m_pRowFields[ uColumn ].type == MYSQL_TYPE_LONG || m_pRowFields[ uColumn ].type == MYSQL_TYPE_SHORT || m_pRowFields[ uColumn ].type == MYSQL_TYPE_INT24 )
	{
		if( m_ppRow[ uColumn ] == NULL )
		{
			return 0;
		}

		return atoi( m_ppRow[ uColumn ] );
	}
	else
	{
		THROW_ERROR( INVALID_TYPE, _T("Field is not a SMALLINT, MEDIUMINT or INTEGER field") );
	}
}

int XMySQL::CResultSet::GetIntByName( LPCTSTR pcszColumnName )
{
	return GetInt( GetFieldIndex( pcszColumnName ) );
}


double XMySQL::CResultSet::GetDouble( UINT uColumn ) const
{
	if( uColumn < 0 || uColumn > m_uNumFields )
	{
		THROW_ERROR( OUT_OF_RANGE, _T("Field column value is out of range") );
	}

	if( m_pRowFields[ uColumn ].type == MYSQL_TYPE_FLOAT || m_pRowFields[ uColumn ].type == MYSQL_TYPE_DOUBLE || m_pRowFields[ uColumn ].type == MYSQL_TYPE_DECIMAL || m_pRowFields[ uColumn ].type == MYSQL_TYPE_NEWDECIMAL )
	{
		if( m_ppRow[ uColumn ] == NULL )
		{
			return 0;
		}

		return atof( m_ppRow[ uColumn ] );
	}
	else
	{
		THROW_ERROR( INVALID_TYPE, _T("Field is not a FLOAT, DOUBLE or DECIMAL field") );
	}
}

double XMySQL::CResultSet::GetDoubleByName( LPCTSTR pcszColumnName )
{
	return GetDouble( GetFieldIndex( pcszColumnName ) );
}


const char * XMySQL::CResultSet::GetString( UINT uColumn ) const
{
	if( uColumn < 0 || uColumn > m_uNumFields )
	{
		THROW_ERROR( OUT_OF_RANGE, _T("Field column value is out of range") );
	}

	if( m_pRowFields[ uColumn ].type == MYSQL_TYPE_VAR_STRING || m_pRowFields[ uColumn ].type == MYSQL_TYPE_STRING || m_pRowFields[ uColumn ].type == MYSQL_TYPE_VARCHAR || m_pRowFields[ uColumn ].type == MYSQL_TYPE_BLOB )
	{
		if( m_ppRow[ uColumn ] == NULL )
		{
			return emptyString;
		}

		return m_ppRow[ uColumn ];
	}
	else
	{
		THROW_ERROR( INVALID_TYPE, _T("Field is not a TEXT or VARCHAR field") );
	}
}

const char * XMySQL::CResultSet::GetStringByName( LPCTSTR pcszColumnName )
{
	return GetString( GetFieldIndex( pcszColumnName ) );
}


const char * XMySQL::CResultSet::GetDateTime( UINT uColumn ) const
{
	if( uColumn < 0 || uColumn > m_uNumFields )
	{
		THROW_ERROR( OUT_OF_RANGE, _T("Field column value is out of range") );
	}

	if( m_pRowFields[ uColumn ].type == MYSQL_TYPE_DATETIME || m_pRowFields[ uColumn ].type == MYSQL_TYPE_TIMESTAMP || m_pRowFields[ uColumn ].type == MYSQL_TYPE_DATE || m_pRowFields[ uColumn ].type == MYSQL_TYPE_TIME )
	{
		if( m_ppRow[ uColumn ] == NULL )
		{
			return emptyString;
		}

		// 1943-02-09 00:00:00
		return m_ppRow[ uColumn ];
	}
	else
	{
		THROW_ERROR( INVALID_TYPE, _T("Field is not a DATE, DATETIME or TIMESTAMP field") );
	}
}

const char * XMySQL::CResultSet::GetDateTimeByName( LPCTSTR pcszColumnName )
{
	return GetDateTime( GetFieldIndex( pcszColumnName ) );
}
