#include <assert.h>
#include <d3d9.h>

#include "imgui_impl_dx9.h"
#include "Texture.h"

#include "ImEditor.h"

namespace ImEditor
{
	namespace Constants
	{
		namespace Step
		{
			const int8_t	s8_one = 1;
			const uint8_t	u8_one = 1;
			const int16_t	s16_one = 1;
			const uint16_t	u16_one = 1;
			const int32_t	s32_one = 1;
			const uint32_t	u32_one = 1;
			const float		f_one = 1.0f;
			const double	d_one = 1.0;
		}

		namespace Limits
		{
			const int8_t	s8_min = -128, s8_max = 127;
			const uint8_t	u8_min = 0, u8_max = 255;
			const int16_t	s16_min = SHRT_MIN, s16_max = SHRT_MAX;
			const uint16_t	u16_min = 0, u16_max = USHRT_MAX;
			const int32_t	s32_min = INT_MIN, s32_max = INT_MAX;
			const uint32_t	u32_min = 0, u32_max = UINT_MAX;
			const int64_t	s64_min = LLONG_MIN, s64_max = LLONG_MAX;
			const uint64_t	u64_min = 0, u64_max = ULLONG_MAX;
			const float		f_min = FLT_MIN, f_max = +10000000000.0f;
			const double	d_min = DBL_MIN, d_max = DBL_MAX;
		}
	}

