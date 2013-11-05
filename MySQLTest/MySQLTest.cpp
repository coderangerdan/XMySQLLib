// MySQLTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int /*argc*/, _TCHAR* /*argv[]*/)
{
	try
	{
		XMySQL::CConnection my;
		my.Open( _T("localhost"), _T("root"), _T("onseod") );

		my.SelectDatabase( _T("exportpdb_live") );
		if( !my.IsConnected() )
		{
			_tprintf( _T("NOT Connected\n") );
		}
		my.Executef( _T("SELECT * FROM Keyword WHERE nKeywordID>=%d"), 1 );

		if( !my.IsConnected() )
		{
			_tprintf( _T("NOT Connected\n") );
		}


		std::vector< XMySQL::CConnection::Table > tables;
		my.GetTableInfo( tables );
		for( UINT u = 0; u < tables.size(); u++ )
		{
			const XMySQL::CConnection::Table &tbl = tables.at( u );
			_tprintf( _T("Found table: %s\n"), tbl.strName.c_str() );
			for( UINT n = 0; n < tbl.arrFields.size(); n++ )
			{
				const XMySQL::CConnection::FieldInfo &fld = tbl.arrFields.at( n );
				_tprintf( _T("\tField: %s%s - %d\n"), fld.strName.c_str(), fld.bIsPrimaryKey ? _T(" (Key)") : _T(""), fld.uType );
			}
		}


		// 		XMySQL::CRecordset rs( my, XMySQL::CRecordset::Client );
		// 		rs.Execute( _T("SELECT * FROM Person WHERE nID=48") );
		// 		while( !rs.IsEOF() )
		// 		{
		// 			printf( "GOT ROW: %d = %s, born %s\n", rs.GetLongByName( _T("nID") ), rs.GetStringByName( _T("strname") ), rs.GetDateTimeByName( _T("dtBirth") ) );
		// 			rs.MoveNext();
		// 		}
		// 
		// 		XMySQL::CRecordset rs2( my, XMySQL::CRecordset::Client );
		// 		rs2.Execute( _T("SELECT nKeywordID, strKeyword FROM Keyword LIMIT 10") );
		// 		while( !rs2.IsEOF() )
		// 		{
		// 			// mySQL API is not unicode compliant so our outputs are ansi so output as such
		// 			printf( "GOT ROW: %d/%d = %s\n", rs2.GetLongByName( _T("NKEYWORDid") ), rs2.GetLong( 0 ), rs2.GetString( 1 ) );
		// 			rs2.MoveNext();
		// 		}

		// 		_T("id")
		// 		_T("bigint")
		// 		_T("tinyint")
		// 		_T("datetime")
		// 		_T("date")
		// 		_T("time")
		// 		_T("timestamp")
		// 		_T("text")
		// 		_T("varchar")
		// 		_T("smallint")
		// 		_T("float")
		// 		_T("real")		// is double
		// 		_T("decimal")
		// 		_T("int")
		// 		_T("double")
		// 		_T("boolean")	// is tinyint

		my.SelectDatabase( _T("test") );

		XMySQL::CRecordset rs( my, XMySQL::CRecordset::Client );
		rs.Execute( _T("SELECT * FROM test") );
		while( !rs.IsEOF() )
		{
			printf( "GOT ROW: %I64u\n", rs.IsNull( _T("id") ) ? -1 : rs.GetUBigIntByName( _T("id") ) );
			printf( "GOT ROW: %d\n", rs.IsNull( _T("bigint") ) ? -1 : rs.GetBigIntByName( _T("bigint") ) );
			printf( "GOT ROW: %d\n", rs.IsNull( _T("tinyint") ) ? -1 : rs.GetTinyIntByName( _T("tinyint") ) );
			printf( "GOT ROW: %s\n", rs.IsNull( _T("datetime") ) ? "null" : rs.GetDateTimeByName( _T("datetime") ) );
			printf( "GOT ROW: %s\n", rs.IsNull( _T("date") ) ? "null" : rs.GetDateTimeByName( _T("date") ) );
			printf( "GOT ROW: %s\n", rs.IsNull( _T("text") ) ? "null" : rs.GetStringByName( _T("text") ) );
			printf( "GOT ROW: %s\n", rs.IsNull( _T("varchar") ) ? "null" : rs.GetStringByName( _T("varchar") ) );
			printf( "GOT ROW: %d\n", rs.IsNull( _T("smallint") ) ? -1 : rs.GetIntByName( _T("smallint") ) );

			printf( "GOT ROW: %f\n", rs.IsNull( _T("float") ) ? -1 : rs.GetDoubleByName( _T("float") ) );
			printf( "GOT ROW: %f\n", rs.IsNull( _T("real") ) ? -1 : rs.GetDoubleByName( _T("real") ) );
			printf( "GOT ROW: %f\n", rs.IsNull( _T("decimal") ) ? -1 : rs.GetDoubleByName( _T("decimal") ) );
			printf( "GOT ROW: %d\n", rs.IsNull( _T("int") ) ? -1 : rs.GetIntByName( _T("int") ) );
			printf( "GOT ROW: %f\n", rs.IsNull( _T("double") ) ? -1 : rs.GetDoubleByName( _T("double") ) );
			printf( "GOT ROW: %I64u\n", rs.IsNull( _T("ubigint") ) ? 0 : rs.GetUBigIntByName( _T("ubigint") ) );
			printf( "GOT ROW: %d\n\n\n", rs.IsNull( _T("boolean") ) ? -1 : rs.GetBoolByName( _T("boolean") ) );
			rs.MoveNext();
		}

	}
	catch( XMySQL::CException *e )
	{
		_tprintf( _T("ERROR: (%d) %s\n"), e->GetErrorCode(), e->GetError() );
		delete e;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

