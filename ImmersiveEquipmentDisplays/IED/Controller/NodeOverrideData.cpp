
#include "pch.h"

#include "NodeOverrideData.h"

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
			{ "CME WeaponDaggerOnBack", { "Dagger On Back", "CME WeaponDaggerOnBack" } },
			{ "CME WeaponDaggerLeftOnBack", { "Dagger On Back Left", "CME WeaponDaggerLeftOnBack" } },
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
			{ "MOV WeaponDaggerOnBack", { "Dagger On Back", "MOV WeaponDaggerOnBack", WeaponPlacementID::OnBack } },
			{ "MOV WeaponDaggerLeftOnBack", { "Dagger On Back Left", "MOV WeaponDaggerLeftOnBack", WeaponPlacementID::OnBack } },
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

			{ "WeaponBackIED", {

								   "WeaponBackIED",
								   "MOV WeaponBackDefault",
								   "Two-Handed IED",
								   AnimationWeaponSlot::None,
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
							   true,
							   {
								   { "MOV WeaponBowDefault", { "Bow" } },
								   { "MOV WeaponBowChesko", { "Bow Chesko" } },
								   { "MOV WeaponBowBetter", { "Bow Better" } },
								   { "MOV WeaponBowFSM", { "Bow FSM" } },
							   }

						   } },

			{ "WeaponBowIED", {

								  "WeaponBowIED",
								  "MOV WeaponBowDefault",
								  "Bow IED",
								  AnimationWeaponSlot::None,
								  false,
								  {
									  { "MOV WeaponBowDefault", { "Bow" } },
									  { "MOV WeaponBowChesko", { "Bow Chesko" } },
									  { "MOV WeaponBowBetter", { "Bow Better" } },
									  { "MOV WeaponBowFSM", { "Bow FSM" } },
								  }

							  } },

			{ "QUIVER", {

							"QUIVER",
							"MOV QUIVERDefault",
							"Quiver",
							AnimationWeaponSlot::Quiver,
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
								false,
								{

									{ "MOV ShieldBackDefault", { "Shield Back" } },
								}

							} }

		}),

		m_extra(std::initializer_list<exn_ctor_init_t>{
			{

				"MOV WeaponDaggerOnBack",
				"CME WeaponDaggerOnBack",
				"CME Spine2 [Spn2]",
				{ 1.0f, { 8.6871f, 0.8402f, 18.6266f }, { -2.0656f, 0.8240f, 3.0770f } },
				{ 1.0f, { 8.7244f, 2.1135f, 17.6729f }, { -2.0656f, 0.8240f, 3.0770f } },
				WeaponPlacementID::OnBack

			},
			{

				"MOV WeaponDaggerLeftOnBack",
				"CME WeaponDaggerLeftOnBack",
				"CME Spine2 [Spn2]",
				{ 1.0f, { -8.1261f, 1.9337f, 18.4871f }, { 2.0656f, -0.8239f, 3.0770f } },
				{ 1.0f, { -8.1435f, 3.4921f, 18.5906f }, { 2.0656f, -0.8239f, 3.0770f } },
				WeaponPlacementID::OnBack

			},
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

		})
	{
	}

	void NodeOverrideData::Create()
	{
		if (!m_Instance)
		{
			m_Instance = std::make_unique<NodeOverrideData>();
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
			auto index = rng->Get();
			return std::addressof(movs[index]);
		}
		catch (...)
		{
			return nullptr;
		}
	}

}