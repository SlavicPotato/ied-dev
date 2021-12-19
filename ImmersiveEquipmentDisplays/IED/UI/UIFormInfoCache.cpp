#include "pch.h"

#include "UIFormInfoCache.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	class Controller;
	namespace UI
	{
		UIFormInfoCache::UIFormInfoCache(
			Controller& a_controller) :
			m_controller(a_controller)
		{
		}

		auto UIFormInfoCache::LookupForm(
			Game::FormID a_form)
			-> const formInfoResult_t*
		{
			if (!a_form)
			{
				return nullptr;
			}

			/*if (m_data.size() > 5000)
			{
				m_data.clear();
			}*/

			auto r = m_data.try_emplace(a_form);
			if (r.second)
			{
				m_controller.QueueLookupFormInfo(
					a_form,
					[this, a_form](std::unique_ptr<formInfoResult_t> a_info) {
						if (!a_info)
						{
							return;
						}

						auto it = m_data.find(a_form);
						if (it != m_data.end())
						{
							it->second.info = std::move(a_info);
						}
					});

				return nullptr;
			}
			else
			{
				return r.first->second.info.get();
			}
		}

		void UIFormInfoCache::Trim()
		{
		}

	}
}