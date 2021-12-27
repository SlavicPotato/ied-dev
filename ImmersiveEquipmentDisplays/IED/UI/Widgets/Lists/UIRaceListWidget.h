#pragma once

#include "IED/ActorInfoEntry.h"
#include "IED/ConfigOverride.h"
#include "IED/Data.h"
#include "IED/SettingHolder.h"

#include "../UIWidgetsCommon.h"
#include "UIListCommon.h"

#include "Localization/ILocalization.h"

namespace IED
{
	namespace UI
	{
		template <class T>
		class UIRaceList :
			public UIListBase<T, Game::FormID>,
			public virtual UILocalizationInterface
		{
		protected:
			using listValue_t = UIListBase<T, Game::FormID>::listValue_t;

			UIRaceList(
				Localization::ILocalization& a_localization,
				float a_itemWidthScalar = -10.0f);

			virtual ~UIRaceList() noexcept = default;

		private:
			virtual const ActorInfoHolder& GetActorInfoHolder() const = 0;
			virtual Data::SettingHolder::EditorPanelRaceSettings& GetRaceSettings() const = 0;

			virtual void OnListOptionsChange() = 0;

			virtual void ListUpdate() override;
			virtual void ListDrawInfoText(const listValue_t& a_entry) override;
			virtual void ListDrawOptions() override;
			virtual void ListDrawOptionsExtra();
		};

		template <typename T>
		UIRaceList<T>::UIRaceList(
			Localization::ILocalization& a_localization,
			float a_itemWidthScalar) :
			UIListBase<T, Game::FormID>(a_itemWidthScalar),
			UILocalizationInterface(a_localization)
		{}

		template <class T>
		void UIRaceList<T>::ListUpdate()
		{
			bool isFirstUpdate = m_listFirstUpdate;

			m_listFirstUpdate = true;

			m_listData.clear();

			const auto& globalConfig = GetRaceSettings();
			const auto& raceConf = GetRaceSettings();
			auto& rl = Data::IData::GetRaceList();

			for (auto& e : rl)
			{
				if (raceConf.playableOnly && !e.second.playable)
				{
					continue;
				}

				stl::snprintf(
					m_listBuf1,
					"[%.8X] %s",
					e.first.get(),
					raceConf.showEditorIDs ?
                        e.second.edid.c_str() :
                        e.second.fullname.c_str());

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
					auto& actorCache = GetActorInfoHolder();
					auto it = actorCache.find(*crosshairRef);
					if (it != actorCache.end())
					{
						if (m_listData.contains(it->second.race))
						{
							if (ListSetCurrentItem(it->second.race))
							{
								return;
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
				ListSetCurrentItem(m_listData.begin()->first);
			}
		}

		template <class T>
		void UIRaceList<T>::ListDrawInfoText(const listValue_t& a_entry)
		{
			auto& raceCache = Data::IData::GetRaceList();
			auto& modList = Data::IData::GetPluginInfo().GetIndexMap();

			std::ostringstream ss;

			auto itr = raceCache.find(a_entry.handle);
			if (itr != raceCache.end())
			{
				ss << "EDID:  " << itr->second.edid << std::endl;
				ss << LS(CommonStrings::Name) << ":  " << itr->second.fullname << std::endl;
				ss << LS(CommonStrings::Flags) << ": " << std::bitset<8>(itr->second.flags) << std::endl;
			}

			std::uint32_t modIndex;
			if (a_entry.handle.GetPluginPartialIndex(modIndex))
			{
				auto itm = modList.find(modIndex);
				if (itm != modList.end())
					ss << LS(CommonStrings::Mod) << ":   " << itm->second.name.get() << " [" << sshex(2)
					   << itm->second.GetPartialIndex() << "]" << std::endl;
			}

			ImGui::TextUnformatted(ss.str().c_str());
		}

		template <class T>
		void UIRaceList<T>::ListDrawOptions()
		{
			auto& config = GetRaceSettings();

			if (ImGui::Checkbox(
					LS(UIWidgetCommonStrings::PlayableOnly, "1"),
					std::addressof(config.playableOnly)))
			{
				OnListOptionsChange();
				QueueListUpdate();
			}

			ImGui::SameLine(0.0f, 10.0f);

			if (ImGui::Checkbox(
					LS(UIWidgetCommonStrings::ShowEditorIDs, "2"),
					std::addressof(config.showEditorIDs)))
			{
				OnListOptionsChange();
				QueueListUpdate();
			}

			ListDrawOptionsExtra();
		}

		template <class T>
		void UIRaceList<T>::ListDrawOptionsExtra()
		{
		}

	}
}