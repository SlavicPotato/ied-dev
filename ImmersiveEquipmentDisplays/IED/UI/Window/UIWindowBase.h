#pragma once

#include "IED/UI/UIChildWindowID.h"

namespace IED
{
	namespace UI
	{
		class UIChildWindowBase
		{
		public:
			inline static constexpr auto CHILD_ID = ChildWindowID::kMax;

			UIChildWindowBase()                   = default;
			virtual ~UIChildWindowBase() noexcept = default;

			UIChildWindowBase(const UIChildWindowBase&) = delete;
			UIChildWindowBase(UIChildWindowBase&&)      = delete;
			UIChildWindowBase& operator=(const UIChildWindowBase&) = delete;
			UIChildWindowBase& operator=(UIChildWindowBase&&) = delete;

			void SetOpenState(bool a_state, bool a_notify = false);
			void ToggleOpenState(bool a_notify = false);

			[[nodiscard]] inline constexpr auto GetOpenState() noexcept
			{
				return std::addressof(m_openState);
			}

			[[nodiscard]] inline constexpr auto GetOpenState() const noexcept
			{
				return std::addressof(m_openState);
			}

			[[nodiscard]] inline constexpr auto IsWindowOpen() const noexcept
			{
				return m_openState;
			}

			virtual void Initialize(){};
			virtual void Draw(){};
			virtual void OnOpen(){};
			virtual void OnClose(){};
			virtual void Reset(){};

			void DrawWrapper();

		private:

			bool m_openState{ false };
			bool m_stateChanged{ false };
		};

	}
}