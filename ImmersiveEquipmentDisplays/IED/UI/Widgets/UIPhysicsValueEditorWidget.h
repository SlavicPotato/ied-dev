#pragma once

#include "UIPhysicsValueEditorWidgetStrings.h"

#include "IED/UI/UITips.h"

namespace IED
{
	class Controller;

	namespace Data
	{
		struct configNodePhysicsValues_t;
	}

	namespace UI
	{
		class UIPhysicsValueEditorWidget :
			public virtual UITipsInterface
		{
		public:
			UIPhysicsValueEditorWidget(Localization::ILocalization& a_localization);

			bool DrawPhysicsValues(Data::configNodePhysicsValues_t& a_data);

		private:
			bool DrawPhysicsValuesImpl(Data::configNodePhysicsValues_t& a_data);

			bool DrawGeneralOpts(Data::configNodePhysicsValues_t& a_data);
			bool DrawSphereConstraintOpts(Data::configNodePhysicsValues_t& a_data);
			bool DrawBoxConstraintOpts(Data::configNodePhysicsValues_t& a_data);

			bool DrawOffsetParams(btVector4& a_params);
		};
	}

}
