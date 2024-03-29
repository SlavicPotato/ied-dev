#include "pch.h"

#include "UICommonModals.h"

namespace IED
{
	namespace UI
	{
		auto UICommonModals::TextInputDialog(
			const char*                 a_name,
			const char*                 a_text,
			ImGuiInputTextFlags         a_flags,
			const cm_func_t&            a_func,
			const std::optional<float>& a_twsz,
			bool                        a_allowEmpty,
			std::string&                a_out)
			-> ModalStatus
		{
			return TextInputDialog(
				a_name,
				"%s",
				a_flags,
				a_func,
				a_twsz,
				a_allowEmpty,
				a_out,
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