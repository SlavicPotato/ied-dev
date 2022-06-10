#pragma once

#include "IED/ConfigBlockList.h"
#include "IED/ConfigStore.h"

namespace IED
{
	class IMaintenance :
		public virtual ILog
	{
	private:
		static void CleanFormList(Data::configFormList_t& a_list);
		void        CleanCustomConfig(Game::FormID a_handle, Data::configCustomPluginMap_t& a_data);
		bool        CleanSlotConfig(Game::FormID a_handle, Data::configSlotHolder_t& a_data);

	public:
		void        CleanConfigStore(Data::configStore_t& a_data);
		static void ClearConfigStoreRand(Data::configStore_t& a_data);
		void        CleanBlockList(Data::actorBlockList_t& a_data);
	};
}