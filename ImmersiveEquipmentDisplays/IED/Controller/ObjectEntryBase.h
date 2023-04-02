#pragma once

#include "EffectShaderData.h"
#include "INode.h"
#include "ObjectDatabase.h"
#include "ObjectLight.h"
#include "ObjectManagerCommon.h"
#include "ObjectSound.h"

#include "IED/AnimationUpdateController.h"
#include "IED/ConfigBaseValues.h"
#include "IED/Physics/SimComponent.h"

namespace IED
{

	enum class ObjectEntryFlags : std::uint32_t
	{
		kNone = 0,

		kRefSyncDisableFailedOrphan = 1u << 1,
		kScbLeft                    = 1u << 2,
		kSyncReferenceTransform     = 1u << 6,
		kPlayEquipSound             = 1u << 8,
		kIsGroup                    = 1u << 9,
		kWantUnloadAfterHide        = 1u << 10,
		kHasCollisionObjectScale    = 1u << 11,
		kHideLight                  = 1u << 24,
		kInvisible                  = 1u << 31,

		//kInvisibilityFlags = kInvisible | kWantUnloadAfterHide,
	};

	DEFINE_ENUM_CLASS_BITWISE(ObjectEntryFlags);

	enum class GroupObjectEntryFlags : std::uint32_t
	{
		kNone = 0,

		kHasLight = 1u << 1,
	};

	DEFINE_ENUM_CLASS_BITWISE(GroupObjectEntryFlags);

	class IObjectManager;

	struct ObjectEntryBase
	{
		ObjectEntryBase() noexcept  = default;
		~ObjectEntryBase() noexcept = default;

		ObjectEntryBase(const ObjectEntryBase&)            = delete;
		ObjectEntryBase& operator=(const ObjectEntryBase&) = delete;

		bool reset(
			Game::ObjectRefHandle    a_handle,
			const NiPointer<NiNode>& a_root,
			const NiPointer<NiNode>& a_root1p,
			IObjectManager&          a_db,
			bool                     a_defer) noexcept;

		bool SetObjectVisible(const bool a_switch) const noexcept;
		bool DeferredHideObject(const std::uint8_t a_delay) const noexcept;
		void ResetDeferredHide() const noexcept;

		SKMP_143_CONSTEXPR auto IsNodeVisible() const noexcept
		{
			return data.state && !data.state->flags.test(ObjectEntryFlags::kInvisible);
		}

		SKMP_143_CONSTEXPR auto IsActive() const noexcept
		{
			return IsNodeVisible();
		}

		SKMP_143_CONSTEXPR auto GetFormIfActive() const noexcept
		{
			return IsActive() ? data.state->form : nullptr;
		}

		struct CommonNodes
		{
			CommonNodes() = default;

			explicit CommonNodes(
				NiNode* a_rootNode,
				NiNode* a_object) noexcept :
				rootNode(a_rootNode),
				object(a_object)
			{
			}

			NiPointer<NiNode> rootNode;
			NiPointer<NiNode> object;
		};

		struct QuiverArrowState
		{
			explicit QuiverArrowState(NiNode* a_arrowQuiver) noexcept;

			void Update(std::int32_t a_count) noexcept;

			stl::vector<NiPointer<NiAVObject>> arrows;
			std::int32_t                       inventoryCount{ 0 };
		};

		struct ObjectAnim
		{
			RE::WeaponAnimationGraphManagerHolderPtr holder;
			stl::fixed_string                        currentEvent;

			void UpdateAndSendAnimationEvent(const stl::fixed_string& a_event) noexcept;
			void Cleanup();
		};

		struct Object
		{
			Object() = default;

			explicit Object(TESForm* a_modelForm) noexcept :
				modelForm(a_modelForm)
			{
			}

			explicit Object(
				TESForm* a_modelForm,
				NiNode*  a_rootNode,
				NiNode*  a_object) noexcept :
				modelForm(a_modelForm),
				commonNodes(a_rootNode, a_object)
			{
			}

			void UnregisterFromControllers(Game::FormID a_owner) noexcept;
			void CleanupObject(Game::ObjectRefHandle a_handle) noexcept;

			TESForm*                     modelForm{ nullptr };
			CommonNodes                  commonNodes;
			Data::cacheTransform_t       transform;
			ObjectDatabaseEntry          dbEntry;
			std::unique_ptr<ObjectLight> light;
			ObjectSound                  sound;
			ObjectAnim                   anim;
		};

