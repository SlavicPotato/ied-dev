#pragma once

#include "Controls/UICollapsibles.h"

#include "Widgets/Lists/UIActorListWidget.h"

#include "Window/UIWindow.h"
#include "Window/UIWindowBase.h"

#include "UILocalizationInterface.h"

#include "IED/ActorInventoryInfo.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		struct ActorInfoAggregate
		{
			stl::critical_section lock;
			long long             lastUpdate{ 0 };
			bool                  initialized{ false };
			bool                  succeeded{ false };
			Game::FormID          actor;
			actorInfoEntry_t      entry;
			actorInventoryInfo_t  inventory;
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
			public UIChildWindowBase,
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

		private:
			void DrawActorInfo();

			void DrawActorInfoContents(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);
			
			void DrawInventoryTreeContents(
				Game::FormID         a_handle,
				const ActorInfoData& a_data);

			void DrawInventoryFilterTree();

			void DrawInventoryEntries(
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
				const actorInventoryInfo_t::Base& a_data);
			
			void DrawInventoryExtraLists(
				Game::FormID                      a_handle,
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

			Controller& m_controller;
		};
	}
}