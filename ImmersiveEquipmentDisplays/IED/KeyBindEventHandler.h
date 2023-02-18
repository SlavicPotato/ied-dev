#pragma once

#include "Drivers/Input/Handlers.h"

#include "KeyBindDataHolder.h"

namespace IED
{
	namespace KB
	{
		class KeyBindEventHandler :
			public Events::EventSink<Handlers::KeyEvent>
		{
		public:
			KeyBindEventHandler(
				const stl::smart_ptr<KeyBindDataHolder>& a_data)  //
				noexcept(std::is_nothrow_copy_constructible_v<stl::smart_ptr<KeyBindDataHolder>>) :
				m_data(a_data)
			{
			}

			KeyBindEventHandler(
				stl::smart_ptr<KeyBindDataHolder>&& a_data)  //
				noexcept(std::is_nothrow_move_constructible_v<stl::smart_ptr<KeyBindDataHolder>>) :
				m_data(std::move(a_data))
			{
			}

			[[nodiscard]] constexpr auto& GetKeyBindDataHolder() const noexcept
			{
				return m_data;
			}

		private:
			virtual void Receive(const Handlers::KeyEvent& a_evn) override;
			virtual void OnKBStateChanged() = 0;

			stl::smart_ptr<KeyBindDataHolder> m_data;
		};
	}
}