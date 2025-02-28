/*
Copyright (c) 2024-2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#include "imgui.h"

#include "FileDialogNative.h"
#include "AssetsErrHandler.h"
#include "ImEditor.h"
#include "Utils.h"
#include "BlocksWnd.h"

void CBlocksWnd::RenderBegin()
{
	ImGui::Begin( m_sWindowName.c_str(), &m_bOpen );
}

void CBlocksWnd::RenderEnd()
{
	ImGui::End();
}

void CBlocksWnd::Render()
{
	if ( ImGui::Button( "Load Bin" ) ) {
		if ( auto osFilePath = FileDialog::OpenFile( FileDialog::Filter::DAT ) ) {
			g_ErrHandler.HandleResult( m_Blocks.LoadBin( *osFilePath ) );
			m_Blocks.DestroyTexture();
		}
	} ImGui::SameLine();
	if ( ImGui::Button( "Load Image" ) ) {
		if ( auto osFilePath = FileDialog::OpenFile( FileDialog::Filter::BMP ) ) {
			g_ErrHandler.HandleResult( m_Blocks.LoadImg( *osFilePath ) );
			m_Blocks.DestroyTexture();
		}
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Image" ) ) {
		g_ErrHandler.HandleResult( m_Blocks.ExportImg( Util::FileSystem::FormatExportPathFromFileName( GetAssetName() ) ) );
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Bin" ) ) {
		g_ErrHandler.HandleResult( m_Blocks.ExportBin( Util::FileSystem::FormatExportPathFromFileName( GetAssetName() ) ) );
	}

	if ( m_Blocks.GetTexture() == nullptr ) {
		for ( size_t i = 0; i < Assets::Blocks::k_uNumBlocks; i++ ) {
			m_Blocks.CreateSubTexture( m_pd3dDevice, i );
		}
		m_Blocks.CreateTexture( m_pd3dDevice );
	}
	else {
		ImEditor::SetPointFiltering( m_pd3dDevice );

		ImGui::Checkbox( "Draw Atlas", &m_bDrawAtlas );
		if ( m_bDrawAtlas ) {
			ImEditor::RenderTexture( m_Blocks.GetTexture() );
		}
		else {
			size_t uTexturesCountRow = Assets::Blocks::k_uWidth / Assets::Blocks::k_uBlockWidth;
			size_t uTexturesCountCol = Assets::Blocks::k_uHeight / Assets::Blocks::k_uBlockHeight;
			for ( size_t i = 0; i < uTexturesCountCol; i++ ) {
				for ( size_t j = 0; j < uTexturesCountRow; j++ ) {
					const auto uIndex = j * uTexturesCountRow + i;
					if ( ImEditor::ImageButton( m_Blocks.GetTexture( uIndex ) ) ) {
						g_ErrHandler.HandleResult( m_Blocks.ExportSubImg( Util::FileSystem::FormatExportPathFromFileName( GetAssetName() ), uIndex ) );
					}
					ImGui::SameLine();
				}
				ImGui::NewLine();
			}
		}

		ImEditor::ResetRenderState();
	}
}

void CBlocksWnd::OnPaletteChange()
{
	m_Blocks.DestroyTextures();
}
