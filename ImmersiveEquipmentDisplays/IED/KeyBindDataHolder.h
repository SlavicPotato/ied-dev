#pragma once

#include "KeyToggleStateEntryHolder.h"

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

			[[nodiscard]] constexpr const auto& GetLastException() const noexcept
			{
				return m_lastException;
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
			KeyToggleStateEntryHolder::state_data GetKeyToggleStates() const;
			void InitializeKeyToggleStates(const KeyToggleStateEntryHolder::state_data& a_states) noexcept;

			bool GetKeyState(const stl::fixed_string& a_id) const noexcept;

			bool Save(const fs::path& a_path) const;
			bool Load(const fs::path& a_path);

			template <class T>
			bool SaveIfDirty(const T& a_path) const
			{
				return m_dirty ? Save(a_path) : true;
			}

			constexpr void MarkDirty() noexcept
			{
				m_dirty = true;
			}

		private:
			mutable stl::fast_spin_lock m_lock;
			KeyToggleStateEntryHolder   m_data;
			mutable except::descriptor  m_lastException;
			mutable bool                m_dirty{ false };
		};
	}
}