/*
Copyright (c) 2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#pragma once
#include <optional>

#include "Assets.h"

namespace Assets
{
	namespace Log
	{
		enum class Level
		{
			TRC = 0,
			DBG,
			INF,
			WRN,
			ERR,
			CRT,
		};
	}

	class CErrHandler
	{
	public:
		//static CErrHandler& Instance()
		//{
		//	static CErrHandler sHandler;
		//	return sHandler;
		//}

		CErrHandler();
		~CErrHandler();

		void Log( const char* pszMsg );
		void Log( Log::Level eLevel, const char* pszMsg );
		void LogFmt( const char* pszFmt, ... );
		void LogFmt( Log::Level eLevel, const char* pszFmt, ... );
		void SetFileType( FileType eType );

		Result HandleResult( Result eResult, std::optional<std::string> sErrorMsg = std::nullopt );

	private:
		void EmitLogBuffer();
		void EmitLog( const char* pszMsg, Log::Level eLevel );


		void SetFileTypeSz( const char* pszFileType );
		void SetError( Result eResult );
		[[nodiscard]] FileType GetFileType();
		const char* GetLastFileTypeSz();
		const char* GetLastErrorSz();

	private:
		FileType m_FileType;
		Result m_Err;

		const size_t m_kErrBufferSize = 256;
		char* m_szErrBuffer;
		const size_t m_kTypeBufferSize = 256;
		char* m_szTypeBuffer;
	};
}

extern Assets::CErrHandler g_ErrHandler;

