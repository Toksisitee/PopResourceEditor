#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "spdlog/spdlog.h"
#include "imgui.h"

#include "Editor.h"
#include "Assets.h"
#include "AssetsErrHandler.h"

namespace Assets
{
	CErrHandler::CErrHandler()
	{
		m_szErrBuffer = (char*)malloc( m_kErrBufferSize );
		m_szTypeBuffer = (char*)malloc( m_kTypeBufferSize );
		m_FileType = FileType::None;
		m_Err = ErrorCode::OK;
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
			case Assets::ErrorCode::LOAD:
			case Assets::ErrorCode::PARSE:
				spdlog::critical( m_szErrBuffer );
				break;
			case Assets::ErrorCode::EXPORT:
				spdlog::error( m_szErrBuffer );
				break;
			case Assets::ErrorCode::OK:
			default:
				spdlog::info( m_szErrBuffer );
				break;
		}
	}

	void CErrHandler::HandleResult( Assets::ErrorCode code, std::optional<std::string> sErrorMsg )
	{
		assert( m_FileType != FileType::None && "CErrHandler: File type not set" );
		SetError( code );

		char szErrCode[128];
		switch ( m_Err ) {
			case Assets::ErrorCode::OK:
				sprintf_s( szErrCode, sizeof( szErrCode ), "OK" );
				break;
			case Assets::ErrorCode::LOAD:
				sprintf_s( szErrCode, sizeof( szErrCode ), "Failed to load file" );
				break;
			case Assets::ErrorCode::EXPORT:
				sprintf_s( szErrCode, sizeof( szErrCode ), "Failed to export file" );
				break;
			case Assets::ErrorCode::PARSE:
				sprintf_s( szErrCode, sizeof( szErrCode ), "Failed to parse file" );
				break;
			default:
				sprintf_s( szErrCode, sizeof( szErrCode ), "Unknown error code" );
				break;
		}

		if ( sErrorMsg.has_value() ) {
			sprintf_s( m_szErrBuffer, m_kErrBufferSize, "[%s]: %s", GetLastFileTypeSz(), sErrorMsg.value().c_str() );
		}
		else {
			sprintf_s( m_szErrBuffer, m_kErrBufferSize, "[%s]: %s", GetLastFileTypeSz(), szErrCode);
		}

		Flush();
	}

	const char* CErrHandler::GetLastFileTypeSz()
	{
		switch ( m_FileType ) {
			case Assets::FileType::Palette:
				return "Palette";
			case Assets::FileType::Alpha:
				return "Alpha";
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

	void CErrHandler::SetError( Assets::ErrorCode code )
	{
		m_Err = code;
	}
}