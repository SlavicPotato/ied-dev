#pragma once

#include "UIContext.h"
#include "UILocalizationInterface.h"

#include "Window/UIWindow.h"

#include "UICommon.h"

namespace IED
{
	class Controller;

	namespace Tasks
	{
		class UIRenderTaskBase;
	};

	namespace UI
	{
		namespace detail
		{
			template <class T>
			concept accept_toast_msg_type = std::is_convertible_v<T, std::string>;
		}

		class UIToast :
			public UIContext,
			public UIWindow
		{
			inline static constexpr auto WINDOW_ID = "ied_toast";

			inline static constexpr auto MESSAGE_LIFETIME = 2.0f;

			struct ToastMessage
			{
			public:
				std::string           text;
				luid_tag              tag;
				float                 lifetime;
				std::optional<ImVec4> color;
			};

		public:
			UIToast(Tasks::UIRenderTaskBase& a_owner);

			virtual ~UIToast() noexcept override = default;

			virtual void Draw() override;

			virtual std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(-1);
			}

			template <detail::accept_toast_msg_type Tm, class... Args>
			void QueueMessage(
				Tm&& a_message,
				Args&&... a_args)
			{
				const stl::lock_guard lock(m_lock);

				if (!m_queue.empty())
				{
					auto& front = m_queue.front();

					if (hash::stricmp(front.text, a_message) == 0)
					{
						front = {
							std::forward<Tm>(a_message),
							ILUID()(),
							MESSAGE_LIFETIME,
							std::forward<Args>(a_args)...
						};

						return;
					}
				}

				m_queue.emplace(
					std::forward<Tm>(a_message),
					ILUID()(),
					MESSAGE_LIFETIME,
					std::forward<Args>(a_args)...);
			}

		private:
			stl::queue<ToastMessage>                        m_queue;
			std::optional<luid_tag>                         m_currentTag;
			UICommon::float_anim_t<float, 0.0f, 1.0f, 8.0f> m_animbg;
			stl::mutex                                      m_lock;

			Tasks::UIRenderTaskBase& m_owner;
		};
	}
}