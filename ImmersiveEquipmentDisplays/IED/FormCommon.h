#pragma once

namespace IED
{
	namespace Data
	{
		struct configCachedForm_t;
	}

	enum class FormInfoFlags : std::uint8_t
	{
		kNone = 0,

		kInventory   = 1u << 0,
		kValidCustom = 1u << 1,
		kValidSlot   = 1u << 2,

		kAll = (kInventory | kValidCustom | kValidSlot)
	};

	DEFINE_ENUM_CLASS_BITWISE(FormInfoFlags);

	template <class T>
	concept AcceptHasKeyword =
		std::is_base_of_v<TESForm, T> &&
		requires(T a_form, BGSKeyword* a_keyword)
	{
		{
			a_form.keyword.HasKeyword(a_keyword)
			} -> std::same_as<bool>;
	};

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

		static bool HasKeyword(TESForm* a_form, BGSKeyword* a_keyword);
		static bool HasKeyword(TESForm* a_form, Game::FormID a_keyword);
		static bool HasKeyword(TESForm* a_form, const Data::configCachedForm_t& a_keyword);

		template <AcceptHasKeyword T>
		inline static constexpr bool HasKeyword(T* a_form, BGSKeyword* a_keyword) noexcept
		{
			return a_form->keyword.HasKeyword(a_keyword);
		}

		template <AcceptHasKeyword T>
		inline static constexpr bool HasKeyword(
			T*           a_form,
			Game::FormID a_keyword)
		{
			if (auto keyword = a_keyword.As<BGSKeyword>())
			{
				return a_form->keyword.HasKeyword(keyword);
			}
			else
			{
				return false;
			}
		}

		template <AcceptHasKeyword T>
		inline static constexpr bool HasKeyword(
			T*                              a_form,
			const Data::configCachedForm_t& a_keyword)
		{
			if (auto keyword = a_keyword.get_form<BGSKeyword>())
			{
				return a_form->keyword.HasKeyword(keyword);
			}

			return false;
		}
	};
}