	void SetPointFiltering( LPDIRECT3DDEVICE9 pD3DDevice )
	{
		assert( pD3DDevice != nullptr && "pD3DDevice is nullptr" );

		static auto PointFiltering = []( const ImDrawList* parentList, const ImDrawCmd* cmd ) {
			auto device = static_cast<LPDIRECT3DDEVICE9>(cmd->UserCallbackData);
			device->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
			device->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
			device->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );
		};

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->AddCallback( PointFiltering, pD3DDevice );

	}

	void ResetRenderState()
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->AddCallback( ImDrawCallback_ResetRenderState, nullptr );
	}

	void RenderTexture( CTexture2D* pTexture, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col )
	{
		assert( pTexture != nullptr && "pTexture in RenderTexture is nullptr" );
		if ( size.x == 0 && size.y == 0 )
			ImGui::Image( pTexture->GetTexture(), ImVec2( static_cast<float>(pTexture->GetWidth()), static_cast<float>(pTexture->GetHeight()) ), uv0, uv1, tint_col, border_col );
		else
			ImGui::Image( pTexture->GetTexture(), size, uv0, uv1, tint_col, border_col );
	}

	bool ImageButton( CTexture2D* pTexture, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col )
	{
		assert( pTexture != nullptr && "pTexture in ImageButton is nullptr" );

		ImVec2 imageSize = size;
		if ( size.x == 0 && size.y == 0 ) {
			imageSize.x = static_cast<float>(pTexture->GetWidth());
			imageSize.y = static_cast<float>(pTexture->GetHeight());
		}

		return ImGui::ImageButton( pTexture->GetTexture(), imageSize, uv0, uv1, frame_padding, bg_col, tint_col );
	}

	void RenderModifiablePalette( void* pPalette, size_t uMin, size_t uMax, std::unordered_set<uint8_t>* psIndicies )
	{
		Assets::CPalette* pPal = static_cast<Assets::CPalette*>(pPalette);
		Color* pColorTable = pPal->GetColorTable();
		char szColorLabel[8];
		const int k_iColorEditFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoBorder;
		std::vector<std::pair<ImVec2, ImVec2>> vecCellPos;

		if ( psIndicies && ImGui::Button( "Clear Selected" ) ) {
			psIndicies->clear();
		}

		ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 5.0f, 5.0f ) );
		ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 6.0f, 6.0f ) );
		for ( auto uIndex = uMin; uIndex < uMax; uIndex++ ) {
			if ( uIndex % 16 != 0 ) {
				ImGui::SameLine();
			}

			Color& col = pColorTable[uIndex];
			float color[3] = { col.r / 255.0f, col.g / 255.0f, col.b / 255.0f };
			sprintf_s( szColorLabel, sizeof( szColorLabel ), "##%i", uIndex );
			if ( ImGui::ColorEdit3( szColorLabel, color, k_iColorEditFlags ) ) {
				col.r = static_cast<uint8_t>(color[0] * 255);
				col.g = static_cast<uint8_t>(color[1] * 255);
				col.b = static_cast<uint8_t>(color[2] * 255);
			}

			vecCellPos.push_back( { ImGui::GetItemRectMin(), ImGui::GetItemRectMax() } );
			if ( psIndicies && psIndicies->count( uIndex ) ) {
				ImDrawList* drawList = ImGui::GetWindowDrawList();
				drawList->AddRect( vecCellPos.back().first, vecCellPos.back().second, IM_COL32( 255, 0, 0, 180 ), 0.0f, 0, 2.0f );
			}

			if ( ImGui::IsItemDeactivatedAfterEdit() ) {
				pPal->SetChanged( true );
			}

			if ( ImGui::IsItemHovered() ) {
				ImGui::BeginTooltip();
				ImGui::Text( "Index: %d", uIndex );
				ImGui::EndTooltip();
			}
		}
		ImGui::PopStyleVar( 2 );

		// Drag feature
		{
			if ( !psIndicies ) {
				return;
			}

			static bool bDragging = false;
			static ImVec2 vecDragStart, vecDragEnd;
			const float fTolerance = 0.3f;

			ImVec2 vecMousePos = ImGui::GetMousePos();
			ImDrawList* pDrawList = ImGui::GetWindowDrawList();

			if ( ImGui::IsMouseClicked( ImGuiMouseButton_Right ) ) {
				bDragging = true;
				vecDragStart = vecMousePos;
				vecDragEnd = vecMousePos;
			}

			if ( bDragging && ImGui::IsMouseDragging( ImGuiMouseButton_Right ) ) {
				vecDragEnd = vecMousePos;
				pDrawList->AddRectFilled( vecDragStart, vecDragEnd, IM_COL32( 0, 120, 255, 50 ) );
				pDrawList->AddRect( vecDragStart, vecDragEnd, IM_COL32( 0, 120, 255, 255 ), 0.0f, 0, 2.0f );
			}

			if ( bDragging && ImGui::IsMouseReleased( ImGuiMouseButton_Right ) ) {
				bDragging = false;

				float fDist = std::hypot( vecDragEnd.x - vecDragStart.x, vecDragEnd.y - vecDragStart.y );
				if ( fDist < 15.0f ) {
					return;
				}

				ImVec2 vecSelMin( std::min( vecDragStart.x, vecDragEnd.x ), std::min( vecDragStart.y, vecDragEnd.y ) );
				ImVec2 vecSelMax( std::max( vecDragStart.x, vecDragEnd.x ), std::max( vecDragStart.y, vecDragEnd.y ) );

				for ( size_t i = 0; i < vecCellPos.size(); i++ ) {
					ImVec2 vecRectMin = vecCellPos[i].first;
					ImVec2 vecRectMax = vecCellPos[i].second;
					float fOverlapX = std::max( 0.0f, std::min( vecSelMax.x, vecRectMax.x ) - std::max( vecSelMin.x, vecRectMin.x ) );
					float fOverlapY = std::max( 0.0f, std::min( vecSelMax.y, vecRectMax.y ) - std::max( vecSelMin.y, vecRectMin.y ) );
					float fCellArea = (vecRectMax.x - vecRectMin.x) * (vecRectMax.y - vecRectMin.y);
					float fOverlapArea = fOverlapX * fOverlapY;
					if ( fOverlapArea / fCellArea >= fTolerance ) {
						if ( psIndicies->count( uMin + i ) ) {
							psIndicies->erase( uMin + i );
						}
						else {
							psIndicies->insert( uMin + i );
						}
					}
				}
			}
		}

	}
};