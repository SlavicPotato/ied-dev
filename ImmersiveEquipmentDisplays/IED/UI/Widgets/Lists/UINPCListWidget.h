#pragma once

#include "IED/ActorInfoEntry.h"
#include "IED/ConfigStore.h"
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

			virtual void OnListSetHandleInternal(Game::FormID a_handle) override;

			std::uint64_t m_lastCacheUpdateId{ 0 };
		};

		template <class Td>
		UINPCList<Td>::UINPCList(
			Controller& a_controller,
			float       a_itemWidthScalar) :
			UIListBase<Td, Game::FormID>(a_controller, a_itemWidthScalar),
			UINPCInfoAddInterface(a_controller)
		{
		}

		template <class Td>
		void UINPCList<Td>::ListUpdate()
		{
			bool isFirstUpdate = this->m_listFirstUpdate;

			this->m_listFirstUpdate = true;

			const auto& settings = GetActorSettings();
			auto&       npcInfo       = GetNPCInfo();

			this->m_listData.clear();

			for (auto& e : npcInfo)
			{
				if (!settings.showAll && !e.second->active)
				{
					continue;
				}

				auto& id = e.second->get_npc_or_template();

				stl::snprintf(
					this->m_listBuf1,
					"[%.8X] %s",
					id.get(),
					e.second->name.c_str());

				this->m_listData.try_emplace(id.get(), this->m_listBuf1);
			}

			if (this->m_listData.empty())
			{
				this->m_listBuf1[0] = 0;
				this->ListClearCurrentItem();
				return;
			}

			stl::snprintf(this->m_listBuf1, "%zu", this->m_listData.size());

			if (!isFirstUpdate && GetSettings().data.ui.selectCrosshairActor)
			{
				if (auto& crosshairRef = GetCrosshairRef())
				{
					auto& actorInfo = GetActorInfo();
					if (auto it = actorInfo.find(*crosshairRef); it != actorInfo.end())
					{
						if (it->second.npc)
						{
							if (this->m_listData.contains(it->second.npc->get_npc_or_template()))
							{
								if (this->ListSetCurrentItem(it->second.npc->get_npc_or_template()))
								{
									return;
								}
							}
						}
					}
				}
			}

			if (this->m_listCurrent)
			{
				if (!this->m_listData.contains(this->m_listCurrent->handle))
				{
					this->ListClearCurrentItem();
				}
				else
				{
					this->ListSetCurrentItem(this->m_listCurrent->handle);
				}
			}

			if (!this->m_listCurrent)
			{
				if (settings.lastSelected &&
				    this->m_listData.contains(settings.lastSelected))
				{
					this->ListSetCurrentItem(settings.lastSelected);
				}
			}

			if (!this->m_listCurrent)
			{
				this->ListSetCurrentItem(*this->m_listData.begin());
			}
		}

		template <class Td>
		void UINPCList<Td>::ListTick()
		{
			const auto cacheUpdateId = GetActorInfoUpdateID();

			if (cacheUpdateId != m_lastCacheUpdateId)
			{
				m_lastCacheUpdateId = cacheUpdateId;
				this->m_listNextUpdate = true;
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
			auto& npcInfo  = GetNPCInfo();
			auto& raceInfo = Data::IData::GetRaceList();
			auto& modList  = Data::IData::GetPluginInfo().GetIndexMap();

			auto it = npcInfo.find(a_entry.handle);
			if (it != npcInfo.end())
			{
				if (it->second->templ)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", LS(CommonStrings::Template));

					ImGui::TableSetColumnIndex(1);
					ImGui::TextWrapped(
						"%.8X",
						it->second->templ.get());
				}

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", LS(CommonStrings::Flags));

				ImGui::TableSetColumnIndex(1);
				ImGui::TextWrapped(
					"%.8X",
					it->second->flags);

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", LS(CommonStrings::Sex));

				ImGui::TableSetColumnIndex(1);
				ImGui::TextWrapped(
					"%s",
					it->second->female ?
						LS(CommonStrings::Female) :
                        LS(CommonStrings::Male));

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", LS(CommonStrings::Race));

				ImGui::TableSetColumnIndex(1);

				auto race = it->second->race;
				auto itr  = raceInfo.find(race);
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

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", LS(CommonStrings::Weight));

				ImGui::TableSetColumnIndex(1);
				ImGui::TextWrapped("%.0f", it->second->weight);
			}

			std::uint32_t modIndex;
			if (a_entry.handle.GetPluginPartialIndex(modIndex))
			{
				auto itm = modList.find(modIndex);
				if (itm != modList.end())
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", LS(CommonStrings::Mod));

					ImGui::TableSetColumnIndex(1);

					ImGui::TextWrapped(
						"[%X] %s",
						itm->second.GetPartialIndex(),
						itm->second.name.c_str());
				}
			}

			if (it != npcInfo.end())
			{
				if (IPerfCounter::delta_us(it->second->ts, IPerfCounter::Query()) > 100000)
				{
					QueueUpdateNPCInfo(it->first);
				}
			}
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
				this->QueueListUpdate();
			}

			ImGui::SameLine(0.0f, 10.0f);

			if (ImGui::Checkbox(
					LS(UIWidgetCommonStrings::ShowAll, "2"),
					std::addressof(config.showAll)))
			{
				OnListOptionsChange();
				this->QueueListUpdate();
			}

			ListDrawOptionsExtra();
		}

		template <class Td>
		void UINPCList<Td>::ListDrawOptionsExtra()
		{}

		template <class Td>
		void UINPCList<Td>::ListDrawExtraControls()
		{
			auto& current = this->ListGetSelected();

			DrawNPCInfoAdd(current ? current->handle : Game::FormID{});
		}

		template <class Td>
		void UINPCList<Td>::OnNPCInfoAdded(Game::FormID a_npc)
		{
			this->QueueListUpdate(a_npc);
		}

		template <class Td>
		void UINPCList<Td>::OnListSetHandleInternal(Game::FormID a_handle)
		{
			auto& settings = GetActorSettings();

			if (settings.lastSelected != a_handle)
			{
				settings.lastSelected = a_handle;
				MarkSettingsDirty();
			}
		}

	}
}