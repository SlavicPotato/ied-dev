#pragma once

namespace IED
{
	class SPtrHolder
	{
	public:
		static void Initialize();

		[[nodiscard]] static constexpr const auto& GetSingleton() noexcept
		{
			return m_Instance;
		}

		[[nodiscard]] constexpr bool HasTimespan() const noexcept
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