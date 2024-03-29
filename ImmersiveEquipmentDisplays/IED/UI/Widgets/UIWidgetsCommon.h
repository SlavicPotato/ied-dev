#pragma once

#define sshex(size) \
	std::uppercase << std::setfill('0') << std::setw(size) << std::hex

#include "IED/ConfigStore.h"

#include "UIWidgetCommonStrings.h"

namespace IED
{
	namespace UI
	{
		using UIGlobalEditorDummyHandle = Game::FormID;

		enum class PostChangeAction
		{
			None,
			Reset,
			Evaluate,
			UpdateTransform
		};

		enum class SwapDirection : std::uint8_t
		{
			None,
			Up,
			Down
		};

		template <class T>
		constexpr bool IterSwap(
			T&                    a_data,
			typename T::iterator& a_it,
			SwapDirection         a_dir)
		{
			auto sit = a_it;

			if (a_dir == SwapDirection::Up)
			{
				if (a_it != a_data.begin())
				{
					sit = std::prev(a_it);
				}
			}
			else if (a_dir == SwapDirection::Down)
			{
				if (a_it != a_data.end())
				{
					sit = std::next(a_it);
				}
			}

			if (sit != a_it && sit != a_data.end())
			{
				std::iter_swap(a_it, sit);
				return true;
			}
			else
			{
				return false;
			}
		}

		void DrawConfigClassInUse(Data::ConfigClass a_class);


	}
}