#pragma once

namespace IED
{
	class SPtrHolder
	{
	public:
		static void Initialize();

		[[nodiscard]] static constexpr auto& GetSingleton() noexcept
		{
			return m_Instance;
		}

		[[nodiscard]] constexpr bool HasTimespan() noexcept
		{
			return m_hasTimespan;
		}

		float* fTorchLightLevelNight{ nullptr };
		float* fTimeSpanSunriseStart{ nullptr };
		float* fTimeSpanSunriseEnd{ nullptr };
		float* fTimeSpanSunsetStart{ nullptr };
		float* fTimeSpanSunsetEnd{ nullptr };

	private:
		void InitializeImpl();

		bool m_hasTimespan{ false };

		static SPtrHolder m_Instance;
	};
}