#pragma once
//#include <vector>

#include "Palette.h"

class CTexture2D;
typedef struct IDirect3DDevice9* LPDIRECT3DDEVICE9, * PDIRECT3DDEVICE9;
extern inline void SafeDestroyTexture( CTexture2D*& pTexture );

namespace Assets
{
	//template <typename T>
	//struct Bank {
	//	uint32_t Entries;
	//	std::vector<std::pair<uint32_t, T>> Entry;
	//};

	class CAsset
	{
	public:
		~CAsset() { SafeDestroyTexture( m_pTexture ); }

		virtual Result	LoadBin( const std::string& sFilePath ) = 0;
		virtual Result	LoadImg( const std::string& sFilePath ) = 0;
		virtual Result	ExportImg( const std::string& sFilePath ) = 0;
		virtual Result	ExportBin( const std::string& sFilePath ) = 0;
		virtual bool	CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice ) = 0;
		inline virtual void* GetPtr() = 0;

		inline void DestroyTexture()
		{
			SafeDestroyTexture( m_pTexture );
		}
		[[nodiscard]] inline CTexture2D* GetTexture()
		{
			return m_pTexture;
		}
		[[nodiscard]] inline CPalette* GetPalette()
		{
			return &m_Palette;
		}

	protected:
		CPalette m_Palette;
		CTexture2D* m_pTexture;
	};

#if 0
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
		char* m_pBuffer;
		uint32_t        m_nBufferLength;
	};
#endif

	const char* GetFileTypeSz( FileType eFileType );
	extern Result OpenWnd( const std::string& sFilePath, FileType eFileType );
	extern Result QuickLoad( void* pAsset, const std::string& sFilePath, FileType eFileType );
	extern CTexture2D* LoadTexture( const std::string& sFilePath, const Assets::FileType eType );
}
