#include "Plugin.h"

namespace Plugin
{
    void Hooks::Initialize()
    {
        REX::DEBUG("{}", __FUNCTION__);
        const char* soundCategoryEditorID = "AudioCategoryVOCPlayer";
        audioCategoryVOCPlayer = RE::TESForm::GetFormByEditorID<RE::BGSSoundCategory>(soundCategoryEditorID);
        if (!audioCategoryVOCPlayer) {
            REX::ERROR("Failed to find sound category: {}", soundCategoryEditorID);
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
        REX::DEBUG("{}", __FUNCTION__);
        _SetGlobalTimeMultipler(a_this, a_magicTimeSlowdown, a_playerMagicTimeSlowdown);
        UpdatePlayerFrequency();
    }

    void Hooks::UpdatePlayerFrequency()
    {
        REX::DEBUG("{}", __FUNCTION__);
        if (audioCategoryVOCPlayer) {
            audioCategoryVOCPlayer->frequencyMult = Plugin::Settings::General::fPlayerVoiceFrequency.GetValue();
            REX::DEBUG("Set frequencyMult to {}", audioCategoryVOCPlayer->frequencyMult);

            audioCategoryVOCPlayer->minFrequencyMult = Plugin::Settings::General::fPlayerVoiceFrequency.GetValue() * 0.1f;
            REX::DEBUG("Set minFrequencyMult to {}", audioCategoryVOCPlayer->minFrequencyMult);
        }
    }

}
