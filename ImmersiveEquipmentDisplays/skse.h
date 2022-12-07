#pragma once

#include <ext/ISKSE.h>
#include <ext/ISKSESerialization.h>

class ISKSE :
	public ISKSEBase<
		SKSEInterfaceFlags::kMessaging |
			SKSEInterfaceFlags::kSerialization |
			SKSEInterfaceFlags::kTrampoline |
			SKSEInterfaceFlags::kPapyrus,
		304,
		384>,
	public ISKSESerialization,
	::Events::EventSink<LoggerMessageEvent>
{
public:
	[[nodiscard]] inline static constexpr auto& GetSingleton()
	{
		return m_Instance;
	}

	[[nodiscard]] inline static constexpr auto& GetBranchTrampoline()
	{
		return m_Instance.GetTrampoline(TrampolineID::kBranch);
	}

	[[nodiscard]] inline static constexpr auto& GetLocalTrampoline()
	{
		return m_Instance.GetTrampoline(TrampolineID::kLocal);
	}

	[[nodiscard]] inline static constexpr auto& GetBacklog()
	{
		return m_Instance.m_backlog;
	}

	void CloseBacklog();

private:
	ISKSE() = default;

	virtual void          OnLogOpen() override;
	virtual const char*   GetPluginName() const override;
	virtual std::uint32_t GetPluginVersion() const override;
	virtual bool          CheckRuntimeVersion(std::uint32_t a_version) const override;

	virtual void Receive(const LoggerMessageEvent& a_evn) override;

	BackLog m_backlog;

	static ISKSE m_Instance;
};