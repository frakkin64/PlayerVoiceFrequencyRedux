#include "SimpleIni.h"

CSimpleIniA ini(true, false, false);
RE::BGSSoundCategory* playerSoundCategory;
float playerVoiceFrequency = 1.0f;
REL::Relocation<uintptr_t> ptr_changeTime{ REL::ID { 249054, 2237201 }, REL::Offset { 0x2B, 0x2B } };
REL::Relocation<uintptr_t> ptr_revertTime{ REL::ID { 249054, 2237201 }, REL::Offset { 0x6B, 0x6B }  };
REL::Relocation<uintptr_t> ptr_overrideJump{ REL::ID { 157156, 2267454 }, REL::Offset { 0x97, 0x97 } };
REL::Relocation<float*> ptr_globalTimeMultiplier{ REL::ID { 1266509, 2666308 } };

void SetPlayerVoiceFrequency([[maybe_unused]] float currentTime) {
	ini.LoadFile("Data\\F4SE\\Plugins\\PlayerVoiceFrequency.ini");
	playerVoiceFrequency = std::stof(ini.GetValue("General", "Frequency", "1.0f"));
	if (playerVoiceFrequency < 0.05f) {
		playerVoiceFrequency = 0.05f;
	}

	if (playerSoundCategory) {
		playerSoundCategory->frequencyMult = playerVoiceFrequency;
		playerSoundCategory->minFrequencyMult = playerVoiceFrequency * 0.1f;
	}
}

void SetGlobalTimeMultiplier(uint64_t unk, float f) {
	typedef void (*func_t)(uint64_t, float);
	REL::Relocation<func_t> func{ REL::ID { 1419977, 2267970 } };
	func(unk, f);
	SetPlayerVoiceFrequency(f);
}

struct MenuWatcher : public RE::BSTEventSink<RE::MenuOpenCloseEvent> {
	virtual RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent & evn, [[maybe_unused]] RE::BSTEventSource<RE::MenuOpenCloseEvent>*a_source) override {
		if (evn.menuName == RE::BSFixedString("LoadingMenu") && !evn.opening) {
			SetPlayerVoiceFrequency(*ptr_globalTimeMultiplier);
		}
		return RE::BSEventNotifyControl::kContinue;
	}
};

F4SE_PLUGIN_QUERY(const F4SE::QueryInterface* a_f4se, F4SE::PluginInfo* a_info)
{
	if (const auto data = F4SE::PluginVersionData::GetSingleton()) {
		a_info->infoVersion = F4SE::PluginInfo::kVersion;
		a_info->name = data->GetPluginName().data();
		a_info->version = data->GetPluginVersion().pack();
	}

	const auto ver = a_f4se->RuntimeVersion();
	if (ver < REL::Version(F4SE::RUNTIME_1_10_163)) {
		return false;
	}

	return true;
}

F4SE_PLUGIN_LOAD(const F4SE::LoadInterface* a_f4se)
{
	F4SE::Init(a_f4se, { .trampoline = true, .trampolineSize = 14 });

	REX::INFO("{} v{}", F4SE::GetPluginName(), F4SE::GetPluginVersion());

	REL::Trampoline& trampoline = REL::GetTrampoline();
	trampoline.write_call<5>(ptr_changeTime.address(), &SetGlobalTimeMultiplier);
	trampoline.write_call<5>(ptr_revertTime.address(), &SetGlobalTimeMultiplier);
	ptr_overrideJump.write_fill(REL::NOP, 2);

	REX::INFO("Hooks applied.");

	const F4SE::MessagingInterface* message = F4SE::GetMessagingInterface();
	message->RegisterListener([](F4SE::MessagingInterface::Message* msg) -> void {
		if (msg->type == F4SE::MessagingInterface::kGameDataReady) {
			MenuWatcher* mw = new MenuWatcher();
			RE::UI::GetSingleton()->GetEventSource<RE::MenuOpenCloseEvent>()->RegisterSink(mw);
			RE::TESDataHandler* dh = RE::TESDataHandler::GetSingleton();
			RE::BSTArray<RE::BGSSoundCategory*> sc = dh->GetFormArray<RE::BGSSoundCategory>();
			for (auto it = sc.begin(); it != sc.end(); ++it) {
				if (strcmp((*it)->fullName.c_str(), "AudioCategoryVOCPlayer") == 0) {
					playerSoundCategory = *it;
					//REX::WARN("AudioCategoryVOCPlayer {:#x}", reinterpret_cast<uintptr_t>(*it));
				}
			}
		}
	});

	return true;
}
