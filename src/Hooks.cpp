#include "Plugin.h"

namespace Plugin
{
    void Hooks::Install()
    {
        REX::DEBUG("{}", __FUNCTION__);

        REL::Relocation<std::uintptr_t> target{ RE::ID::BSGameSound::UpdateFrequencyModifier };
        if (MH_CreateHook(
                reinterpret_cast<void*>(target.address()),
                reinterpret_cast<void*>(&UpdateFrequencyModifier),
                reinterpret_cast<void**>(&_UpdateFrequencyModifier)) != MH_OK) {
            REX::ERROR("Failed to hook BSGameSound::UpdateFrequencyModifier.");
        }

        REL::Relocation<std::uintptr_t> timerTarget{ RE::ID::Actor::UpdateVoiceTimer };
        if (MH_CreateHook(
                reinterpret_cast<void*>(timerTarget.address()),
                reinterpret_cast<void*>(&UpdateVoiceTimer),
                reinterpret_cast<void**>(&_UpdateVoiceTimer)) != MH_OK) {
            REX::ERROR("Failed to hook Actor::UpdateVoiceTimer.");
        }

        REL::Relocation<std::uintptr_t> lipTarget{ RE::ID::BSFaceGenAnimationData::UpdateMorphsFromLip };
        if (MH_CreateHook(
                reinterpret_cast<void*>(lipTarget.address()),
                reinterpret_cast<void*>(&UpdateMorphsFromLip),
                reinterpret_cast<void**>(&_UpdateMorphsFromLip)) != MH_OK) {
            REX::ERROR("Failed to hook BSFaceGenAnimationData::UpdateMorphsFromLip.");
        }
    }

    void Hooks::Initialize()
    {
        REX::DEBUG("{}", __FUNCTION__);

        auto category = RE::TESForm::GetFormByID<RE::BGSSoundCategory>(0x000B0EA4);
        if (!category) {
            REX::ERROR("Failed to find AudioCategoryVOCPlayer sound category.");
            return;
        }
        audioCategoryVOCPlayer.store(category);
    }

    bool Hooks::IsPlayerVoice(const RE::BSGameSound* a_sound)
    {
        auto voice = audioCategoryVOCPlayer.load();
        auto category = a_sound->category;
        return voice && category && category->Matches(voice, false);
    }

    // Audio consumes (real delta * ratio) of the clip per frame; the engine's game delta is
    // (real delta * global time multiplier). This is the factor that turns one into the other.
    float Hooks::GetPlayerVoiceRate()
    {
        const float multiplier = RE::BSTimer::QGlobalTimeMultiplier();
        if (multiplier <= 0.001f) {
            return 1.0f;
        }
        return std::clamp(playerVoiceRatio.load() / multiplier, 0.25f, 2.0f);
    }

    float Hooks::UpdateFrequencyModifier(RE::BSGameSound* a_this)
    {
        const float modifier = _UpdateFrequencyModifier(a_this);
        if (!IsPlayerVoice(a_this)) {
            return modifier;
        }

        const float influence = std::clamp(Plugin::Settings::General::fSlowTimeInfluence.GetValue(), 0.0f, 1.0f);
        const float scale = std::clamp(Plugin::Settings::General::fPlayerVoiceFrequency.GetValue(), 0.5f, 1.5f);

        const float adjusted = (1.0f + (modifier - 1.0f) * influence) * scale;
        const float result = std::max(adjusted, a_this->category->GetMinFrequencyMult());
        playerVoiceRatio.store(result);
        return result;
    }

    // The engine counts a spoken line down in game-time seconds from the clip's stored
    // length (Actor::voiceTimer), and uses that timer, not the audio, to decide when the
    // speaker is done. Make the player's timer run at the rate the voice actually plays.
    void Hooks::UpdateVoiceTimer(RE::Actor* a_this, bool a_force)
    {
        if (!Settings::General::bSyncDialogueTimer.GetValue() || a_this != RE::PlayerCharacter::GetSingleton()) {
            return _UpdateVoiceTimer(a_this, a_force);
        }

        const float saved = a_this->voiceTimer;
        const float delta = RE::BSTimer::QGameDeltaTime();
        if (saved <= 0.0f || delta <= 0.0f) {
            return _UpdateVoiceTimer(a_this, a_force);
        }

        // The original subtracts delta; pre-compensate so the net decrement is delta * rate.
        const float rate = GetPlayerVoiceRate();
        const float pre = std::max(saved + delta * (1.0f - rate), 0.0001f);
        a_this->voiceTimer = pre;
        _UpdateVoiceTimer(a_this, a_force);

        // untouched means the original skipped its countdown this call
        if (a_this->voiceTimer == pre) {
            a_this->voiceTimer = saved;
        }
    }

    // The lip sync clock (BSFaceGenAnimationData::speechTimer) is max(sound playback position,
    // clock + delta). The playback position is in clip time, so it follows the pitch, but as soon
    // as the clock gets ahead of it (position updates are coarse) it free-runs at game delta.
    // Scale the delta for the player's face so that free-run matches the audio.
    bool Hooks::UpdateMorphsFromLip(RE::BSFaceGenAnimationData* a_this, float a_delta)
    {
        if (!Settings::General::bSyncDialogueTimer.GetValue()) {
            return _UpdateMorphsFromLip(a_this, a_delta);
        }

        const auto player = RE::PlayerCharacter::GetSingleton();
        const auto process = player ? player->currentProcess : nullptr;
        const auto middleHigh = process ? process->middleHigh : nullptr;
        if (!middleHigh || middleHigh->faceAnimationData != a_this) {
            return _UpdateMorphsFromLip(a_this, a_delta);
        }

        return _UpdateMorphsFromLip(a_this, a_delta * GetPlayerVoiceRate());
    }
}
