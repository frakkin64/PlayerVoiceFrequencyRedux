#pragma once

namespace Plugin
{
    class Settings
    {
    public:
        class General
        {
        public:
            inline static REX::INI::I32 iLogLevel{ "General", "iLogLevel", static_cast<std::int32_t>(REX::ELogLevel::Info) };
            inline static REX::INI::F32 fPlayerVoiceFrequency{ "General", "fFrequency", 1.0f };
        };

        static void Load();
        static void Update();

    private:
        // Nested classes/structs must be fully declared in the header
        class EventHandler :
            public REX::TSingleton<EventHandler>,
            public RE::BSTEventSink<RE::MenuOpenCloseEvent>
        {
        public:
            virtual RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent& a_event, [[maybe_unused]] RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_sink) override
            {
                REX::DEBUG("{} {}", a_event.menuName, a_event.opening);

                auto processMenuEvent = std::ranges::any_of(std::initializer_list<std::string>{"PauseMenu", "LoadingMenu"},
                    [&a_event](const std::string& str) {
                        return str == a_event.menuName;
                    });
                if (processMenuEvent && !a_event.opening)
                {
                    Plugin::Settings::Update();
                }
                return RE::BSEventNotifyControl::kContinue;
            }
        };

        static void Register();
        static void UpdateGameSetting();

        inline static bool bRegistered{ false };
    };
}