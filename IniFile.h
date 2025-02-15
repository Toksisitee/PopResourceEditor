#pragma once
#include <string>
#include <unordered_map>

enum class EIniSetting : uint8_t
{
    ExportDirectory,
    PopulousDirectory,
};

class CIniFile
{
public:
    explicit CIniFile( const std::string& sConfigName );

    void Initialize();

    std::string GetString( EIniSetting eSetting, const std::string& sDefault = "" );
    void SetString( EIniSetting eSetting, const std::string& sValue );

    int GetInt( EIniSetting eSetting, int nDefault = 0 );
    void SetInt( EIniSetting eSetting, int nValue );

    bool GetBool( EIniSetting eSetting, bool bDefault = false );
    void SetBool( EIniSetting eSetting, bool bValue );

    float GetFloat( EIniSetting eSetting, float fDefault = 0.0f );
    void SetFloat( EIniSetting eSetting, float fValue );

    void ResetSection();

private:
    std::string m_sFilePath;
    static const std::string m_sSection;
    static const std::unordered_map<EIniSetting, std::string> m_mapSettingKeys;
    std::string GetKey( EIniSetting eSetting );
};
