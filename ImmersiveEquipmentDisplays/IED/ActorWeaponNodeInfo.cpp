#include "pch.h"

#include "ActorWeaponNodeInfo.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	void actorWeaponNodeInfo_t::Update(
		Actor*            a_actor,
		const Controller& a_controller)
	{
		data.clear();

		const stl::lock_guard lock(a_controller.GetLock());

		auto& objects = a_controller.GetActorMap();

		auto it = objects.find(a_actor->formID);
		if (it != objects.end())
		{
			for (auto& e : it->second.GetGearNodes())
			{
				const auto* const node = e.node3p.node.get();

				data.emplace_back(
					e.nodeName,
					node->m_parent ?
						node->m_parent->m_name.c_str() :
						stl::fixed_string(),
					node->m_children.size(),
					node->IsVisible());
			}
		}
	}
}