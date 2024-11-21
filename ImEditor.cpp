#include "assert.h"

#include "ImEditor.h"
#include "imgui_impl_dx9.h"

namespace ImEditor
{
	const int8_t k_s8_one = 1;
	const int16_t k_s16_one = 1;
	const int32_t k_s32_one = 1;
	const uint8_t k_u8_one = 1;
	const uint16_t k_u16_one = 1;
	const uint32_t k_u32_one = 1;
	const float k_f_one = 1.0f;
	const double k_d_one = 1.0;

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
		ImGui::Image( pTexture->GetTexture(), size, uv0, uv1, tint_col, border_col );
	}

};