#include "Plugin.h"

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
	Plugin::Settings::Load();

	F4SE::Init(a_f4se, {
		.logLevel = REX::ELogLevel { Plugin::Settings::General::iLogLevel.GetValue() },
		.trampoline = true,
		.trampolineSize = 14
		});

	F4SE::GetMessagingInterface()->RegisterListener([](F4SE::MessagingInterface::Message* a_msg)
		{
			switch (a_msg->type)
			{
			case F4SE::MessagingInterface::kGameDataReady:
				REX::DEBUG("kGameDataReady {}", static_cast<bool>(a_msg->data));
				if (static_cast<bool>(a_msg->data)) {
					Plugin::Settings::Update();
					Plugin::Hooks::Initialize();
				}
				break;
			default:
				break;
			}
		});

	return true;
}
