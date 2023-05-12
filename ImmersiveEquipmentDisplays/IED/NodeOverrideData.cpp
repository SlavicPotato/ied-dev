
#include "pch.h"

#include "NodeOverrideData.h"

#include "IED/Parsers/JSONConfigAdditionalCMENodeListParser.h"
#include "IED/Parsers/JSONConfigExtraNodeListParser.h"
#include "IED/Parsers/JSONConfigNodeMonitorEntryListParser.h"
#include "IED/Parsers/JSONConvertNodesListParser.h"

#include "Serialization/Serialization.h"

namespace IED
{
	std::unique_ptr<NodeOverrideData> NodeOverrideData::m_Instance;

	NodeOverrideData::NodeOverrideData() :
		m_cme(std::initializer_list<init_list_cm>{

			{ "CME WeaponAxeDefault", { "Axe", "CME WeaponAxeDefault" } },
			{ "CME WeaponAxeLeftDefault", { "Axe Left", "CME WeaponAxeLeftDefault" } },
			{ "CME WeaponAxeReverse", { "Axe Reverse", "CME WeaponAxeReverse" } },
			{ "CME WeaponAxeLeftReverse", { "Axe Reverse Left", "CME WeaponAxeLeftReverse" } },
			{ "CME WeaponAxeOnBack", { "Axe Back", "CME WeaponAxeOnBack" } },
			{ "CME WeaponAxeLeftOnBack", { "Axe Back Left", "CME WeaponAxeLeftOnBack" } },
			{ "CME WeaponMaceDefault", { "Mace", "CME WeaponMaceDefault" } },
			{ "CME WeaponMaceLeftDefault", { "Mace Left", "CME WeaponMaceLeftDefault" } },
			{ "CME WeaponSwordDefault", { "Sword", "CME WeaponSwordDefault" } },
			{ "CME WeaponSwordLeftDefault", { "Sword Left", "CME WeaponSwordLeftDefault" } },
			{ "CME WeaponSwordOnBack", { "Sword Back", "CME WeaponSwordOnBack" } },
			{ "CME WeaponSwordLeftOnBack", { "Sword Back Left", "CME WeaponSwordLeftOnBack" } },
			{ "CME WeaponSwordSWP", { "Sword SWP", "CME WeaponSwordSWP" } },
			{ "CME WeaponSwordLeftSWP", { "Sword SWP Left", "CME WeaponSwordLeftSWP" } },
			{ "CME WeaponSwordFSM", { "Sword FSM", "CME WeaponSwordFSM" } },
			{ "CME WeaponSwordLeftFSM", { "Sword FSM Left", "CME WeaponSwordLeftFSM" } },
			{ "CME WeaponSwordLeftHip", { "Sword Left-Hip", "CME WeaponSwordLeftHip" } },
			{ "CME WeaponSwordLeftLeftHip", { "Sword Left-Hip Left", "CME WeaponSwordLeftLeftHip" } },
			{ "CME WeaponSwordNMD", { "Sword NMD", "CME WeaponSwordNMD" } },
			{ "CME WeaponSwordLeftNMD", { "Sword NMD Left", "CME WeaponSwordLeftNMD" } },
			{ "CME WeaponDaggerDefault", { "Dagger", "CME WeaponDaggerDefault" } },
			{ "CME WeaponDaggerLeftDefault", { "Dagger Left", "CME WeaponDaggerLeftDefault" } },
			{ "CME WeaponDaggerBackHip", { "Dagger Back Hip", "CME WeaponDaggerBackHip" } },
			{ "CME WeaponDaggerLeftBackHip", { "Dagger Back Hip Left", "CME WeaponDaggerLeftBackHip" } },
			{ "CME WeaponDaggerAnkle", { "Dagger Ankle", "CME WeaponDaggerAnkle" } },
			{ "CME WeaponDaggerLeftAnkle", { "Dagger Ankle Left", "CME WeaponDaggerLeftAnkle" } },
			{ "CME WeaponBackDefault", { "Two-Handed", "CME WeaponBackDefault" } },
			{ "CME WeaponBackSWP", { "Two-Handed SWP", "CME WeaponBackSWP" } },
			{ "CME WeaponBackFSM", { "Two-Handed FSM", "CME WeaponBackFSM" } },
			{ "CME WeaponBackAxeMaceDefault", { "Two-Handed Axe/Mace", "CME WeaponBackAxeMaceDefault" } },
			{ "CME WeaponBackAxeMaceSWP", { "Two-Handed Axe/Mace SWP", "CME WeaponBackAxeMaceSWP" } },
			{ "CME WeaponBackAxeMaceFSM", { "Two-Handed Axe/Mace FSM", "CME WeaponBackAxeMaceFSM" } },
			{ "CME WeaponStaffDefault", { "Staff", "CME WeaponStaffDefault" } },
			{ "CME WeaponStaffLeftDefault", { "Staff Left", "CME WeaponStaffLeftDefault" } },
			{ "CME WeaponBowDefault", { "Bow", "CME WeaponBowDefault" } },
			{ "CME WeaponBowChesko", { "Bow Chesko", "CME WeaponBowChesko" } },
			{ "CME WeaponBowBetter", { "Bow Better", "CME WeaponBowBetter" } },
			{ "CME WeaponBowFSM", { "Bow FSM", "CME WeaponBowFSM" } },
			{ "CME WeaponCrossbowDefault", { "Crossbow", "CME WeaponCrossbowDefault" } },
			{ "CME WeaponCrossbowChesko", { "Crossbow Chesko", "CME WeaponCrossbowChesko" } },
			{ "CME QUIVERDefault", { "Quiver", "CME QUIVERDefault" } },
			{ "CME QUIVERChesko", { "Quiver Chesko", "CME QUIVERChesko" } },
			{ "CME QUIVERLeftHipBolt", { "Quiver Left-Hip Bolt", "CME QUIVERLeftHipBolt" } },
			{ "CME BOLTDefault", { "Bolt", "CME BOLTDefault" } },
			{ "CME BOLTChesko", { "Bolt Chesko", "CME BOLTChesko" } },
			{ "CME BOLTLeftHipBolt", { "Bolt Left-Hip", "CME BOLTLeftHipBolt" } },
			{ "CME BOLTXP32", { "Bolt XP32", "CME BOLTXP32" } },
			{ "CME BOLTABQ", { "Bolt ABQ", "CME BOLTABQ" } },
			{ "CME ShieldBackDefault", { "Shield Back", "CME ShieldBackDefault" } }

		}),

