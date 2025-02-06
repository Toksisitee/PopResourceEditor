#pragma once
#include <cstdint>
#include <vector>
#include <fstream>

namespace Assets
{
	enum class FileType : uint8_t
	{
		None = 0,
		Palette,
		Alpha,
		Sky,
		Sprite,
		Ghost,
		Fade,
		BigFade,
		Cliff,
		Disp,
		Blocks,
		Level,
		Unknown
	};

	enum class Result : uint8_t
	{
		OK = 0,
		FAIL,
		OK_LOAD,
		FAIL_LOAD,
		OK_PARSE,
		FAIL_PARSE,
		OK_EXPORT,
		FAIL_EXPORT,
		OK_GENERATE,
		FAIL_GENERATE,
	};

	template <typename T>
	struct Bank {
		uint32_t Entries;
		std::vector<std::pair<uint32_t, T>> Entry;
	};

	class CAsset
	{
	public:
		virtual         ~CAsset() {};

		virtual bool    Load( const char* pszFile ) = 0;
		virtual void    Create( const char* pszFile ) = 0;
		virtual void    Export() = 0;
	protected:
		char* m_pBuffer;
		uint32_t        m_nBufferLength;
	};

	template <typename T>
	class CAssetBank : public CAsset
	{
	public:
		virtual         ~CAssetBank() {};

		virtual bool    Load( const char* pszFile )
		{
			m_Bank.Entries = 0;
			m_Bank.Entry.clear();
			if ( m_pBuffer ) {
				delete[] m_pBuffer;
			}

			std::ifstream ifs( pszFile, std::ios::binary );
			if ( ifs.is_open() ) {
				ifs.seekg( 0 );
				m_nBufferLength = static_cast<uint32_t>(ifs.tellg());
				if ( m_nBufferLength > 0 ) {
					m_pBuffer = new char[m_nBufferLength];
					ifs.read( m_pBuffer, m_nBufferLength );
					ifs.close();

					if ( m_pBuffer != NULL ) {
						char* pBuffer = m_pBuffer;
						m_Bank.Entries = *(reinterpret_cast<uint32_t*>(pBuffer));
						if ( m_Bank.Entries == 0 ) {
							this->Clear();
							return false;
						}

						pBuffer += sizeof( uint32_t );
						for ( uint32_t i = 0; i < m_Bank.Entries; i++ ) {
							auto offset = *(reinterpret_cast<uint32_t*>(pBuffer));
							m_Bank.Entry.push_back( std::make_pair( offset, *(reinterpret_cast<T*>(&m_pBuffer[offset])) ) );
							pBuffer += sizeof( uint32_t );
						}

						return true;
					}
				}
				return false;
			}
		}
	protected:
		virtual void    Clear()
		{
			delete[] m_pBuffer;
			m_pBuffer = nullptr;
			m_Bank.Entry.clear();
			m_nBufferLength = 0;
		}
	private:
		struct Bank<T>  m_Bank;
		char*			m_pBuffer;
		uint32_t        m_nBufferLength;
	};

	const char* GetFileTypeSz( FileType eFileType );
	extern Result OpenWnd( const std::string& sFilePath, FileType eFileType );
}
