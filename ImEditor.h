#pragma once
#include "imgui.h"

#include "Editor.h"
#include "Texture.h"

namespace ImEditor
{
	extern const int8_t k_s8_one;
	extern const int16_t k_s16_one;
	extern const int32_t k_s32_one;
	extern const uint8_t k_u8_one;
	extern const uint16_t k_u16_one;
	extern const uint32_t k_u32_one;
	extern const float k_f_one;
	extern const double k_d_one;

	template<typename PT>
	const void* GetImGuiStepOne( PT p_data )
	{
		using T = std::remove_pointer_t<PT>;

		if constexpr ( std::is_same_v<T, int8_t> ) {
			return &k_s8_one;
		}
		if constexpr ( std::is_same_v<T, uint8_t> ) {
			return &k_u8_one;
		}

		if constexpr ( std::is_same_v<T, int16_t> ) {
			return &k_s16_one;
		}
		if constexpr ( std::is_same_v<T, uint16_t> ) {
			return &k_u16_one;
		}

		if constexpr ( std::is_same_v<T, int32_t> ) {
			return &k_s32_one;
		}
		if constexpr ( std::is_same_v<T, uint32_t> ) {
			return &k_u32_one;
		}

		if constexpr ( std::is_same_v<T, float> ) {
			return &k_f_one;
		}
		if constexpr ( std::is_same_v<T, double> ) {
			return &k_d_one;
		}

		assert( false && "Invalid data type in InputScalar" );
		return NULL;
	}

	template<typename PT>
	ImGuiDataType GetImGuiDataType( PT p_data )
	{
		using T = std::remove_pointer_t<PT>;

		if constexpr ( std::is_same_v<T, int8_t> ) {
			return ImGuiDataType_S8;
		}
		if constexpr ( std::is_same_v<T, uint8_t> ) {
			return ImGuiDataType_U8;
		}

		if constexpr ( std::is_same_v<T, int16_t> ) {
			return ImGuiDataType_S16;
		}
		if constexpr ( std::is_same_v<T, uint16_t> ) {
			return ImGuiDataType_U16;
		}

		if constexpr ( std::is_same_v<T, int32_t> ) {
			return ImGuiDataType_S32;
		}
		if constexpr ( std::is_same_v<T, uint32_t> ) {
			return ImGuiDataType_U32;
		}

		if constexpr ( std::is_same_v<T, float> ) {
			return ImGuiDataType_Float;
		}
		if constexpr ( std::is_same_v<T, double> ) {
			return ImGuiDataType_Double;
		}

		assert( false && "Invalid data type in InputScalar" );
		return ImGuiDataType_COUNT;
	}

	template<typename PT>
	const char* GetImGuiFormatSpec( PT p_data )
	{
		using T = std::remove_pointer_t<PT>;

		if constexpr ( std::is_same_v<T, int8_t> ) {
			return "%d";
		}
		if constexpr ( std::is_same_v<T, uint8_t> ) {
			return "%u";
		}

		if constexpr ( std::is_same_v<T, int16_t> ) {
			return "%d";
		}
		if constexpr ( std::is_same_v<T, uint16_t> ) {
			return "%u";
		}

		if constexpr ( std::is_same_v<T, int32_t> ) {
			return "%d";
		}
		if constexpr ( std::is_same_v<T, uint32_t> ) {
			return "%u";
		}

		if constexpr ( std::is_same_v<T, float> ) {
			return "%f";
		}
		if constexpr ( std::is_same_v<T, double> ) {
			return "%lf";
		}

		assert( false && "Invalid data type in GetImGuiFormatSpec" );
		return "%u";
	}

	template<typename T>
	bool InputScalar( const char* label, T* p_data, const void* p_step = NULL, ImGuiInputTextFlags flags = 0 )
	{
		//if ( *p_data < v_min ) *p_data = v_min;
		//else if ( *p_data > v_max ) *p_data = v_max;

		auto eDataType = GetImGuiDataType( p_data );
		auto pStepOne = GetImGuiStepOne( p_data );
		return ImGui::InputScalar( label, eDataType, p_data, p_step ? (T*)pStepOne : NULL, NULL, GetImGuiFormatSpec( p_data ), flags );
	}


	extern void SetPointFiltering( LPDIRECT3DDEVICE9 pD3DDevice );
	extern void ResetRenderState();
	extern void RenderTexture( CTexture2D* pTexture, const ImVec2& size = ImVec2( 0, 0 ), const ImVec2& uv0 = ImVec2( 0, 0 ), const ImVec2& uv1 = ImVec2( 1, 1 ), const ImVec4& tint_col = ImVec4( 1, 1, 1, 1 ), const ImVec4& border_col = ImVec4( 0, 0, 0, 0 ) );
	extern bool ImageButton( CTexture2D* pTexture, const ImVec2& size = ImVec2( 0, 0 ), const ImVec2& uv0 = ImVec2( 0, 0 ), const ImVec2& uv1 = ImVec2( 1, 1 ), int frame_padding = -1, const ImVec4& bg_col = ImVec4( 0, 0, 0, 0 ), const ImVec4& tint_col = ImVec4( 1, 1, 1, 1 ) );
};