		m_mov(std::initializer_list<init_list_cm>{

			{ "MOV WeaponAxeDefault", { "Axe", "MOV WeaponAxeDefault" } },
			{ "MOV WeaponAxeLeftDefault", { "Axe Left", "MOV WeaponAxeLeftDefault" } },
			{ "MOV WeaponAxeReverse", { "Axe Reverse", "MOV WeaponAxeReverse" } },
			{ "MOV WeaponAxeLeftReverse", { "Axe Reverse Left", "MOV WeaponAxeLeftReverse" } },
			{ "MOV WeaponAxeOnBack", { "Axe Back", "MOV WeaponAxeOnBack", WeaponPlacementID::OnBack } },
			{ "MOV WeaponAxeLeftOnBack", { "Axe Back Left", "MOV WeaponAxeLeftOnBack", WeaponPlacementID::OnBack } },
			{ "MOV WeaponMaceDefault", { "Mace", "MOV WeaponMaceDefault" } },
			{ "MOV WeaponMaceLeftDefault", { "Mace Left", "MOV WeaponMaceLeftDefault" } },
			{ "MOV WeaponSwordDefault", { "Sword", "MOV WeaponSwordDefault" } },
			{ "MOV WeaponSwordLeftDefault", { "Sword Left", "MOV WeaponSwordLeftDefault" } },
			{ "MOV WeaponSwordOnBack", { "Sword Back", "MOV WeaponSwordOnBack", WeaponPlacementID::OnBack } },
			{ "MOV WeaponSwordLeftOnBack", { "Sword Back Left", "MOV WeaponSwordLeftOnBack", WeaponPlacementID::OnBack } },
			{ "MOV WeaponSwordSWP", { "Sword SWP", "MOV WeaponSwordSWP", WeaponPlacementID::OnBack } },
			{ "MOV WeaponSwordLeftSWP", { "Sword SWP Left", "MOV WeaponSwordLeftSWP", WeaponPlacementID::OnBack } },
			{ "MOV WeaponSwordFSM", { "Sword FSM", "MOV WeaponSwordFSM" } },
			{ "MOV WeaponSwordLeftFSM", { "Sword FSM Left", "MOV WeaponSwordLeftFSM" } },
			{ "MOV WeaponSwordLeftHip", { "Sword Left-Hip", "MOV WeaponSwordLeftHip" } },
			{ "MOV WeaponSwordLeftLeftHip", { "Sword Left-Hip Left", "MOV WeaponSwordLeftLeftHip" } },
			{ "MOV WeaponSwordNMD", { "Sword NMD", "MOV WeaponSwordNMD" } },
			{ "MOV WeaponSwordLeftNMD", { "Sword NMD Left", "MOV WeaponSwordLeftNMD" } },
			{ "MOV WeaponDaggerDefault", { "Dagger", "MOV WeaponDaggerDefault" } },
			{ "MOV WeaponDaggerLeftDefault", { "Dagger Left", "MOV WeaponDaggerLeftDefault" } },
			{ "MOV WeaponDaggerBackHip", { "Dagger Back Hip", "MOV WeaponDaggerBackHip", WeaponPlacementID::OnBackHip } },
			{ "MOV WeaponDaggerLeftBackHip", { "Dagger Back Hip Left", "MOV WeaponDaggerLeftBackHip", WeaponPlacementID::OnBackHip } },
			{ "MOV WeaponDaggerAnkle", { "Dagger Ankle", "MOV WeaponDaggerAnkle", WeaponPlacementID::Ankle } },
			{ "MOV WeaponDaggerLeftAnkle", { "Dagger Ankle Left", "MOV WeaponDaggerLeftAnkle", WeaponPlacementID::Ankle } },
			{ "MOV WeaponBackDefault", { "Two-Handed", "MOV WeaponBackDefault" } },
			{ "MOV WeaponBackSWP", { "Two-Handed SWP", "MOV WeaponBackSWP" } },
			{ "MOV WeaponBackFSM", { "Two-Handed FSM", "MOV WeaponBackFSM", WeaponPlacementID::AtHip } },
			{ "MOV WeaponBackAxeMaceDefault", { "Two-Handed Axe/Mace", "MOV WeaponBackAxeMaceDefault" } },
			{ "MOV WeaponBackAxeMaceSWP", { "Two-Handed Axe/Mace SWP", "MOV WeaponBackAxeMaceSWP" } },
			{ "MOV WeaponBackAxeMaceFSM", { "Two-Handed Axe/Mace FSM", "MOV WeaponBackAxeMaceFSM", WeaponPlacementID::AtHip } },
			{ "MOV WeaponStaffDefault", { "Staff", "MOV WeaponStaffDefault" } },
			{ "MOV WeaponStaffLeftDefault", { "Staff Left", "MOV WeaponStaffLeftDefault" } },
			{ "MOV WeaponBowDefault", { "Bow", "MOV WeaponBowDefault" } },
			{ "MOV WeaponBowChesko", { "Bow Chesko", "MOV WeaponBowChesko" } },
			{ "MOV WeaponBowBetter", { "Bow Better", "MOV WeaponBowBetter" } },
			{ "MOV WeaponBowFSM", { "Bow FSM", "MOV WeaponBowFSM" } },
			{ "MOV WeaponCrossbowDefault", { "Crossbow", "MOV WeaponCrossbowDefault" } },
			{ "MOV WeaponCrossbowChesko", { "Crossbow Chesko", "MOV WeaponCrossbowChesko" } },
			{ "MOV QUIVERDefault", { "Quiver", "MOV QUIVERDefault" } },
			{ "MOV QUIVERChesko", { "Quiver Chesko", "MOV QUIVERChesko", WeaponPlacementID::Frostfall } },
			{ "MOV QUIVERLeftHipBolt", { "Quiver Left-Hip Bolt", "MOV QUIVERLeftHipBolt" } },
			{ "MOV BOLTDefault", { "Bolt", "MOV BOLTDefault" } },
			{ "MOV BOLTChesko", { "Bolt Chesko", "MOV BOLTChesko" } },
			{ "MOV BOLTLeftHipBolt", { "Bolt Left-Hip", "MOV BOLTLeftHipBolt" } },
			{ "MOV BOLTABQ", { "Bolt ABQ", "MOV BOLTABQ" } },
			{ "MOV BOLTXP32", { "Bolt XP32", "MOV BOLTXP32", WeaponPlacementID::Frostfall } },
			{ "MOV ShieldBackDefault", { "Shield Back", "MOV ShieldBackDefault" } }

		}),

