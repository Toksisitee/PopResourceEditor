#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "spdlog/spdlog.h"
#include "imgui.h"

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

	void CErrHandler::LogFmt( const char* fmt, ... )
	{
		char* pszBuffer = (char*)malloc( m_kErrBufferSize );
		assert( pszBuffer && "CErrHandler: Failed to allocate memory" );

		if ( pszBuffer ) {
			va_list argptr;
			va_start( argptr, fmt );
			vsprintf_s( pszBuffer, m_kErrBufferSize, fmt, argptr );
			va_end( argptr );
			Log( pszBuffer );
			free( pszBuffer );
		}
	}


	void CErrHandler::Log( const char* pszError )
	{
		assert( m_FileType != FileType::None && "CErrHandler: File type not set" );
		sprintf_s( m_szErrBuffer, m_kErrBufferSize, "[%s]: %s", GetLastFileTypeSz(), pszError );
		Flush();
	}

	void CErrHandler::Flush()
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

	Result CErrHandler::HandleResult( Assets::Result code, std::optional<std::string> sErrorMsg )
	{
		assert( m_FileType != FileType::None && "CErrHandler: File type not set" );
		SetError( code );

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
			default:
				sprintf_s( szErrCode, sizeof( szErrCode ), "Unknown error code." );
				break;
		}

		if ( sErrorMsg.has_value() ) {
			sprintf_s( m_szErrBuffer, m_kErrBufferSize, "[%s]: %s", GetLastFileTypeSz(), sErrorMsg.value().c_str() );
		}
		else {
			sprintf_s( m_szErrBuffer, m_kErrBufferSize, "[%s]: %s", GetLastFileTypeSz(), szErrCode);
		}

		Flush();

		return code;
	}

	const char* CErrHandler::GetLastFileTypeSz()
	{
		switch ( m_FileType ) {
			case Assets::FileType::Palette:
				return "Palette";
			case Assets::FileType::Alpha:
				return "Alpha";
			case Assets::FileType::Sky:
				return "Sky";
			case Assets::FileType::Sprite:
				return "Sprite";
			case Assets::FileType::Ghost:
				return "Ghost";
			case Assets::FileType::Fade:
				return "Fade";
			case Assets::FileType::BigFade:
				return "BigFade";
			case Assets::FileType::Disp:
				return "Disp";
		}

		return "Unknown";
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

	void CErrHandler::SetFileType( Assets::FileType type )
	{
		m_FileType = type;
	}

	void CErrHandler::SetError( Assets::Result code )
	{
		m_Err = code;
	}
}