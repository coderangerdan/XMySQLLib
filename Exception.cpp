/*----------------------------------------------------------------------
Copyright (c) Dan Petitt, http://www.coderanger.com
All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	Exception.cpp
Owner:	danp@coderanger.com

Purpose:	Simple exception wrapper class for library
					See XMySQLLib.h for more info and examples
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "Exception.h"


XMySQL::CException::CException( ErrorCode error, LPCTSTR pcszError )
	: m_strError( pcszError )
	, m_uErrorCode( error )
{
}


LPCTSTR XMySQL::CException::GetError() const
{
	return m_strError.c_str();
}


XMySQL::CException::ErrorCode XMySQL::CException::GetErrorCode() const
{
	return m_uErrorCode;
}
