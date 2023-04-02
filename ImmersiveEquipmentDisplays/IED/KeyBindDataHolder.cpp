#include "pch.h"

#include "KeyBindDataHolder.h"

namespace IED
{
	namespace KB
	{
		void KeyBindDataHolder::ResetKeyToggleStates() noexcept
		{
			const stl::lock_guard lock(m_lock);

			for (auto& e : m_data.entries)
			{
				e.second.SetState(0);
			}
		}

		KeyToggleStateEntryHolder::state_data2 KeyBindDataHolder::GetKeyToggleStates() const
		{
			const stl::lock_guard lock(m_lock);

			return m_data.make_state_data();
		}

		void KeyBindDataHolder::Clear()
		{
			const stl::lock_guard lock(m_lock);

			m_data.entries.clear();
			m_data.entries.shrink_to_fit();
		}

		void KeyBindDataHolder::SetFromConfig(const Data::configKeybindEntryHolder_t& a_data)
		{
			const stl::lock_guard lock(m_lock);

			m_data.entries.clear();

			for (auto& e : a_data.data)
			{
				m_data.entries.emplace(e.first, e.second);
			}

			m_data.entries.shrink_to_fit();
		}

		void KeyBindDataHolder::MergeFromConfig(const Data::configKeybindEntryHolder_t& a_data)
		{
			const stl::lock_guard lock(m_lock);

			for (auto& e : a_data.data)
			{
				m_data.entries.insert_or_assign(e.first, e.second);
			}
		}

		bool KeyBindDataHolder::GetKeyState(
			const stl::fixed_string& a_id,
			std::uint32_t&           a_stateOut) const noexcept
		{
			const stl::lock_guard lock(m_lock);

			auto& entries = m_data.entries;

			const auto it = entries.find(a_id);

			if (it == entries.end())
			{
				return false;
			}

			a_stateOut= it->second.GetState();

			return true;
		}

	}
}