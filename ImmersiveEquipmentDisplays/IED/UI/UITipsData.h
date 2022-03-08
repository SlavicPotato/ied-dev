#pragma once

#include "Localization/Common.h"

namespace IED
{
	namespace UI
	{
		enum class UITip : Localization::StringID
		{
			SyncReferenceNode                   = 1000,
			DropOnDeath                         = 1001,
			TargetNode                          = 1002,
			HideInFurniture                     = 1003,
			AttachmentMode                      = 1004,
			Position                            = 1005,
			Rotation                            = 1006,
			CustomInventoryItem                 = 1007,
			CustomFormModelSwap                 = 1008,
			CustomCountRange                    = 1009,
			CustomForm                          = 1010,
			PreferredItem                       = 1011,
			SkipTempRefs                        = 1012,
			Invisible                           = 1013,
			PropagateMemberToEquipmentOverrides = 1014,
			AlwaysUnloadSlot                    = 1015,
			HideEquipped                        = 1016,
			LeftWeapon                          = 1017,
			AlwaysUnloadCustom                  = 1018,
			HideLayingDown                      = 1019,
			RemoveScabbard                      = 1020,
			Load1pWeaponModel                   = 1021,
			CheckCannotWear                     = 1022,
			CustomChance                        = 1023,
			CustomEquipmentMode                 = 1024,
			IgnoreRaceEquipTypes                = 1025,
			DisableIfEquipped                   = 1026,
			ModelCache                          = 1027,
			CloseOnESC                          = 1028,
			EnableRestrictions                  = 1029,
			ControlLock                         = 1030,
			UIScale                             = 1031,
			AnimSupportWarning                  = 1032,
			CacheInfo                           = 1033,
			ImportMode                          = 1034,
			LoadARMA                            = 1035,
			SelectCrosshairActor                = 1036,
			KeepTorchFlame                      = 1037,
			DisableHavok                        = 1038,
			MatchSkin                           = 1039,
			SyncSexes                           = 1040,
			IsFavorited                         = 1041,
			NoCheckFav                          = 1042,
			FreezeTime                          = 1043,
			UseWorldModel                       = 1044,
			ReleaseFontData                     = 1045,
			IgnoreRaceEquipTypesSlot            = 1046,
			MatchChildLoc                       = 1047,
			MatchEitherFormKW                   = 1048,
			MatchWorldspaceParent               = 1049,
			PlayAnimation                       = 1050,
		};

	}
}