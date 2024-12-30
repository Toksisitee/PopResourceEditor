#include <fstream>
#include <assert.h>
#include <algorithm>
#include <cmath>

#include "EasyBMP/EasyBMP.h"

#include "spdlog\spdlog.h"
#include "Editor.h"
#include "Utils.h"

#include "Level.h"

namespace Assets
{
	using namespace Level;

	std::string CharToHexString( char c )
	{
		char buffer[2];
		sprintf_s( buffer, sizeof( buffer ), "%x", static_cast<unsigned char>(c) & 0xF );
		return std::string( 1, buffer[0] );
	}

	// TODO: Load starting positions
	Result CLevel::Load( std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Level );

		memset( &m_LevelData, 0, sizeof( LevelDataV2 ) );

		std::ifstream ifsHeader( sFilePath + ".hdr", std::ios::binary );
		if ( ifsHeader.is_open() ) {
			ifsHeader.seekg( 0 );
			ifsHeader.read( reinterpret_cast<char*>(&m_LevelHeader), sizeof( LevelHeaderV2 ) );
		}
		else return Result::FAIL_LOAD;

		std::ifstream ifs( sFilePath + ".dat", std::ios::binary );
		if ( ifs.is_open() ) {
			ifs.seekg( 0 );

			ifs.read( reinterpret_cast<char*>(&m_LevelData.GroundHeight), sizeof( m_LevelData.GroundHeight ) );
			ifs.read( reinterpret_cast<char*>(&m_LevelData.LandBlocks), sizeof( m_LevelData.LandBlocks ) );
			ifs.read( reinterpret_cast<char*>(&m_LevelData.LandOrients), sizeof( m_LevelData.LandOrients ) );
			ifs.read( reinterpret_cast<char*>(&m_LevelData.NoAccessSquares), sizeof( m_LevelData.NoAccessSquares ) );
			ifs.read( reinterpret_cast<char*>(&m_LevelData.psi), sizeof( PLAYERSAVEINFO ) * 4 );
			ifs.read( reinterpret_cast<char*>(&m_LevelData.ssi), sizeof( SUNLIGHTSAVEINFO ) );

			ifs.close();
		}
		else return Result::FAIL_LOAD;


		char szType;
		if ( m_LevelHeader.LevelType < 10 ) szType = '0' + m_LevelHeader.LevelType;
		else szType = 'a' + m_LevelHeader.LevelType - 10;

		char szBuffer[256];
		sprintf_s( szBuffer, sizeof( szBuffer ), "pal0-%c.dat", szType );
		auto sFileDataPath = Util::FileSystem::FormatPath( szBuffer );
		g_ErrHandler.HandleResult( m_Palette.Load( sFileDataPath ) );
		sprintf_s( szBuffer, sizeof( szBuffer ), "bigf0-%c.dat", szType );
		sFileDataPath = Util::FileSystem::FormatPath( szBuffer );
		g_ErrHandler.HandleResult( m_BigFade.Load( sFileDataPath ) );

		// TODO: this only has to be loaded once
		sFileDataPath = Util::FileSystem::FormatPath( "watdisp.dat" );
		g_ErrHandler.HandleResult( m_Disp.Load( sFileDataPath ) );

