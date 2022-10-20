#pragma once

#include "EffectShaderData.h"
#include "INode.h"
#include "ObjectDatabase.h"
#include "ObjectManagerCommon.h"

#include "IED/ConfigBaseValues.h"

namespace IED
{

	enum class ObjectEntryFlags : std::uint32_t
	{
		kNone = 0,

		kRefSyncDisableFailedOrphan = 1u << 1,
		kScbLeft                    = 1u << 2,
		kSyncReferenceTransform     = 1u << 6,
		kPlaySound                  = 1u << 8,
		kIsGroup                    = 1u << 9,
		kInvisible                  = 1u << 24,
	};

	DEFINE_ENUM_CLASS_BITWISE(ObjectEntryFlags);

	struct ObjectEntryBase
	{
		ObjectEntryBase()  = default;
		~ObjectEntryBase() = default;

		ObjectEntryBase(const ObjectEntryBase&) = delete;
		ObjectEntryBase(ObjectEntryBase&&)      = delete;
		ObjectEntryBase& operator=(const ObjectEntryBase&) = delete;
		ObjectEntryBase& operator=(ObjectEntryBase&&) = delete;

		bool reset(
			Game::ObjectRefHandle    a_handle,
			const NiPointer<NiNode>& a_root,
			const NiPointer<NiNode>& a_root1p);

		bool SetNodeVisible(const bool a_switch) const noexcept;
		bool DeferredHideNode(const std::uint8_t a_delay) const noexcept;
		void ResetDeferredHide() const noexcept;

		SKMP_FORCEINLINE auto IsNodeVisible() const
		{
			return state && !state->flags.test(ObjectEntryFlags::kInvisible);
		}

		SKMP_FORCEINLINE auto IsActive() const
		{
			return IsNodeVisible();
		}

		SKMP_FORCEINLINE auto GetFormIfActive() const
		{
			return IsActive() ? state->form : nullptr;
		}

		struct AnimationState
		{
			RE::WeaponAnimationGraphManagerHolderPtr weapAnimGraphManagerHolder;
			stl::fixed_string                        currentAnimationEvent;

			void UpdateAndSendAnimationEvent(const stl::fixed_string& a_event);
		};

		struct State :
			AnimationState
		{
			State()  = default;
			~State() = default;

			State(const State&) = delete;
			State(State&&)      = delete;
			State& operator=(const State&) = delete;
			State& operator=(State&&) = delete;

			struct GroupObject :
				AnimationState
			{
				GroupObject(
					NiNode*            a_rootNode,
					NiPointer<NiNode>& a_object) :
					rootNode(a_rootNode),
					object(a_object)
				{
				}

				NiPointer<NiNode>      rootNode;
				NiPointer<NiNode>      object;
				Data::cacheTransform_t transform;

				void PlayAnimation(Actor* a_actor, const stl::fixed_string& a_sequence);
			};

			void UpdateData(
				const Data::configBaseValues_t& a_in)
			{
				UpdateFlags(a_in);
				transform.Update(a_in);

				resetTriggerFlags = a_in.flags & Data::BaseFlags::kResetTriggerFlags;
			}

			void UpdateFlags(
				const Data::configBaseValues_t& a_in) noexcept
			{
				// gross but efficient

				static_assert(
					std::is_same_v<std::underlying_type_t<ObjectEntryFlags>, std::underlying_type_t<Data::BaseFlags>> &&
					stl::underlying(ObjectEntryFlags::kPlaySound) == stl::underlying(Data::BaseFlags::kPlaySound) &&
					stl::underlying(ObjectEntryFlags::kSyncReferenceTransform) == stl::underlying(Data::BaseFlags::kSyncReferenceTransform));

				flags =
					(flags & ~(ObjectEntryFlags::kPlaySound | ObjectEntryFlags::kSyncReferenceTransform | ObjectEntryFlags::kRefSyncDisableFailedOrphan)) |
					static_cast<ObjectEntryFlags>((a_in.flags & (Data::BaseFlags::kPlaySound | Data::BaseFlags::kSyncReferenceTransform)));
			}

			void UpdateAnimationGraphs(const BSAnimationUpdateData& a_data);

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

			void Cleanup(Game::ObjectRefHandle a_handle);

			void UpdateAndPlayAnimation(
				Actor*                   a_actor,
				const stl::fixed_string& a_sequence);

			TESForm*                                           form{ nullptr };
			Game::FormID                                       formid;
			Game::FormID                                       modelForm;
			stl::flag<ObjectEntryFlags>                        flags{ ObjectEntryFlags::kNone };
			stl::flag<Data::BaseFlags>                         resetTriggerFlags{ Data::BaseFlags::kNone };
			Data::NodeDescriptor                               nodeDesc;
			nodesRef_t                                         nodes;
			Data::cacheTransform_t                             transform;
			stl::list<ObjectDatabase::ObjectDatabaseEntry>     dbEntries;
			stl::unordered_map<stl::fixed_string, GroupObject> groupObjects;
			stl::fixed_string                                  currentSequence;
			long long                                          created{ 0 };
			std::uint8_t                                       hideCountdown{ 0 };
			bool                                               atmReference{ true };
		};

		/*struct EffectShaderState
		{
			EffectShaderData effectShaders;
		};*/

		std::unique_ptr<State>            state;
		std::unique_ptr<EffectShaderData> effectShaderData;
	};

}