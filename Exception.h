/*----------------------------------------------------------------------
Copyright (c) Dan Petitt, http://www.coderanger.com
All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	Exception.h
Owner:	danp@coderanger.com

Purpose:	Simple exception wrapper class for library
					See XMySQLLib.h for more info and examples
----------------------------------------------------------------------*/
#pragma once

namespace XMySQL
{
	class CException
	{
	public:
		enum ErrorCode{
			INITIALISATION_FAILURE,
			CONNECTION_FAILURE,
			DATA_ERROR,
			INVALID_TYPE,
			OUT_OF_RANGE,
			NOT_FOUND,
		};

		CException( ErrorCode error, LPCTSTR pcszError );

		LPCTSTR GetError() const;
		ErrorCode GetErrorCode() const;

	private:
		std::basic_string< TCHAR > m_strError;
		ErrorCode m_uErrorCode;
	};
};
