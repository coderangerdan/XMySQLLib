XMySQLLib
=======================

XMySQLLib is a a simple mysql c++ wrapper library


## Usage:

Just include the "XMySQLLib.h" file into your project and it will automatically link the appropriate version (32/64 bit)

There are two class objects:

  - A CConnection class that creates a connection to a mySQL database but can also perform SQL commands that require no result set iteration.
  - A CResultSet class that allows you to execute a SQL command and iterate over any results.


## Example:
    try
    {
        XMySQL::CConnection myDB;
        myDB.Open( "localhost", "root", "abc123", 3306 );
        myDB.SelectDatabase( "Test );
        myDB.Executef( "DELETE FROM TableTest Where nID < %d", 10 );    // Perform a one off SQL statement

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


## Licence:

    XMySQLLib - a simple mysql c++ wrapper library

    Copyright (c) 2013 Dan Petitt (CoderRanger.com)

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:

        1)     Redistributions of source code must retain the above copyright
               notice, this list of conditions and the following disclaimer.

        2)     Redistributions in binary form must reproduce the above copyright
               notice, this list of conditions and the following disclaimer in
               the documentation and/or other materials provided with the
               distribution.
             
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
    ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
