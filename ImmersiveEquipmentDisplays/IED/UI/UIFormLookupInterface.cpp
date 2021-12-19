#include "pch.h"

#include "UIFormLookupInterface.h"
#include "UIFormInfoCache.h"

#include "IED/Controller/IForm.h"
#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UIFormLookupInterface::UIFormLookupInterface(
			Controller& a_controller) :
			m_controller(a_controller)
		{
		}

		const formInfoResult_t* UIFormLookupInterface::LookupForm(Game::FormID a_form) const
		{
			return m_controller.UIGetFormLookupCache().LookupForm(a_form);
		}

	}
}