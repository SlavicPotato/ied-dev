#pragma once

namespace IED
{
	class IPersistentCounter
	{
		friend class boost::serialization::access;

	public:
		enum class PersistentCounterSerializationVersion : unsigned int
		{
			DataVersion1 = 1,
		};

		inline constexpr auto IncrementCounter() noexcept
		{
			return m_counter++;
		}

		[[nodiscard]] inline constexpr auto GetCounterValue() const noexcept
		{
			return m_counter;
		}

		inline constexpr void ResetCounter() noexcept
		{
			m_counter = 0;
		}

	private:
		template <class Archive>
		void serialize(Archive& a_ar, const unsigned int a_version)
		{
			a_ar& m_counter;
		}

		std::uint64_t m_counter{ 0 };
	};
}

BOOST_CLASS_VERSION(
	::IED::IPersistentCounter,
	::stl::underlying(::IED::IPersistentCounter::PersistentCounterSerializationVersion::DataVersion1));
