#include <imgui.h>
#include <imgui_internal.h>
#include <chrono>
#include <string>
#include <time.h>
#include <filesystem>
#include <sstream>

#include "L2DFileDialog.h"

#define EDITOR_VOLUMES			(1)
#define EDITOR_INPUT_PATH		(1)
#define EDITOR_CRUMBS			(1)

namespace FileDialog {
	bool file_dialog_open = false;
	FileDialogType file_dialog_open_type = FileDialogType::OpenFile;

#if EDITOR_VOLUMES
	std::vector<std::filesystem::path> GetVolumes()
	{
		std::vector<std::filesystem::path> volumes;
		for ( char drive = 'A'; drive <= 'Z'; ++drive ) {
			std::filesystem::path drive_path = std::string( 1, drive ) + ":\\";
			if ( std::filesystem::exists( drive_path ) ) {
				volumes.push_back( drive_path );
			}
		}
		return volumes;
	}
#endif

	void ShowFileDialog( bool* open, char* buffer, [[maybe_unused]] unsigned int buffer_size, FileDialogType type )
	{
		static int file_dialog_file_select_index = 0;
		static int file_dialog_folder_select_index = 0;
		static std::string file_dialog_current_path = std::filesystem::current_path().string();
		static std::string file_dialog_current_file = "";
		static std::string file_dialog_current_folder = "";
		static char file_dialog_error[500] = "";
		static FileDialogSortOrder file_name_sort_order = FileDialogSortOrder::None;
		static FileDialogSortOrder size_sort_order = FileDialogSortOrder::None;
		static FileDialogSortOrder date_sort_order = FileDialogSortOrder::None;
		static FileDialogSortOrder type_sort_order = FileDialogSortOrder::None;
		static double error_timeout = 0.0;
		static bool initial_path_set = false;

		if ( open ) {
			// Check if there was already something in the buffer. If so, try to use that path (if it exists).
			// If it doesn't exist, just put them into the current path.
			if ( !initial_path_set && strlen( buffer ) > 0 ) {
				auto path = std::filesystem::path( buffer );
				if ( std::filesystem::is_directory( path ) ) {
					file_dialog_current_path = buffer;
				}
				else {
					// Check if this is just a file in a real path. If so, use the real path.
					// If that still doesn't work, use current path.
					if ( std::filesystem::exists( path ) ) {
						// It's a file! Take the path and set it.
						file_dialog_current_path = path.remove_filename().string();
					}
					else {
						// An invalid path was entered
						file_dialog_current_path = std::filesystem::current_path().string();
					}
				}
				initial_path_set = true;
			}

			const char* window_title = (type == FileDialogType::OpenFile ? "Select a file" : "Select a folder");
			ImGui::Begin( window_title, nullptr, ImGuiWindowFlags_AlwaysAutoResize );

#if EDITOR_VOLUMES
			std::vector<std::filesystem::path> volumes = GetVolumes();
			ImGui::Text( "Volumes:" );
			ImGui::SameLine();
			for ( size_t i = 0; i < volumes.size(); i++ ) {
				ImGui::SameLine();
				if ( ImGui::Button( volumes[i].string().c_str() ) ) {
					file_dialog_current_path = volumes[i].string();
					file_dialog_folder_select_index = 0;
					file_dialog_file_select_index = 0;
					ImGui::SetScrollHereY( 0.0f );
					file_dialog_current_folder = "";
				}
			}
#endif

#if EDITOR_INPUT_PATH
			static char folder_path_buffer[512] = "< Paste Directory Path >";
			ImGui::PushItemWidth( 600 );

			if ( ImGui::InputText( "##FolderPathInput", folder_path_buffer, sizeof( folder_path_buffer ), ImGuiInputTextFlags_EnterReturnsTrue ) ) {
				if ( std::filesystem::exists( folder_path_buffer ) && std::filesystem::is_directory( folder_path_buffer ) ) {
					file_dialog_current_path = folder_path_buffer;
					file_dialog_folder_select_index = 0;
					file_dialog_file_select_index = 0;
					ImGui::SetScrollHereY( 0.0f );
				}
				else {
					strcpy_s( file_dialog_error, "Error: Invalid folder path!" );
				}
			}
			ImGui::PopItemWidth();

			ImGui::SameLine();
			if ( ImGui::Button( "Navigate" ) ) {
				if ( std::filesystem::exists( folder_path_buffer ) && std::filesystem::is_directory( folder_path_buffer ) ) {
					file_dialog_current_path = folder_path_buffer;
					file_dialog_folder_select_index = 0;
					file_dialog_file_select_index = 0;
					ImGui::SetScrollHereY( 0.0f );
				}
				else {
					strcpy_s( file_dialog_error, "Error: Invalid folder path!" );
				}
			}
#endif

			std::vector<std::filesystem::directory_entry> files;
			std::vector<std::filesystem::directory_entry> folders;
			try {
				for ( auto& p : std::filesystem::directory_iterator( file_dialog_current_path ) ) {
					if ( p.is_directory() ) {
						folders.push_back( p );
					}
					else {
						files.push_back( p );
					}
				}
			}
			catch ( ... ) {}

#if EDITOR_CRUMBS
			std::filesystem::path path( file_dialog_current_path );
			std::vector<std::string> path_segments;

			for ( auto& part : path ) {
				if ( !part.empty() && part != "\\" ) {
					path_segments.push_back( part.string() );
				}
			}

			std::filesystem::path current_path = "";
			for ( size_t i = 0; i < path_segments.size(); i++ ) {
				if ( ImGui::Button( path_segments[i].c_str() ) ) {
					current_path /= path_segments[i];
					file_dialog_current_path = "";
					for ( size_t j = 0; j <= i; j++ ) {
						file_dialog_current_path += path_segments[j] + "\\";
					}
				}

				if ( i < path_segments.size() - 1 ) {
					ImGui::SameLine();
					ImGui::Text( ">" ); // Separator
					ImGui::SameLine();
				}
			}
#else
			ImGui::Text( "%s", file_dialog_current_path.c_str() );
#endif

			ImGui::BeginChild( "Directories##1", ImVec2( 200, 300 ), true, ImGuiWindowFlags_HorizontalScrollbar );

			if ( ImGui::Selectable( "..", false, ImGuiSelectableFlags_AllowDoubleClick, ImVec2( ImGui::GetContentRegionAvail().x, 0 ) ) ) {
				if ( ImGui::IsMouseDoubleClicked( 0 ) ) {
					file_dialog_current_path = std::filesystem::path( file_dialog_current_path ).parent_path().string();
				}
			}
			for ( int i = 0; i < folders.size(); ++i ) {
				if ( ImGui::Selectable( folders[i].path().stem().string().c_str(), i == file_dialog_folder_select_index, ImGuiSelectableFlags_AllowDoubleClick, ImVec2( ImGui::GetContentRegionAvail().x, 0 ) ) ) {
					file_dialog_current_file = "";
					if ( ImGui::IsMouseDoubleClicked( 0 ) ) {
						file_dialog_current_path = folders[i].path().string();
						file_dialog_folder_select_index = 0;
						file_dialog_file_select_index = 0;
						ImGui::SetScrollHereY( 0.0f );
						file_dialog_current_folder = "";
					}
					else {
						file_dialog_folder_select_index = i;
						file_dialog_current_folder = folders[i].path().stem().string();
					}
				}
			}
			ImGui::EndChild();

			ImGui::SameLine();

			ImGui::BeginChild( "Files##1", ImVec2( 516, 300 ), true, ImGuiWindowFlags_HorizontalScrollbar );
			ImGui::Columns( 4 );
			static float initial_spacing_column_0 = 230.0f;
			if ( initial_spacing_column_0 > 0 ) {
				ImGui::SetColumnWidth( 0, initial_spacing_column_0 );
				initial_spacing_column_0 = 0.0f;
			}
			static float initial_spacing_column_1 = 80.0f;
			if ( initial_spacing_column_1 > 0 ) {
				ImGui::SetColumnWidth( 1, initial_spacing_column_1 );
				initial_spacing_column_1 = 0.0f;
			}
			static float initial_spacing_column_2 = 80.0f;
			if ( initial_spacing_column_2 > 0 ) {
				ImGui::SetColumnWidth( 2, initial_spacing_column_2 );
				initial_spacing_column_2 = 0.0f;
			}
			if ( ImGui::Selectable( "File" ) ) {
				size_sort_order = FileDialogSortOrder::None;
				date_sort_order = FileDialogSortOrder::None;
				type_sort_order = FileDialogSortOrder::None;
				file_name_sort_order = (file_name_sort_order == FileDialogSortOrder::Down ? FileDialogSortOrder::Up : FileDialogSortOrder::Down);
			}
			ImGui::NextColumn();
			if ( ImGui::Selectable( "Size" ) ) {
				file_name_sort_order = FileDialogSortOrder::None;
				date_sort_order = FileDialogSortOrder::None;
				type_sort_order = FileDialogSortOrder::None;
				size_sort_order = (size_sort_order == FileDialogSortOrder::Down ? FileDialogSortOrder::Up : FileDialogSortOrder::Down);
			}
			ImGui::NextColumn();
			if ( ImGui::Selectable( "Type" ) ) {
				file_name_sort_order = FileDialogSortOrder::None;
				date_sort_order = FileDialogSortOrder::None;
				size_sort_order = FileDialogSortOrder::None;
				type_sort_order = (type_sort_order == FileDialogSortOrder::Down ? FileDialogSortOrder::Up : FileDialogSortOrder::Down);
			}
			ImGui::NextColumn();
			if ( ImGui::Selectable( "Date" ) ) {
				file_name_sort_order = FileDialogSortOrder::None;
				size_sort_order = FileDialogSortOrder::None;
				type_sort_order = FileDialogSortOrder::None;
				date_sort_order = (date_sort_order == FileDialogSortOrder::Down ? FileDialogSortOrder::Up : FileDialogSortOrder::Down);
			}
			ImGui::NextColumn();
			ImGui::Separator();

			// Sort files
			if ( file_name_sort_order != FileDialogSortOrder::None ) {
				std::sort( files.begin(), files.end(), []( const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b ) {
					if ( file_name_sort_order == FileDialogSortOrder::Down ) {
						return a.path().filename().string() > b.path().filename().string();
					}
					else {
						return a.path().filename().string() < b.path().filename().string();
					}
				} );
			}
			else if ( size_sort_order != FileDialogSortOrder::None ) {
				std::sort( files.begin(), files.end(), []( const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b ) {
					if ( size_sort_order == FileDialogSortOrder::Down ) {
						return a.file_size() > b.file_size();
					}
					else {
						return a.file_size() < b.file_size();
					}
				} );
			}
			else if ( type_sort_order != FileDialogSortOrder::None ) {
				std::sort( files.begin(), files.end(), []( const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b ) {
					if ( type_sort_order == FileDialogSortOrder::Down ) {
						return a.path().extension().string() > b.path().extension().string();
					}
					else {
						return a.path().extension().string() < b.path().extension().string();
					}
				} );
			}
			else if ( date_sort_order != FileDialogSortOrder::None ) {
				std::sort( files.begin(), files.end(), []( const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b ) {
					if ( date_sort_order == FileDialogSortOrder::Down ) {
						return a.last_write_time() > b.last_write_time();
					}
					else {
						return a.last_write_time() < b.last_write_time();
					}
				} );
			}

			for ( int i = 0; i < files.size(); ++i ) {
				if ( ImGui::Selectable( files[i].path().filename().string().c_str(), i == file_dialog_file_select_index, ImGuiSelectableFlags_AllowDoubleClick, ImVec2( ImGui::GetContentRegionAvail().x, 0 ) ) ) {
					file_dialog_file_select_index = i;
					file_dialog_current_file = files[i].path().filename().string();
					file_dialog_current_folder = "";
				}
				ImGui::NextColumn();
				ImGui::TextUnformatted( std::to_string( files[i].file_size() ).c_str() );
				ImGui::NextColumn();
				ImGui::TextUnformatted( files[i].path().extension().string().c_str() );
				ImGui::NextColumn();
				auto ftime = files[i].last_write_time();
				auto st = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - decltype(ftime)::clock::now() + std::chrono::system_clock::now());
				std::time_t tt = std::chrono::system_clock::to_time_t( st );

				std::tm mt;
				localtime_s( &mt, &tt );
				std::stringstream ss;
				ss << std::put_time( &mt, "%F %R" );

				ImGui::TextUnformatted( ss.str().c_str() );
				ImGui::NextColumn();
			}
			ImGui::EndChild();

			std::string selected_file_path = file_dialog_current_path + (file_dialog_current_path.back() == '\\' ? "" : "\\") + (file_dialog_current_folder.size() > 0 ? file_dialog_current_folder : file_dialog_current_file);
			char* buf = &selected_file_path[0];
			ImGui::PushItemWidth( 724 );
			ImGui::InputText( "##text", buf, sizeof( buf ), ImGuiInputTextFlags_ReadOnly );

			ImGui::SetCursorPosY( ImGui::GetCursorPosY() + 6 );

			if ( ImGui::Button( "New folder" ) ) {
				ImGui::OpenPopup( "NewFolderPopup" );
			}
			ImGui::SameLine();

#if 0
			static bool disable_delete_button = false;
			disable_delete_button = (file_dialog_current_folder == "");
			if ( disable_delete_button ) {
				ImGui::PushItemFlag( ImGuiItemFlags_Disabled, true );
				ImGui::PushStyleVar( ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f );
			}
			if ( ImGui::Button( "Delete folder" ) ) {
				ImGui::OpenPopup( "DeleteFolderPopup" );
			}
			if ( disable_delete_button ) {
				ImGui::PopStyleVar();
				ImGui::PopItemFlag();
			}
#endif

			ImVec2 center( ImGui::GetWindowPos().x + ImGui::GetWindowSize().x * 0.5f, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y * 0.5f );
			ImGui::SetNextWindowPos( center, ImGuiCond_Appearing, ImVec2( 0.5f, 0.5f ) );
			if ( ImGui::BeginPopup( "NewFolderPopup", ImGuiWindowFlags_Modal ) ) {
				ImGui::Text( "Enter a name for the new folder" );
				static char new_folder_name[500] = "";
				static char new_folder_error[500] = "";
				ImGui::InputText( "##newfolder", new_folder_name, sizeof( new_folder_name ) );
				if ( ImGui::Button( "Create##1" ) ) {
					if ( strlen( new_folder_name ) <= 0 ) {
						strcpy_s( new_folder_error, "Folder name can't be empty" );
					}
					else {
						std::string new_file_path = file_dialog_current_path + (file_dialog_current_path.back() == '\\' ? "" : "\\") + new_folder_name;
						std::filesystem::create_directory( new_file_path );
						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::SameLine();
				if ( ImGui::Button( "Cancel##1" ) ) {
					strcpy_s( new_folder_name, "" );
					strcpy_s( new_folder_error, "" );
					ImGui::CloseCurrentPopup();
				}
				ImGui::TextColored( ImColor( 1.0f, 0.0f, 0.2f, 1.0f ), new_folder_error );
				ImGui::EndPopup();
			}

			ImGui::SetNextWindowPos( center, ImGuiCond_Appearing, ImVec2( 0.5f, 0.5f ) );
			if ( ImGui::BeginPopup( "DeleteFolderPopup", ImGuiWindowFlags_Modal ) ) {
				ImGui::TextColored( ImColor( 1.0f, 0.0f, 0.2f, 1.0f ), "Are you sure you want to delete this folder?" );
				ImGui::SetCursorPosY( ImGui::GetCursorPosY() + 6 );
				ImGui::TextUnformatted( file_dialog_current_folder.c_str() );
				ImGui::SetCursorPosY( ImGui::GetCursorPosY() + 6 );
				if ( ImGui::Button( "Yes" ) ) {
					std::filesystem::remove( file_dialog_current_path + (file_dialog_current_path.back() == '\\' ? "" : "\\") + file_dialog_current_folder );
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if ( ImGui::Button( "No" ) ) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			ImGui::SameLine();
			ImGui::SetCursorPosX( ImGui::GetWindowWidth() - 120 );

			static auto reset_everything = [&]() {
				file_dialog_file_select_index = 0;
				file_dialog_folder_select_index = 0;
				file_dialog_current_file = "";
				strcpy_s( file_dialog_error, "" );
				initial_path_set = false;
				file_dialog_open = false;
			};

			if ( ImGui::Button( "Cancel" ) ) {
				reset_everything();
			}
			ImGui::SameLine();
			if ( ImGui::Button( "Choose" ) ) {
				if ( type == FileDialogType::SelectFolder ) {
					auto path = file_dialog_current_path + (file_dialog_current_path.back() == '\\' ? "" : "\\") + file_dialog_current_file;
					strcpy_s( buffer, path.length() + 1, path.c_str() );
					strcpy_s( file_dialog_error, "" );
					reset_everything();
				}
				else if ( type == FileDialogType::OpenFile ) {
					if ( file_dialog_current_file == "" ) {
						strcpy_s( file_dialog_error, "Error: You must select a file!" );
					}
					else {
						auto path = file_dialog_current_path + (file_dialog_current_path.back() == '\\' ? "" : "\\") + file_dialog_current_file;
						strcpy_s( buffer, path.length() + 1, path.c_str() );
						strcpy_s( file_dialog_error, "" );
						reset_everything();
					}
				}
			}

			if ( strlen( file_dialog_error ) > 0 ) {
				if ( error_timeout == 0.0 ) {
					error_timeout = ImGui::GetTime();
				}
				if ( ImGui::GetTime() - error_timeout < 4.0 ) {
					ImGui::TextColored( ImColor( 1.0f, 0.0f, 0.2f, 1.0f ), file_dialog_error );
				}
				else {
					strcpy_s( file_dialog_error, "" );
					error_timeout = 0.0;
				}
			}

			ImGui::End();
		}
	}

	void ShowFileDialog_s( bool* open, char* buffer, FileDialogType type )
	{
		ShowFileDialog( open, buffer, 500, type );
	}
}