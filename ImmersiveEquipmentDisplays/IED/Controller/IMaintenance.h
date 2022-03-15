#pragma once

#include "IED/ConfigBlockList.h"
#include "IED/ConfigStore.h"

namespace IED
{
	class IMaintenance :
		public virtual ILog
	{
	private:
		void CleanFormList(Data::configFormList_t& a_list);
		void CleanCustomConfig(Game::FormID a_id, Data::configCustomPluginMap_t& a_data);
		bool CleanSlotConfig(Data::configSlotHolder_t& a_data);

	public:
		void CleanConfigStore(Data::configStore_t& a_data);
		void CleanBlockList(Data::actorBlockList_t& a_data);
	};
}