		struct State :
			Object
		{
			State() noexcept  = default;
			~State() noexcept = default;

			State(const State&)            = delete;
			State& operator=(const State&) = delete;

			struct GroupObject :
				Object
			{
				explicit GroupObject(
					TESForm* a_modelForm,
					NiNode*  a_rootNode,
					NiNode*  a_object) noexcept :
					Object(a_modelForm, a_rootNode, a_object)
				{
				}

				void PlayAnimation(Actor* a_actor, const stl::fixed_string& a_sequence) noexcept;
			};

			void UpdateData(const Data::configBaseValues_t& a_in) noexcept
			{
				UpdateFlags(a_in);
				transform.Update(a_in);

				resetTriggerFlags = a_in.flags & Data::BaseFlags::kResetTriggerFlags;
			}

			constexpr void UpdateFlags(
				const Data::configBaseValues_t& a_in) noexcept
			{
				// gross but efficient

				static_assert(
					std::is_same_v<std::underlying_type_t<ObjectEntryFlags>, std::underlying_type_t<Data::BaseFlags>> &&
					stl::underlying(ObjectEntryFlags::kPlayEquipSound) == stl::underlying(Data::BaseFlags::kPlayEquipSound) &&
					stl::underlying(ObjectEntryFlags::kHideLight) == stl::underlying(Data::BaseFlags::kHideLight) &&
					stl::underlying(ObjectEntryFlags::kSyncReferenceTransform) == stl::underlying(Data::BaseFlags::kSyncReferenceTransform));

				flags =
					(flags & ~(ObjectEntryFlags::kPlayEquipSound | ObjectEntryFlags::kSyncReferenceTransform | ObjectEntryFlags::kRefSyncDisableFailedOrphan | ObjectEntryFlags::kHideLight)) |
					static_cast<ObjectEntryFlags>((a_in.flags & (Data::BaseFlags::kPlayEquipSound | Data::BaseFlags::kSyncReferenceTransform | Data::BaseFlags::kHideLight)));
			}

			void UpdateArrows(std::int32_t a_count) noexcept;

			/*void UpdateGroupTransforms(const Data::configModelGroup_t& a_group)
			{
				for (auto& e : a_group.entries)
				{
					if (auto it = groupObjects.find(e.first);
					    it != groupObjects.end())
					{
						it->second.transform.Update(e.second.transform);
					}
				}
			}*/

			void Cleanup(Game::ObjectRefHandle a_handle) noexcept;

			void UpdateAndPlayAnimationSequence(
				Actor*                   a_actor,
				const stl::fixed_string& a_sequence) noexcept;

			void SetLightsVisible(bool a_switch) noexcept;
			void SetVisible(bool a_switch) noexcept;

			template <class Tf>
			constexpr void visit_db_entries(Tf a_func)                           //
				noexcept(std::is_nothrow_invocable_v<Tf, ObjectDatabaseEntry&>)  //
				requires(std::invocable<Tf, ObjectDatabaseEntry&>)
			{
				if (auto& d = dbEntry)
				{
					a_func(d);
				}

				for (auto& e : groupObjects)
				{
					if (auto& d = e.second.dbEntry)
					{
						a_func(d);
					}
				}
			}

			constexpr bool IsReferenceMovedOrOphaned() const noexcept
			{
				if (const auto* const objParent = commonNodes.rootNode->m_parent)
				{
					if (const auto* const objParentParent = objParent->m_parent)
					{
						if (const auto* const refParent = ref->m_parent)
						{
							return refParent != objParentParent;
						}
						else
						{
							return true;
						}
					}
				}

				return false;
			}

			[[nodiscard]] constexpr bool HasPhysicsNode() const noexcept
			{
				return static_cast<bool>(physics.get());
			}
			TESForm*                                           form{ nullptr };
			stl::flag<ObjectEntryFlags>                        flags{ ObjectEntryFlags::kNone };
			stl::flag<Data::BaseFlags>                         resetTriggerFlags{ Data::BaseFlags::kNone };
			Data::NodeDescriptor                               nodeDesc;
			NiPointer<NiNode>                                  ref;
			NiPointer<NiNode>                                  physics;
			stl::unordered_map<stl::fixed_string, GroupObject> groupObjects;
			stl::smart_ptr<PHYSimComponent>                    simComponent;
			stl::fixed_string                                  currentSequence;
			std::optional<luid_tag>                            currentGeomTransformTag;
			stl::optional<float>                               colliderScale;
			std::unique_ptr<QuiverArrowState>                  arrowState;
			Game::FormID                                       owner;
			std::uint8_t                                       hideCountdown{ 0 };
			bool                                               atmReference{ true };
		};

		struct ObjectEntryData
		{
			void Cleanup(
				Game::ObjectRefHandle    a_handle,
				const NiPointer<NiNode>& a_root,
				const NiPointer<NiNode>& a_root1p,
				ObjectDatabase&          a_db) noexcept;

			[[nodiscard]] inline SKMP_143_CONSTEXPR explicit operator bool() const noexcept
			{
				return state || effectShaderData;
			}

			std::unique_ptr<State>            state;
			std::unique_ptr<EffectShaderData> effectShaderData;
		};

		SKMP_143_CONSTEXPR void DisableRefSync() noexcept
		{
			if (auto& state = data.state)
			{
				state->flags.set(ObjectEntryFlags::kRefSyncDisableFailedOrphan);
			}
		}

		ObjectEntryData data;
	};

}