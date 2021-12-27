#pragma once

#include "IED/ActorInfoEntry.h"
#include "IED/ConfigOverride.h"
#include "IED/Data.h"
#include "IED/SettingHolder.h"

#include "../UIWidgetsCommon.h"
#include "UIListCommon.h"

#include "UIActorInfoAddInterface.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		template <class T>
		class UIActorList :
			public UIListBase<T, Game::FormID>,
			UIActorInfoAddInterface,
			public virtual UILocalizationInterface
		{
		public:
			virtual void ListTick() override;
			virtual void ListReset() override;

			using listValue_t = UIListBase<T, Game::FormID>::listValue_t;

			UIActorList(Controller& a_controller, float a_itemWidthScalar = -11.0f);
			virtual ~UIActorList() noexcept = default;

		private:
			virtual Data::SettingHolder::EditorPanelActorSettings& GetActorSettings() const = 0;
			virtual const ActorInfoHolder& GetActorInfoHolder() const = 0;
			virtual std::uint64_t GetActorInfoUpdateID() const = 0;

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

		template <typename T>
		UIActorList<T>::UIActorList(
			Controller& a_controller,
			float a_itemWidthScalar) :
			UIListBase<T, Game::FormID>(a_itemWidthScalar),
			UIActorInfoAddInterface(a_controller),
			UILocalizationInterface(a_controller)
		{}

		template <typename T>
		void UIActorList<T>::ListUpdate()
		{
			bool isFirstUpdate = m_listFirstUpdate;

			m_listFirstUpdate = true;

			const auto& globalConfig = GetActorSettings();
			const auto& actorSettings = GetActorSettings();
			auto& actorInfo = GetActorInfoHolder();

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

			if (!isFirstUpdate && globalConfig.selectCrosshairActor)
			{
				if (auto& crosshairRef = GetCrosshairRef(); crosshairRef)
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
				ListSetCurrentItem(m_listData.begin()->first);
			}
		}

		template <typename T>
		void UIActorList<T>::ListTick()
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
		void UIActorList<T>::ListReset()
		{
			UIListBase<T, Game::FormID>::ListReset();
			m_lastCacheUpdateId = GetActorInfoUpdateID() - 1;
		}

		template <class T>
		void UIActorList<T>::ListDrawInfoText(const listValue_t& a_entry)
		{
			auto& actorInfo = GetActorInfoHolder();
			auto& raceInfo = Data::IData::GetRaceList();
			auto& modList = Data::IData::GetPluginInfo().GetIndexMap();

			ImGui::Columns(2, nullptr, false);

			auto it = actorInfo.find(a_entry.handle);
			if (it != actorInfo.end())
			{
				ImGui::Text("%s:", LS(CommonStrings::Base));
				ImGui::Text("%s:", LS(CommonStrings::Sex));
				ImGui::Text("%s:", LS(CommonStrings::Race));
				ImGui::Text("%s:", LS(CommonStrings::Weight));
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

		template <class T>
		void UIActorList<T>::ListDrawExtraActorInfo(
			const listValue_t& a_entry)
		{
		}

		template <class T>
		void UIActorList<T>::ListDrawOptions()
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
		void UIActorList<T>::ListDrawOptionsExtra()
		{
		}

		template <class T>
		void UIActorList<T>::ListDrawExtraControls()
		{
			DrawActorInfoAdd();
		}

		template <class T>
		void UIActorList<T>::OnActorInfoAdded(Game::FormID a_handle)
		{
			QueueListUpdate(a_handle);
		}

	}
}