		m_monitor(std::initializer_list<const char*>{

			"WeaponAxe",
			"WeaponMace",
			"WeaponSword",
			"WeaponDagger",
			"WeaponBack",
			"WeaponBow",
			"QUIVER",
			"WeaponAxeLeft",
			"WeaponMaceLeft",
			"WeaponSwordLeft",
			"WeaponDaggerLeft",
			"ShieldBack",
			"WeaponStaff",
			"WeaponStaffLeft"

		}),

		m_weap(std::initializer_list<init_list_weap>{

			{ "WeaponSword", {

								 "WeaponSword",
								 "MOV WeaponSwordDefault",
								 "Sword",
								 AnimationWeaponSlot::Sword,
								 I3DIModelID::kOneHandedSword,
								 GearNodeID::k1HSword,
								 true,
								 {

									 { "MOV WeaponSwordDefault", { "Sword" } },
									 { "MOV WeaponSwordOnBack", { "Sword Back" } },
									 { "MOV WeaponSwordLeftHip", { "Sword Left-Hip" } },
									 { "MOV WeaponSwordSWP", { "Sword SWP" } },
									 { "MOV WeaponSwordFSM", { "Sword FSM" } },
									 { "MOV WeaponSwordNMD", { "Sword NMD" } }

								 }

							 } },

			{ "WeaponSwordLeft", {

									 "WeaponSwordLeft",
									 "MOV WeaponSwordLeftDefault",
									 "Sword Left",
									 AnimationWeaponSlot::SwordLeft,
									 I3DIModelID::kOneHandedSword,
									 GearNodeID::k1HSwordLeft,
									 false,
									 {

										 { "MOV WeaponSwordLeftDefault", { "Sword Left" } },
										 { "MOV WeaponSwordLeftOnBack", { "Sword Back Left" } },
										 { "MOV WeaponSwordLeftLeftHip", { "Sword Left-Hip Left" } },
										 { "MOV WeaponSwordLeftSWP", { "Sword SWP Left" } },
										 { "MOV WeaponSwordLeftFSM", { "Sword FSM Left" } },
										 { "MOV WeaponSwordLeftNMD", { "Sword NMD Left" } }

									 }

								 } },

			{ "WeaponAxe", {

							   "WeaponAxe",
							   "MOV WeaponAxeDefault",
							   "Axe",
							   AnimationWeaponSlot::Axe,
							   I3DIModelID::kOneHandedAxe,
							   GearNodeID::k1HAxe,
							   true,
							   {

								   { "MOV WeaponAxeDefault", { "Axe" } },
								   { "MOV WeaponAxeReverse", { "Axe Reverse" } },
								   { "MOV WeaponAxeOnBack", { "Axe Back" } },

							   }

						   } },

			{ "WeaponAxeLeft", {

								   "WeaponAxeLeft",
								   "MOV WeaponAxeLeftDefault",
								   "Axe Left",
								   AnimationWeaponSlot::AxeLeft,
								   I3DIModelID::kOneHandedAxe,
								   GearNodeID::k1HAxeLeft,
								   false,
								   {

									   { "MOV WeaponAxeLeftDefault", { "Axe Left" } },
									   { "MOV WeaponAxeLeftReverse", { "Axe Reverse Left" } },
									   { "MOV WeaponAxeLeftOnBack", { "Axe Back Left" } },

								   }

							   } },

			{ "WeaponDagger", {

								  "WeaponDagger",
								  "MOV WeaponDaggerDefault",
								  "Dagger",
								  AnimationWeaponSlot::Dagger,
								  I3DIModelID::kDagger,
								  GearNodeID::kDagger,
								  true,
								  {

									  { "MOV WeaponDaggerDefault", { "Dagger" } },
									  { "MOV WeaponDaggerBackHip", { "Dagger Back Hip" } },
									  { "MOV WeaponDaggerAnkle", { "Dagger Ankle" } },
									  { "MOV WeaponDaggerOnBack", { "Dagger On Back" } }

								  }

							  } },

			{ "WeaponDaggerLeft", {

									  "WeaponDaggerLeft",
									  "MOV WeaponDaggerLeftDefault",
									  "Dagger Left",
									  AnimationWeaponSlot::DaggerLeft,
									  I3DIModelID::kDagger,
									  GearNodeID::kDaggerLeft,
									  false,
									  {

										  { "MOV WeaponDaggerLeftDefault", { "Dagger Left" } },
										  { "MOV WeaponDaggerLeftBackHip", { "Dagger Back Hip Left" } },
										  { "MOV WeaponDaggerLeftAnkle", { "Dagger Ankle Left" } },
										  { "MOV WeaponDaggerLeftOnBack", { "Dagger On Back Left" } }

									  }

								  } },

			{ "WeaponMace", {

								"WeaponMace",
								"MOV WeaponMaceDefault",
								"Mace",
								AnimationWeaponSlot::Mace,
								I3DIModelID::kMace,
								GearNodeID::kMace,
								true,
								{

									{ "MOV WeaponMaceDefault", { "Mace" } },

								}

							} },

			{ "WeaponMaceLeft", {

									"WeaponMaceLeft",
									"MOV WeaponMaceLeftDefault",
									"Mace Left",
									AnimationWeaponSlot::MaceLeft,
									I3DIModelID::kMace,
									GearNodeID::kMaceLeft,
									false,
									{

										{ "MOV WeaponMaceLeftDefault", { "Mace Left" } },

									}

								} },

			{ "WeaponStaff", {

								 "WeaponStaff",
								 "MOV WeaponStaffDefault",
								 "Staff",
								 AnimationWeaponSlot::None,
								 I3DIModelID::kStaff,
								 GearNodeID::kStaff,
								 false,
								 {

									 { "MOV WeaponStaffDefault", { "Staff" } },

								 }

							 } },

			{ "WeaponStaffLeft", {

									 "WeaponStaffLeft",
									 "MOV WeaponStaffLeftDefault",
									 "Staff Left",
									 AnimationWeaponSlot::None,
									 I3DIModelID::kStaff,
									 GearNodeID::kStaffLeft,
									 false,
									 {

										 { "MOV WeaponStaffLeftDefault", { "Staff Left" } }

									 }

								 } },

			{ "WeaponBack", {

								"WeaponBack",
								"MOV WeaponBackDefault",
								"Two-Handed",
								AnimationWeaponSlot::TwoHanded,
								I3DIModelID::kTwoHanded,
								GearNodeID::kTwoHanded,
								true,
								{

									{ "MOV WeaponBackDefault", { "Two-Handed" } },
									{ "MOV WeaponBackSWP", { "Two-Handed SWP" } },
									{ "MOV WeaponBackFSM", { "Two-Handed FSM" } },
									{ "MOV WeaponBackAxeMaceDefault", { "Two-Handed Axe/Mace" } },
									{ "MOV WeaponBackAxeMaceSWP", { "Two-Handed Axe/Mace SWP" } },
									{ "MOV WeaponBackAxeMaceFSM", { "Two-Handed Axe/Mace FSM" } }

								}

							} },

			{ "WeaponBackExtra", {

									 "WeaponBackExtra",
									 "MOV WeaponBackDefault",
									 "Two-Handed Extra",
									 AnimationWeaponSlot::None,
									 I3DIModelID::kTwoHanded,
									 GearNodeID::kNone,
									 false,
									 {

										 { "MOV WeaponBackDefault", { "Two-Handed" } },
										 { "MOV WeaponBackSWP", { "Two-Handed SWP" } },
										 { "MOV WeaponBackFSM", { "Two-Handed FSM" } },
										 { "MOV WeaponBackAxeMaceDefault", { "Two-Handed Axe/Mace" } },
										 { "MOV WeaponBackAxeMaceSWP", { "Two-Handed Axe/Mace SWP" } },
										 { "MOV WeaponBackAxeMaceFSM", { "Two-Handed Axe/Mace FSM" } }

									 }

								 } },

			{ "WeaponBow", {

							   "WeaponBow",
							   "MOV WeaponBowDefault",
							   "Bow",
							   AnimationWeaponSlot::None,
							   I3DIModelID::kBow,
							   GearNodeID::kBow,
							   true,
							   {
								   { "MOV WeaponBowDefault", { "Bow" } },
								   { "MOV WeaponBowChesko", { "Bow Chesko" } },
								   { "MOV WeaponBowBetter", { "Bow Better" } },
								   { "MOV WeaponBowFSM", { "Bow FSM" } },
								   { "MOV WeaponCrossbowDefault", { "Crossbow" } },
								   { "MOV WeaponCrossbowChesko", { "Crossbow Chesko" } },
							   }

						   } },

			{ "WeaponBowExtra", {

									"WeaponBowExtra",
									"MOV WeaponBowDefault",
									"Bow Extra",
									AnimationWeaponSlot::None,
									I3DIModelID::kBow,
									GearNodeID::kNone,
									false,
									{
										{ "MOV WeaponBowDefault", { "Bow" } },
										{ "MOV WeaponBowChesko", { "Bow Chesko" } },
										{ "MOV WeaponBowBetter", { "Bow Better" } },
										{ "MOV WeaponBowFSM", { "Bow FSM" } },
										{ "MOV WeaponCrossbowDefault", { "Crossbow" } },
										{ "MOV WeaponCrossbowChesko", { "Crossbow Chesko" } },
									}

								} },

			{ "QUIVER", {

							"QUIVER",
							"MOV QUIVERDefault",
							"Quiver",
							AnimationWeaponSlot::Quiver,
							I3DIModelID::kQuiver,
							GearNodeID::kQuiver,
							true,
							{
								{ "MOV QUIVERDefault", { "Quiver" } },
								{ "MOV QUIVERChesko", { "Quiver Chesko" } },
								{ "MOV QUIVERLeftHipBolt", { "Quiver Left-Hip Bolt" } },
								{ "MOV BOLTDefault", { "Bolt" } },
								{ "MOV BOLTChesko", { "Bolt Chesko" } },
								{ "MOV BOLTLeftHipBolt", { "Bolt Left-Hip" } },
								{ "MOV BOLTABQ", { "Bolt ABQ" } },
								{ "MOV BOLTXP32", { "Bolt XP32" } },
							}

						} },

			{ "ShieldBack", {

								"ShieldBack",
								"MOV ShieldBackDefault",
								"Shield",
								AnimationWeaponSlot::None,
								I3DIModelID::kShield,
								GearNodeID::kShield,
								false,
								{
									{ "MOV ShieldBackDefault", { "Shield Back" } },
								}

							} }

		}),

