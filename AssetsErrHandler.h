#pragma once
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
		void SetFileType( FileType type );
		[[nodiscard]] FileType GetFileType();
		const char* GetLastFileTypeSz();
		const char* GetLastErrorSz();
		void Log( const char* pszError );
		void LogFmt( const char* fmt, ... );

	private:
		void SetFileTypeSz( const char* pszError );

	private:
		FileType m_FileType;
		ErrorCode m_Err;

		const size_t m_kErrBufferSize = 256;
		char* m_szErrBuffer;
		const size_t m_kTypeBufferSize = 256;
		char* m_szTypeBuffer;
	};
}

