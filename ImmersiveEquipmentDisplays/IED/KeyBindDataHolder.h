#pragma once

#include "KeyToggleStateEntryHolder.h"

#include "IED/ConfigKeybind.h"

namespace IED
{
	namespace KB
	{
		class KeyBindDataHolder :
			public stl::intrusive_ref_counted
		{
		public:
			[[nodiscard]] constexpr auto& GetData() noexcept
			{
				return m_data;
			}

			[[nodiscard]] constexpr auto& GetData() const noexcept
			{
				return m_data;
			}

			[[nodiscard]] constexpr auto& GetLock() const noexcept
			{
				return m_lock;
			}

			template <class Tf>
			void visit(Tf a_func) const                                                                            //
				noexcept(std::is_nothrow_invocable_v<Tf, KeyToggleStateEntryHolder::container_type::value_type&>)  //
				requires(std::invocable<Tf, KeyToggleStateEntryHolder::container_type::value_type&>)
			{
				const stl::lock_guard lock(m_lock);

				for (auto& e : m_data.entries)
				{
					a_func(e);
				}
			}

			template <class Tf>
			void visit(Tf a_func)                                                                                  //
				noexcept(std::is_nothrow_invocable_v<Tf, KeyToggleStateEntryHolder::container_type::value_type&>)  //
				requires(std::invocable<Tf, KeyToggleStateEntryHolder::container_type::value_type&>)
			{
				const stl::lock_guard lock(m_lock);

				for (auto& e : m_data.entries)
				{
					a_func(e);
				}
			}

			void ResetKeyToggleStates() noexcept;
			
			KeyToggleStateEntryHolder::state_data2 GetKeyToggleStates() const;

			void Clear();
			void SetFromConfig(const Data::configKeybindEntryHolder_t& a_data);
			void MergeFromConfig(const Data::configKeybindEntryHolder_t& a_data);

			template <class T>
			void InitializeKeyToggleStates(const T& a_states) noexcept //
				requires(std::is_integral_v<typename T::value_type::second_type>);

			bool GetKeyState(const stl::fixed_string& a_id, std::uint32_t &a_stateOut) const noexcept;

		private:
			mutable stl::fast_spin_lock m_lock;
			KeyToggleStateEntryHolder   m_data;
		};

		template <class T>
		void KeyBindDataHolder::InitializeKeyToggleStates(
			const T& a_states) noexcept //
			requires(std::is_integral_v<typename T::value_type::second_type>)
		{
			const stl::lock_guard lock(m_lock);

			auto& entries = m_data.entries;

			for (auto& e : entries)
			{
				e.second.SetState(0);
			}

			for (auto& e : a_states)
			{
				const auto it = entries.find(e.first);

				if (it != entries.end())
				{
					it->second.SetState(static_cast<std::uint32_t>(e.second));
				}
			}
		}
	}
}