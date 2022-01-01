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

		class UIPopupAction
		{
			friend class UIPopupQueue;

			using func_type = std::function<void(const UIPopupAction&)>;
			using func_type_draw = std::function<bool()>;

		public:
			UIPopupAction() = delete;

			UIPopupAction(const UIPopupAction&) = delete;
			UIPopupAction(UIPopupAction&&) = delete;

			UIPopupAction& operator=(const UIPopupAction&) = delete;
			UIPopupAction& operator=(UIPopupAction&&) = delete;

			template <class... Args>
			UIPopupAction(
				UIPopupType a_type,
				const char* a_key,
				const char* a_fmt,
				Args... a_args) noexcept
				:
				m_type(a_type),
				m_key(a_key)
			{
				mk_key();
				stl::snprintf(
					m_buf,
					a_fmt,
					std::forward<Args>(a_args)...);
			}

			template <class... Args>
			UIPopupAction(
				UIPopupType a_type,
				const char* a_key) noexcept
				:
				m_type(a_type),
				m_key(a_key)
			{
				mk_key();
			}

			auto& call(func_type a_func) noexcept
			{
				m_func = std::move(a_func);
				return *this;
			}

			auto& draw(func_type_draw a_func) noexcept
			{
				m_funcDraw = std::move(a_func);
				return *this;
			}

			inline constexpr const auto& GetInput() const noexcept
			{
				return m_input;
			}

		private:
			void mk_key() noexcept
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

			UIPopupType m_type;
			func_type m_func;
			func_type_draw m_funcDraw;
		};

	}
}