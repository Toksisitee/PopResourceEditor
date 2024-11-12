#include "ImEditor.h"

namespace ImEditor
{
	void RenderTexture( CTexture2D* pTexture, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col )
	{
		assert( pTexture != nullptr && "pTexture in RenderTexture is nullptr" );
		ImGui::Image( pTexture->GetTexture(), size, uv0, uv1, tint_col, border_col );
	}
};