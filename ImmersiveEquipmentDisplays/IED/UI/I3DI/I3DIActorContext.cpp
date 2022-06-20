#include "pch.h"

#include "I3DIActorContext.h"

#include "I3DICommonData.h"

#include "IED/Controller/Controller.h"
#include "IED/Controller/NodeOverrideData.h"

namespace IED
{
	namespace UI
	{
		I3DIActorContext::I3DIActorContext(
			I3DICommonData& a_data,
			Controller&     a_controller,
			Game::FormID    a_actor) noexcept(false) :
			m_controller(a_controller),
			m_actor(a_actor)
		{
			auto& data = m_controller.GetData();

			auto it = data.find(m_actor);
			if (it == data.end())
			{
				throw std::exception("actor doesn't exist");
			}

			auto& wn = it->second.GetWeapNodes();

			auto& nod = NodeOverrideData::GetWeaponNodeData();

			for (auto& e : nod)
			{
				auto& model = a_data.assets.GetModel(e.second.modelID);
				if (!model)
				{
					continue;
				}

				auto itwn = std::find_if(
					wn.begin(),
					wn.end(),
					[&](auto& a_v) {
						return a_v.nodeName == e.first;
					});

				if (itwn == wn.end())
				{
					continue;
				}

				auto r = m_weaponNodes.try_emplace(
					e.first,
					a_data.scene.GetDevice().Get(),
					model,
					e.first,
					*this);

				m_objectController.GetData().emplace_back(std::addressof(r.first->second));
			}
		}

		bool I3DIActorContext::Update()
		{
			auto& data = m_controller.GetData();

			auto it = data.find(m_actor);
			if (it == data.end())
			{
				m_lastUpdateFailed = true;
				return false;
			}

			auto& wn = it->second.GetWeapNodes();

			for (auto& e : wn)
			{
				auto itwn = m_weaponNodes.find(e.nodeName);
				if (itwn == m_weaponNodes.end())
				{
					continue;
				}

				itwn->second.UpdateLocalMatrix(e.node->m_localTransform);
				itwn->second.UpdateWorldMatrix(e.node->m_worldTransform);
				itwn->second.UpdateBound();
			}

			m_lastUpdateFailed = false;
			m_ranFirstUpdate = true;

			return true;
		}

		void I3DIActorContext::Draw(I3DICommonData& a_data)
		{
			if (!m_ranFirstUpdate)
			{
				return;
			}

			m_objectController.Update(a_data);
		}

		void I3DIActorContext::Render(I3DICommonData& a_data)
		{
			if (!m_ranFirstUpdate)
			{
				return;
			}

			for (auto& e : m_weaponNodes)
			{
				e.second.Draw(a_data.scene);
			}
		}
	}
}