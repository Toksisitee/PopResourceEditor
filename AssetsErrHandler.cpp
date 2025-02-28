/*
Copyright (c) 2024-2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#include "spdlog/spdlog.h"

#include "Editor.h"
#include "AssetsErrHandler.h"

Assets::CErrHandler g_ErrHandler;

namespace Assets
{

	CErrHandler::CErrHandler()
	{
		m_szErrBuffer = (char*)malloc( m_kErrBufferSize );
		m_szTypeBuffer = (char*)malloc( m_kTypeBufferSize );
		m_FileType = FileType::None;
		m_Err = Result::OK;
	}

	CErrHandler::~CErrHandler()
	{
		SAFE_FREE( m_szErrBuffer );
		SAFE_FREE( m_szTypeBuffer );
	}

	void CErrHandler::LogFmt( const char* pszFmt, ... )
	{
		char* pszBuffer = (char*)malloc( m_kErrBufferSize );
		assert( pszBuffer && "CErrHandler: Failed to allocate memory" );

		if ( pszBuffer ) {
			va_list argptr;
			va_start( argptr, pszFmt );
			vsprintf_s( pszBuffer, m_kErrBufferSize, pszFmt, argptr );
			va_end( argptr );
			Log( pszBuffer );
			free( pszBuffer );
		}
	}

	void CErrHandler::LogFmt( Log::Level eLevel, const char* pszFmt, ... )
	{
		char* pszBuffer = (char*)malloc( m_kErrBufferSize );
		assert( pszBuffer && "CErrHandler: Failed to allocate memory" );

		if ( pszBuffer ) {
			va_list argptr;
			va_start( argptr, pszFmt );
			vsprintf_s( pszBuffer, m_kErrBufferSize, pszFmt, argptr );
			va_end( argptr );
			EmitLog( pszBuffer, eLevel );
			free( pszBuffer );
		}
	}

	void CErrHandler::Log( const char* pszMsg )
	{
		assert( m_FileType != FileType::None && "CErrHandler: File type not set" );
		sprintf_s( m_szErrBuffer, m_kErrBufferSize, "[%s]: %s", GetLastFileTypeSz(), pszMsg );
		EmitLogBuffer();
	}

	void CErrHandler::Log( Log::Level eLevel, const char* pszMsg )
	{
		EmitLog( pszMsg, eLevel );
	}

	void CErrHandler::EmitLogBuffer()
	{
		switch ( m_Err ) {
			case Assets::Result::FAIL:
			case Assets::Result::FAIL_PARSE:
			case Assets::Result::FAIL_LOAD:
				spdlog::critical( m_szErrBuffer );
				break;
			case Assets::Result::FAIL_EXPORT:
				spdlog::error( m_szErrBuffer );
				break;
			case Assets::Result::OK:
			default:
				spdlog::info( m_szErrBuffer );
				break;
		}
	}

	void CErrHandler::EmitLog( const char* pszMsg, Log::Level eLevel )
	{
		switch ( eLevel ) {
			case Log::Level::TRC:
				spdlog::trace( pszMsg );
				break;
			case Log::Level::DBG:
				spdlog::debug( pszMsg );
				break;
			case Log::Level::WRN:
				spdlog::warn( pszMsg );
				break;
			case Log::Level::ERR:
				spdlog::error( pszMsg );
				break;
			case Log::Level::CRT:
				spdlog::critical( pszMsg );
				break;
			case Log::Level::INF:
			default:
				spdlog::info( pszMsg );
				break;
		}
	}

	Result CErrHandler::HandleResult( Assets::Result eResult, std::optional<std::string> sErrorMsg )
	{
		assert( m_FileType != FileType::None && "CErrHandler: File type not set" );
		SetError( eResult );

		char szErrCode[128];
		switch ( m_Err ) {
			case Assets::Result::OK:
				sprintf_s( szErrCode, sizeof( szErrCode ), "OK." );
				break;
			case Assets::Result::OK_LOAD:
				sprintf_s( szErrCode, sizeof( szErrCode ), "Successfully loaded file." );
				break;
			case Assets::Result::OK_EXPORT:
				sprintf_s( szErrCode, sizeof( szErrCode ), "Successfully exported file." );
				break;
			case Assets::Result::OK_PARSE:
				sprintf_s( szErrCode, sizeof( szErrCode ), "Successfully parsed file.." );
				break;
			case Assets::Result::FAIL_LOAD:
				sprintf_s( szErrCode, sizeof( szErrCode ), "Failed to load file." );
				break;
			case Assets::Result::FAIL_EXPORT:
				sprintf_s( szErrCode, sizeof( szErrCode ), "Failed to export file." );
				break;
			case Assets::Result::FAIL_PARSE:
				sprintf_s( szErrCode, sizeof( szErrCode ), "Failed to parse file." );
				break;
			case Assets::Result::OK_GENERATE:
				sprintf_s( szErrCode, sizeof( szErrCode ), "Generate OK." );
				break;
			case Assets::Result::FAIL_GENERATE:
				sprintf_s( szErrCode, sizeof( szErrCode ), "Generate FAIL." );
				break;
			default:
				sprintf_s( szErrCode, sizeof( szErrCode ), "Unknown error code." );
				break;
		}

		if ( sErrorMsg.has_value() ) {
			sprintf_s( m_szErrBuffer, m_kErrBufferSize, "[%s]: %s", GetLastFileTypeSz(), sErrorMsg.value().c_str() );
		}
		else {
			sprintf_s( m_szErrBuffer, m_kErrBufferSize, "[%s]: %s", GetLastFileTypeSz(), szErrCode );
		}

		EmitLogBuffer();
		return eResult;
	}

	const char* CErrHandler::GetLastFileTypeSz()
	{
		return Assets::GetFileTypeSz( m_FileType );
	}

	const char* CErrHandler::GetLastErrorSz()
	{
		return m_szErrBuffer;
	};

	FileType CErrHandler::GetFileType()
	{
		return m_FileType;
	}

	void CErrHandler::SetFileTypeSz( const char* pszFileType )
	{
		sprintf_s( m_szTypeBuffer, m_kTypeBufferSize, pszFileType );
	}

	void CErrHandler::SetFileType( Assets::FileType eType )
	{
		m_FileType = eType;
	}

	void CErrHandler::SetError( Assets::Result eResult )
	{
		m_Err = eResult;
	}
}
