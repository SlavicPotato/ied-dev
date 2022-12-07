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

			virtual void OnListSetHandleInternal(Game::FormID a_handle) override;

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
			const bool isFirstUpdate = this->m_listFirstUpdate;

			this->m_listFirstUpdate = true;

			const auto& settings  = GetActorSettings();
			auto&       actorInfo = GetActorInfo();

			this->m_listData.clear();

			for (auto& e : actorInfo)
			{
				if (!settings.showAll && !e.second.active)
				{
					continue;
				}

				stl::snprintf(
					this->m_listBuf1,
					"[%.8X] %s",
					e.first.get(),
					e.second.name.c_str());

				this->m_listData.try_emplace(e.first, this->m_listBuf1);
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
					if (this->m_listData.contains(*crosshairRef))
					{
						if (this->ListSetCurrentItem(*crosshairRef))
						{
							return;
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
		void UIActorList<Td>::ListTick()
		{
			const auto cacheUpdateId = GetActorInfoUpdateID();

			if (cacheUpdateId != m_lastCacheUpdateId)
			{
				this->m_lastCacheUpdateId = cacheUpdateId;
				this->m_listNextUpdate = true;
			}

			UIListBase<Td, Game::FormID>::ListTick();
		}

		template <typename Td>
		void UIActorList<Td>::ListReset()
		{
			UIListBase<Td, Game::FormID>::ListReset();
			this->m_lastCacheUpdateId = GetActorInfoUpdateID() - 1;
		}

		template <class Td>
		void UIActorList<Td>::ListDrawInfoText(const listValue_t& a_entry)
		{
			auto& actorInfo = GetActorInfo();
			auto& raceInfo  = Data::IData::GetRaceList();
			auto& modList   = Data::IData::GetPluginInfo().GetIndexMap();

			auto it = actorInfo.find(a_entry.handle);
			if (it != actorInfo.end())
			{
				if (it->second.npc)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", LS(CommonStrings::Base));

					ImGui::TableSetColumnIndex(1);
					ImGui::TextWrapped(
						"%.8X [%.8X]",
						it->second.npc->form.get(),
						it->second.npc->flags);

					if (it->second.npc->templ)
					{
						ImGui::TableNextRow();

						ImGui::TableSetColumnIndex(0);
						ImGui::Text("%s:", LS(CommonStrings::Template));

						ImGui::TableSetColumnIndex(1);
						ImGui::TextWrapped(
							"%.8X",
							it->second.npc->templ.get());
					}

					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", LS(CommonStrings::Sex));

					ImGui::TableSetColumnIndex(1);
					ImGui::TextWrapped(
						"%s",
						it->second.npc->female ?
							LS(CommonStrings::Female) :
							LS(CommonStrings::Male));
				}

				auto race = it->second.GetRace();

				auto itr = raceInfo.find(race);
				if (itr != raceInfo.end())
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", LS(CommonStrings::Race));

					ImGui::TableSetColumnIndex(1);

					ImGui::TextWrapped(
						"%s [%.8X]",
						itr->second.edid.c_str(),
						race.get());
				}

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", LS(CommonStrings::Weight));

				ImGui::TableSetColumnIndex(1);
				ImGui::TextWrapped("%.0f", it->second.weight);

				if (it->second.worldspace)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", LS(CommonStrings::Worldspace));

					ImGui::TableSetColumnIndex(1);
					ImGui::TextWrapped("%.8X", it->second.worldspace.get());
				}

				if (it->second.cell.first)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", LS(CommonStrings::Cell));

					ImGui::TableSetColumnIndex(1);
					ImGui::TextWrapped("%.8X", it->second.cell.first.get());
				}

				if (it->second.skin.first)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", LS(CommonStrings::Skin));

					ImGui::TableSetColumnIndex(1);
					ImGui::TextWrapped("%.8X", it->second.skin.first.get());
				}

				if (it->second.idle.first)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", LS(CommonStrings::Idle));

					ImGui::TableSetColumnIndex(1);
					ImGui::TextWrapped(
						"%.8X [%s]",
						it->second.idle.first.get(),
						it->second.idle.second.c_str());
				}

				if (it->second.package)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", LS(CommonStrings::Package));

					ImGui::TableSetColumnIndex(1);
					ImGui::TextWrapped("%.8X", it->second.package.get());
				}

				if (it->second.furniture.first)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", LS(CommonStrings::Furniture));

					ImGui::TableSetColumnIndex(1);
					ImGui::TextWrapped(
						"%.8X [%s]",
						it->second.furniture.first.get(),
						it->second.furniture.second.c_str());
				}
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

			ListDrawExtraActorInfo(a_entry);

			if (it != actorInfo.end())
			{
				if (IPerfCounter::delta_us(it->second.ts, IPerfCounter::Query()) > 100000)
				{
					QueueUpdateActorInfo(it->first);
				}
			}
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
			this->QueueListUpdate(a_handle);
		}

		template <class Td>
		void UIActorList<Td>::OnListSetHandleInternal(Game::FormID a_handle)
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