#include "pch.h"

ISKSE ISKSE::m_Instance;

void ISKSE::CloseBacklog()
{
	gLog.RemoveSink(std::addressof(m_Instance));
	m_Instance.m_backlog.Clear();
}

void ISKSE::OnLogOpen()
{
	m_backlog.SetLimit(200);
	gLog.AddSink(this);
}

const char* ISKSE::GetLogPath() const
{
	return PLUGIN_LOG_PATH;
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
	return a_version >= MIN_RUNTIME_VERSION;
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