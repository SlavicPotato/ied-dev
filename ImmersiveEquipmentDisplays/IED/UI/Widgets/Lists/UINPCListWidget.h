#pragma once

#include "IED/ActorInfoEntry.h"
#include "IED/ConfigOverride.h"
#include "IED/Data.h"
#include "IED/SettingHolder.h"

#include "IED/UI/UIActorInfoInterface.h"
#include "IED/UI/UISettingsInterface.h"
#include "IED/UI/Widgets/UIWidgetsCommon.h"

#include "UIListCommon.h"

#include "UINPCInfoAddInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		template <class Td>
		class UINPCList :
			public UIListBase<Td, Game::FormID>,
			UINPCInfoAddInterface,
			public virtual UISettingsInterface,
			public virtual UIActorInfoInterface
		{
		public:
			virtual void ListReset() override;

			using listValue_t = UIListBase<Td, Game::FormID>::listValue_t;

			UINPCList(Controller& a_controller, float a_itemWidthScalar = -6.5f);
			virtual ~UINPCList() noexcept = default;

		protected:
			virtual void ListTick() override;

		private:
			virtual Data::SettingHolder::EditorPanelActorSettings& GetActorSettings() const = 0;

			virtual void OnListOptionsChange() = 0;

			virtual void ListUpdate() override;
			virtual void ListDrawInfoText(const listValue_t& a_entry) override;
			virtual void ListDrawExtraActorInfo(const listValue_t& a_entry);
			virtual void ListDrawOptions() override;
			virtual void ListDrawOptionsExtra();
			virtual void ListDrawExtraControls() override;

			virtual void OnNPCInfoAdded(Game::FormID a_npc) override;

			std::uint64_t m_lastCacheUpdateId{ 0 };
		};

		template <class Td>
		UINPCList<Td>::UINPCList(
			Controller& a_controller,
			float a_itemWidthScalar) :
			UIListBase<Td, Game::FormID>(a_controller, a_itemWidthScalar),
			UINPCInfoAddInterface(a_controller),
			UISettingsInterface(a_controller),
			UIActorInfoInterface(a_controller)
		{
		}

		template <class Td>
		void UINPCList<Td>::ListUpdate()
		{
			bool isFirstUpdate = m_listFirstUpdate;

			m_listFirstUpdate = true;

			const auto& actorSettings = GetActorSettings();
			auto& npcInfo = GetNPCInfo();

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

			if (!isFirstUpdate && GetSettings().data.ui.selectCrosshairActor)
			{
				if (auto& crosshairRef = GetCrosshairRef())
				{
					auto& actorInfo = GetActorInfo();
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
				ListSetCurrentItem(*m_listData.begin());
			}
		}

		template <class Td>
		void UINPCList<Td>::ListTick()
		{
			const auto cacheUpdateId = GetActorInfoUpdateID();

			if (cacheUpdateId != m_lastCacheUpdateId)
			{
				m_lastCacheUpdateId = cacheUpdateId;
				m_listNextUpdate = true;
			}

			UIListBase<Td, Game::FormID>::ListTick();
		}

		template <class Td>
		void UINPCList<Td>::ListReset()
		{
			UIListBase<Td, Game::FormID>::ListReset();
			m_lastCacheUpdateId = GetActorInfoUpdateID() - 1;
		}

		template <class Td>
		void UINPCList<Td>::ListDrawInfoText(const listValue_t& a_entry)
		{
			auto& npcInfo = GetNPCInfo();
			auto& raceInfo = Data::IData::GetRaceList();
			auto& modList = Data::IData::GetPluginInfo().GetIndexMap();

			ImGui::Columns(2, nullptr, false);

			auto it = npcInfo.find(a_entry.handle);
			if (it != npcInfo.end())
			{
				ImGui::Text("%s:", LS(CommonStrings::Flags));
				ImGui::Text("%s:", LS(CommonStrings::Sex));
				ImGui::Text("%s:", LS(CommonStrings::Race));
				ImGui::Text("%s:", LS(CommonStrings::Weight));
			}

			ImGui::Text("%s:", LS(CommonStrings::Mod));

			ImGui::NextColumn();

			if (it != npcInfo.end())
			{
				ImGui::TextWrapped(
					"%s",
					std::bitset<8>(it->second->flags).to_string().c_str());

				ImGui::TextWrapped(
					"%s",
					it->second->female ?
                        LS(CommonStrings::Female) :
                        LS(CommonStrings::Male));

				auto race = it->second->race;

				auto itr = raceInfo.find(race);
				if (itr != raceInfo.end())
				{
					ImGui::TextWrapped(
						"%s [%.8X]",
						itr->second.edid.c_str(),
						race.get());
				}
				else
				{
					ImGui::TextWrapped("%s", "N/A");
				}

				ImGui::TextWrapped("%.0f", it->second->weight);
			}

			std::uint32_t modIndex;
			if (a_entry.handle.GetPluginPartialIndex(modIndex))
			{
				auto itm = modList.find(modIndex);
				if (itm != modList.end())
				{
					ImGui::TextWrapped(
						"%s [%.2X]",
						itm->second.name.c_str(),
						itm->second.GetPartialIndex());
				}
				else
				{
					ImGui::TextWrapped("%s", LS(CommonStrings::Unknown));
				}
			}
			else
			{
				ImGui::TextWrapped("%s", LS(CommonStrings::Unknown));
			}

			ImGui::Columns();
		}

		template <class Td>
		void UINPCList<Td>::ListDrawExtraActorInfo(
			const listValue_t& a_entry)
		{
		}

		template <class Td>
		void UINPCList<Td>::ListDrawOptions()
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

		template <class Td>
		void UINPCList<Td>::ListDrawOptionsExtra()
		{}

		template <class Td>
		void UINPCList<Td>::ListDrawExtraControls()
		{
			auto& current = ListGetSelected();

			DrawNPCInfoAdd(current ? current->handle : Game::FormID{});
		}

		template <class Td>
		void UINPCList<Td>::OnNPCInfoAdded(Game::FormID a_npc)
		{
			QueueListUpdate(a_npc);
		}

	}  // namespace UI
}  // namespace IED