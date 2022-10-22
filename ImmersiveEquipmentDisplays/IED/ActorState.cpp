#include "pch.h"

#include "ActorState.h"

#include "Controller/ObjectManagerData.h"
#include "Controller/ActorObjectHolder.h"

#include "IED/Data.h"

namespace IED
{
	namespace Data
	{
		actorStateHolder_t::actorStateHolder_t(
			const ObjectManagerData& a_data)
		{
			for (auto& e : a_data.GetObjects())
			{
				data.try_emplace(e.first, e.second);
			}
		}

		actorStateEntry_t::actorStateEntry_t(
			const ActorObjectHolder& a_holder)
		{
			using enum_type = std::underlying_type_t<ObjectType>;

			for (enum_type i = 0; i < stl::underlying(ObjectType::kMax); i++)
			{
				auto mainSlot = ItemData::GetSlotFromType(static_cast<ObjectType>(i));

				auto& sms = a_holder.GetSlot(mainSlot);
				auto& smd = slots[stl::underlying(mainSlot)];

				smd.lastEquipped = sms.slotState.lastEquipped;

				if (auto leftSlot = ItemData::GetLeftSlot(mainSlot);
				    leftSlot != ObjectSlot::kMax)
				{
					auto& sls = a_holder.GetSlot(leftSlot);
					auto& sld = slots[stl::underlying(leftSlot)];

					if (sls.slotState.lastEquipped)
					{
						sld.lastEquipped = sls.slotState.lastEquipped;

						if (sls.slotState.lastSeenEquipped > sms.slotState.lastSeenEquipped)
						{
							smd.lastSeenEquipped = std::numeric_limits<long long>::min();
							sld.lastSeenEquipped = std::numeric_limits<long long>::min() + 1;
						}
						else
						{
							smd.lastSeenEquipped = std::numeric_limits<long long>::min() + 1;
							sld.lastSeenEquipped = std::numeric_limits<long long>::min();
						}
					}
				}
			}
		}
	}
}