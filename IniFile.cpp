/*
Copyright (c) 2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#include <windows.h>
#include <stdexcept>
#include <fstream>

#include "IniFile.h"

namespace Util
{
	namespace FileSystem
	{
		void RemoveFileSpec( char* pszPath );
	}
}

CIniFile g_IniFile( "config.ini" );

const std::string CIniFile::m_sSection = "Settings";

const std::unordered_map<EIniSetting, std::string> CIniFile::m_mapSettingKeys = {
	{EIniSetting::ExportDirectory, "ExportDirectory"} ,
	{EIniSetting::PopulousDirectory, "PopulousDirectory"} ,
};

CIniFile::CIniFile( const std::string& sConfigName ) : m_sFilePath( sConfigName ) {}

void CIniFile::Initialize()
{
	char szPath[MAX_PATH];
	GetModuleFileNameA( NULL, szPath, MAX_PATH );
	Util::FileSystem::RemoveFileSpec( szPath );

	m_sFilePath = std::string( szPath ) + "\\" + m_sFilePath;

	std::ifstream file( m_sFilePath );

	if ( !file.good() ) {
		std::ofstream outFile( m_sFilePath );
		if ( outFile.is_open() ) {
			outFile << "[" << m_sSection << "]\n";
			outFile.close();
		}
	}
}

std::string CIniFile::GetKey( EIniSetting eSetting )
{
	auto it = m_mapSettingKeys.find( eSetting );
	if ( it != m_mapSettingKeys.end() ) {
		return it->second;
	}
	throw std::runtime_error( "Invalid setting key." );
}

std::string CIniFile::GetString( EIniSetting eSetting, const std::string& sDefault )
{
	char szBuffer[256];
	GetPrivateProfileStringA( m_sSection.c_str(), GetKey( eSetting ).c_str(), sDefault.c_str(), szBuffer, sizeof( szBuffer ), m_sFilePath.c_str() );
	return std::string( szBuffer );
}

void CIniFile::SetString( EIniSetting eSetting, const std::string& sValue )
{
	WritePrivateProfileStringA( m_sSection.c_str(), GetKey( eSetting ).c_str(), sValue.c_str(), m_sFilePath.c_str() );
}

int CIniFile::GetInt( EIniSetting eSetting, int nDefault )
{
	return GetPrivateProfileIntA( m_sSection.c_str(), GetKey( eSetting ).c_str(), nDefault, m_sFilePath.c_str() );
}

void CIniFile::SetInt( EIniSetting eSetting, int nValue )
{
	SetString( eSetting, std::to_string( nValue ) );
}

bool CIniFile::GetBool( EIniSetting eSetting, bool bDefault )
{
	return GetInt( eSetting, bDefault ? 1 : 0 ) != 0;
}

void CIniFile::SetBool( EIniSetting eSetting, bool bValue )
{
	SetInt( eSetting, bValue ? 1 : 0 );
}

float CIniFile::GetFloat( EIniSetting eSetting, float fDefault )
{
	std::string strValue = GetString( eSetting, std::to_string( fDefault ) );
	try {
		return std::stof( strValue );
	}
	catch ( const std::exception& ) {
		return fDefault;
	}
}

void CIniFile::SetFloat( EIniSetting eSetting, float fValue )
{
	SetString( eSetting, std::to_string( fValue ) );
}

void CIniFile::ResetSection()
{
	WritePrivateProfileStringA( m_sSection.c_str(), nullptr, nullptr, m_sFilePath.c_str() );
}