		m_extraCopy(std::initializer_list<exn_copy_ctor_init_t>{
			{ "WeaponBack", "WeaponBackIED" },
			{ "WeaponBow", "WeaponBowIED" },
		}),

		m_transformOverride(std::initializer_list<xfrm_ovr_ctor_init_t>{
			{ "WeaponSwordLeft", { -1.687309f, -0.940114f, -3.022884f } },
			{ "HDT WeaponSwordLeft", { -1.687309f, -0.940114f, -3.022884f } },
			{ "WeaponAxeLeft", { -2.579556f, -0.866133f, -1.205785f } },
			{ "HDT WeaponAxeLeft", { -2.579556f, -0.866133f, -1.205785f } },
			{ "WeaponMaceLeft", { 1.595767f, 1.240697f, -3.068691f } },
			{ "HDT WeaponMaceLeft", { 1.595767f, 1.240697f, -3.068691f } },
			{ "WeaponDaggerLeft", { -0.444131f, -0.803249f, -1.805654f } },
			{ "HDT WeaponDaggerLeft", { -0.444131f, -0.803249f, -1.805654f } },
			{ "WeaponStaffLeft", { 0.445457f, -0.274162f, 1.455669f } },
			{ "HDT WeaponStaffLeft", { 0.445457f, -0.274162f, 1.455669f } },
		}),

