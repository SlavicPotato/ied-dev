#pragma once

#include "BackgroundCloneLevel.h"
#include "ControllerCommon.h"
#include "IED/ConfigModelGroup.h"
#include "IED/ConfigStore.h"
#include "IED/Data.h"
#include "IED/ProcessParams.h"
#include "IModel.h"
#include "INode.h"
#include "IPersistentCounter.h"
#include "IRNG.h"
#include "ISound.h"
#include "ObjectDatabase.h"
#include "ObjectManagerData.h"

namespace IED
{
	enum class AttachObjectResult
	{
		kSucceeded,
		kFailed,
		kPending
	};

	class IObjectManager :
		public ObjectManagerData,
		public ObjectDatabase,
		public IPersistentCounter,
		public IRNG,
		public ISound,
		virtual public ILog
	{
	public:
		enum class AttachResultFlags : std::uint8_t
		{
			kNone = 0,

			kScbLeft            = 1ui8 << 0,
			kTorchFlameRemoved  = 1ui8 << 1,
			kTorchCustomRemoved = 1ui8 << 2,
		};

		FN_NAMEPROC("IObjectManager");

		constexpr void SetPlaySound(bool a_switch) noexcept
		{
			m_playSound = a_switch;
		}

		constexpr void SetPlaySoundNPC(bool a_switch) noexcept
		{
			m_playSoundNPC = a_switch;
		}

		[[nodiscard]] constexpr auto& GetLock() const noexcept
		{
			return m_lock;
		}

		bool RemoveActorImpl(
			TESObjectREFR*                   a_actor,
			Game::ObjectRefHandle            a_handle,
			stl::flag<ControllerUpdateFlags> a_flags) noexcept;

		bool RemoveActorImpl(
			Game::FormID                     a_actor,
			stl::flag<ControllerUpdateFlags> a_flags) noexcept;

		/*void QueueReSinkAnimationGraphs(
			Game::FormID a_actor);*/

		void RequestEvaluate(
			Game::FormID a_actor,
			bool         a_defer,
			bool         a_xfrmUpdate,
			bool         a_xfrmUpdateNoDefer) const noexcept;

		void RequestEvaluateLF(
			Game::FormID a_actor) const noexcept;

		// use when acquiring global lock may be detrimental to performance
		void QueueRequestEvaluate(
			Game::FormID a_actor,
			bool         a_defer,
			bool         a_xfrmUpdate,
			bool         a_xfrmUpdateNoDefer = false) const noexcept;

		void QueueRequestEvaluateLF(
			Game::FormID a_actor) const noexcept;

		void QueueRequestEvaluateAll() const noexcept;
		void QueueRequestEvaluateLFAll() const noexcept;

		void QueueRequestEvaluate(
			TESObjectREFR* a_actor,
			bool           a_defer,
			bool           a_xfrmUpdate,
			bool           a_xfrmUpdateNoDefer = false) const noexcept;

		void QueueClearVariablesOnAll(bool a_requestEval) noexcept;

		void QueueClearVariables(
			Game::FormID a_handle,
			bool         a_requestEval) noexcept;

		void QueueRequestVariableUpdateOnAll() const noexcept;
		void QueueRequestVariableUpdate(Game::FormID a_handle) const noexcept;

	protected:
		bool RemoveObject(
			TESObjectREFR*                   a_actor,
			Game::ObjectRefHandle            a_handle,
			ObjectEntryBase&                 a_objectEntry,
			ActorObjectHolder&               a_data,
			stl::flag<ControllerUpdateFlags> a_flags,
			bool                             a_defer,
			bool                             a_removeCloningTask = true) noexcept;

		void RemoveActorGear(
			TESObjectREFR*                   a_actor,
			Game::ObjectRefHandle            a_handle,
			stl::flag<ControllerUpdateFlags> a_flags) noexcept;

		bool RemoveActorGear(
			TESObjectREFR*                   a_actor,
			Game::ObjectRefHandle            a_handle,
			ActorObjectHolder&               a_holder,
			stl::flag<ControllerUpdateFlags> a_flags) noexcept;

		bool RemoveInvisibleObjects(
			ActorObjectHolder&    a_holder,
			Game::ObjectRefHandle a_handle) noexcept;

		void ClearObjectsImpl() noexcept;

		static void GetNodeName(
			TESForm*                   a_form,
			const IModel::ModelParams& a_params,
			char (&a_out)[INode::NODE_NAME_BUFFER_SIZE]) noexcept;

		bool ShouldBackgroundClone(
			const ProcessParams&       a_params,
			const IModel::ModelParams& a_modelParams) noexcept;

		AttachObjectResult LoadAndAttach(
			ProcessParams&                  a_params,
			const Data::configBaseValues_t& a_activeConfig,
			const Data::configBase_t&       a_config,
			ObjectEntryBase&                a_objectEntry,
			TESForm*                        a_form,
			TESForm*                        a_modelForm,
			const bool                      a_leftWeapon,
			const bool                      a_visible,
			const bool                      a_disableHavok,
			const bool                      a_physics) noexcept;

		AttachObjectResult LoadAndAttachGroup(
			ProcessParams&                  a_params,
			const Data::configBaseValues_t& a_activeConfig,
			const Data::configBase_t&       a_baseConfig,
			const Data::configModelGroup_t& a_group,
			ObjectEntryBase&                a_objectEntry,
			TESForm*                        a_form,
			const bool                      a_leftWeapon,
			const bool                      a_visible,
			const bool                      a_disableHavok,
			const bool                      a_physics) noexcept;

		static void FinalizeObjectState(
			const std::unique_ptr<ObjectEntryBase::State>& a_state,
			TESForm*                                       a_form,
			NiNode*                                        a_rootNode,
			const NiPointer<NiNode>&                       a_objectNode,
			targetNodes_t&                                 a_targetNodes,
			const Data::configBaseValues_t&                a_config,
			Actor*                                         a_actor) noexcept;

		static void TryMakeArrowState(
			const std::unique_ptr<ObjectEntryBase::State>& a_state,
			NiNode*                                        a_object) noexcept;

		static void TryCreatePointLight(
			Actor*                        a_actor,
			NiNode*                       a_object,
			TESObjectLIGH*                a_lightForm,
			const Data::ExtraLightData&   a_config,
			std::unique_ptr<ObjectLight>& a_out) noexcept;

		static TESObjectLIGH* GetLightFormForAttach(TESForm* a_modelForm) noexcept;

		static void TryInitializeAndPlayLoopSound(
			Actor*       a_actor,
			ObjectSound& a_sound) noexcept;

		static BGSSoundDescriptorForm* GetSoundDescriptor(
			const TESForm* a_modelForm) noexcept;

		void PlayEquipObjectSound(
			const ProcessParams&            a_params,
			const Data::configBaseValues_t& a_config,
			const ObjectEntryBase&          a_objectEntry,
			bool                            a_equip) noexcept;

		static bool AttachNodeImpl(
			NiNode*                     a_root,
			const Data::NodeDescriptor& a_node,
			bool                        a_atmReference,
			const ObjectEntryBase&      a_entry) noexcept;

		constexpr void SetBackgroundCloneLevel(
			bool                 a_player,
			BackgroundCloneLevel a_level) noexcept
		{
			m_backgroundClone[a_player] = a_level;
		}

	public:
		[[nodiscard]] constexpr auto GetBackgroundCloneLevel(
			bool a_player) const noexcept
		{
			return m_backgroundClone[a_player];
		}

	private:
		NiPointer<ObjectCloningTask> DispatchCloningTask(
			const ProcessParams&       a_params,
			const ObjectDatabaseEntry& a_entry,
			TESModelTextureSwap*       a_textureSwap,
			float                      a_colliderScale) noexcept;

		struct unks_01
		{
			std::uint16_t p1;
			std::uint16_t p2;
		};

		using getObjectByName_t                  = NiAVObject* (*)(NiAVObject* a_root, const BSFixedString& a_name, bool a_recursiveLookup) noexcept;
		inline static const auto GetObjectByName = IAL::Address<getObjectByName_t>(74481, 76207);

	public:
		using applyTextureSwap_t                  = void (*)(TESModelTextureSwap* a_swap, NiAVObject* a_object) noexcept;
		inline static const auto ApplyTextureSwap = IAL::Address<applyTextureSwap_t>(14660, 14837);  // 19baa0

	private:
		using shrinkToSize_t                  = NiAVObject* (*)(NiNode*) noexcept;
		inline static const auto ShrinkToSize = IAL::Address<shrinkToSize_t>(15571, 15748);

		using attachAddonNodes_t                  = bool (*)(NiNode* a_node) noexcept;
		inline static const auto AttachAddonNodes = IAL::Address<attachAddonNodes_t>(19206, 19632);

		using attachAddonParticles_t                  = void (*)(NiAVObject* a_object) noexcept;
		inline static const auto AttachAddonParticles = IAL::Address<attachAddonParticles_t>(19207, 19633);

		using setShaderPropsFadeNode_t                  = NiNode* (*)(NiAVObject* a_object, BSFadeNode* a_node) noexcept;
		inline static const auto SetShaderPropsFadeNode = IAL::Address<setShaderPropsFadeNode_t>(98895, 105542);

		using unkSSN1_t                      = NiNode* (*)(RE::ShadowSceneNode* a_node, NiAVObject* a_object) noexcept;
		inline static const auto fUnk12ba3e0 = IAL::Address<unkSSN1_t>(99702, 106336);
		inline static const auto fUnk12b99f0 = IAL::Address<unkSSN1_t>(99696, 106330);

		using unk1CD130_t                   = bool (*)(NiAVObject* a_object, std::uint32_t a_collisionFilterInfo) noexcept;
		inline static const auto fUnk1CD130 = IAL::Address<unk1CD130_t>(15567, 15745);

		using queueAttachHavok_t                  = void (*)(BSTaskPool* a_taskpool, NiAVObject* a_object, std::uint32_t a_unk3, bool a_unk4) noexcept;
		inline static const auto QueueAttachHavok = IAL::Address<queueAttachHavok_t>(35950, 36925);

		using unk5EBD90_t                   = unks_01& (*)(TESObjectREFR* a_ref, unks_01& a_out) noexcept;
		inline static const auto fUnk5EBD90 = IAL::Address<unk5EBD90_t>(36559, 37560);

		using unk5C39F0_t                   = void (*)(BSTaskPool* a_taskpool, NiAVObject* a_object, RE::bhkWorld* a_world, std::uint32_t a_unk4) noexcept;
		inline static const auto fUnk5C39F0 = IAL::Address<unk5C39F0_t>(35947, 36922);

		using unkDC6140_t                   = NiAVObject* (*)(NiAVObject*, bool) noexcept;
		inline static const auto fUnkDC6140 = IAL::Address<unkDC6140_t>(76545, 78389);

		using stripCollision_t                  = bool (*)(NiAVObject* a_object, bool a_recursive, bool a_ignoreHavokFlag) noexcept;
		inline static const auto StripCollision = IAL::Address<stripCollision_t>(76037, 77870);

		/*using NiAVObject_unk39_col_t              = bool (*)(NiAVObject* a_object, std::int32_t a_unk1, bool a_recursive, bool a_ignoreHavokFlag, std::uint8_t a_unk4) noexcept;
		inline static const auto NiAVObject_unk39_col = IAL::Address<NiAVObject_unk39_col_t>(76033, 77866);*/

		static bool RemoveAllChildren(
			NiNode*              a_object,
			const BSFixedString& a_name) noexcept;

		static bool RemoveObjectByName(
			NiNode*              a_object,
			const BSFixedString& a_name) noexcept;

		static BSXFlags* GetBSXFlags(NiObjectNET* a_object) noexcept;

		static stl::flag<AttachResultFlags> AttachObject(
			Actor*      a_actor,
			TESForm*    a_modelForm,
			BSFadeNode* a_root,
			NiNode*     a_targetNode,
			NiNode*     a_object,
			ModelType   a_modelType,
			bool        a_leftWeapon,
			bool        a_dropOnDeath,
			bool        a_removeScabbards,
			bool        a_keepTorchFlame,
			bool        a_disableHavok,
			bool        a_removeTracers,
			bool        a_removeEditorMarker) noexcept;

	public:
		virtual void OnAsyncModelClone(const NiPointer<ObjectCloningTask>& a_task);
		virtual bool IsWeaponNodeSharingDisabled() const = 0;

	private:
		virtual void OnAsyncModelLoad(const NiPointer<QueuedModel>& a_task) override;

	public:
		using cleanupObject_t                      = void (*)(const Game::ObjectRefHandle& a_handle, NiAVObject* a_object) noexcept;
		inline static const auto CleanupObjectImpl = IAL::Address<cleanupObject_t>(15495, 15660);

	protected:
		bool                 m_playSound{ false };
		bool                 m_playSoundNPC{ false };
		BackgroundCloneLevel m_backgroundClone[2]{ BackgroundCloneLevel::kNone };

		mutable stl::recursive_mutex m_lock;
	};

	DEFINE_ENUM_CLASS_BITWISE(IObjectManager::AttachResultFlags);
}