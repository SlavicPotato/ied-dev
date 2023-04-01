#pragma once

#include "Widgets/Form/UIFormInfoTooltipWidget.h"

#include "UIContext.h"
#include "Window/UIWindow.h"

#include "IED/GeneralInfo.h"

namespace IED
{
	class Controller;

	namespace UI
	{

		class UIGeneralInfo :
			public UIContext,
			public UIWindow,
			UIFormInfoTooltipWidget
		{
			struct Data :
				stl::intrusive_ref_counted
			{
				SKMP_REDEFINE_NEW_PREF();

				stl::mutex lock;
				long long  lastUpdate{ 0 };
				bool       initialized{ false };
				//bool       succeeded{ false };

				generalInfoEntry_t general;
			};

			static constexpr auto WINDOW_ID = "ied_ginfo";

		public:
			static constexpr auto CHILD_ID = ChildWindowID::kGeneralInfo;

			UIGeneralInfo(Controller& a_controller);

			void Draw() override;
			void OnOpen() override;
			void OnClose() override;

			virtual std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(CHILD_ID);
			}

		private:
			void DrawGeneralInfo();

			static void UpdateData(const stl::smart_ptr<Data>& a_data);
			static void QueueInfoUpdate(const stl::smart_ptr<Data>& a_data);

			void DrawContents(const Data& a_data);
			void DrawGeneralTree(const Data& a_data);
			void DrawCalendarTree(const Data& a_data);
			void DrawSkyTree(const Data& a_data);

			template <class T>
			void draw_form_row(
				T            a_strid,
				Game::FormID a_formid,
				bool         a_testForm = false);

			stl::smart_ptr<Data> m_data;

			Controller& m_controller;
		};
	}
}