		m_randPlacement(std::initializer_list<rw_ctor_init_t>{

			{

				"WeaponSword",
				"WeaponSwordLeft",
				{

					{ "MOV WeaponSwordDefault", "MOV WeaponSwordLeftDefault" },
					{ "MOV WeaponSwordOnBack", "MOV WeaponSwordLeftOnBack" },
					{ "MOV WeaponSwordSWP", "MOV WeaponSwordLeftSWP" },

				}

			},
			{

				"WeaponAxe",
				"WeaponAxeLeft",
				{

					{ "MOV WeaponAxeDefault", "MOV WeaponAxeLeftDefault" },
					{ "MOV WeaponAxeOnBack", "MOV WeaponAxeLeftOnBack" },

				}

			},
			{

				"WeaponDagger",
				"WeaponDaggerLeft",
				{

					{ "MOV WeaponDaggerDefault", "MOV WeaponDaggerLeftDefault" },
					{ "MOV WeaponDaggerBackHip", "MOV WeaponDaggerLeftBackHip" },
					{ "MOV WeaponDaggerAnkle", "MOV WeaponDaggerLeftAnkle" },

				}

			},
			/*{

				"WeaponBow",
				nullptr,
				{

					{ "MOV WeaponBowDefault", nullptr },
					{ "MOV WeaponBowChesko", nullptr },
					{ "MOV WeaponBowBetter", nullptr },

				}

			},*/

		}),

