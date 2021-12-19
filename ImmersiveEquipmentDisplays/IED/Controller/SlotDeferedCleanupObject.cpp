#include "pch.h"

#include "SlotDeferedCleanupObject.h"
#include "ObjectManagerData.h"

#include "IED/EngineExtensions.h"

namespace IED
{
	/*bool SlotDeferedCleanupObject::Queue(
		Game::ObjectRefHandle a_handle,
		const objectEntrySlot_t& a_object)
	{
		if (biped == Biped::BIPED_OBJECT::kNone)
		{
			return false;
		}

		if (object)
		{
			EngineExtensions::CleanupNode(a_handle, object);
			object.reset();
		}

		if (!a_object.nodes.obj)
		{
			return false;
		}

		object = a_object.nodes.obj;
		timeout = IPerfCounter::Query() + IPerfCounter::T(1000000);

		return true;
	}

	void SlotDeferedCleanupObject::TryCleanup(
		Game::ObjectRefHandle a_handle)
	{
		if (!object)
		{
			return;
		}

		NiPointer<TESObjectREFR> ref;
		if (!a_handle.Lookup(ref))
		{
			EngineExtensions::CleanupNode(0, object);
			object.reset();
			return;
		}

		auto actor = ref->As<Actor>();
		if (actor)
		{
			if (auto bip = actor->GetBiped(false))
			{
				if (auto data = bip->ptr)
				{
					if (!data->objects[biped].object &&
					    IPerfCounter::Query() < timeout)
					{
						return;
					}
				}
			}
		}

		EngineExtensions::CleanupNode(a_handle, object);
		object.reset();
	}

	void SlotDeferedCleanupObject::ForceCleanup(
		Game::ObjectRefHandle a_handle)
	{
		if (object)
		{
			EngineExtensions::CleanupNode(a_handle, object);
			object.reset();
		}
	}*/
}