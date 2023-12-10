#pragma once

namespace IED
{
	class Controller;
	class ConfigINI;
	class PluginInterface;

	class Initializer :
		ILog,
		::Events::EventSink<SKSEMessagingEvent>
	{
		static constexpr std::uint32_t SKSE_SERIALIZATION_UID = 'ADEI';

	public:
		[[nodiscard]] static constexpr auto& GetSingleton() noexcept
		{
			return m_Instance;
		}

		[[nodiscard]] static constexpr auto* GetController() noexcept
		{
			return m_Instance.m_controller;
		}

		[[nodiscard]] static constexpr auto* GetPluginInterface() noexcept
		{
			return m_Instance.m_pluginInterface.get();
		}

		bool Run(const SKSEInterface* a_skse);

		FN_NAMEPROC("Init");

	private:
		bool InitUI(bool a_enableDPIAwarness) const;
		void RunChecks();

		static const char* GetLanguage();

		void SetupSKSEEventHandlers(const SKSEInterface* a_skse) const;

		virtual void Receive(const SKSEMessagingEvent& a_evn) override;

		Controller*                      m_controller{ nullptr };
		stl::smart_ptr<const ConfigINI>  m_config;
		std::unique_ptr<PluginInterface> m_pluginInterface;
		bool                             m_done{ false };

		static Initializer m_Instance;
	};

}