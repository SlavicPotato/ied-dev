#pragma once

#include "UICommon.h"

#include "IED/Controller/ObjectManagerData.h"

#include "IED/UI/Controls/UICollapsibles.h"

#include "IED/UI/Window/UIWindow.h"
#include "IED/UI/Window/UIWindowBase.h"

#include "UILocalizationInterface.h"

#include "UITips.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIStats :
			public UIWindow,
			public UIWindowBase,
			UICollapsibles,
			public virtual UILocalizationInterface,
			public virtual UITipsInterface
		{
			inline static constexpr auto WINDOW_ID = "ied_stats";

		public:
			UIStats(Controller& a_controller);

			void Draw();

		private:
			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData() override;
			virtual void                         OnCollapsibleStatesUpdate() override;

			struct sorted_list_entry_t
			{
				const ActorObjectMap::value_type& obj;
				stl::fixed_string                 name;
				std::size_t                       nslot;
				std::size_t                       ncust;
				long long                         age;
				Game::FormID                      race;

				SKMP_REDEFINE_NEW_PREF();
			};

			using sort_comp_func_t = std::function<
				bool(
					const std::unique_ptr<sorted_list_entry_t>&,
					const std::unique_ptr<sorted_list_entry_t>&)>;

			static sort_comp_func_t get_sort_comp_lambda_default();
			static sort_comp_func_t get_sort_comp_lambda(const ImGuiTableSortSpecs* a_specs);

			void DrawActorTable();

			Controller& m_controller;
		};

	}
}