#pragma once

namespace IED
{
	class ISessionCycleCounter
	{
		friend class boost::serialization::access;

	public:
		enum : unsigned int
		{
			DataVersion1 = 1,
		};

		inline constexpr void IncrementCycles() noexcept
		{
			m_cycles++;
		}

		inline constexpr auto GetNumCycles() const noexcept
		{
			return m_cycles;
		}

		inline constexpr void ResetCycles() noexcept
		{
			m_cycles = 0;
		}

	private:
		template <class Archive>
		void serialize(Archive& a_ar, const unsigned int a_version)
		{
			a_ar& m_cycles;
		}

		std::uint64_t m_cycles{ 0 };
	};
}

BOOST_CLASS_VERSION(
	::IED::ISessionCycleCounter,
	::IED::ISessionCycleCounter::DataVersion1);