		return Result::OK_LOAD;
	}

	Result CLevel::Export( std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Level );

		BMP BMP;
		size_t uIndex = 0;
		auto pColorTable = m_Palette.GetColorTable();

		BMP.SetSize( k_uWidth, k_uHeight );
		BMP.SetBitDepth( 24 );

		for ( uint32_t y = 0; y < k_uHeight; y++ ) {
			for ( uint32_t x = 0; x < k_uWidth; x++ ) {
				BMP.SetPixel( x, y,
							  {
								  pColorTable[m_Data[uIndex]].B,
								  pColorTable[m_Data[uIndex]].G,
								  pColorTable[m_Data[uIndex]].R,
								  0
							  } );
				uIndex++;
			}
		}

		if ( !BMP.WriteToFile( (sFilePath).c_str() ) ) {
			return Result::FAIL_EXPORT;
		}

		return Result::OK_EXPORT;
	}

	int32_t CLevel::GetHeightAt( int32_t x, int32_t y )
	{
		while ( x < 0 ) x += k_uWidth;
		while ( y < 0 ) y += k_uHeight;
		while ( x >= k_uWidth ) x -= k_uWidth;
		while ( y >= k_uHeight ) y -= k_uHeight;
		return (int32_t)m_LevelData.GroundHeight[y * k_uWidth + x];
	}

	int32_t CLevel::GetGaussianSmoothedHeightAt( int32_t x, int32_t y, int32_t nIterations )
	{
		auto h = GetHeightAt( x, y );
		const int32_t k_nKernel[3][3] = {
			{1, 2, 1},
			{2, 4, 2},
			{1, 2, 1}
		};

		for ( int32_t i = 0; i < nIterations; ++i ) {
			int32_t nSum = 0;
			int32_t nWeightSum = 1;

			for ( int32_t oy = -1; oy <= 1; ++oy ) {
				for ( int32_t ox = -1; ox <= 1; ++ox ) {
					int32_t nx = x + ox;
					int32_t ny = y + oy;

					if ( nx >= 0 && nx < k_uWidth && ny >= 0 && ny < k_uHeight ) {
						nSum += GetHeightAt( nx, ny ) * k_nKernel[oy + 1][ox + 1];
						nWeightSum += k_nKernel[oy + 1][ox + 1];
					}
				}
			}

			if ( nWeightSum > 0 ) h = nSum / nWeightSum;
		}

		return h;
	}

	int32_t CLevel::GetThresholdFilteredHeightAt( int32_t x, int32_t y )
	{
		int32_t h, hl, hr, hu, hd, avg;

		if ( m_bGaussian ) {
			h = GetGaussianSmoothedHeightAt( x, y );
			hl = GetGaussianSmoothedHeightAt( x - 1, y );
			hr = GetGaussianSmoothedHeightAt( x + 1, y );
			hu = GetGaussianSmoothedHeightAt( x, y - 1 );
			hd = GetGaussianSmoothedHeightAt( x, y + 1 );
		}
		else {
			h = GetHeightAt( x, y );
			hl = GetHeightAt( x - 1, y );
			hr = GetHeightAt( x + 1, y );
			hu = GetHeightAt( x, y - 1 );
			hd = GetHeightAt( x, y + 1 );
		}

		avg = (hl + hr + hu + hd) >> 4;
		if ( std::abs( h - avg ) > 10 ) {
			return avg;
		}

		return h;
	}

	void CLevel::MapToDisp( uint8_t* pData, int32_t x, int32_t y )
	{
		uint8_t* pDisp = m_Disp.GetPtr();
		const float fFrequency = 0.1f;
		const float fAmplitude = 10.0f;

		int32_t fWave = static_cast<int32_t>(fAmplitude * std::sinf( fFrequency * (x + y) ));
		int32_t dispX = std::clamp( x + fWave, 0, static_cast<int32_t>(Disp::k_uWidth - 1) );
		int32_t dispY = std::clamp( y + fWave, 0, static_cast<int32_t>(Disp::k_uHeight - 1) );
		int32_t u = pDisp[dispY * Disp::k_uWidth + dispX];

		*pData = m_BigFade.GetColor( u );
	}

	void CLevel::MapToTextureColor( uint8_t* pData, int32_t x, int32_t y, uint32_t uHeight, uint16_t uCliff, float fLightX, float fLightY )
	{
		int32_t u = 64;
		int32_t v = uHeight + 140;
		if ( v > BigFade::k_uHeight ) v = BigFade::k_uHeight - 1;
		if ( v < 0 ) v = 0;
		v = v << 8;

		if ( m_bCalcLight ) {
			auto hl = GetThresholdFilteredHeightAt( x - 1, y );
			auto hr = GetThresholdFilteredHeightAt( x + 1, y );
			auto hu = GetThresholdFilteredHeightAt( x, y - 1 );
			auto hd = GetThresholdFilteredHeightAt( x, y + 1 );

			int32_t dx = hr - hl;
			int32_t dy = hd - hu;

			if ( (std::abs( dx ) > uCliff) || (std::abs( dy ) > uCliff) ) {
				u -= 16;
			}

			float fShadow = (dx * fLightX + dy * fLightY) * 0.5f;
			if ( fShadow < -16.0f ) fShadow = -16.0f;
			if ( fShadow > 16.0f )  fShadow = 16.0f;
			u += (int32_t)fShadow;

			int32_t avg = (hl + hr + hu + hd) / 4;
			int32_t ao = avg - uHeight;
			if ( ao > 0 ) {
				u -= (ao / 4);
			}
		}
		int32_t uv = u + v;

		if ( uv > BigFade::k_uSize ) {
			spdlog::error( "MapToTextureColor UV oob!" );
			return;
		}

		*pData = m_BigFade.GetColor( uv );
	}

	std::pair<int32_t, int32_t> CLevel::CalculateCentroid()
	{
		fVec2 fSin = { 0.0f, 0.0f };
		fVec2 fCos = { 0.0f, 0.0f };
		int64_t nTotalHeight = 0;

		for ( uint32_t y = 0; y < k_uHeight; y++ ) {
			for ( uint32_t x = 0; x < k_uWidth; x++ ) {
				auto h = GetThresholdFilteredHeightAt( x, y );
				if ( h > 0 ) {
					double dAngleX = 2.0 * EDITOR_PI * x / k_uWidth;
					double dAngleY = 2.0 * EDITOR_PI * y / k_uHeight;
					fSin.x += std::sin( dAngleX ) * h;
					fCos.x += std::cos( dAngleX ) * h;
					fSin.y += std::sin( dAngleY ) * h;
					fCos.y += std::cos( dAngleY ) * h;
					nTotalHeight += h;
				}
			}
		}

		if ( nTotalHeight == 0 ) {
			return { 0, 0 };
		}

		float fAvgAngleX = std::atan2( fSin.x, fCos.x );
		float fAvgAngleY = std::atan2( fSin.y, fCos.y );

		if ( fAvgAngleX < 0 ) fAvgAngleX += 2.0 * EDITOR_PI;
		if ( fAvgAngleY < 0 ) fAvgAngleY += 2.0 * EDITOR_PI;

		auto cx = static_cast<int32_t>(std::round( (fAvgAngleX * k_uWidth) / (2.0 * EDITOR_PI) ));
		auto cy = static_cast<int32_t>(std::round( (fAvgAngleY * k_uHeight) / (2.0 * EDITOR_PI) ));

		return { cx, cy };
	}


	Result CLevel::GeneratePreview( uint16_t uCliff, float fLightX, float fLightY, bool bWater )
	{
		g_ErrHandler.SetFileType( FileType::Level );
		DestroyTexture();
		auto* pData = &m_Data[0];

		int32_t tx, ty, sx, sy, ox, oy;
		auto [cx, cy] = CalculateCentroid();

		tx = k_uWidth / 2;
		ty = k_uHeight / 2;
		sx = (tx - cx) % k_uWidth;
		sy = (ty - cy) % k_uHeight;

		for ( uint32_t y = 0; y < k_uHeight; y++ ) {
			for ( uint32_t x = 0; x < k_uWidth; x++, pData++ ) {
				ox = (x + sx) % k_uWidth;
				oy = (y + sy) % k_uHeight;

				auto uHeight = GetHeightAt( ox, oy );
				MapToTextureColor( pData, ox, oy, uHeight, uCliff, fLightX, fLightY );

				if ( bWater && uHeight == 0 ) {
					MapToDisp( pData, x, y );
				}
			}
		}
		return Result::OK_GENERATE;
	}

	bool CLevel::CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice )
	{
		m_pTexture = new CTexture2D( pD3DDevice, k_uWidth, k_uHeight, &m_Data[0], &m_Palette );
		return true;
	}
}