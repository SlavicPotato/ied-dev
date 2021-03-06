#pragma once

#include "IED/ActorInfoEntry.h"
#include "IED/ConfigStore.h"
#include "IED/Data.h"
#include "IED/SettingHolder.h"

#include "IED/UI/UIActorInfoInterface.h"
#include "IED/UI/UISettingsInterface.h"
#include "IED/UI/Widgets/UIWidgetsCommon.h"

#include "UIListCommon.h"

#include "UIActorInfoAddInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		template <class Td>
		class UIActorList :
			public UIListBase<Td, Game::FormID>,
			UIActorInfoAddInterface,
			public virtual UISettingsInterface,
			public virtual UIActorInfoInterface
		{
		public:
			virtual void ListReset() override;

			using listValue_t = UIListBase<Td, Game::FormID>::listValue_t;

			UIActorList(
				Controller& a_controller,
				float       a_itemWidthScalar = -6.5f);

			virtual ~UIActorList() noexcept = default;

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

			virtual void OnActorInfoAdded(Game::FormID a_handle) override;

			std::uint64_t m_lastCacheUpdateId{ 0 };
		};

		template <class Td>
		UIActorList<Td>::UIActorList(
			Controller& a_controller,
			float       a_itemWidthScalar) :
			UIListBase<Td, Game::FormID>(a_controller, a_itemWidthScalar),
			UIActorInfoAddInterface(a_controller)
		{}

		template <class Td>
		void UIActorList<Td>::ListUpdate()
		{
			bool isFirstUpdate = m_listFirstUpdate;

			m_listFirstUpdate = true;

			const auto& actorSettings = GetActorSettings();
			auto&       actorInfo     = GetActorInfo();

			m_listData.clear();

			for (auto& e : actorInfo)
			{
				if (!actorSettings.showAll && !e.second.active)
				{
					continue;
				}

				stl::snprintf(
					m_listBuf1,
					"[%.8X] %s",
					e.first.get(),
					e.second.name.c_str());

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
					if (m_listData.contains(*crosshairRef))
					{
						if (ListSetCurrentItem(*crosshairRef))
						{
							return;
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
		void UIActorList<Td>::ListTick()
		{
			const auto cacheUpdateId = GetActorInfoUpdateID();

			if (cacheUpdateId != m_lastCacheUpdateId)
			{
				m_lastCacheUpdateId = cacheUpdateId;
				m_listNextUpdate    = true;
			}

			UIListBase<Td, Game::FormID>::ListTick();
		}

		template <typename Td>
		void UIActorList<Td>::ListReset()
		{
			UIListBase<Td, Game::FormID>::ListReset();
			m_lastCacheUpdateId = GetActorInfoUpdateID() - 1;
		}

		template <class Td>
		void UIActorList<Td>::ListDrawInfoText(const listValue_t& a_entry)
		{
			auto& actorInfo = GetActorInfo();
			auto& raceInfo  = Data::IData::GetRaceList();
			auto& modList   = Data::IData::GetPluginInfo().GetIndexMap();

			ImGui::Columns(2, nullptr, false);

			auto it = actorInfo.find(a_entry.handle);
			if (it != actorInfo.end())
			{
				ImGui::Text("%s:", LS(CommonStrings::Base));

				if (it->second.npc && it->second.npc->templ)
				{
					ImGui::Text("%s:", LS(CommonStrings::Template));
				}

				ImGui::Text("%s:", LS(CommonStrings::Sex));
				ImGui::Text("%s:", LS(CommonStrings::Race));
				ImGui::Text("%s:", LS(CommonStrings::Weight));

				if (it->second.worldspace)
				{
					ImGui::Text("%s:", LS(CommonStrings::Worldspace));
				}

				if (it->second.cell)
				{
					ImGui::Text("%s:", LS(CommonStrings::Cell));
				}

				if (it->second.skin)
				{
					ImGui::Text("%s:", LS(CommonStrings::Skin));
				}
			}

			ImGui::Text("%s:", LS(CommonStrings::Mod));

			ImGui::NextColumn();

			if (it != actorInfo.end())
			{
				if (it->second.npc)
				{
					ImGui::TextWrapped(
						"%.8X [%s]",
						it->second.npc->form.get(),
						std::bitset<8>(it->second.npc->flags).to_string().c_str());

					if (it->second.npc->templ)
					{
						ImGui::TextWrapped(
							"%.8X",
							it->second.npc->templ.get());
					}

					ImGui::TextWrapped(
						"%s",
						it->second.npc->female ?
							LS(CommonStrings::Female) :
                            LS(CommonStrings::Male));
				}
				else
				{
					ImGui::TextWrapped("%s", "N/A");
					ImGui::TextWrapped("%s", LS(CommonStrings::Unknown));
				}

				auto race = it->second.GetRace();

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

				ImGui::TextWrapped("%.0f", it->second.weight);

				if (it->second.worldspace)
				{
					ImGui::TextWrapped("%.8X", it->second.worldspace.get());
				}

				if (it->second.cell)
				{
					ImGui::TextWrapped("%.8X", it->second.cell.get());
				}

				if (it->second.skin)
				{
					ImGui::TextWrapped("%.8X", it->second.skin.get());
				}
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
			
			ListDrawExtraActorInfo(a_entry);
		}

		template <class Td>
		void UIActorList<Td>::ListDrawExtraActorInfo(
			const listValue_t& a_entry)
		{
		}

		template <class Td>
		void UIActorList<Td>::ListDrawOptions()
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
		void UIActorList<Td>::ListDrawOptionsExtra()
		{
		}

		template <class Td>
		void UIActorList<Td>::ListDrawExtraControls()
		{
			DrawActorInfoAdd();
		}

		template <class Td>
		void UIActorList<Td>::OnActorInfoAdded(Game::FormID a_handle)
		{
			QueueListUpdate(a_handle);
		}

	}
}