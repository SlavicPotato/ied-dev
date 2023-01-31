#pragma once

#include "UILocalizationInterface.h"

namespace IED
{
	class IUI;

	namespace UI
	{
		namespace concepts
		{
			template <class T>
			concept is_string_id = std::is_same_v<std::underlying_type_t<T>, Localization::StringID>;
		}

		enum class TextCopyableResult
		{
			kNone    = 0,
			kHovered = 1,
			kCopied  = 2
		};

		class UIMiscTextInterface
		{
		public:
			UIMiscTextInterface(IUI& a_iui);

			using copy_func_t = std::function<void(char*, std::size_t)>;

			template <class... Args>
			constexpr auto TextCopyable(const char* a_fmt, Args... a_args) const
			{
				return TextCopyableImpl(ImGui::TextV, a_fmt, a_args...);
			}

			template <class... Args>
			constexpr auto TextCopyable(copy_func_t a_copyfunc, const char* a_fmt, Args... a_args) const
			{
				return TextCopyableImpl(ImGui::TextV, a_copyfunc, a_fmt, a_args...);
			}

			template <class... Args>
			constexpr auto TextWrappedCopyable(const char* a_fmt, Args... a_args) const
			{
				return TextCopyableImpl(ImGui::TextWrappedV, a_fmt, a_args...);
			}

		private:
			using func_t = void (*)(const char*, va_list);

			TextCopyableResult TextCopyableImpl(func_t a_func, const char* a_fmt, ...) const;
			TextCopyableResult TextCopyableImpl(func_t a_imtextfunc, copy_func_t a_copyfunc, const char* a_fmt, ...) const;

			IUI& m_iui;
		};
	}
}