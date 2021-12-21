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

		kInventory = 1u << 0,
		kValidCustom = 1u << 1,
		kValidSlot = 1u << 2,

		kAll = (kInventory | kValidCustom | kValidSlot)
	};

	DEFINE_ENUM_CLASS_BITWISE(FormInfoFlags);

	class IFormCommon
	{
	public:
		static bool IsValidCustomForm(TESForm* a_form);
		static bool IsInventoryForm(TESForm* a_form);
		static bool IsInventoryFormType(std::uint8_t a_type);
		static bool IsValidSlotForm(TESForm* a_form);
		static bool IsEquippableForm(TESForm* a_form);

		static stl::flag<FormInfoFlags> GetFormFlags(TESForm* a_form);

		static std::string ConvertToUTF8(const char* a_in);
		static std::string GetFormName(TESForm* a_form);
		static const char* GetFormTypeDesc(std::uint8_t a_type);

		static bool HasKeyword(TESForm* a_form, BGSKeyword* a_keyword);
		static bool HasKeyword(TESForm* a_form, Game::FormID a_keyword);
		static bool HasKeyword(TESForm* a_form, const Data::configCachedForm_t& a_keyword);

		template <class T>
		inline static constexpr bool HasKeyword(T* a_form, BGSKeyword* a_keyword) noexcept
		{
			return a_form->keyword.HasKeyword(a_keyword);
		}

		template <class T>
		inline static bool HasKeyword(
			T* a_form,
			Game::FormID a_keyword)
		{
			if (auto keyword = a_keyword.As<BGSKeyword>())
			{
				return HasKeyword(a_form, keyword);
			}
			else
			{
				return false;
			}
		}

		template <class T>
		inline static constexpr bool HasKeyword(
			T* a_form,
			const Data::configCachedForm_t& a_keyword) 
		{
			if (auto form = a_keyword.get_form())
			{
				if (auto keyword = form->As<BGSKeyword>())
				{
					return HasKeyword(a_form, keyword);
				}
			}

			return false;
		}
	};
}