		m_humanoidSkeletonSignatures{

			11462500511823126705,
			2191670159909408727,
			11827777347581811248,
			6419035761879502692,
			6034431282899458572,
			271026958073932624,
			4625650181268377090,
			10927971447177431054,
			17428621824148309550

		},

		m_humanoidSkeletonSignaturesXp32{

			11462500511823126705,
			2191670159909408727,
			11827777347581811248,
			6419035761879502692,
			6034431282899458572

		}
	{
	}

	void NodeOverrideData::Create()
	{
		if (!m_Instance)
		{
			m_Instance = std::make_unique_for_overwrite<NodeOverrideData>();
		}
	}

	void NodeOverrideData::LoadAndAddExtraNodes(const char* a_path)
	{
		stl::list<Data::configExtraNodeList_t> data;
		if (m_Instance->LoadEntryList(a_path, data))
		{
			m_Instance->AddExtraNodeData(data);
		}
	}

	void NodeOverrideData::LoadAndAddNodeMonitor(const char* a_path)
	{
		stl::list<Data::configNodeMonitorEntryList_t> data;
		if (m_Instance->LoadEntryList(a_path, data))
		{
			m_Instance->AddNodeMonitorData(std::move(data));
		}
	}

	void NodeOverrideData::LoadAndAddConvertNodes(const char* a_path)
	{
		stl::list<Data::configConvertNodesList_t> data;
		if (m_Instance->LoadEntryList(a_path, data))
		{
			m_Instance->AddConvertNodesData(data);
		}
	}

