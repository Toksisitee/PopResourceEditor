#pragma once
#include <iostream>
#include <vector>
#include <mutex>

#include "spdlog/sinks/base_sink.h"
#include "spdlog/spdlog.h"
#include "imgui.h"

namespace ImSpdlog {
	typedef std::tuple<std::string, spdlog::level::level_enum> ImLog;

	constexpr static const char* k_pszLogLevels[spdlog::level::n_levels] = {
		"TRC", "DBG", "INF", "WRN", "ERR", "CRT", "OFF"
	};
	constexpr static ImColor k_LogColors[spdlog::level::n_levels] = {
		ImColor( 127, 255, 255 ),   // TRACE
		ImColor( 255, 51, 102 ),    // DEBUG
		ImColor( 0, 255, 204 ),     // INFO
		ImColor( 255, 255, 102 ),   // WARN
		ImColor( 235, 39, 89 ),		// ERR
		ImColor( 255, 51, 102 ),    // CRITICAL
		ImColor( 0, 255, 204 )      // OFF
	};

	template <typename Mutex> class ImGui_sink : public spdlog::sinks::base_sink<Mutex> {
	public:
		ImGui_sink()
		{
			m_LogList.reserve( 100 );
		}

		void Render()
		{
			if ( !m_Draw ) {
				return;
			}

			std::lock_guard<std::mutex> lock( m_LogMutex );
			ImGui::Begin( "Console", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar );
			size_t uStart = m_LogList.size() > m_uMaxLogs ? m_LogList.size() - m_uMaxLogs : 0;

			for ( size_t i = uStart; i < m_LogList.size(); ++i ) {
				auto [sMsg, iLevel] = m_LogList[i];
				auto pszMsg = sMsg.c_str();
				ImGui::TextColored( k_LogColors[iLevel], k_pszLogLevels[iLevel] );
				ImGui::SameLine();
				if ( iLevel == spdlog::level::level_enum::critical ) {
					float fFlicker = (std::sinf( static_cast<float>(ImGui::GetTime() * 4.0f) ) * 0.5f) + 0.8f;
					ImVec4 baseColor = k_LogColors[iLevel];
					ImVec4 finalColor = ImVec4( baseColor.x, baseColor.y, baseColor.z, fFlicker );
					ImGui::TextColored( finalColor, pszMsg );
				}
				else if ( iLevel == spdlog::level::level_enum::err ) {
					ImGui::TextColored( k_LogColors[iLevel], pszMsg );
				}
				else
					ImGui::Text( pszMsg );
			}
			if ( m_bScroll ) {
				ImGui::SetScrollHereY();
				m_bScroll = false;
			}
			if ( ImGui::Button( "Copy" ) ) {
				ImGui::LogToClipboard();
				for ( auto it = m_LogList.begin(); it != m_LogList.end(); ++it ) {
					ImGui::LogText( "%s\n", std::get<0>( *it ).c_str() );
				}
				ImGui::LogFinish();
			}
			ImGui::SameLine();
			if ( ImGui::Button( "X" ) ) {
				m_Draw = !m_Draw;
			}
			ImGui::End();
		}
		void SetMaxShown( size_t uMax ) { m_uMaxLogs = uMax; }

	protected:
		void flush_() override {}
		void sink_it_( const spdlog::details::log_msg& msg ) override
		{
			std::lock_guard<Mutex> lock( m_LogMutex );

			std::string sMsg( msg.payload.begin(), msg.payload.end() );
			m_LogList.push_back( std::make_tuple( sMsg, msg.level ) );

			m_bScroll = true;
		}

	protected:
		std::mutex m_LogMutex;
		std::vector<ImLog> m_LogList = {};
		size_t m_uMaxLogs = 140;
		bool m_bScroll = true;
		bool m_Draw = true;
	};
}

typedef std::shared_ptr<ImSpdlog::ImGui_sink<std::mutex>> ImGui_sink_mt_t;
const ImGui_sink_mt_t ImGui_sink_mt();
