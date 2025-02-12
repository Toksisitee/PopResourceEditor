#pragma once
#include "Assets.h"
#include "BigFade.h"
#include "Disp.h"

namespace Assets
{
	namespace Level
	{
		constexpr uint8_t	k_uWidth = 128;
		constexpr uint8_t	k_uHeight = 128;
		constexpr uint32_t	k_uMapSize = k_uWidth * k_uHeight;
		constexpr uint16_t	k_uMaxMarkers = 256;
		namespace V2
		{
			constexpr uint16_t  k_uMaxThings = 2000;
			constexpr uint8_t	k_uMaxSaveName = 32;
			constexpr uint8_t	k_uMaxScripts = 10;
		}
	}

#pragma pack(push, 1)
	// Taken from: https://github.com/Toksisitee/ALACNPopWorldEditor/blob/1adcc222c6f35cdc76429cbb9c536b6410359df6/pop.h
	struct PLAYERTHINGS
	{
		uint32_t SpellsAvailable;
		uint32_t BuildingsAvailable;
		uint32_t BuildingsAvailableLevel;
		uint32_t BuildingsAvailableOnce;
		union {
			uint32_t SpellsAvailableLevel;
			uint32_t SpellsNotCharging;
		};
		uint8_t SpellsAvailableOnce[32];
		uint16_t VehiclesAvailable;
		uint8_t TrainingManaOff;
		uint8_t Flags;
	};

	struct ACCESSSAVEINFO
	{
		uint8_t Model;
		uint8_t Type;
		uint8_t Rights;
	};

	struct SUNLIGHTSAVEINFO
	{
		uint8_t ShadeStart;
		uint8_t ShadeRange;
		uint8_t Inclination;
	};

	struct PLAYERSAVEINFO
	{
		int16_t StartPosX;
		int16_t StartPosY;
		int32_t Future1;
		int32_t Future2;
		int32_t Future3;
	};

	struct BUILDINGSAVE
	{
		int32_t Angle;
	};

	struct SCENERYSAVE
	{
		uint8_t	PortalStatus;
		uint8_t PortalLevel;
		uint8_t PortalType;
		int16_t Angle;
		uint8_t UserId;
		int16_t IslandAlt;
		uint8_t IslandNum;
		uint8_t BridgeNum;
	};

	struct GENERALSAVE
	{
		uint8_t DiscoveryType;
		uint8_t DiscoveryModel;
		uint8_t AvailabilityType;
		uint8_t TriggerType;
		int32_t ManaAmt;
	};

	struct TRIGGERSAVE
	{
		uint8_t	TriggerType;
		uint8_t CellRadius;
		uint8_t RandomValue;
		int8_t NumOccurences;
		uint16_t TriggerCount;
		uint16_t ThingIdxs[10];
		int16_t PrayTime;
		uint8_t StartInactive;
		uint8_t CreatePlayerOwned;
		int16_t InactiveTime;
	};

	struct THINGSAVE
	{
		uint8_t Model;
		uint8_t Type;
		int8_t Owner;
		uint16_t PosX;
		uint16_t PosZ;
		union
		{
			BUILDINGSAVE Building;
			SCENERYSAVE Scenery;
			GENERALSAVE General;
			TRIGGERSAVE Trigger;
			uint32_t Bluff[12];
		};
	};

	struct LevelDataV2
	{
		uint16_t GroundHeight[Level::k_uMapSize];
		uint8_t LandBlocks[Level::k_uMapSize];
		uint8_t LandOrients[Level::k_uMapSize];
		uint8_t NoAccessSquares[Level::k_uMapSize];
		PLAYERSAVEINFO psi[4];
		SUNLIGHTSAVEINFO ssi;
		THINGSAVE Things[Level::V2::k_uMaxThings];
		ACCESSSAVEINFO asi[50];
	};

	struct LEVELVERSION
	{
		int32_t	VersionNum;
		int8_t	CreatedBy[Level::V2::k_uMaxSaveName];
		int8_t	CreatedOn[28];
		int32_t	CheckSum;
	};

	struct LevelHeaderV2
	{
		PLAYERTHINGS DefaultThings;
		int8_t Name[Level::V2::k_uMaxSaveName];
		uint8_t NumPlayers;
		uint8_t ComputerPlayerIndex[3];
		uint8_t DefaultAllies[4];
		uint8_t LevelType;
		uint8_t ObjectsBankNum;
		uint8_t LevelFlags;
		uint8_t Pad[1];
		uint16_t Markers[Level::k_uMaxMarkers];
		uint16_t StartPos;
		uint16_t StartAngle;
	};
#pragma pack(pop)
	class CLevel : public CAsset
	{
	public:
		// ====== Virtual Overrides  ======
		Result	LoadBin( const std::string& sFilePath ) override;
		Result	LoadImg( const std::string& sFilePath ) override { return Result::FAIL_LOAD; }
		Result	ExportImg( const std::string& sFilePath ) override;
		Result  ExportBin( const std::string& sFilePath ) override { return Result::FAIL_EXPORT; }
		bool	CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice ) override;
		inline void* GetPtr() override { return static_cast<void*>(&m_Data); }
		// ================================

		Result	GeneratePreview( uint16_t uCliff, float fLightX, float fLightY, bool bWater );

		[[nodiscard]] inline CBigFade* GetBigFade()
		{
			return &m_BigFade;
		}
		[[nodiscard]] inline CDisp* GetDisp()
		{
			return &m_Disp;
		}
		[[nodiscard]] inline LevelHeaderV2* GetHeader()
		{
			return &m_LevelHeader;
		}
	public:
		bool m_bCalcLight = true;
		bool m_bGaussian = false;
	protected:
		int32_t GetGaussianSmoothedHeightAt( int32_t x, int32_t y, int32_t iterations = 12 );
		int32_t GetThresholdFilteredHeightAt( int32_t x, int32_t y );
		int32_t GetHeightAt( int32_t x, int32_t y );
		std::pair<int32_t, int32_t> CalculateCentroid();
		void MapToTextureColor( uint8_t* pData, int32_t x, int32_t y, uint32_t uHeight, uint16_t uCliff, float fLightX, float fLightY );
		void MapToDisp( uint8_t* pData, int32_t x, int32_t y );
	private:
		uint8_t m_Data[Level::k_uMapSize];
		LevelHeaderV2 m_LevelHeader;
		LevelDataV2 m_LevelData;
		CBigFade m_BigFade;
		CDisp m_Disp;
	};
}