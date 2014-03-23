/*----------------------------------------------------------------------
Copyright (c) Dan Petitt, http://www.coderanger.com
All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	UnicodeUtils.h
Owner:	danp@coderanger.com

Purpose:	Helper class to convert unicode to ansi strings back and forth
----------------------------------------------------------------------*/
#ifndef UNICODEUTILS_H
#define UNICODEUTILS_H

class CGetAnsi
{
public:
	CGetAnsi( LPCWSTR pcwszText, UINT uLength )
		: m_szText( new char [ uLength * sizeof( TCHAR ) + 1 ] )
	{
		WideCharToMultiByte( CP_ACP, 0, pcwszText, uLength, m_szText, uLength * sizeof( TCHAR ), NULL, NULL );
		m_szText[ uLength ] = '\000';
	}

	CGetAnsi( const char * pcszText, UINT uLength )
	{
		m_szText = new char[ uLength + 1 ];
		strcpy_s( m_szText, uLength + 1, pcszText );
		m_szText[ uLength ] = '\000';
	}

	~CGetAnsi()
	{
		delete[] m_szText;
	}

	const char * GetAnsi() const { return m_szText; }

private:
	char * m_szText;

private:
	CGetAnsi();
	CGetAnsi( const CGetAnsi & );
	CGetAnsi & operator = ( const CGetAnsi & );
};



class CGetUnicode
{
public:
	CGetUnicode( const char * pcszText, UINT uLength )
		: m_wszText( new WCHAR [ uLength * sizeof( TCHAR ) + 1 ] )
	{
		MultiByteToWideChar( CP_ACP, 0, pcszText, uLength, (WCHAR*)m_wszText, uLength * sizeof( TCHAR ) );
		m_wszText[ uLength ] = '\000';
	}

	~CGetUnicode()
	{
		delete[] m_wszText;
	}

	LPCWSTR GetUnicode() const { return m_wszText; }

private:
	LPWSTR m_wszText;

private:
	CGetUnicode();
	CGetUnicode( const CGetUnicode & );
	CGetUnicode & operator = ( const CGetUnicode & );
};

#endif //UNICODEUTILS_H
