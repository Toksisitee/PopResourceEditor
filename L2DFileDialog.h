/*
	Copyright 2020 Limeoats

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.

	Modified by Toksisitee on 16.02.2025
	Changes made:
		- Removed static variables.
		- Error messages now automatically clear.
		- Added breadcrumbs navigation for improved file path visibility and easier navigation.
		- Removed "Delete Folder" button
		- Added an input text box to allow copy-pasting file paths efficiently.
		- Updated file navigation to support multiple volumes.
		- Window ImGuiWindowFlags_AlwaysAutoResize.
		- Pasted path is treated as a selected folder when clicking "Choose".
*/

#pragma once
namespace FileDialog {
	enum class FileDialogType {
		OpenFile,
		SelectFolder
	};

	enum class FileDialogSortOrder {
		Up,
		Down,
		None
	};

	extern bool file_dialog_open;
	extern FileDialogType file_dialog_open_type;

	extern void ShowFileDialog( bool* open, char* buffer, [[maybe_unused]] unsigned int buffer_size, FileDialogType type = FileDialogType::OpenFile );
	extern void ShowFileDialog_s( bool* open, char* buffer, FileDialogType type = FileDialogType::OpenFile );
}
