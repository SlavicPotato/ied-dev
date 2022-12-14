#pragma once

#include "CommonParams.h"
#include "ConfigData.h"
#include "Inventory.h"

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

		kEffectShadersReset = 1ui8 << 4,
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
			Actor* const                            a_actor,
			const Game::ObjectRefHandle             a_handle,
			const Data::ConfigSex                   a_configSex,
			const stl::flag<ControllerUpdateFlags>  a_flags,
			SlotResults&                            a_sr,
			Data::CollectorData::container_type&    a_idt,
			Data::CollectorData::eq_container_type& a_eqt,
			UseCountContainer&                      a_uc) :
			handle(a_handle),
			configSex(a_configSex),
			flags(a_flags),
			useCount(a_uc),
			collector(a_sr, a_idt, a_eqt, a_actor)
		{
			a_uc.clear();
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
			const Data::ConfigSex                   a_configSex,
			const stl::flag<ControllerUpdateFlags>  a_flags,
			Actor* const                            a_actor,
			const Game::ObjectRefHandle             a_handle,
			SlotResults&                            a_sr,
			Data::CollectorData::container_type&    a_idt,
			Data::CollectorData::eq_container_type& a_eqt,
			UseCountContainer&                      a_uc,
			Args&&... a_args) :
			processParamsData_t(
				a_actor,
				a_handle,
				a_configSex,
				a_flags,
				a_sr,
				a_idt,
				a_eqt,
				a_uc),
			CommonParams(
				std::forward<Args>(a_args)...)
		{
		}

		void ResetEffectShaders();

		processState_t state;
	};

}