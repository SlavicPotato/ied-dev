#pragma once

namespace IED
{
	namespace UI
	{
		class I3DIDraggable;
		class I3DIDropTarget;
		class I3DIObject;

		class I3DIObjectBase
		{
		public:
			virtual ~I3DIObjectBase() noexcept = default;

			virtual I3DIDraggable* GetAsDraggable()
			{
				return nullptr;
			};

			virtual I3DIDropTarget* GetAsDropTarget()
			{
				return nullptr;
			};

			virtual I3DIObject* GetAsObject()
			{
				return nullptr;
			};
		};

	}
}