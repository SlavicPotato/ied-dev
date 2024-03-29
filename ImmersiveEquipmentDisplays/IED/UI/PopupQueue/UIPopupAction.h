#pragma once

namespace IED
{
	namespace UI
	{
		enum class UIPopupType : std::uint32_t
		{
			Confirm,
			Input,
			MultilineInput,
			Message,
			Custom
		};

		class UIPopupQueue;

		class UIPopupAction
		{
			friend class UIPopupQueue;

			using func_type      = std::function<void(const UIPopupAction&)>;
			using func_type_draw = std::function<bool()>;

			static constexpr std::size_t BUFFER_SIZE = 8192;

		public:
			UIPopupAction() = delete;

			UIPopupAction(const UIPopupAction&)            = delete;
			UIPopupAction& operator=(const UIPopupAction&) = delete;

			template <class... Args>
			explicit UIPopupAction(
				UIPopupType a_type,
				const char* a_key,
				const char* a_fmt,
				Args... a_args) :
				m_type(a_type),
				m_key(a_key)
			{
				make_key();

				::_snprintf_s(
					m_buf.get(),
					BUFFER_SIZE,
					_TRUNCATE,
					a_fmt,
					a_args...);
			}

			explicit UIPopupAction(
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

			template <class Ts>
			constexpr auto& set_input(const Ts& a_text)                         //
				noexcept(std::is_nothrow_assignable_v<std::string, const Ts&>)  //
				requires(std::is_assignable_v<std::string, const Ts&>)
			{
				m_input = a_text;
				sanitize_input();
				return *this;
			}

			template <class Ts>
			constexpr auto& set_input(Ts&& a_text)                         //
				noexcept(std::is_nothrow_assignable_v<std::string, Ts&&>)  //
				requires(std::is_assignable_v<std::string, Ts &&>)
			{
				m_input = std::move(a_text);
				sanitize_input();
				return *this;
			}

			constexpr auto& set_allow_empty(bool a_switch) noexcept
			{
				a_allowEmpty = a_switch;
				return *this;
			}

			[[nodiscard]] constexpr auto& GetInput() noexcept
			{
				return m_input;
			}

			[[nodiscard]] constexpr auto& GetInput() const noexcept
			{
				return m_input;
			}

		private:
			void sanitize_input();

			void make_key();

			std::string m_key;
			std::string m_input;

			std::unique_ptr<char[]> m_buf{ std::make_unique<char[]>(BUFFER_SIZE) };

			UIPopupType    m_type;
			bool           a_allowEmpty{ false };
			func_type      m_func;
			func_type_draw m_funcDraw;

			std::optional<float> m_textWrapSize;
		};

	}
}