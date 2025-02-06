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

		void Log( const char* psMsg );
		void Log( Log::Level eLevel, const char* pszMsg );
		void LogFmt( const char* pFmt, ... );
		void LogFmt( Log::Level eLevel, const char* pFmt, ... );
		void SetFileType( FileType type );

		Result HandleResult( Result code, std::optional<std::string> sErrorMsg = std::nullopt );

	private:
		void EmitLogBuffer();
		void EmitLog( const char* pszMsg, Log::Level eLevel );


		void SetFileTypeSz( const char* pszFileType );
		void SetError( Result code );
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
