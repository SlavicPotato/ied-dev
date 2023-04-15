#pragma once

#include "Localization/Common.h"

namespace IED
{
	namespace UI
	{
		enum class UIActorInfoStrings : Localization::StringID
		{
			DataUnavailable      = 6900,
			CouldNotUpdate       = 6901,
			PlayerEnemy          = 6902,
			SellBuy              = 6903,
			ActiveEffects        = 6904,
			SpellOrEnchantment   = 6905,
			InventoryWeight      = 6906,
			SitSleepState        = 6907,
			FlyState             = 6908,
			LifeState            = 6909,
			KnockState           = 6910,
			AttackState          = 6911,
			WeaponState          = 6912,
			MovementState        = 6913,
			WalkingState         = 6914,
			RunningState         = 6915,
			SprintingState       = 6916,
			SneakingState        = 6917,
			SwimmingState        = 6918,
			TalkingToPlayerState = 6919,
			ForceRunState        = 6920,
			ForceSneakState      = 6921,
			HeadTrackingState    = 6922,
			ReanimatingState     = 6923,
			WantBlockingState    = 6924,
			FlightBlockedState   = 6925,
			RecoilState          = 6926,
			AllowFlyingState     = 6927,
			StaggeredState       = 6928,
			ActorState           = 6929,

			Normal              = 6930,
			WantToSit           = 6931,
			WaitingForSitAnim   = 6932,
			IsSitting           = 6933,
			WantToStand         = 6934,
			WantToSleep         = 6935,
			WaitingForSleepAnim = 6936,
			IsSleeping          = 6937,
			WantToWake          = 6938,

			TakeOff  = 6939,
			Cruising = 6940,
			Hovering = 6941,
			Landing  = 6942,
			Perching = 6943,
			Action   = 6944,

			Alive         = 6945,
			Dying         = 6946,
			Unconcious    = 6947,
			Reanimate     = 6948,
			Recycle       = 6949,
			Restrained    = 6950,
			EssentialDown = 6951,
			Bleedout      = 6952,

			Explode          = 6953,
			ExplodeLeadIn    = 6954,
			Out              = 6955,
			OutLeadIn        = 6956,
			Queued           = 6957,
			GetUp            = 6958,
			Down             = 6959,
			WaitForTaskQueue = 6960,

			Draw             = 6961,
			Swing            = 6962,
			Hit              = 6963,
			NextAttack       = 6964,
			FollowThrough    = 6965,
			Bash             = 6966,
			BowDraw          = 6967,
			BowAttached      = 6968,
			BowDrawn         = 6969,
			BowReleasing     = 6970,
			BowReleased      = 6971,
			BowNextAttack    = 6972,
			BowFollowThrough = 6973,
			Fire             = 6974,
			Firing           = 6975,
			Fired            = 6976,

			Sheathed      = 6977,
			WantToDraw    = 6978,
			Drawing       = 6979,
			Drawn         = 6980,
			WantToSheathe = 6981,
			Sheathing     = 6982,

			ActorValues = 6983,

			Skeleton3D    = 6984,
			WeaponNodes   = 6985,
			CMENodes      = 6986,
			MOVNodes      = 6987,
			NumChildNodes = 6988,

			BipedData = 6989,
		};
	}
}