#pragma once

#include "IED/ActorInfoEntry.h"
#include "IED/ConfigStore.h"
#include "IED/Data.h"
#include "IED/SettingHolder.h"

#include "IED/UI/UIActorInfoInterface.h"
#include "IED/UI/UISettingsInterface.h"
#include "IED/UI/Widgets/UIWidgetsCommon.h"

#include "UIListCommon.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		template <class Td>
		class UIRaceList :
			public UIListBase<Td, Game::FormID>,
			public virtual UISettingsInterface,
			public virtual UIActorInfoInterface
		{
		protected:
			using listValue_t = UIListBase<Td, Game::FormID>::listValue_t;

			UIRaceList(
				Controller& a_controller,
				float       a_itemWidthScalar = -6.5f);

			virtual ~UIRaceList() noexcept = default;

		private:
			virtual Data::SettingHolder::EditorPanelRaceSettings& GetRaceSettings() const = 0;

			virtual void OnListOptionsChange() = 0;

			virtual void ListUpdate() override;
			virtual void ListDrawInfoText(const listValue_t& a_entry) override;
			virtual void ListDrawOptions() override;
			virtual void ListDrawOptionsExtra();

			virtual void OnListSetHandleInternal(Game::FormID a_handle) override;
		};

		template <class Td>
		UIRaceList<Td>::UIRaceList(
			Controller& a_controller,
			float       a_itemWidthScalar) :
			UIListBase<Td, Game::FormID>(a_itemWidthScalar)
		{
		}

		template <class Td>
		void UIRaceList<Td>::ListUpdate()
		{
			const bool isFirstUpdate = this->m_listFirstUpdate;

			this->m_listFirstUpdate = true;

			this->m_listData.clear();

			const auto& settings = GetRaceSettings();
			const auto& rl       = Data::IData::GetRaceList();

			for (auto& [i, e] : rl)
			{
				if (settings.playableOnly &&
				    !e.flags.test(TESRace::Flag::kPlayable))
				{
					continue;
				}

				stl::snprintf(
					this->m_listBuf1,
					"[%.8X] %s",
					i.get(),
					settings.showEditorIDs ?
						e.edid.c_str() :
                        e.fullname.c_str());

				this->m_listData.try_emplace(i, this->m_listBuf1);
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
					auto& actorCache = GetActorInfo();
					auto  it         = actorCache.find(*crosshairRef);
					if (it != actorCache.end())
					{
						if (this->m_listData.contains(it->second.race))
						{
							if (this->ListSetCurrentItem(it->second.race))
							{
								return;
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
		void UIRaceList<Td>::ListDrawInfoText(const listValue_t& a_entry)
		{
			auto& raceCache = Data::IData::GetRaceList();
			auto& modList   = Data::IData::GetPluginInfo().GetIndexMap();

			auto itr = raceCache.find(a_entry.handle);
			if (itr != raceCache.end())
			{
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::TextUnformatted("EDID:");

				ImGui::TableSetColumnIndex(1);
				ImGui::TextWrapped("%s", itr->second.edid.c_str());

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", UIL::LS(CommonStrings::Name));

				ImGui::TableSetColumnIndex(1);
				ImGui::TextWrapped("%s", itr->second.fullname.c_str());

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s:", UIL::LS(CommonStrings::Flags));

				ImGui::TableSetColumnIndex(1);
				ImGui::TextWrapped("%.8X", itr->second.flags.underlying());
			}

			std::uint32_t modIndex;
			if (a_entry.handle.GetPluginPartialIndex(modIndex))
			{
				const auto itm = modList.find(modIndex);
				if (itm != modList.end())
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s:", UIL::LS(CommonStrings::Mod));

					ImGui::TableSetColumnIndex(1);
					ImGui::TextWrapped(
						"[%X] %s",
						itm->second.GetPartialIndex(),
						itm->second.name.c_str());
				}
			}
		}

		template <class Td>
		void UIRaceList<Td>::ListDrawOptions()
		{
			auto& config = GetRaceSettings();

			if (ImGui::Checkbox(
					UIL::LS(UIWidgetCommonStrings::PlayableOnly, "1"),
					std::addressof(config.playableOnly)))
			{
				OnListOptionsChange();
				this->QueueListUpdate();
			}

			ImGui::SameLine(0.0f, 10.0f);

			if (ImGui::Checkbox(
					UIL::LS(UIWidgetCommonStrings::ShowEditorIDs, "2"),
					std::addressof(config.showEditorIDs)))
			{
				OnListOptionsChange();
				this->QueueListUpdate();
			}

			ListDrawOptionsExtra();
		}

		template <class Td>
		void UIRaceList<Td>::ListDrawOptionsExtra()
		{
		}

		template <class Td>
		void UIRaceList<Td>::OnListSetHandleInternal(Game::FormID a_handle)
		{
			auto& settings = GetRaceSettings();

			if (settings.lastSelected != a_handle)
			{
				settings.lastSelected = a_handle;
				MarkSettingsDirty();
			}
		}

	}
}