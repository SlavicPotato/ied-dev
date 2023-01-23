#pragma once

#include "ConfigCommon.h"

namespace IED
{
	enum class FormInfoFlags : std::uint8_t
	{
		kNone = 0,

		kInventory   = 1u << 0,
		kValidCustom = 1u << 1,
		kValidSlot   = 1u << 2,

		kAll = (kInventory | kValidCustom | kValidSlot)
	};

	DEFINE_ENUM_CLASS_BITWISE(FormInfoFlags);

	class IFormCommon
	{
	public:
		static bool IsValidCustomForm(const TESForm* a_form) noexcept;
		static bool IsValidCustomFormType(std::uint8_t a_type) noexcept;
		static bool IsInventoryForm(const TESForm* a_form) noexcept;
		static bool IsInventoryFormType(std::uint8_t a_type) noexcept;
		static bool IsValidSlotForm(const TESForm* a_form) noexcept;
		static bool IsEquippableForm(const TESForm* a_form) noexcept;

		static stl::flag<FormInfoFlags> GetFormFlags(TESForm* a_form) noexcept;

		static std::string GetFormName(TESForm* a_form);

		template <class T>
		static constexpr bool HasKeywordImpl(
			const T*          a_form,
			const BGSKeyword* a_keyword)  //
			noexcept                      //
			requires(std::is_convertible_v<T*, BGSKeywordForm*>)
		{
			return static_cast<const T*>(a_form)->HasKeyword(a_keyword);
		}

		static bool HasKeyword(const TESForm* a_form, const BGSKeyword* a_keyword) noexcept;
		static bool HasKeyword(const TESForm* a_form, Game::FormID a_keyword) noexcept;
		static bool HasKeyword(const TESForm* a_form, const Data::configCachedForm_t& a_keyword) noexcept;

		template <class T>
		static constexpr bool HasKeyword(
			const T*          a_form,
			const BGSKeyword* a_keyword)  //
			noexcept                      //
			requires(std::is_convertible_v<T*, BGSKeywordForm*>)
		{
			return HasKeywordImpl(a_form, a_keyword);
		}

		template <class T>
		static constexpr bool HasKeyword(
			const T*     a_form,
			Game::FormID a_keyword)  //
			noexcept                 //
			requires(std::is_convertible_v<T*, BGSKeywordForm*>)
		{
			if (const auto* const keyword = a_keyword.As<BGSKeyword>())
			{
				return HasKeywordImpl(a_form, keyword);
			}
			else
			{
				return false;
			}
		}

		template <class T>
		static constexpr bool HasKeyword(
			const T*                        a_form,
			const Data::configCachedForm_t& a_keyword)  //
			noexcept                                    //
			requires(std::is_convertible_v<T*, BGSKeywordForm*>)
		{
			if (const auto* const keyword = a_keyword.get_form<BGSKeyword>())
			{
				return HasKeywordImpl(a_form, keyword);
			}

			return false;
		}

		template <class T>
		static constexpr bool HasKeyword(
			const T*          a_form,
			const BGSKeyword* a_keyword)  //
			noexcept                      //
			requires(std::is_convertible_v<T*, TESObjectREFR*>)
		{
			return static_cast<const T*>(a_form)->HasKeywordHelper(a_keyword);
		}

		template <class T>
		static constexpr bool HasKeyword(
			const T*     a_form,
			Game::FormID a_keyword)  //
			noexcept                 //
			requires(std::is_convertible_v<T*, TESObjectREFR*>)
		{
			if (const auto* const keyword = a_keyword.As<BGSKeyword>())
			{
				return static_cast<const T*>(a_form)->HasKeywordHelper(keyword);
			}
			else
			{
				return false;
			}
		}

		template <class T>
		static constexpr bool HasKeyword(
			const T*                        a_form,
			const Data::configCachedForm_t& a_keyword)  //
			noexcept                                    //
			requires(std::is_convertible_v<T*, TESObjectREFR*>)
		{
			if (const auto* const keyword = a_keyword.get_form<BGSKeyword>())
			{
				return static_cast<const T*>(a_form)->HasKeywordHelper(keyword);
			}

			return false;
		}
	};
}