#pragma once

#include <ext/ConcreteFormFactory.h>

namespace IED
{
	namespace OM
	{
		class BGSOutfitHolder :
			public stl::intrusive_ref_counted
		{
		public:
			BGSOutfitHolder();

			explicit BGSOutfitHolder(
				Game::FormID             a_formid,
				RE::BSTArray<TESForm*>&& a_items,
				BSFixedString&&          a_name);

			explicit BGSOutfitHolder(
				BGSOutfit*               a_form,
				RE::BSTArray<TESForm*>&& a_items,
				BSFixedString&&          a_name);

			template <class Ts>
			explicit BGSOutfitHolder(
				Ts&& a_name)
			{
				if (const auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<BGSOutfit>())
				{
					if (const auto form = factory->Create())
					{
						m_name = std::forward<Ts>(a_name);
						m_form = form;
					}
				}
			}

			~BGSOutfitHolder();

			BGSOutfitHolder(const BGSOutfitHolder&)            = delete;
			BGSOutfitHolder& operator=(const BGSOutfitHolder&) = delete;

			[[nodiscard]] constexpr BGSOutfit* operator->() noexcept
			{
				assert(m_form);
				return m_form;
			}

			[[nodiscard]] constexpr const BGSOutfit* operator->() const noexcept
			{
				assert(m_form);
				return m_form;
			}

			[[nodiscard]] constexpr BGSOutfit* get() noexcept
			{
				return m_form;
			}

			[[nodiscard]] constexpr const BGSOutfit* get() const noexcept
			{
				return m_form;
			}

			[[nodiscard]] constexpr auto& get_name() const noexcept
			{
				return m_name;
			}

			template <class Ts>
			constexpr void set_name(Ts&& a_arg)  //
				noexcept(std::is_nothrow_assignable_v<BSFixedString, Ts&&>)
				requires(std::is_assignable_v<BSFixedString, Ts &&>)
			{
				m_name = std::forward<Ts>(a_arg);
			}

			[[nodiscard]] constexpr explicit operator bool() const noexcept
			{
				return static_cast<bool>(m_form);
			}

			void on_load_discard();

		private:
			void FailsafeClearOutfitForms();

			BGSOutfit*    m_form{ nullptr };
			BSFixedString m_name;
		};

		using BGSOutfitHolderPtr = stl::smart_ptr<BGSOutfitHolder>;
	}
}