
#include "pch.h"

#include "NodeOverrideData.h"

namespace IED
{
	OverrideNodeInfo OverrideNodeInfo::m_Instance;

	void OverrideNodeInfo::Create()
	{
		m_Instance.m_cme = {

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
			{ "CME BOLTABQ", { "Bolt ABQ", "CME BOLTABQ" } },
			{ "CME ShieldBackDefault", { "Shield Back", "CME ShieldBackDefault" } }

		};

		m_Instance.m_mov = {

			{ "MOV WeaponAxeDefault", { "Axe", "MOV WeaponAxeDefault" } },
			{ "MOV WeaponAxeLeftDefault", { "Axe Left", "MOV WeaponAxeLeftDefault" } },
			{ "MOV WeaponAxeReverse", { "Axe Reverse", "MOV WeaponAxeReverse" } },
			{ "MOV WeaponAxeLeftReverse", { "Axe Reverse Left", "MOV WeaponAxeLeftReverse" } },
			{ "MOV WeaponAxeOnBack", { "Axe Back", "MOV WeaponAxeOnBack" } },
			{ "MOV WeaponAxeLeftOnBack", { "Axe Back Left", "MOV WeaponAxeLeftOnBack" } },
			{ "MOV WeaponMaceDefault", { "Mace", "MOV WeaponMaceDefault" } },
			{ "MOV WeaponMaceLeftDefault", { "Mace Left", "MOV WeaponMaceLeftDefault" } },
			{ "MOV WeaponSwordDefault", { "Sword", "MOV WeaponSwordDefault" } },
			{ "MOV WeaponSwordLeftDefault", { "Sword Left", "MOV WeaponSwordLeftDefault" } },
			{ "MOV WeaponSwordOnBack", { "Sword Back", "MOV WeaponSwordOnBack" } },
			{ "MOV WeaponSwordLeftOnBack", { "Sword Back Left", "MOV WeaponSwordLeftOnBack" } },
			{ "MOV WeaponSwordSWP", { "Sword SWP", "MOV WeaponSwordSWP" } },
			{ "MOV WeaponSwordLeftSWP", { "Sword SWP Left", "MOV WeaponSwordLeftSWP" } },
			{ "MOV WeaponSwordFSM", { "Sword FSM", "MOV WeaponSwordFSM" } },
			{ "MOV WeaponSwordLeftFSM", { "Sword FSM Left", "MOV WeaponSwordLeftFSM" } },
			{ "MOV WeaponSwordLeftHip", { "Sword Left-Hip", "MOV WeaponSwordLeftHip" } },
			{ "MOV WeaponSwordLeftLeftHip", { "Sword Left-Hip Left", "MOV WeaponSwordLeftLeftHip" } },
			{ "MOV WeaponSwordNMD", { "Sword NMD", "MOV WeaponSwordNMD" } },
			{ "MOV WeaponSwordLeftNMD", { "Sword NMD Left", "MOV WeaponSwordLeftNMD" } },
			{ "MOV WeaponDaggerDefault", { "Dagger", "MOV WeaponDaggerDefault" } },
			{ "MOV WeaponDaggerLeftDefault", { "Dagger Left", "MOV WeaponDaggerLeftDefault" } },
			{ "MOV WeaponDaggerBackHip", { "Dagger Back Hip", "MOV WeaponDaggerBackHip" } },
			{ "MOV WeaponDaggerLeftBackHip", { "Dagger Back Hip Left", "MOV WeaponDaggerLeftBackHip" } },
			{ "MOV WeaponDaggerAnkle", { "Dagger Ankle", "MOV WeaponDaggerAnkle" } },
			{ "MOV WeaponDaggerLeftAnkle", { "Dagger Ankle Left", "MOV WeaponDaggerLeftAnkle" } },
			{ "MOV WeaponBackDefault", { "Two-Handed", "MOV WeaponBackDefault" } },
			{ "MOV WeaponBackSWP", { "Two-Handed SWP", "MOV WeaponBackSWP" } },
			{ "MOV WeaponBackFSM", { "Two-Handed FSM", "MOV WeaponBackFSM" } },
			{ "MOV WeaponBackAxeMaceDefault", { "Two-Handed Axe/Mace", "MOV WeaponBackAxeMaceDefault" } },
			{ "MOV WeaponBackAxeMaceSWP", { "Two-Handed Axe/Mace SWP", "MOV WeaponBackAxeMaceSWP" } },
			{ "MOV WeaponBackAxeMaceFSM", { "Two-Handed Axe/Mace FSM", "MOV WeaponBackAxeMaceFSM" } },
			{ "MOV WeaponStaffDefault", { "Staff", "MOV WeaponStaffDefault" } },
			{ "MOV WeaponStaffLeftDefault", { "Staff Left", "MOV WeaponStaffLeftDefault" } },
			{ "MOV WeaponBowDefault", { "Bow", "MOV WeaponBowDefault" } },
			{ "MOV WeaponBowChesko", { "Bow Chesko", "MOV WeaponBowChesko" } },
			{ "MOV WeaponBowBetter", { "Bow Better", "MOV WeaponBowBetter" } },
			{ "MOV WeaponBowFSM", { "Bow FSM", "MOV WeaponBowFSM" } },
			{ "MOV WeaponCrossbowDefault", { "Crossbow", "MOV WeaponCrossbowDefault" } },
			{ "MOV WeaponCrossbowChesko", { "Crossbow Chesko", "MOV WeaponCrossbowChesko" } },
			{ "MOV QUIVERDefault", { "Quiver", "MOV QUIVERDefault" } },
			{ "MOV QUIVERChesko", { "Quiver Chesko", "MOV QUIVERChesko" } },
			{ "MOV QUIVERLeftHipBolt", { "Quiver Left-Hip Bolt", "MOV QUIVERLeftHipBolt" } },
			{ "MOV BOLTDefault", { "Bolt", "MOV BOLTDefault" } },
			{ "MOV BOLTChesko", { "Bolt Chesko", "MOV BOLTChesko" } },
			{ "MOV BOLTLeftHipBolt", { "Bolt Left-Hip", "MOV BOLTLeftHipBolt" } },
			{ "MOV BOLTABQ", { "Bolt ABQ", "MOV BOLTABQ" } },
			{ "MOV ShieldBackDefault", { "Shield Back", "MOV ShieldBackDefault" } }

		};

		m_Instance.m_monitor = {
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
		};

		m_Instance.m_weap = {

			{ "WeaponSword", {

								 "WeaponSword",
								 "MOV WeaponSwordDefault",
								 "Sword",
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
								  {

									  { "MOV WeaponDaggerDefault", { "Dagger" } },
									  { "MOV WeaponDaggerBackHip", { "Dagger Back Hip" } },
									  { "MOV WeaponDaggerAnkle", { "Dagger Ankle" } },

								  }

							  } },

			{ "WeaponDaggerLeft", {

									  "WeaponDaggerLeft",
									  "MOV WeaponDaggerLeftDefault",
									  "Dagger Left",
									  {

										  { "MOV WeaponDaggerLeftDefault", { "Dagger Left" } },
										  { "MOV WeaponDaggerLeftBackHip", { "Dagger Back Hip Left" } },
										  { "MOV WeaponDaggerLeftAnkle", { "Dagger Ankle Left" } }

									  }

								  } },

			{ "WeaponMace", {

								"WeaponMace",
								"MOV WeaponMaceDefault",
								"Mace",
								{

									{ "MOV WeaponMaceDefault", { "Mace" } },

								}

							} },

			{ "WeaponMaceLeft", {

									"WeaponMaceLeft",
									"MOV WeaponMaceLeftDefault",
									"Mace Left",
									{

										{ "MOV WeaponMaceLeftDefault", { "Mace Left" } },

									}

								} },

			{ "WeaponStaff", {

								 "WeaponStaff",
								 "MOV WeaponStaffDefault",
								 "Staff",
								 {

									 { "MOV WeaponStaffDefault", { "Staff" } },

								 }

							 } },

			{ "WeaponStaffLeft", {

									 "WeaponStaffLeft",
									 "MOV WeaponStaffLeftDefault",
									 "Staff Left",
									 {

										 { "MOV WeaponStaffLeftDefault", { "Staff Left" } }

									 }

								 } },

			{ "WeaponBack", {

								"WeaponBack",
								"MOV WeaponBackDefault",
								"Two-Handed",
								{
									{ "MOV WeaponBackDefault", { "Two-Handed" } },
									{ "MOV WeaponBackSWP", { "Two-Handed SWP" } },
									{ "MOV WeaponBackFSM", { "Two-Handed FSM" } }
								}

							} },

			{ "WeaponBow", {

							   "WeaponBow",
							   "MOV WeaponBowDefault",
							   "Bow",
							   {
								   { "MOV WeaponBowDefault", { "Bow" } },
								   { "MOV WeaponBowChesko", { "Bow Chesko" } },
								   { "MOV WeaponBowBetter", { "Bow Better" } },
							   }

						   } },

			{ "QUIVER", {

							"QUIVER",
							"MOV QUIVERDefault",
							"Quiver",
							{
								{ "MOV QUIVERDefault", { "Quiver" } },
								{ "MOV QUIVERChesko", { "Quiver Chesko" } },
								{ "MOV QUIVERLeftHipBolt", { "Quiver Left-Hip Bolt" } },
							}

						} }
		};
	}
}