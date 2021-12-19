#include "pch.h"

#include "ObjectEntryStatus.h"

#include "IED/ProcessParams.h"

namespace IED
{
	void ObjectEntryStatus::set(
		ObjectEntryStatusCode a_code,
		const processParams_t& a_params,
		Game::FormID a_form)
	{
		m_lastStatus = a_code;

		if (a_code == ObjectEntryStatusCode::kLoaded)
		{
			if (m_info)
			{
				m_info.reset();
			}
		}
		else
		{
			if (!m_info)
			{
				m_info = std::make_unique<ObjectEntryStatusInfo>(
					a_params.actor->formID,
					a_params.npc->formID,
					a_params.race->formID);
			}
			else
			{
				*m_info = {
					a_params.actor->formID,
					a_params.npc->formID,
					a_params.race->formID
				};
			}
		}
	}

	void ObjectEntryStatus::clear() noexcept
	{
		m_lastStatus = ObjectEntryStatusCode::kNone;
		m_info.reset();
	}


}