#include "pch.h"

#include "UICommonModals.h"

namespace IED
{
	namespace UI
	{
		UICommonModals::UICommonModals(
			Localization::ILocalization& a_localization) :
			UILocalizationInterface(a_localization)
		{
		}

		auto UICommonModals::TextInputDialog(
			const char*                 a_name,
			const char*                 a_text,
			ImGuiInputTextFlags         a_flags,
			const cm_func_t&            a_func,
			const std::optional<float>& a_twsz,
			char*                       a_buf,
			std::size_t                 a_size)
			-> ModalStatus
		{
			return TextInputDialog(
				a_name,
				"%s",
				a_flags,
				a_func,
				a_twsz,
				a_buf,
				a_size,
				a_text);
		}

		auto UICommonModals::MessageDialog(
			const char*                 name,
			const cm_func_t&            a_func,
			const std::optional<float>& a_twsz,
			const char*                 text)
			-> ModalStatus
		{
			return MessageDialog(name, a_func, a_twsz, "%s", text);
		}

		auto UICommonModals::ConfirmDialog(
			const char*                 name,
			const cm_func_t&            a_func,
			const std::optional<float>& a_twsz,
			const char*                 text)
			-> ModalStatus
		{
			return ConfirmDialog(name, a_func, a_twsz, "%s", text);
		}

	}
}