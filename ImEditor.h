/*
Copyright (c) 2024-2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#pragma once
#include <array>
#include <utility>
#include <unordered_set>

#include "imgui.h"
#include "Editor.h"

class CTexture2D;
typedef struct IDirect3DDevice9* LPDIRECT3DDEVICE9, * PDIRECT3DDEVICE9;

namespace ImEditor
{
	namespace Constants
	{
		namespace Step
		{
			extern const int8_t		s8_one;
			extern const int16_t	s16_one;
			extern const int32_t	s32_one;
			extern const uint8_t	u8_one;
			extern const uint16_t	u16_one;
			extern const uint32_t	u32_one;
			extern const float		f_one;
			extern const double		d_one;
		}

		namespace Limits
		{
			extern const int8_t		s8_min, s8_max;
			extern const uint8_t	u8_min, u8_max;
			extern const int16_t	s16_min, s16_max;
			extern const uint16_t	u16_min, u16_max;
			extern const int32_t	s32_min, s32_max;
			extern const uint32_t	u32_min, u32_max;
			extern const int64_t	s64_min, s64_max;
			extern const uint64_t	u64_min, u64_max;
			extern const float		f_min, f_max;
			extern const double		d_min, d_max;
		}
	}

	template<typename PT>
	const void* GetImGuiLimits( PT p_data, bool min )
	{
		using T = std::remove_pointer_t<PT>;

		constexpr static std::array<std::pair<const void*, const void*>, 10> limits = { {
			{&Constants::Limits::s8_min, &Constants::Limits::s8_max},
			{&Constants::Limits::u8_min, &Constants::Limits::u8_max},
			{&Constants::Limits::s16_min, &Constants::Limits::s16_max},
			{&Constants::Limits::u16_min, &Constants::Limits::u16_max},
			{&Constants::Limits::s32_min, &Constants::Limits::s32_max},
			{&Constants::Limits::u32_min, &Constants::Limits::u32_max},
			{&Constants::Limits::s64_min, &Constants::Limits::s64_max},
			{&Constants::Limits::u64_min, &Constants::Limits::u64_max},
			{&Constants::Limits::f_min, &Constants::Limits::f_max},
			{&Constants::Limits::d_min, &Constants::Limits::d_max},
		} };

		if constexpr ( std::is_same_v<T, int8_t> ) {
			return min ? limits[0].first : limits[0].second;
		}
		else if constexpr ( std::is_same_v<T, uint8_t> ) {
			return min ? limits[1].first : limits[1].second;
		}
		else if constexpr ( std::is_same_v<T, int16_t> ) {
			return min ? limits[2].first : limits[2].second;
		}
		else if constexpr ( std::is_same_v<T, uint16_t> ) {
			return min ? limits[3].first : limits[3].second;
		}
		else if constexpr ( std::is_same_v<T, int32_t> ) {
			return min ? limits[4].first : limits[4].second;
		}
		else if constexpr ( std::is_same_v<T, uint32_t> ) {
			return min ? limits[5].first : limits[5].second;
		}
		else if constexpr ( std::is_same_v<T, int64_t> ) {
			return min ? limits[6].first : limits[6].second;
		}
		else if constexpr ( std::is_same_v<T, uint64_t> ) {
			return min ? limits[7].first : limits[7].second;
		}
		else if constexpr ( std::is_same_v<T, float> ) {
			return min ? limits[8].first : limits[8].second;
		}
		else if constexpr ( std::is_same_v<T, double> ) {
			return min ? limits[9].first : limits[9].second;
		}

		assert( false && "Invalid data type in GetImGuiLimits" );
		return 0;
	}

	template<typename PT>
	const void* GetImGuiStepOne( PT p_data )
	{
		using T = std::remove_pointer_t<PT>;

		if constexpr ( std::is_same_v<T, int8_t> ) {
			return &Constants::Step::s8_one;
		}
		if constexpr ( std::is_same_v<T, uint8_t> ) {
			return &Constants::Step::u8_one;
		}

		if constexpr ( std::is_same_v<T, int16_t> ) {
			return &Constants::Step::s16_one;
		}
		if constexpr ( std::is_same_v<T, uint16_t> ) {
			return &Constants::Step::u16_one;
		}

		if constexpr ( std::is_same_v<T, int32_t> ) {
			return &Constants::Step::s32_one;
		}
		if constexpr ( std::is_same_v<T, uint32_t> ) {
			return &Constants::Step::u32_one;
		}

		if constexpr ( std::is_same_v<T, float> ) {
			return &Constants::Step::f_one;
		}
		if constexpr ( std::is_same_v<T, double> ) {
			return &Constants::Step::d_one;
		}

		assert( false && "Invalid data type in GetImGuiStepOne" );
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

		assert( false && "Invalid data type in GetImGuiDataType" );
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
		auto eDataType = GetImGuiDataType( p_data );
		return ImGui::InputScalar( label, eDataType, p_data, p_step == NULL ? (T*)GetImGuiStepOne( p_data ) : p_step, NULL, GetImGuiFormatSpec( p_data ), flags );
	}

	template<typename T>
	bool SliderScalar( const char* label, T* p_data, const void* p_min = NULL, const void* p_max = NULL, const char* format = NULL, ImGuiSliderFlags flags = 0 )
	{
		auto eDataType = GetImGuiDataType( p_data );
		return ImGui::SliderScalar( label, eDataType, p_data, 
									p_min == NULL ? GetImGuiLimits( p_data, true ) : p_min,
									p_max == NULL ? GetImGuiLimits( p_data, false ) : p_max,
									GetImGuiFormatSpec( p_data ), flags );
	}

	extern void SetPointFiltering( LPDIRECT3DDEVICE9 pD3DDevice );
	extern void ResetRenderState();
	extern void RenderTexture( CTexture2D* pTexture, const ImVec2& size = ImVec2( 0, 0 ), const ImVec2& uv0 = ImVec2( 0, 0 ), const ImVec2& uv1 = ImVec2( 1, 1 ), const ImVec4& tint_col = ImVec4( 1, 1, 1, 1 ), const ImVec4& border_col = ImVec4( 0, 0, 0, 0 ) );
	extern bool ImageButton( CTexture2D* pTexture, const ImVec2& size = ImVec2( 0, 0 ), const ImVec2& uv0 = ImVec2( 0, 0 ), const ImVec2& uv1 = ImVec2( 1, 1 ), int frame_padding = -1, const ImVec4& bg_col = ImVec4( 0, 0, 0, 0 ), const ImVec4& tint_col = ImVec4( 1, 1, 1, 1 ) );
	extern void RenderModifiablePalette( void* pPalette, size_t uMin, size_t uMax, std::unordered_set<uint8_t>* psIndicies );
};
