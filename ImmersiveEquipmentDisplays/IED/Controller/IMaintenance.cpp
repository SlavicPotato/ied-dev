#include "pch.h"

#include "IED/Data.h"
#include "IED/StringHolder.h"
#include "IMaintenance.h"

namespace IED
{
	using namespace Data;

	void IMaintenance::CleanFormList(Data::configFormList_t& a_list)
	{
		auto it = a_list.begin();
		while (it != a_list.end())
		{
			if (!*it)
			{
				it = a_list.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void IMaintenance::CleanCustomConfig(
		Game::FormID                   a_id,
		Data::configCustomPluginMap_t& a_data)
	{
		for (auto it = a_data.begin(); it != a_data.end();)
		{
			if (it->second.empty())
			{
				it = a_data.erase(it);
			}
			else
			{
				if (it->first != StringHolder::GetSingleton().IED)
				{
					auto& pluginInfo = IData::GetPluginInfo().GetLookupRef();
					if (!pluginInfo.contains(it->first))
					{
						Warning(
							"%.8X: erasing %zu custom item(s) from missing plugin '%s'",
							a_id.get(),
							it->second.data.size(),
							it->first.c_str());

						it = a_data.erase(it);
						continue;
					}
				}

				for (auto& h : it->second.data)
				{
					for (auto& i : h.second())
					{
						CleanFormList(i.extraItems);
						if (i.filters)
						{
							i.filters->raceFilter.allow.erase(0);
							i.filters->raceFilter.deny.erase(0);
							i.filters->npcFilter.allow.erase(0);
							i.filters->npcFilter.deny.erase(0);
							i.filters->actorFilter.allow.erase(0);
							i.filters->actorFilter.deny.erase(0);
						}
					}
				}

				++it;
			}
		}
	}

	bool IMaintenance::CleanSlotConfig(Data::configSlotHolder_t& a_data)
	{
		bool empty = true;

		for (auto& g : a_data.data)
		{
			if (g)
			{
				empty = false;

				for (auto& h : (*g)())
				{
					CleanFormList(h.preferredItems);
					if (h.filters)
					{
						h.filters->raceFilter.allow.erase(0);
						h.filters->raceFilter.deny.erase(0);
						h.filters->npcFilter.allow.erase(0);
						h.filters->npcFilter.deny.erase(0);
						h.filters->actorFilter.allow.erase(0);
						h.filters->actorFilter.deny.erase(0);
					}
				}
			}
		}

		return empty;
	}

	void IMaintenance::CleanBlockList(Data::actorBlockList_t& a_data)
	{
		a_data.data.erase(0);

		for (auto it = a_data.data.begin(); it != a_data.data.end();)
		{
			for (auto it2 = it->second.keys.begin(); it2 != it->second.keys.end();)
			{
				if (*it2 != StringHolder::GetSingleton().IED)
				{
					auto& pluginInfo = IData::GetPluginInfo().GetLookupRef();
					if (!pluginInfo.contains(*it2))
					{
						Warning(
							"Erasing actor block from missing plugin '%s'",
							it2->c_str());

						it2 = it->second.keys.erase(it2);
						continue;
					}
				}

				++it2;
			}

			if (it->second.keys.empty())
			{
				it = a_data.data.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void IMaintenance::CleanConfigStore(
		Data::configStore_t& a_data)
	{
		for (auto& e : a_data.custom.GetFormMaps())
		{
			e.erase(0);

			for (auto it = e.begin(); it != e.end();)
			{
				if (it->second.empty())
				{
					it = e.erase(it);
				}
				else
				{
					CleanCustomConfig(it->first, it->second);
					++it;
				}
			}
		}

		for (auto& e : a_data.custom.GetGlobalData())
		{
			CleanCustomConfig(0, e);
		}

		for (auto& e : a_data.slot.GetFormMaps())
		{
			e.erase(0);

			for (auto it = e.begin(); it != e.end();)
			{
				if (CleanSlotConfig(it->second))
				{
					it = e.erase(it);
				}
				else
				{
					++it;
				}
			}
		}

		for (auto& e : a_data.slot.GetGlobalData())
		{
			CleanSlotConfig(e);
		}

		for (auto& e : a_data.transforms.GetFormMaps())
		{
			e.erase(0);

			for (auto it = e.begin(); it != e.end();)
			{
				if (it->second.data.empty())
				{
					it = e.erase(it);
				}
				else
				{
					++it;
				}
			}
		}
	}

}