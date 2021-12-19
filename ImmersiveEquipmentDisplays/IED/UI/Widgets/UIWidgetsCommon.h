#pragma once

#define sshex(size) \
	std::uppercase << std::setfill('0') << std::setw(size) << std::hex

#include "IED/ConfigOverride.h"

#include "UIWidgetCommonStrings.h"

namespace IED
{
	namespace UI
	{
		enum class PostChangeAction
		{
			None,
			Reset,
			Evaluate,
			UpdateTransform,
			AttachNode
		};

		using entrySlotData_t = Data::configStoreSlot_t::result_copy;

		/*template <class T, class U>
		struct configItemExtra_t
		{
			T config;
			U extra;
		};

		struct customConfigExtraData_t
		{

		};*/

		//using customConfigItem_t = configItemExtra_t<Data::configCustom_t, customConfigExtraData_t>;

		using entryCustomData_t = Data::configCustomHolder_t;
		using entryNodeOverrideData_t = Data::configNodeOverrideHolder_t;

		template <class T>
		struct profileSelectorParamsSlot_t
		{
			T handle;
			entrySlotData_t& data;
		};

		template <class T>
		struct profileSelectorParamsCustom_t
		{
			T handle;
			entryCustomData_t& data;
		};

		template <class T>
		struct profileSelectorParamsNodeOverride_t
		{
			T handle;
			entryNodeOverrideData_t& data;
		};

		struct sexInfo_t
		{
			Data::ConfigSex sex;
			const char* name;
		};

		enum class SwapDirection : std::uint8_t
		{
			Up,
			Down
		};

		template <class T>
		bool IterSwap(T& a_data, typename T::iterator& a_it, SwapDirection a_dir)
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

		sexInfo_t GetOppositeSex2(Data::ConfigSex a_sex);

		void DrawConfigClassInUse(Data::ConfigClass a_class);

	}  // namespace UI
}  // namespace IED