#pragma once

namespace Plugin
{
    class Hooks
    {
    public:
        static void Initialize();
        static void SetGlobalTimeMultipler(RE::VATS*, float, float);
        static void UpdatePlayerFrequency();

    private:
        inline static RE::BGSSoundCategory* audioCategoryVOCPlayer = nullptr;
        inline static REL::Relocation<decltype(&SetGlobalTimeMultipler)> _SetGlobalTimeMultipler;
    };
}
