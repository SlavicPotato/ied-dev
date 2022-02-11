#include "pch.h"

#include "IED/Controller/Controller.h"
#include "IED/Main.h"

#include "Papyrus.h"
#include "PapyrusCommon.h"
#include "PapyrusCustomItem.h"

namespace IED
{
	namespace Papyrus
	{
		static std::uint32_t GetVersion(StaticFunctionTag*)
		{
			return MAKE_PLUGIN_VERSION(
				PLUGIN_VERSION_MAJOR,
				PLUGIN_VERSION_MINOR,
				PLUGIN_VERSION_REVISION);
		}

		static void AddActorBlock(StaticFunctionTag*, Actor* a_actor, BSFixedString a_key)
		{
			if (!a_actor)
			{
				return;
			}

			auto key = GetKey(a_key);
			if (key.empty())
			{
				return;
			}

			ITaskPool::AddTask(
				[actor = a_actor->formID, key = std::move(key)]() {
					g_controller->AddActorBlock(actor, key);
				});
		}

		static void RemoveActorBlock(StaticFunctionTag*, Actor* a_actor, BSFixedString a_key)
		{
			if (!a_actor)
			{
				return;
			}

			auto key = GetKey(a_key);
			if (key.empty())
			{
				return;
			}

			ITaskPool::AddTask(
				[actor = a_actor->formID, key = std::move(key)]() {
					g_controller->RemoveActorBlock(actor, key);
				});
		}

		static void Evaluate(StaticFunctionTag*, Actor* a_actor)
		{
			if (a_actor)
			{
				g_controller->QueueEvaluate(a_actor, ControllerUpdateFlags::kNone);
			}
		}

		static void EvaluateAll(StaticFunctionTag*)
		{
			g_controller->QueueEvaluateAll(ControllerUpdateFlags::kNone);
		}

		static void Reset(StaticFunctionTag*, Actor* a_actor)
		{
			if (a_actor)
			{
				g_controller->QueueReset(a_actor, ControllerUpdateFlags::kNone);
			}
		}

		static void ResetAll(StaticFunctionTag*)
		{
			g_controller->QueueResetAll(ControllerUpdateFlags::kNone);
		}

		bool Register(VMClassRegistry* a_registry)
		{
			a_registry->RegisterFunction(
				new NativeFunction0<StaticFunctionTag, std::uint32_t>(
					"GetVersion",
					SCRIPT_NAME,
					GetVersion,
					a_registry));

			a_registry->RegisterFunction(
				new NativeFunction2<StaticFunctionTag, void, Actor*, BSFixedString>(
					"AddActorBlock",
					SCRIPT_NAME,
					AddActorBlock,
					a_registry));

			a_registry->RegisterFunction(
				new NativeFunction2<StaticFunctionTag, void, Actor*, BSFixedString>(
					"RemoveActorBlock",
					SCRIPT_NAME,
					RemoveActorBlock,
					a_registry));

			a_registry->RegisterFunction(
				new NativeFunction1<StaticFunctionTag, void, Actor*>(
					"Evaluate",
					"IED",
					Evaluate,
					a_registry));

			a_registry->RegisterFunction(
				new NativeFunction0<StaticFunctionTag, void>(
					"EvaluateAll",
					SCRIPT_NAME,
					EvaluateAll,
					a_registry));

			a_registry->RegisterFunction(
				new NativeFunction1<StaticFunctionTag, void, Actor*>(
					"Reset",
					SCRIPT_NAME,
					Reset,
					a_registry));

			a_registry->RegisterFunction(
				new NativeFunction0<StaticFunctionTag, void>(
					"ResetAll",
					SCRIPT_NAME,
					ResetAll,
					a_registry));

			Custom::Register(a_registry);

			return true;
		}
	}
}