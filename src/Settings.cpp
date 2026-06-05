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
}
