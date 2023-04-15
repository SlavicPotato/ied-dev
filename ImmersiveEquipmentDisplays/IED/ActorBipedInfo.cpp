#include "pch.h"

#include "ActorBipedInfo.h"

#include "FormCommon.h"

namespace IED
{
	void actorBipedInfo_t::Update(Actor* a_actor)
	{
		const auto& biped = a_actor->GetBiped1(false);

		if (!biped)
		{
			data.reset();
		}
		else
		{
			if (!data)
			{
				data.emplace();
			}

			using enum_type = std::underlying_type_t<BIPED_OBJECT>;
			for (enum_type i = 0; i < stl::underlying(BIPED_OBJECT::kTotal); i++)
			{
				auto& e = biped->objects[i];
				auto& f = (*data)[i];

				if (e.item)
				{
					f.item = e.item->formID;
				}
				else
				{
					f.item = {};
				}

				if (e.addon)
				{
					f.addon = e.addon->formID;
				}
				else
				{
					f.addon = {};
				}

				if (e.object)
				{
					f.geometry = {
						e.object->m_name.data(),
						e.object->IsVisible()
					};
				}
				else
				{
					f.geometry = {};
				}
			}
		}
	}
}