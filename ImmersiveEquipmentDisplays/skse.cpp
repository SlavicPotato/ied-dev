#include "pch.h"

ISKSE ISKSE::m_Instance;

void ISKSE::CloseBacklog()
{
	gLog.RemoveSink(this);
	m_backlog.Clear();
}

void ISKSE::OnLogOpen()
{
	m_backlog.SetLimit(2000);
	gLog.AddSink(this);
}

const char* ISKSE::GetPluginName() const
{
	return PLUGIN_NAME;
};

std::uint32_t ISKSE::GetPluginVersion() const
{
	return MAKE_PLUGIN_VERSION(
		PLUGIN_VERSION_MAJOR,
		PLUGIN_VERSION_MINOR,
		PLUGIN_VERSION_REVISION);
};

bool ISKSE::CheckRuntimeVersion(std::uint32_t a_version) const
{
	return a_version >= RUNTIME_VERSION_1_5_39 &&
	       a_version <= RUNTIME_VERSION_1_5_97;
}

void ISKSE::Receive(const LoggerMessageEvent& a_evn)
{
	m_backlog.Add(a_evn);
}

/*bool ISKSE::CheckInterfaceVersion(std::uint32_t a_interfaceID, std::uint32_t
a_interfaceVersion, std::uint32_t a_compiledInterfaceVersion) const
{
    switch (a_interfaceID)
    {
    case SKSETaskInterface::INTERFACE_TYPE:
    case SKSEMessagingInterface::INTERFACE_TYPE:
    }
}*/