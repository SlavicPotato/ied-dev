#pragma once

#include "Controls/UICollapsibles.h"

#include "Widgets/Lists/UIActorListWidget.h"

#include "UIContext.h"
#include "Window/UIWindow.h"

#include "UILocalizationInterface.h"

#include "IED/ActorActiveEffectInfo.h"
#include "IED/ActorFactionInfo.h"
#include "IED/ActorInventoryInfo.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		struct ActorInfoAggregate
		{
			std::recursive_mutex    lock;
			long long               lastUpdate{ 0 };
			bool                    initialized{ false };
			bool                    succeeded{ false };
			Game::FormID            actor;
			actorInfoEntry_t        entry;
			actorInventoryInfo_t    inventory;
			actorFactionInfo_t      factions;
			actorActiveEffectInfo_t effects;
		};

		struct ActorInfoData
		{
			ActorInfoData() :
				data(std::make_shared<ActorInfoAggregate>())
			{
			}

			std::shared_ptr<ActorInfoAggregate> data;
		};

		class UIActorInfo :
			public UIWindow,
			public UIContext,
			UICollapsibles,
			UIActorList<ActorInfoData>,
			public virtual UILocalizationInterface
		{
			inline static constexpr auto WINDOW_ID = "ied_ainfo";

		public:
			inline static constexpr auto CHILD_ID = ChildWindowID::kUIActorInfo;

			UIActorInfo(Controller& a_controller);

			void Draw() override;
			void OnOpen() override;
			void OnClose() override;
			void Reset() override;

			virtual std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(CHILD_ID);
			}

		private:
			void DrawActorInfo();

			void DrawTabBar(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			template <class T>
			bool DrawActorInfoLineFormStringPair(
				T                                           a_label,
				const std::pair<Game::FormID, std::string>& a_data);

			template <class T>
			bool DrawActorInfoLineForm(
				T            a_label,
				Game::FormID a_data);

			void DrawActorInfoContents(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawActorInfoContentsFirst(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawActorInfoContentsSecond(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawActorInfoContentsThird(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawFactionTreeContents(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawInventoryTreeContents(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);
			
			void DrawEffectTreeContents(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawInventoryFilterTree();

			void DrawFactionEntries(
				Game::FormID              a_handle,
				const ActorInfoAggregate& a_data);

			void DrawInventoryEntries(
				Game::FormID              a_handle,
				const ActorInfoAggregate& a_data);

			void DrawEffectEntries(
				Game::FormID              a_handle,
				const ActorInfoAggregate& a_data);

			void DrawInventoryBaseTree(
				Game::FormID                      a_handle,
				const actorInventoryInfo_t::Base& a_data);

			void DrawInventoryBase(
				Game::FormID                      a_handle,
				const actorInventoryInfo_t::Base& a_data);

			void DrawInventoryExtraListsTree(
				Game::FormID                      a_handle,
				const ActorInfoAggregate&         a_info,
				const actorInventoryInfo_t::Base& a_data);

			void DrawInventoryExtraLists(
				Game::FormID                      a_handle,
				const ActorInfoAggregate&         a_info,
				const actorInventoryInfo_t::Base& a_data);

			void UpdateActorInfoData(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			virtual ActorInfoData GetData(Game::FormID a_handle) override;

			virtual void ListResetAllValues(Game::FormID a_handle);

			virtual void OnListOptionsChange() override;

			virtual Data::SettingHolder::EditorPanelActorSettings& GetActorSettings() const override;

			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData();
			virtual void                         OnCollapsibleStatesUpdate();

			void QueueInfoUpdate(Game::FormID a_handle, const ActorInfoData& a_data);

			UIGenericFilter m_invFilter;

			char m_buffer[256];

			Controller& m_controller;
		};
	}
}