	void NodeOverrideData::LoadAndAddAdditionalCMENodes(const char* a_path)
	{
		stl::list<Data::configAdditionalCMENodeList_t> data;
		if (m_Instance->LoadEntryList(a_path, data))
		{
			m_Instance->AddAdditionalCMENodeData(data);
		}
	}

	namespace detail
	{
		static void make_sorted_path_list(
			const char*         a_path,
			stl::set<fs::path>& a_out)
		{
			const fs::path allowedExt{ ".json" };

			for (const auto& entry : fs::directory_iterator(a_path))
			{
				if (!entry.is_regular_file())
				{
					continue;
				}

				auto& path = entry.path();

				if (!path.has_extension() ||
				    path.extension() != allowedExt)
				{
					continue;
				}

				a_out.emplace(entry.path());
			}
		}
	}

	template <class T>
	bool NodeOverrideData::LoadEntryList(
		const char*   a_path,
		stl::list<T>& a_out)
	{
		try
		{
			stl::set<fs::path> data;

			detail::make_sorted_path_list(a_path, data);

			for (const auto& path : data)
			{
				const auto strPath = Serialization::SafeGetPath(path);

				std::unique_ptr<T> result;

				try
				{
					result = LoadDataFile<T>(path);
				}
				catch (const std::exception& e)
				{
					Exception(
						e,
						"%s: [%s]",
						__FUNCTION__,
						strPath.c_str());

					continue;
				}
				catch (...)
				{
					Error(
						"%s: [%s] exception occured",
						__FUNCTION__,
						strPath.c_str());

					continue;
				}

				Debug(
					"%s: loaded '%s' [%zu]",
					__FUNCTION__,
					strPath.c_str(),
					result->size());

				a_out.emplace_back(std::move(*result));
			}

			return true;
		}
		catch (const std::exception& e)
		{
			Error(
				"%s: %s",
				__FUNCTION__,
				e.what());

			return false;
		}
		catch (...)
		{
			Error(
				"%s: exception occured",
				__FUNCTION__);

			return false;
		}
	}

	template <class T>
	std::unique_ptr<T> NodeOverrideData::LoadDataFile(const fs::path& a_path)
	{
		using namespace Serialization;

		Json::Value root;

		ReadData(a_path, root);

		ParserState state;
		Parser<T>   parser(state);

		auto result = std::make_unique<T>();

		if (!parser.Parse(root, *result))
		{
			throw parser_exception("parse failed");
		}

		return result;
	}

