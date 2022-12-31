#pragma once

namespace IED
{
	class Controller;

	class Initializer :
		ILog,
		::Events::EventSink<SKSEMessagingEvent>
	{
		inline static constexpr std::uint32_t SKSE_SERIALIZATION_UID = 'ADEI';

	public:
		[[nodiscard]] inline static constexpr auto& GetSingleton() noexcept
		{
			return m_Instance;
		}

		[[nodiscard]] inline static constexpr auto* GetController() noexcept
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
		bool m_initRefrLightController{ false };

		static Initializer m_Instance;
	};

}