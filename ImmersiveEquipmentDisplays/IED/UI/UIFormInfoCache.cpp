#include "pch.h"

#include "UIFormInfoCache.h"

#include "IED/Controller/Controller.h"

#include "Drivers/Render.h"

namespace IED
{
	class Controller;
	namespace UI
	{
		UIFormInfoCache::UIFormInfoCache(
			Controller& a_controller) :
			m_lastCleanup(IPerfCounter::Query()),
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

			auto fc = Drivers::UI::GetFrameCount();

			auto r = m_data.try_emplace(a_form, fc);
			if (r.second)
			{
				m_controller.QueueLookupFormInfo(
					a_form,
					[this, a_form](std::unique_ptr<formInfoResult_t> a_info) {
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
				r.first->second.fcaccess = fc;
				return r.first->second.info.get();
			}
		}

		void UIFormInfoCache::RunCleanup()
		{
			if (m_data.size() < CLEANUP_THRESHOLD)
			{
				return;
			}

			auto tp = IPerfCounter::Query();

			if (IPerfCounter::delta_us(m_lastCleanup, tp) < CLEANUP_RUN_INTERVAL)
			{
				return;
			}

			m_lastCleanup = tp;

			auto fc = Drivers::UI::GetFrameCount();

			std::vector<decltype(m_data)::const_iterator> candidates;

			for (auto it = m_data.begin(); it != m_data.end(); ++it)
			{
				if (fc - it->second.fcaccess > 2)
				{
					candidates.emplace_back(it);
				}
			}

			if (candidates.empty())
			{
				return;
			}

			std::sort(
				candidates.begin(),
				candidates.end(),
				[](auto& a_lhs, auto& a_rhs) {
					return a_lhs->second.fcaccess <
				           a_rhs->second.fcaccess;
				});

			for (auto& e : candidates)
			{
				if (m_data.size() <= CLEANUP_TARGET)
				{
					break;
				}

				m_data.erase(e);
			}
		}

	}
}