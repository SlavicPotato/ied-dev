#pragma once

#include "IED/UI/UIChildWindowID.h"

#include "Drivers/Input/Handlers.h"

namespace IED
{
	namespace UI
	{
		class UIContext
		{
		public:

			UIContext()                   = default;
			virtual ~UIContext() noexcept = default;

			UIContext(const UIContext&) = delete;
			UIContext(UIContext&&)      = delete;
			UIContext& operator=(const UIContext&) = delete;
			UIContext& operator=(UIContext&&) = delete;

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

			[[nodiscard]] inline constexpr auto IsContextOpen() const noexcept
			{
				return m_openState;
			}

			virtual void Initialize(){};
			virtual void Draw(){};
			virtual void OnOpen(){};
			virtual void OnClose(){};
			virtual void Reset(){};
			virtual void Notify(std::uint32_t a_code, void* a_params = nullptr){};

			virtual void PrepareGameData(){};
			virtual void Render(){};
			virtual void OnMouseMove(const Handlers::MouseMoveEvent& a_evn){};

			//virtual void OnOpenStateChanged(bool a_newState){};

			void DrawWrapper();

		private:
			bool m_openState{ false };
			bool m_stateChanged{ false };
		};

	}
}