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
		static bool IsValidCustomForm(TESForm* a_form);
		static bool IsValidCustomFormType(std::uint8_t a_type);
		static bool IsInventoryForm(TESForm* a_form);
		static bool IsInventoryFormType(std::uint8_t a_type);
		static bool IsValidSlotForm(TESForm* a_form);
		static bool IsEquippableForm(TESForm* a_form);

		static stl::flag<FormInfoFlags> GetFormFlags(TESForm* a_form);

		static std::string GetFormName(TESForm* a_form);

		template <class T>
		inline static constexpr bool HasKeywordImpl(
			T*          a_form,
			BGSKeyword* a_keyword)  //
			requires(std::is_convertible_v<T*, BGSKeywordForm*>)
		{
			return a_form->HasKeyword(a_keyword);
		}

		template <class T>
		inline static constexpr bool FormHasKeywordImpl(
			TESForm*    a_form,
			BGSKeyword* a_keyword)  //
			requires(std::is_convertible_v<T*, BGSKeywordForm*>)
		{
			return HasKeywordImpl(static_cast<T*>(a_form), a_keyword);
		}

		static bool HasKeyword(TESForm* a_form, BGSKeyword* a_keyword);
		static bool HasKeyword(TESForm* a_form, Game::FormID a_keyword);
		static bool HasKeyword(TESForm* a_form, const Data::configCachedForm_t& a_keyword);

		template <class T>
		inline static constexpr bool HasKeyword(
			T*          a_form,
			BGSKeyword* a_keyword)  //
			requires(std::is_convertible_v<T*, BGSKeywordForm*>)
		{
			return HasKeywordImpl(a_form, a_keyword);
		}

		template <class T>
		inline static constexpr bool HasKeyword(
			T*           a_form,
			Game::FormID a_keyword)  //
			requires(std::is_convertible_v<T*, BGSKeywordForm*>)
		{
			if (auto keyword = a_keyword.As<BGSKeyword>())
			{
				return HasKeywordImpl(a_form, keyword);
			}
			else
			{
				return false;
			}
		}

		template <class T>
		inline static constexpr bool HasKeyword(
			T*                              a_form,
			const Data::configCachedForm_t& a_keyword)  //
			requires(std::is_convertible_v<T*, BGSKeywordForm*>)
		{
			if (auto keyword = a_keyword.get_form<BGSKeyword>())
			{
				return HasKeywordImpl(a_form, keyword);
			}

			return false;
		}
	};
}