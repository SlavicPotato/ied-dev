#pragma once

namespace IED
{
	namespace UI
	{
		enum class UIPopupType : std::uint32_t
		{
			Confirm,
			Input,
			Message,
			Custom
		};

		class UIPopupQueue;

		class UIPopupAction
		{
			friend class UIPopupQueue;

			using func_type      = std::function<void(const UIPopupAction&)>;
			using func_type_draw = std::function<bool()>;

		public:
			UIPopupAction() = delete;

			UIPopupAction(const UIPopupAction&)            = delete;
			UIPopupAction& operator=(const UIPopupAction&) = delete;

			template <class... Args>
			UIPopupAction(
				UIPopupType a_type,
				const char* a_key,
				const char* a_fmt,
				Args... a_args) :
				m_type(a_type),
				m_key(a_key)
			{
				make_key();
				stl::snprintf(
					m_buf,
					a_fmt,
					a_args...);
			}

			UIPopupAction(
				UIPopupType a_type,
				const char* a_key) :
				m_type(a_type),
				m_key(a_key)
			{
				make_key();
			}

			auto& call(func_type a_func) noexcept(
				std::is_nothrow_move_assignable_v<func_type>)
			{
				m_func = std::move(a_func);
				return *this;
			}

			auto& draw(func_type_draw a_func) noexcept(
				std::is_nothrow_move_assignable_v<func_type_draw>)
			{
				m_funcDraw = std::move(a_func);
				return *this;
			}

			constexpr auto& set_text_wrap_size(float a_size) noexcept(
				std::is_nothrow_assignable_v<decltype(m_textWrapSize), float>)
			{
				m_textWrapSize = a_size;
				return *this;
			}

			template <class... Args>
			constexpr auto& fmt_input(const char* a_fmt, Args... a_args)
			{
				stl::snprintf(m_input, a_fmt, a_args...);
				return *this;
			}

			constexpr auto& GetInput() const noexcept
			{
				return m_input;
			}

		private:
			void make_key()
			{
				if (auto it = std::find(
						m_key.begin(),
						m_key.end(),
						'#');
				    it != m_key.end())
				{
					m_key.erase(it, m_key.end());
				}

				m_key += "###pa_key";
			}

			std::string m_key;

			char m_buf[512]{ 0 };
			char m_input[512]{ 0 };

			UIPopupType    m_type;
			func_type      m_func;
			func_type_draw m_funcDraw;

			std::optional<float> m_textWrapSize;
		};

	}
}