	void NodeOverrideData::AddExtraNodeData(
		const stl::list<Data::configExtraNodeList_t>& a_data)
	{
		for (auto& e : a_data)
		{
			for (auto& f : e)
			{
				if (f.skel.empty())
				{
					continue;
				}

				const stl::fixed_string mov = std::string("MOV ") + *f.name;
				const stl::fixed_string cme = std::string("CME ") + *f.name;

				if (m_mov.contains(mov))
				{
					Warning(
						"%s: '%s' - node already exists",
						__FUNCTION__,
						mov.c_str());

					continue;
				}

				if (m_cme.contains(cme))
				{
					Warning(
						"%s: '%s' - node already exists",
						__FUNCTION__,
						cme.c_str());

					continue;
				}

				auto it = std::find_if(
					m_extramov.begin(),
					m_extramov.end(),
					[&](const auto& a_v) {
						return a_v.name_node == f.name;
					});

				if (it != m_extramov.end())
				{
					Warning(
						"%s: '%s' - node already exists",
						__FUNCTION__,
						f.name.c_str());

					continue;
				}

				auto& rv = m_extramov.emplace_back(
					f.name,
					mov,
					cme,
					f.parent,
					f.placementID,
					f.desc);

				for (auto& g : f.skel)
				{
					auto& v = rv.skel.emplace_back(g.match);

					static_assert(
						stl::array_size<decltype(g.sxfrms)>::value ==
						stl::array_size<decltype(v.sxfrms)>::value);

					for (std::size_t i = 0; i < std::size(g.sxfrms); i++)
					{
						auto& src = g.sxfrms[i];
						auto& dst = v.sxfrms[i];

						dst.xfrm = src.xfrm.to_nitransform();

						for (auto& h : src.syncNodes)
						{
							if (!h.name.empty())
							{
								dst.syncNodes.emplace_back(
									h.name.c_str(),
									h.flags);
							}
						}
					}
				}

				m_mov.try_emplace(
					rv.name_mov,
					rv.name_mov,
					rv.desc,
					rv.placementID);

				m_cme.try_emplace(
					rv.name_cme,
					rv.name_cme,
					rv.desc,
					rv.placementID);
			}
		}
	}

	void NodeOverrideData::AddAdditionalCMENodeData(
		const stl::list<Data::configAdditionalCMENodeList_t>& a_data)
	{
		for (auto& e : a_data)
		{
			for (auto& f : e)
			{
				auto r = m_cme.try_emplace(
					f.node,
					f.node,
					f.desc,
					WeaponPlacementID::None,
					OverrideNodeEntryFlags::kAdditionalNode);

				if (!r.second)
				{
					Warning(
						"%s: [%s] - node already exists",
						__FUNCTION__,
						f.node.c_str());
				}
			}
		}
	}

	void NodeOverrideData::AddNodeMonitorData(
		stl::list<Data::configNodeMonitorEntryList_t>&& a_data)
	{
		for (auto& e : a_data)
		{
			for (auto& f : e)
			{
				if (f.data.uid == 0)
				{
					Warning(
						"%s: [%s] - invalid uid",
						__FUNCTION__,
						f.data.description.c_str());

					continue;
				}

				const auto r = m_nodeMonEntries.try_emplace(f.data.uid, std::move(f));

				if (!r.second)
				{
					Warning(
						"%s: %u [%s] - duplicate entry",
						__FUNCTION__,
						r.first->second.data.uid,
						r.first->second.data.description.c_str());

					continue;
				}
			}
		}
	}

	void NodeOverrideData::AddConvertNodesData(
		const stl::list<Data::configConvertNodesList_t>& a_data)
	{
		for (auto& e : a_data)
		{
			for (auto& f : e)
			{
				for (auto& g : f.match.ids)
				{
					m_convertNodes.ids.emplace(g);
				}

				for (auto& g : f.match.signatures)
				{
					m_convertNodes.signatures.emplace(g);
				}
			}
		}
	}

	auto NodeOverrideData::randWeapEntry_t::get_rand_entry() const
		-> const NodeOverrideData::randPlacementEntry_t*
	{
		if (!rng)
		{
			return nullptr;
		}

		try
		{
			return std::addressof(movs[rng->Get()]);
		}
		catch (...)
		{
			return nullptr;
		}
	}

}