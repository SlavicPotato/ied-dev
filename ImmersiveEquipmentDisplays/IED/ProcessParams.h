#pragma once

#include "CommonParams.h"
#include "ConfigData.h"
#include "Inventory.h"

#include "Controller/ActorTempData.h"
#include "Controller/ControllerCommon.h"

namespace IED
{
	enum class ProcessStateUpdateFlags : std::uint8_t
	{
		kNone = 0,

		kMenuUpdate     = 1ui8 << 0,
		kForceUpdate    = 1ui8 << 1,
		kObjectAttached = 1ui8 << 2,

		kUpdateMask = kMenuUpdate | kForceUpdate,

		kEffectShadersSuspended = 1ui8 << 4,
		kBipedDataUpdated   = 1ui8 << 5,
	};

	DEFINE_ENUM_CLASS_BITWISE(ProcessStateUpdateFlags);

	struct processState_t
	{
		stl::flag<ProcessStateUpdateFlags> flags{ ProcessStateUpdateFlags::kNone };
	};

	struct processParamsData_t
	{
		inline processParamsData_t(
			Actor* const                           a_actor,
			const Game::ObjectRefHandle            a_handle,
			const Data::ConfigSex                  a_configSex,
			const stl::flag<ControllerUpdateFlags> a_flags,
			ActorTempData&                         a_tmp) :
			handle(a_handle),
			configSex(a_configSex),
			flags(a_flags),
			useCount(a_tmp.uc),
			collector(a_tmp.sr, a_tmp.idt, a_tmp.eqt, a_actor)
		{
			a_tmp.uc.clear();
		}

		inline constexpr void mark_slot_presence_change(Data::ObjectSlot a_slot) noexcept
		{
			slotPresenceChanges.set(
				static_cast<Data::ObjectSlotBits>(
					1u << stl::underlying(a_slot)));
		}

		const Game::ObjectRefHandle      handle;
		const Data::ConfigSex            configSex;
		stl::flag<ControllerUpdateFlags> flags;
		stl::flag<Data::ObjectSlotBits>  slotPresenceChanges{ Data::ObjectSlotBits::kNone };
		UseCountContainer&               useCount;
		InventoryInfoCollector           collector;
	};

	struct processParams_t :
		processParamsData_t,
		CommonParams
	{
		template <class... Args>
		inline constexpr processParams_t(
			const Data::ConfigSex                  a_configSex,
			const stl::flag<ControllerUpdateFlags> a_flags,
			Actor* const                           a_actor,
			const Game::ObjectRefHandle            a_handle,
			ActorTempData&                         a_tmp,
			Args&&... a_args) :
			processParamsData_t(
				a_actor,
				a_handle,
				a_configSex,
				a_flags,
				a_tmp),
			CommonParams(
				std::forward<Args>(a_args)...)
		{
		}

		void SuspendEffectShaders() noexcept;

		processState_t state;
	};

}