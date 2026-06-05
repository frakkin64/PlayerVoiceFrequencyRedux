#include "Plugin.h"

namespace Plugin
{
    void Hooks::Initialize()
    {
        REX::DEBUG("{}", __FUNCTION__);
        audioCategoryVOCPlayer = RE::TESForm::GetFormByID<RE::BGSSoundCategory>(0x000B0EA4);
        if (!audioCategoryVOCPlayer) {
            REX::ERROR("Failed to find AudioCategoryVOCPlayer sound category.");
            return;
        }

        REL::Trampoline& trampoline = REL::GetTrampoline();

        // BSGameSound::UpdateFrequencyModifier
        REL::Relocation<uintptr_t> ptr_IgnoreOverride{ REL::ID { 157156, 2267454 }, REL::Offset { 0x97, 0x97 } };
        ptr_IgnoreOverride.write_fill(REL::NOP, 2);

        // VATS::SetMagicTimeSlowdown
        REL::Relocation<uintptr_t> ptr_SetGlobalTimeMultipler{ REL::ID { 249054, 2237201 }, REL::Offset { 0x6B, 0x6B } };
        _SetGlobalTimeMultipler = trampoline.write_call<5>(ptr_SetGlobalTimeMultipler.address(), &SetGlobalTimeMultipler);
    }

    void Hooks::SetGlobalTimeMultipler(RE::VATS *a_this, float a_magicTimeSlowdown, float a_playerMagicTimeSlowdown)
    {
        REX::DEBUG("{}({}, {})", __FUNCTION__, a_magicTimeSlowdown, a_playerMagicTimeSlowdown);
        _SetGlobalTimeMultipler(a_this, a_magicTimeSlowdown, a_playerMagicTimeSlowdown);
        UpdatePlayerFrequency();
    }

    void Hooks::UpdatePlayerFrequency()
    {
        REX::DEBUG("{}", __FUNCTION__);
        if (audioCategoryVOCPlayer) {
            float clampedFrequency = std::clamp(Plugin::Settings::General::fPlayerVoiceFrequency.GetValue(), 0.5f, 1.5f);

            audioCategoryVOCPlayer->frequencyMult = clampedFrequency;
            REX::DEBUG("Set frequencyMult to {}", audioCategoryVOCPlayer->frequencyMult);

            audioCategoryVOCPlayer->minFrequencyMult = clampedFrequency * 0.1f;
            REX::DEBUG("Set minFrequencyMult to {}", audioCategoryVOCPlayer->minFrequencyMult);
        }
    }
}
