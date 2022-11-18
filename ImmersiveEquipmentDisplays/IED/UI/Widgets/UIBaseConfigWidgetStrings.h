#pragma once

#include "Localization/Common.h"

namespace IED
{
	namespace UI
	{
		enum class UIBaseConfigString : Localization::StringID
		{
			EquipmentOverrides      = 4000,
			HideInFurniture         = 4001,
			HideLayingDown          = 4002,
			RemoveScabbard          = 4003,
			DropOnDeath             = 4004,
			SyncReference           = 4005,
			Use1pWeaponModels       = 4006,
			AttachmentModeColon     = 4007,
			EmptyNodeWarning        = 4008,
			KeepTorchFlame          = 4009,
			UseWorldModel           = 4010,
			IgnoreRaceEquipTypes    = 4011,
			EffectShaders           = 4012,
			AddShaderEntry          = 4013,
			EffectShaderEntryExists = 4014,
			InventoryCheck          = 4015,
			CreatePhysicsDataInfo   = 4016,
		};
	}
}