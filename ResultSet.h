/*----------------------------------------------------------------------
Copyright (c) Dan Petitt, http://www.coderanger.com
All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	ResultSet.h
Owner:	danp@coderanger.com

Purpose:	Allows you to execute a SQL command on a mySQL database
					connection and iterate over any results
					See XMySQLLib.h for more info and examples
----------------------------------------------------------------------*/
#pragma once

namespace XMySQL
{
	class CResultSet
	{
	public:
		enum CursorType { Server = 0, Client = 1 };

		CResultSet( CConnection &conn, CursorType cursor = Server );
		virtual ~CResultSet(void);

		void Execute( LPCTSTR pcszSQL );
		void Executef( LPCTSTR pcszSQL, ... );

		bool IsEOF() const;
		void MoveNext();
		uint64 GetRowCount() const;
		uint64 GetCurrentRowNum() const;

		UINT GetFieldCount() const;
		UINT GetFieldType( UINT uColumn ) const;
		UINT GetFieldIndex( LPCTSTR pcszColumnName );

		bool IsNull( UINT uColumn ) const;
		bool IsNull( LPCTSTR pcszColumnName );

		bool IsFieldUnsigned( UINT uColumn ) const;
		bool IsFieldPrimaryKey( UINT uColumn ) const;
		bool IsFieldUniqueKey( UINT uColumn ) const;

		const char * GetFieldName( UINT uColumn ) const;
		const char * GetFieldValue( UINT uColumn ) const;

		bool GetBool( UINT uColumn ) const;
		bool GetBoolByName( LPCTSTR pcszColumnName );

		int64 GetBigInt( UINT uColumn ) const;
		int64 GetBigIntByName( LPCTSTR pcszColumnName );

		uint64 GetUBigInt( UINT uColumn ) const;
		uint64 GetUBigIntByName( LPCTSTR pcszColumnName );

		char GetTinyInt( UINT uColumn ) const;
		char GetTinyIntByName( LPCTSTR pcszColumnName );

		int GetInt( UINT uColumn ) const;
		int GetIntByName( LPCTSTR pcszColumnName );

		double GetDouble( UINT uColumn ) const;
		double GetDoubleByName( LPCTSTR pcszColumnName );

		const char * GetDateTime( UINT uColumn ) const;
		const char * GetDateTimeByName( LPCTSTR pcszColumnName );

		const char * GetString( UINT uColumn ) const;
		const char * GetStringByName( LPCTSTR pcszColumnName );


	private:
		inline void RetrieveRowData();


	private:
		CConnection &m_conn;
		CursorType m_cursor;

		MYSQL_ROW m_ppRow;
		MYSQL_RES *m_pResult;

		uint64 m_uRowCount;
		uint64 m_uCurrentRow;

		UINT m_uNumFields;
		MYSQL_FIELD *m_pRowFields;
#ifdef _UNICODE
		#define FIELDMAPSTRING std::wstring
#else
		#define FIELDMAPSTRING std::string
#endif
		std::map< FIELDMAPSTRING, UINT > m_mapFieldNames;


	private:
		CResultSet( const CResultSet & );
		CResultSet & operator = ( const CResultSet & );


		#ifdef _UNICODE
			#define THROW_LAST_MYSQL_QUERY_ERROR( code ) \
					{ \
					const char *pcsError = mysql_error( m_conn.m_myData ); \
					CGetUnicode uniError( pcsError, static_cast<int>( strlen( pcsError ) ) ); \
					throw new XMySQL::CException( XMySQL::CException##::code, uniError.GetUnicode() );	\
					}
		#else		// _UNICODE
			#define THROW_LAST_MYSQL_QUERY_ERROR( code ) \
					{ \
					const char *pcsError = mysql_error( m_conn.m_myData ); \
					throw new XMySQL::CException( XMySQL::CException##::code, (LPCTSTR)pcsError );	\
					}
		#endif	// _UNICODE

		#define THROW_MYSQL_QUERY_BUSY \
			throw new XMySQL::CException( XMySQL::CException##::CONNECTION_FAILURE, _T("MySQL is already busy with a query") );
	};
};
