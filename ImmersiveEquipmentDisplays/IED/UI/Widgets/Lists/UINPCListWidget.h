#pragma once

#include "IED/ActorInfoEntry.h"
#include "IED/ConfigOverride.h"
#include "IED/Data.h"
#include "IED/SettingHolder.h"

#include "../UIWidgetsCommon.h"
#include "UIListCommon.h"

#include "UINPCInfoAddInterface.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		template <class T>
		class UINPCList :
			public UIListBase<T, Game::FormID>,
			UINPCInfoAddInterface,
			public virtual UILocalizationInterface
		{
		public:
			virtual void ListTick() override;
			virtual void ListReset() override;

			using listValue_t = UIListBase<T, Game::FormID>::listValue_t;

			UINPCList(Controller& a_controller, float a_itemWidthScalar = -10.0f);
			virtual ~UINPCList() noexcept = default;

		private:
			virtual Data::SettingHolder::EditorPanelActorSettings& GetActorSettings() const = 0;
			virtual const ActorInfoHolder& GetActorInfoHolder() const = 0;
			virtual const NPCInfoHolder& GetNPCInfoHolder() const = 0;
			virtual std::uint64_t GetActorInfoUpdateID() const = 0;

			virtual void OnListOptionsChange() = 0;

			virtual void ListUpdate() override;
			virtual void ListDrawInfoText(listValue_t* a_entry) override;
			virtual void ListDrawExtraActorInfo(listValue_t* a_entry);
			virtual void ListDrawOptions() override;
			virtual void ListDrawOptionsExtra();
			virtual void ListDrawExtraControls() override;

			virtual void OnNPCInfoAdded(Game::FormID a_npc) override;

			std::uint64_t m_lastCacheUpdateId{ 0 };
		};

		template <typename T>
		UINPCList<T>::UINPCList(
			Controller& a_controller,
			float a_itemWidthScalar) :
			UIListBase<T, Game::FormID>(a_itemWidthScalar),
			UINPCInfoAddInterface(a_controller),
			UILocalizationInterface(a_controller)
		{
		}

		template <typename T>
		void UINPCList<T>::ListUpdate()
		{
			bool isFirstUpdate = m_listFirstUpdate;

			m_listFirstUpdate = true;

			const auto& globalConfig = GetActorSettings();
			const auto& actorSettings = GetActorSettings();
			auto& npcInfo = GetNPCInfoHolder();

			m_listData.clear();

			for (auto& e : npcInfo)
			{
				if (!actorSettings.showAll && !e.second->active)
				{
					continue;
				}

				stl::snprintf(
					m_listBuf1,
					"[%.8X] %s",
					e.first.get(),
					e.second->name.c_str());

				m_listData.try_emplace(e.first, m_listBuf1);
			}

			if (m_listData.empty())
			{
				m_listBuf1[0] = 0;
				ListClearCurrentItem();
				return;
			}

			stl::snprintf(m_listBuf1, "%zu", m_listData.size());

			if (!isFirstUpdate && globalConfig.selectCrosshairActor)
			{
				if (auto& crosshairRef = GetCrosshairRef(); crosshairRef)
				{
					auto& actorInfo = GetActorInfoHolder();
					if (auto it = actorInfo.find(*crosshairRef); it != actorInfo.end())
					{
						if (it->second.npc)
						{
							if (m_listData.contains(it->second.npc->form))
							{
								if (ListSetCurrentItem(it->second.npc->form))
								{
									return;
								}
							}
						}
					}
				}
			}

			if (m_listCurrent)
			{
				if (!m_listData.contains(m_listCurrent->handle))
				{
					ListClearCurrentItem();
				}
				else
				{
					ListSetCurrentItem(m_listCurrent->handle);
				}
			}

			if (!m_listCurrent)
			{
				if (actorSettings.lastActor &&
				    m_listData.contains(actorSettings.lastActor))
				{
					ListSetCurrentItem(actorSettings.lastActor);
				}
			}

			if (!m_listCurrent)
			{
				ListSetCurrentItem(m_listData.begin()->first);
			}
		}

		template <typename T>
		void UINPCList<T>::ListTick()
		{
			const auto cacheUpdateId = GetActorInfoUpdateID();

			if (cacheUpdateId != m_lastCacheUpdateId)
			{
				m_lastCacheUpdateId = cacheUpdateId;
				m_listNextUpdate = true;
			}

			UIListBase<T, Game::FormID>::ListTick();
		}

		template <typename T>
		void UINPCList<T>::ListReset()
		{
			UIListBase<T, Game::FormID>::ListReset();
			m_lastCacheUpdateId = GetActorInfoUpdateID() - 1;
		}

		template <class T>
		void UINPCList<T>::ListDrawInfoText(listValue_t* a_entry)
		{
			/*auto& actorInfo = GetActorInfoHolder();
			auto& raceInfo = Data::IData::GetRaceList();
			auto& modList = Data::IData::GetPluginInfo().GetIndexMap();

			std::ostringstream ss;

			auto it = actorInfo.find(a_entry->handle);
			if (it != actorInfo.end())
			{
				if (it->second.npc)
				{
					ss << "Base:    " << sshex(8) << it->second.npc->base << " ["
					   << std::bitset<8>(it->second.npc->flags) << "]" << std::endl;

					ss << "Sex:     " << (it->second.npc->female ? "Female" : "Male") << std::endl;
				}

				auto race = it->second.GetRace();

				auto itr = raceInfo.find(race);
				if (itr != raceInfo.end())
				{
					ss << "Race:    " << itr->second.edid.get() << " [" << sshex(8)
					   << race.get() << "]" << std::endl;
				}
				else
				{
					ss << "Race:    " << sshex(8) << race.get() << std::endl;
				}

				ss << "Weight:  " << std::fixed << std::setprecision(0) << it->second.weight
				   << std::endl;
			}

			std::uint32_t modIndex;
			if (a_entry->handle.GetPluginPartialIndex(modIndex))
			{
				auto itm = modList.find(modIndex);
				if (itm != modList.end())
				{
					ss << "Mod:     " << itm->second.name.get() << " [" << sshex(2)
					   << itm->second.GetPartialIndex() << "]" << std::endl;
				}
			}

			ListDrawExtraActorInfo(a_entry, ss);

			ImGui::TextUnformatted(ss.str().c_str());*/
		}

		template <class T>
		void UINPCList<T>::ListDrawExtraActorInfo(
			listValue_t* a_entry)
		{
		}

		template <class T>
		void UINPCList<T>::ListDrawOptions()
		{
			auto& config = GetActorSettings();

			if (ImGui::Checkbox(
					LS(UIWidgetCommonStrings::AutoSelectSex, "1"),
					std::addressof(config.autoSelectSex)))
			{
				OnListOptionsChange();
				QueueListUpdate();
			}

			ImGui::SameLine(0.0f, 10.0f);

			if (ImGui::Checkbox(
					LS(UIWidgetCommonStrings::ShowAll, "2"),
					std::addressof(config.showAll)))
			{
				OnListOptionsChange();
				QueueListUpdate();
			}

			ListDrawOptionsExtra();
		}

		template <class T>
		void UINPCList<T>::ListDrawOptionsExtra()
		{}

		template <class T>
		void UINPCList<T>::ListDrawExtraControls()
		{
			auto current = ListGetSelected();

			DrawNPCInfoAdd(current ? current->handle : Game::FormID{});
		}

		template <class T>
		void UINPCList<T>::OnNPCInfoAdded(Game::FormID a_npc)
		{
			QueueListUpdate(a_npc);
		}

	}  // namespace UI
}  // namespace IED