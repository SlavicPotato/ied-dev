#pragma once

namespace IED
{
	class Controller;
	class ConfigINI;

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

		bool Run(const SKSEInterface* a_skse);

		FN_NAMEPROC("Init");

	private:

		void RunChecks();

		static const char* GetLanguage();

		void SetupSKSEEventHandlers(const SKSEInterface* a_skse);

		virtual void Receive(const SKSEMessagingEvent& a_evn) override;

		Controller* m_controller{ nullptr };

		bool m_done{ false };

		std::shared_ptr<ConfigINI> m_config;

		static Initializer m_Instance;
	};

}