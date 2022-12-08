#include "pch.h"

#include "UIMiscTextInterface.h"

#include "UICommon.h"

#include "IED/Controller/IUI.h"

#include "UITipsData.h"

namespace IED
{
	namespace UI
	{
		UIMiscTextInterface::UIMiscTextInterface(IUI& a_iui) :
			m_iui(a_iui)
		{
		}

		/*void UIMiscTextInterface::QueueToast(
			const std::string& a_message) const
		{
			m_iui.QueueToast(a_message);
		}*/

		TextCopyableResult UIMiscTextInterface::TextCopyableImpl(func_t a_func, const char* a_fmt, ...) const
		{
			if (ImGui::GetCurrentWindow()->SkipItems)
			{
				return TextCopyableResult::kNone;
			}

			{
				va_list args;
				va_start(args, a_fmt);
				a_func(a_fmt, args);
				va_end(args);
			}

			if (!ImGui::IsItemHovered())
			{
				return TextCopyableResult::kNone;
			}

			UICommon::DrawItemUnderline(ImGuiCol_Text);

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				constexpr std::size_t BUFFER_SIZE = 4096;

				auto buffer = std::make_unique_for_overwrite<char[]>(BUFFER_SIZE);

				va_list args;
				va_start(args, a_fmt);
				::_vsnprintf_s(buffer.get(), BUFFER_SIZE, _TRUNCATE, a_fmt, args);
				va_end(args);

				ImGui::SetClipboardText(buffer.get());

				m_iui.QueueToast(UIL::L(UITip::CopiedToClipboard));

				return TextCopyableResult::kCopied;
			}
			else
			{
				return TextCopyableResult::kHovered;
			}
		}

		TextCopyableResult UIMiscTextInterface::TextCopyableImpl(
			func_t      a_imtextfunc,
			copy_func_t a_copyfunc,
			const char* a_fmt,
			...) const
		{
			if (ImGui::GetCurrentWindow()->SkipItems)
			{
				return TextCopyableResult::kNone;
			}

			{
				va_list args;
				va_start(args, a_fmt);
				a_imtextfunc(a_fmt, args);
				va_end(args);
			}

			if (!ImGui::IsItemHovered())
			{
				return TextCopyableResult::kNone;
			}

			UICommon::DrawItemUnderline(ImGuiCol_Text);

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				constexpr auto BUFFER_SIZE = 4096;

				auto buffer = std::make_unique<char[]>(BUFFER_SIZE);

				a_copyfunc(buffer.get(), BUFFER_SIZE);

				ImGui::SetClipboardText(buffer.get());

				m_iui.QueueToast(UIL::L(UITip::CopiedToClipboard));

				return TextCopyableResult::kCopied;
			}
			else
			{
				return TextCopyableResult::kHovered;
			}
		}
	}
}