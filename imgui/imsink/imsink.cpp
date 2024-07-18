#include "imsink/imsink.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/base_sink.h>

const ImGui_sink_mt_t ImGui_sink_mt()
{
	auto log = spdlog::create<ImSpdlog::ImGui_sink<std::mutex>>( std::string( "log" ) );
	return std::dynamic_pointer_cast<ImSpdlog::ImGui_sink<std::mutex>>(log->sinks()[0]);
}
