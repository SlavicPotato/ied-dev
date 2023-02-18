#pragma once

namespace IED
{
	class KeyToggleStates
	{
		friend class boost::serialization::access;

	public:
		enum : unsigned int
		{
			DataVersion1 = 1
		};

		[[nodiscard]] constexpr bool GetKeyToggleState(std::uint8_t a_id) const noexcept
		{
			assert(a_id < 64);
			return static_cast<bool>((m_states >> a_id) & 0x1);
		}
		
		[[nodiscard]] constexpr auto GetKeyToggleStates() const noexcept
		{
			return m_states;
		}

		constexpr void UpdateKeyToggleStates(std::uint64_t a_mask, std::uint64_t a_newStates) noexcept
		{
			m_states = (m_states & ~a_mask) | a_newStates;
		}

	private:
		std::uint64_t m_states{ 0 };

		template <class Archive>
		void serialize(Archive& a_ar, const unsigned int a_version)
		{
			a_ar& m_states;
		}
	};
}

BOOST_CLASS_VERSION(
	::IED::KeyToggleStates,
	::IED::KeyToggleStates::DataVersion1);
