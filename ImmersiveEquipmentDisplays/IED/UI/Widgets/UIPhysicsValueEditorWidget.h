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
		class UIPhysicsValueEditorWidget
		{
		public:
			static bool DrawPhysicsValues(Data::configNodePhysicsValues_t& a_data);

		private:
			static bool DrawPhysicsValuesImpl(Data::configNodePhysicsValues_t& a_data);

			static bool DrawGeneralOpts(Data::configNodePhysicsValues_t& a_data);
			static bool DrawSphereConstraintOpts(Data::configNodePhysicsValues_t& a_data);
			static bool DrawBoxConstraintOpts(Data::configNodePhysicsValues_t& a_data);

			static bool DrawOffsetParams(btVector4& a_params);
		};
	}

}
