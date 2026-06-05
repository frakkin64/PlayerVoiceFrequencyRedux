#pragma once

namespace Plugin
{
    class Hooks
    {
    public:
        static void Install();
        static void Initialize();

    private:
        static float UpdateFrequencyModifier(RE::BSGameSound* a_this);
        static void  UpdateVoiceTimer(RE::Actor* a_this, bool a_force);
        static bool  UpdateMorphsFromLip(RE::BSFaceGenAnimationData* a_this, float a_delta);

        static bool IsPlayerVoice(const RE::BSGameSound* a_sound);
        static float GetPlayerVoiceRate();

        inline static std::atomic<RE::BGSSoundCategory*> audioCategoryVOCPlayer{ nullptr };
        inline static std::atomic<float> playerVoiceRatio{ 1.0f };
        inline static float (*_UpdateFrequencyModifier)(RE::BSGameSound*) = nullptr;
        inline static void (*_UpdateVoiceTimer)(RE::Actor*, bool) = nullptr;
        inline static bool (*_UpdateMorphsFromLip)(RE::BSFaceGenAnimationData*, float) = nullptr;
    };
}
