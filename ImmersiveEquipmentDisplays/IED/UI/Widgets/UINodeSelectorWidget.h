#pragma once

#include "IED/UI/UICommon.h"

#include "Filters/UIGenericFilter.h"

#include "IED/ConfigStore.h"
#include "IED/NodeMap.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UINodeSelectorWidget :
			public virtual UILocalizationInterface
		{
		public:
			UINodeSelectorWidget(
				Controller& a_controller);

			bool DrawNodeSelector(
				const char*           a_label,
				bool                  a_hideManaged,
				Data::NodeDescriptor& a_data);

		private:
			char m_buffer[256];

			UIGenericFilter m_filter;

			Controller& m_controller;
		};
	}
}