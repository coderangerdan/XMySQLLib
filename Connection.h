/*----------------------------------------------------------------------
Copyright (c) Dan Petitt, http://www.coderanger.com
All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	Connection.h
Owner:	danp@coderanger.com

Purpose:	Creates a connection to a mySQL database
					See XMySQLLib.h for more info and examples
----------------------------------------------------------------------*/
#pragma once

namespace XMySQL
{
	class CConnection
	{
	public:
		CConnection(void);
		virtual ~CConnection(void);

		void Open( LPCTSTR pcszServerHost, LPCTSTR pcszUser, LPCTSTR pcszPassword, UINT uPort = MYSQL_PORT );
		void Close();

		bool IsConnected();

		void SelectDatabase( LPCTSTR pcszDatabase );
		LPCTSTR CurrentDatabase() const { return m_pcszCurrentDatabase; }

		void Execute( LPCTSTR pcszSQL );
		void Executef( LPCTSTR pcszSQL, ... );

		void EscapeString( std::string &str );

		struct FieldInfo
		{
			FieldInfo() : uType( 0 ), bIsPrimaryKey( false ) {};
			std::string strName;
			DWORD uType;
			bool bIsPrimaryKey;
		};
		struct Table
		{
			std::string strName;
			std::vector< FieldInfo > arrFields;
		};

		void GetTableInfo( std::vector< Table > &arr, LPCTSTR pcszWildcard = _T("%") );


	private:
		MYSQL *m_myData;
		bool m_bConnected;
		LPCTSTR m_pcszCurrentDatabase;

		friend class CResultSet;

	private:
		CConnection( const CConnection & );
		CConnection & operator = ( const CConnection & );




		#ifdef _UNICODE
			#define THROW_LAST_MYSQL_ERROR( code ) \
					{ \
					const char *pcsError = mysql_error( m_myData ); \
					CGetUnicode uniError( pcsError, static_cast<int>( strlen( pcsError ) ) ); \
					throw new XMySQL::CException( XMySQL::CException##::code, uniError.GetUnicode() );	\
					}
		#else		// _UNICODE
			#define THROW_LAST_MYSQL_ERROR( code ) \
					{ \
					const char *pcsError = mysql_error( m_myData ); \
					throw new XMySQL::CException( XMySQL::CException##::code, (LPCTSTR)pcsError );	\
					}
		#endif	// _UNICODE

		#define THROW_ERROR( code, str ) \
			throw new XMySQL::CException( XMySQL::CException##::code, str );

		#define THROW_MYSQL_GONE_AWAY \
			throw new XMySQL::CException( XMySQL::CException##::CONNECTION_FAILURE, _T("There is no valid mySQL connection, perhaps mySQL timed out") );
	};


	// #ifdef _UNICODE
	// 	#define MYSQL_STRING( str ) \
	// 		CUnicodeToAnsi ansServer( str, static_cast<int>( _tcslen( str ) ) );
	// 
	// 	CUnicodeToAnsi ansServer( pcszServerHost, static_cast<int>( _tcslen( pcszServerHost ) ) );
	// 	CUnicodeToAnsi ansUser( pcszUser, static_cast<int>( _tcslen( pcszUser ) ) );
	// 	CUnicodeToAnsi ansPass( pcszPassword, static_cast<int>( _tcslen( pcszPassword ) ) );
	// #else
	// 	LPCTSTR ansServer = pcszServerHost;
	// 	LPCTSTR ansUser = pcszUser;
	// 	LPCTSTR ansPass = pcszPassword;
	// #endif	// _UNICODE
};
