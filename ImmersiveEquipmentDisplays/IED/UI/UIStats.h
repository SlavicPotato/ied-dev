#pragma once

#include "UICommon.h"

#include "IED/Controller/ObjectManagerData.h"

#include "IED/UI/Controls/UICollapsibles.h"

#include "IED/UI/UIContext.h"
#include "IED/UI/UIMiscTextInterface.h"
#include "IED/UI/Window/UIWindow.h"

#include "UILocalizationInterface.h"

#include "UITips.h"

#include "Drivers/UI/Tasks.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIStats :
			public UIContext,
			public UIWindow,
			UIMiscTextInterface,
			UICollapsibles
		{
			static constexpr auto WINDOW_ID = "ied_stats";

		public:
			static constexpr auto CHILD_ID = ChildWindowID::kUIStats;

			UIStats(
				Tasks::UIRenderTaskBase& a_owner,
				Controller&              a_controller);

			void Draw() override;

			virtual std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(CHILD_ID);
			}

		private:
			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData() override;
			virtual void                         OnCollapsibleStatesUpdate() override;

			struct sorted_list_entry_t
			{
				const ActorObjectMap::value_type& obj;
				std::size_t                       nslot;
				std::size_t                       ncust;
				long long                         age;
				stl::fixed_string                 name;
				Game::FormID                      race;
			};

			using sort_comp_func_t = std::function<
				bool(
					const std::unique_ptr<sorted_list_entry_t>&,
					const std::unique_ptr<sorted_list_entry_t>&)>;

			static sort_comp_func_t get_sort_comp_lambda_default();
			static sort_comp_func_t get_sort_comp_lambda(const ImGuiTableSortSpecs* a_specs);

			void DrawActorTable();

			Tasks::UIRenderTaskBase& m_owner;
			Controller&              m_controller;
		};

	}
}