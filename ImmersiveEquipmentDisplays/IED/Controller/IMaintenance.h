#pragma once

#include "IED/ConfigBlockList.h"
#include "IED/ConfigStore.h"

namespace IED
{
	class IMaintenance
	{
	private:
		static void CleanFormList(Data::configFormList_t& a_list);
		static void CleanCustomConfig(Game::FormID a_handle, Data::configCustomPluginMap_t& a_data);
		static bool CleanSlotConfig(Game::FormID a_handle, Data::configSlotHolder_t& a_data);

	public:
		static void CleanConfigStore(Data::configStore_t& a_data);
		static void ClearConfigStoreRand(Data::configStore_t& a_data);
		static void CleanBlockList(Data::actorBlockList_t& a_data);
	};
}