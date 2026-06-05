#include "Plugin.h"

namespace Plugin
{
    void Settings::Load()
    {
        REX::DEBUG("{}", __FUNCTION__);

        const auto ini = REX::INI::SettingStore::GetSingleton();
        ini->Init(
            "Data/MCM/Config/PlayerVoiceFrequencyRedux/settings.ini",
            "Data/MCM/Settings/PlayerVoiceFrequencyRedux.ini");
        ini->Load();
    }

    void Settings::Update()
    {
        REX::DEBUG("{}", __FUNCTION__);

        Load();
        UpdateGameSetting();
        Plugin::Hooks::UpdatePlayerFrequency();
    }

    void Settings::Register()
    {
        REX::DEBUG("{}", __FUNCTION__);
        if (bRegistered)
        {
            return;
        }

        if (auto UI = RE::UI::GetSingleton())
        {
            REX::DEBUG("EventHandler Registered");
            UI->RegisterSink<RE::MenuOpenCloseEvent>(EventHandler::GetSingleton());
            bRegistered = true;
        }
    }

    void Settings::UpdateGameSetting()
    {
        REX::DEBUG("{}", __FUNCTION__);
        if (auto INISettingCollection = RE::INISettingCollection::GetSingleton())
        {
            const char* settingName = "fFrequency:General";
            if (auto fPlayerVoiceFrequency = INISettingCollection->GetSetting(settingName))
            {
                REX::DEBUG("Setting INISettingCollection {} to {}", settingName, Plugin::Settings::General::fPlayerVoiceFrequency.GetValue());
                fPlayerVoiceFrequency->SetFloat(Plugin::Settings::General::fPlayerVoiceFrequency.GetValue());
            }
        }
    }
}