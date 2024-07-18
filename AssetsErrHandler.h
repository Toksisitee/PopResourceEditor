#pragma once
#include <optional>

namespace Assets
{
	class CErrHandler
	{
	public:
		static CErrHandler& Instance()
		{
			static CErrHandler sHandler;
			return sHandler;
		}

		CErrHandler();
		~CErrHandler();
		
		void Log( const char* pszError );
		void LogFmt( const char* fmt, ... );
		void SetFileType( FileType type );

		void HandleResult( ErrorCode code, std::optional<std::string> sErrorMsg = std::nullopt );

	private:
		void Flush();

		void SetFileTypeSz( const char* pszError );
		void SetError( ErrorCode code );
		[[nodiscard]] FileType GetFileType();
		const char* GetLastFileTypeSz();
		const char* GetLastErrorSz();

	private:
		FileType m_FileType;
		ErrorCode m_Err;

		const size_t m_kErrBufferSize = 256;
		char* m_szErrBuffer;
		const size_t m_kTypeBufferSize = 256;
		char* m_szTypeBuffer;
	};
}

