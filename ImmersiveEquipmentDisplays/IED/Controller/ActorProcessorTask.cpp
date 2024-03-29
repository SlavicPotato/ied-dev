#include "pch.h"

#include "ActorProcessorTask.h"

#include "Controller.h"
#include "IConditionalVariableProcessor.h"
#include "IObjectManager.h"

#include "IED/AnimationUpdateController.h"
#include "IED/AreaLightingDetection.h"
#include "IED/EngineExtensions.h"
#include "IED/Inventory.h"
#include "IED/ReferenceLightController.h"
#include "IED/StringHolder.h"
#include "IED/Util/Common.h"

namespace IED
{

	ActorProcessorTask::ActorProcessorTask() :
		m_globalState(IPerfCounter::Query())
#if defined(IED_PERF_BUILD)
		,
		m_updateProc(std::make_unique<ThreadPool>(*this))
#endif
	{
	}

	static constexpr void sync_ref_transform(
		ObjectEntryBase::State* a_state) noexcept
	{
		if (a_state->flags.test(ObjectEntryFlags::kSyncReferenceTransform) &&
		    !a_state->commonNodes.rootNode->IsHidden())
		{
			if (a_state->transform.data)
			{
				a_state->commonNodes.rootNode->m_localTransform =
#if defined(IED_PERF_BUILD)
					Bullet::btTransformEx(a_state->refNode->m_localTransform)
#else
					a_state->refNode->m_localTransform
#endif
					* *a_state->transform.data;
			}
			else
			{
				a_state->commonNodes.rootNode->m_localTransform =
					a_state->refNode->m_localTransform;
			}
		}
	}

	bool ActorProcessorTask::SyncRefParentNode(
		ActorObjectHolder& a_record,
		ObjectEntryBase&   a_entry) noexcept
	{
		bool result;

		auto& controller = GetController();

		if (const auto info = controller.LookupCachedActorInfo2(
				a_record.m_actor,
				a_record))
		{
			result = controller.AttachNodeImpl(
				info->npcRoot,
				a_entry.data.state->nodeDesc,
				a_entry.data.state->atmReference,
				a_entry);

			if (result)
			{
				INode::UpdateRootConditional(a_record.m_actor, info->root);
			}
		}
		else
		{
			result = false;
		}

		return result;
	}

	void ActorProcessorTask::DoObjectRefSync(
		ActorObjectHolder& a_record,
		ObjectEntryBase&   a_entry) noexcept
	{
		const auto state = a_entry.data.state.get();

		if (!state->refNode)
		{
			return;
		}

		if (state->IsReferenceMovedOrOphaned())
		{
			if (state->flags.test(ObjectEntryFlags::kRefSyncDisableFailedOrphan))
			{
				return;
			}

			const bool result = SyncRefParentNode(a_record, a_entry);
			if (result)
			{
				a_record.RequestEval();
			}
			else
			{
				state->flags.set(ObjectEntryFlags::kRefSyncDisableFailedOrphan);
				return;
			}

			if (!state->refNode)
			{
				return;
			}
		}

		sync_ref_transform(state);
	}

	void ActorProcessorTask::DoObjectRefSyncMTSafe(
		ActorObjectHolder& a_record,
		ObjectEntryBase&   a_entry) noexcept
	{
		const auto state = a_entry.data.state.get();

		if (!state->refNode)
		{
			return;
		}

		if (state->IsReferenceMovedOrOphaned())
		{
			if (!state->flags.test(ObjectEntryFlags::kRefSyncDisableFailedOrphan))
			{
				m_syncRefParentQueue.emplace(
					std::addressof(a_record),
					std::addressof(a_entry));
			}
		}
		else
		{
			sync_ref_transform(state);
		}
	}

	void ActorProcessorTask::ProcessTransformUpdateRequest(
		ActorObjectHolder& a_data) noexcept
	{
		if (!a_data.m_flags.test(ActorObjectHolderFlags::kWantTransformUpdate))
		{
			return;
		}

		if (!a_data.m_flags.test(ActorObjectHolderFlags::kImmediateTransformUpdate) &&
		    a_data.m_flags.test(ActorObjectHolderFlags::kSkipNextTransformUpdate))
		{
			a_data.m_flags.clear(ActorObjectHolderFlags::kSkipNextTransformUpdate);
		}
		else
		{
			a_data.m_flags.clear(ActorObjectHolderFlags::kRequestTransformUpdateMask);

			GetController().EvaluateTransformsImpl(
				a_data,
				ControllerUpdateFlags::kUseCachedParams);
		}
	}

	void ActorProcessorTask::ProcessEvalRequest(ActorObjectHolder& a_data) noexcept
	{
		if (a_data.m_flags.consume(ActorObjectHolderFlags::kEvalThisFrame))
		{
			if (auto& params = a_data.GetCurrentProcessParams())
			{
				GetController().EvaluateImpl(
					*params,
					a_data,
					ControllerUpdateFlags::kPlayEquipSound |
						ControllerUpdateFlags::kFromProcessorTask |
						ControllerUpdateFlags::kUseCachedParams);
			}
			else
			{
				GetController().EvaluateImpl(
					a_data,
					ControllerUpdateFlags::kPlayEquipSound |
						ControllerUpdateFlags::kFromProcessorTask |
						ControllerUpdateFlags::kUseCachedParams);
			}
		}
	}

	static constexpr bool CheckMonitorNodes(
		ActorObjectHolder& a_data) noexcept
	{
		bool result = false;

		for (auto& f : a_data.GetMonitorNodes())
		{
			if (f.parent != f.node->m_parent)
			{
				f.parent = f.node->m_parent;

				result = true;
			}

			if (const auto size = f.node->m_children.size();
			    f.size != size)
			{
				f.size = size;

				result = true;
			}

			if (const bool visible = f.node->IsVisible();
			    visible != f.visible)
			{
				f.visible = visible;

				result = true;
			}
		}

		return result;
	}

	void ActorProcessorTask::UpdateGlobalState() noexcept
	{
		enum class UpdateFlag : std::uint8_t
		{
			kNone = 0,

			kPlayer = 1ui8 << 0,
			kGlobal = 1ui8 << 1
		};

		stl::flag<UpdateFlag> update{ UpdateFlag::kNone };

		if (const auto v = (*g_thePlayer)->lastRiddenHorseHandle;
		    v != m_globalState.playerLastRidden)
		{
			m_globalState.playerLastRidden = v;
			update.set(UpdateFlag::kGlobal);
		}

		if (const auto v = IsInFirstPerson();
		    v != m_globalState.inFirstPerson)
		{
			m_globalState.inFirstPerson = v;
			update.set(UpdateFlag::kPlayer);
		}

		if (const auto v = MenuTopicManager::GetSingleton()->talkingHandle;
		    v != m_globalState.talkingHandle)
		{
			const auto h = v && v.IsValid() ? v : m_globalState.talkingHandle;

			m_globalState.talkingHandle = v;

			const auto& data = GetController().GetActorMap().getvec();

			const auto it = std::find_if(
				data.begin(),
				data.end(),
				[h](auto& a_v) noexcept {
					return a_v->second.GetHandle() == h;
				});

			if (it != data.end())
			{
				(*it)->second.RequestEval();
			}

			update.set(UpdateFlag::kPlayer);
		}

		if (m_timer.GetStartTime() >= m_globalState.nextRun)
		{
			m_globalState.nextRun =
				m_timer.GetStartTime() +
				IPerfCounter::T(COMMON_STATE_CHECK_INTERVAL);

			const auto* const sky = RE::TES::GetSingleton()->sky;

			if (const auto v = sky ? sky->GetCurrentWeatherHalfPct() : nullptr;
			    v != m_globalState.currentWeather)
			{
				m_globalState.currentWeather = v;
				update.set(UpdateFlag::kGlobal);
			}

			if (const auto v = ALD::GetRoomLightingTemplate(sky);
			    v != m_globalState.roomLightingTemplate)
			{
				m_globalState.roomLightingTemplate = v;
				update.set(UpdateFlag::kPlayer);
			}

#if defined(IED_ENABLE_CONDITION_EN)

			auto player = *g_thePlayer;
			assert(player);

			if (player->loadedState)
			{
				auto pl = Game::ProcessLists::GetSingleton();
				assert(pl);

				if (bool n = pl->PlayerHasEnemiesNearby(0);
				    n != m_state.playerEnemiesNearby)
				{
					m_state.playerEnemiesNearby = n;

					if (auto it = m_controller.m_actors.find(Data::IData::GetPlayerRefID());
					    it != m_controller.m_actors.end())
					{
						it->second.RequestEval();
					}
				}
			}

#endif
		}

		if (m_timer.GetStartTime() >= m_globalState.nextRunMF)
		{
			m_globalState.nextRunMF =
				m_timer.GetStartTime() +
				IPerfCounter::T(COMMON_STATE_CHECK_INTERVAL_MF);

			const auto* const sky = RE::TES::GetSingleton()->sky;

			const auto t1 = Data::GetTimeOfDay2(sky);

			if (t1.first != m_globalState.timeOfDay)
			{
				m_globalState.timeOfDay = t1.first;
				update.set(UpdateFlag::kGlobal);
			}

			if (t1.second != m_globalState.isDaytime)
			{
				m_globalState.isDaytime = t1.second;
				update.set(UpdateFlag::kGlobal);
			}

			if (const auto v = ALD::IsExteriorDark(sky);
			    v != m_globalState.isExteriorDark)
			{
				m_globalState.isExteriorDark = v;
				update.set(UpdateFlag::kGlobal);
			}

			if (const auto v = ALD::GetRoundedSunAngle(sky);
			    v != m_globalState.sunAngle)
			{
				m_globalState.sunAngle = v;
				update.set(UpdateFlag::kGlobal);
			}
		}

		if (m_timer.GetStartTime() >= m_globalState.nextRunLF)
		{
			m_globalState.nextRunLF =
				m_timer.GetStartTime() +
				IPerfCounter::T(COMMON_STATE_CHECK_INTERVAL_LF);

			const auto calendar = RE::Calendar::GetSingleton();
			assert(calendar);

			if (const auto v = calendar->GetDayOfWeek();
			    v != m_globalState.dayOfWeek)
			{
				m_globalState.dayOfWeek = v;
				update.set(UpdateFlag::kGlobal);
			}
		}

		if (update.test(UpdateFlag::kGlobal))
		{
			GetController().RequestLFEvaluateAll();
		}

		if (update.test(UpdateFlag::kPlayer))
		{
			const auto& actorMap = GetController().GetActorMap();

			if (auto it = actorMap.find(Data::IData::GetPlayerRefID());
			    it != actorMap.end())
			{
				it->second.RequestEval();
			}
		}
	}

	template <bool _ParUnsafe>
	void ActorProcessorTask::DoActorUpdate(
		const float                              a_interval,
		const Game::Unk2f6b948::TimeMultipliers& a_stepMuls,
		const std::optional<PhysicsUpdateData>&  a_physUpdData,
		ActorObjectHolder&                       a_holder,
		bool                                     a_unpaused) noexcept
	{
		if (!a_holder.m_queuedModels.empty())
		{
			if (a_holder.ProcessQueuedModels())
			{
				GetController().RequestCleanup();
			}
		}

		auto& state = a_holder.m_state;

		NiPointer<TESObjectREFR> refr;
		if (!a_holder.GetHandle().Lookup(refr))
		{
			state.active = false;
			return;
		}

		const auto actor = refr->As<Actor>();

		if (!Util::Common::IsREFRValid(actor) ||
		    a_holder.m_actor.get() != actor)  // ??
		{
#if !defined(IED_PERF_BUILD)
			if constexpr (_ParUnsafe)
			{
				ITaskPool::AddPriorityTask([r = std::move(refr)] {});
			}
#endif

			state.active = false;
			return;
		}

		const auto cell = actor->GetParentCell();

		if (cell && cell->IsAttached())
		{
			if (!state.active)
			{
				a_holder.RequestEvalDefer();
				state.active = true;
			}
		}
		else
		{
			state.active = false;
			return;
		}

		if (state.UpdateState(actor, cell))
		{
			a_holder.RequestEvalDefer();
		}

		if (state.UpdateStateLF(actor))
		{
			a_holder.m_wantLFUpdate = true;
		}

		if (m_timer.GetStartTime() >= a_holder.m_nextLFStateCheck)
		{
			a_holder.m_nextLFStateCheck =
				m_timer.GetStartTime() +
				IPerfCounter::T(ActorObjectHolder::STATE_CHECK_INTERVAL_LOW);

			bool wantEval = state.DoLFUpdates(actor);

			wantEval |= a_holder.m_wantLFUpdate;

			if (wantEval)
			{
				a_holder.m_wantLFUpdate = false;
				a_holder.RequestEval();
			}
		}

		if (m_timer.GetStartTime() >= a_holder.m_nextMFStateCheck)
		{
			a_holder.m_nextMFStateCheck =
				m_timer.GetStartTime() +
				IPerfCounter::T(ActorObjectHolder::STATE_CHECK_INTERVAL_MH);

			if (state.UpdateEffects(actor))
			{
				a_holder.RequestEval();
			}
		}

		if (m_timer.GetStartTime() >= a_holder.m_nextHFStateCheck)
		{
			a_holder.m_nextHFStateCheck =
				m_timer.GetStartTime() +
				IPerfCounter::T(ActorObjectHolder::STATE_CHECK_INTERVAL_HIGH);

			if (a_holder.m_wantHFUpdate)
			{
				a_holder.m_wantHFUpdate = false;
				a_holder.RequestEval();
			}
		}

		if (a_holder.UpdateNodeMonitorEntries())
		{
			a_holder.RequestEval();
		}

		auto& fbf = a_holder.m_flags.bf();

		if (fbf.wantEval)
		{
			if (fbf.evalCountdown != 0)
			{
				fbf.evalCountdown--;
			}

			if (fbf.evalCountdown == 0 ||
			    fbf.immediateEval)
			{
				a_holder.m_flags.clear(ActorObjectHolderFlags::kRequestEvalMask);
				a_holder.m_flags.set(ActorObjectHolderFlags::kEvalThisFrame);
			}
		}

		if (CheckMonitorNodes(a_holder))
		{
			a_holder.RequestTransformUpdate();
		}

		bool update = false;

		a_holder.visit([&](auto& a_v) noexcept [[msvc::forceinline]] {
			auto& state = a_v.data.state;

			if (!state)
			{
				return;
			}

			if constexpr (_ParUnsafe)
			{
				DoObjectRefSyncMTSafe(a_holder, a_v);
			}
			else
			{
				DoObjectRefSync(a_holder, a_v);
			}

			if constexpr (
				std::is_same_v<
					std::remove_cvref_t<decltype(a_v)>,
					ObjectEntrySlot>)
			{
				if (state->hideCountdown)
				{
					if (--state->hideCountdown == 0)
					{
						if (state->flags.test(ObjectEntryFlags::kInvisible))
						{
							state->SetVisible(false);

							if (state->HasPhysicsNode())
							{
								if (auto& simComponent = state->simComponent)
								{
									a_holder.RemoveAndDestroySimComponent(simComponent);
								}
							}

							update = true;
						}
					}
				}
			}
		});

		if (update)
		{
			a_holder.RequestTransformUpdateDefer();
		}

		auto& controller = GetController();

		if (a_holder.m_flags.test(ActorObjectHolderFlags::kEvalThisFrame) ||
		    (!controller.GetActiveConfig().condvars.empty() &&
		     a_holder.m_flags.test(ActorObjectHolderFlags::kWantVarUpdate)))
		{
			if (const auto info = controller.LookupCachedActorInfo2(a_holder.m_actor, a_holder))
			{
				a_holder.CreateProcessParams(
					ControllerUpdateFlags::kPlayEquipSound,
					a_holder.m_actor.get(),
					a_holder.GetHandle(),
					a_holder.GetTempData(),
					a_holder.m_actor.get(),
					info->npc,
					info->race,
					info->root,
					info->npcRoot,
					a_holder,
					controller);
			}
		}

		if (a_unpaused && actor->IsAIEnabled())
		{
			const stl::ftz_daz_ctl_scoped<_MM_FLUSH_ZERO_ON | _MM_DENORMALS_ZERO_ON> fds;

			a_holder.UpdateSyncNodes();

			RunEffectUpdates(a_interval, a_stepMuls, a_physUpdData, a_holder);
		}
	}

	void ActorProcessorTask::RunPreUpdates(bool a_unpaused) noexcept
	{
		const auto interval = *Game::g_frameTimerSlow;

		std::optional<PhysicsUpdateData> physUpdateData;

		if (PhysicsProcessingEnabled() && a_unpaused)
		{
			PreparePhysicsUpdateData(interval, physUpdateData);
		}

		const auto stepMuls = Game::Unk2f6b948::GetTimeMultipliers();

		const auto& data = GetController().GetActorMap().getvec();

#if defined(IED_PERF_BUILD)
		const auto updateProc = m_updateProc.get();
#endif

		if (ParallelProcessingEnabled() &&

#if defined(IED_PERF_BUILD)
		    updateProc->IsEnabled() &&
#endif
		    data.size() > 1)
		{
#if defined(IED_PERF_BUILD)

			updateProc->make_shared_data(
				interval,
				std::addressof(stepMuls),
				std::addressof(physUpdateData),
				a_unpaused);

			updateProc->allocate_workers(data.size());
			updateProc->distribute_tasks(data);
			updateProc->notify_tasks_available();
			updateProc->wait_until_tasks_complete();
#else

			std::for_each(
				std::execution::par,
				data.begin(),
				data.end(),
				[&](auto& a_e) noexcept {
					DoActorUpdate<true>(
						interval,
						stepMuls,
						physUpdateData,
						a_e->second,
						a_unpaused);
				});
#endif

			m_syncRefParentQueue.process(
				[this](const auto& a_e) noexcept [[msvc::forceinline]] {
					const bool result = SyncRefParentNode(*a_e.first, *a_e.second);

					if (result)
					{
						a_e.first->RequestEval();
					}
					else
					{
						a_e.second->DisableRefSync();
					}
				});
		}
		else
		{
			for (auto& e : data)
			{
				DoActorUpdate<false>(
					interval,
					stepMuls,
					physUpdateData,
					e->second,
					a_unpaused);
			}
		}
	}

	void ActorProcessorTask::StartAPThreadPool()
	{
#if defined(IED_PERF_BUILD)
		const auto numThreads = WinApi::GetNumPhysicalCores();
		if (numThreads >= 2)
		{
			m_updateProc->Start(std::min(numThreads, 12u));
		}
#endif
	}

	void ActorProcessorTask::Run() noexcept
	{
		auto& controller = GetController();

		const stl::lock_guard lock(controller.m_lock);

		if (!m_run)
		{
			return;
		}

		m_timer.Begin();

		const bool notPaused = !Game::IsPaused();

		if (notPaused)
		{
			UpdateGlobalState();
		}

		RunPreUpdates(notPaused);

		const auto& actorList = controller.GetActorMap().getvec();

		const auto& cvdata = controller.GetActiveConfig().condvars;

		if (!cvdata.empty())
		{
			for (auto& e : actorList)
			{
				auto& holder = e->second;

				if (!holder.IsActive())
				{
					continue;
				}

				const bool wantVarUpdate = holder.m_flags.consume(ActorObjectHolderFlags::kWantVarUpdate);

				if (wantVarUpdate || holder.m_flags.test(ActorObjectHolderFlags::kEvalThisFrame))
				{
					if (auto& params = holder.GetCurrentProcessParams())
					{
						if (IConditionalVariableProcessor::UpdateVariableMap(
								*params,
								cvdata,
								holder.GetVariables()))
						{
							controller.RequestHFEvaluateAll(e->first);
							holder.m_flags.set(ActorObjectHolderFlags::kEvalThisFrame);
						}
					}
				}
			}
		}

		for (auto& e : actorList)
		{
			auto& holder = e->second;

			if (holder.IsActive())
			{
				ProcessEvalRequest(holder);
				ProcessTransformUpdateRequest(holder);
			}

			holder.ClearCurrentProcessParams();
		}

		m_globalParams.reset();

		controller.RunObjectCleanup();

		m_timer.End(m_currentTime);
	}

	Controller& ActorProcessorTask::GetController() noexcept
	{
		return static_cast<Controller&>(*this);
	}

	const Controller& ActorProcessorTask::GetController() const noexcept
	{
		return static_cast<const Controller&>(*this);
	}

	ActorProcessorTask::ThreadPool::Thread::Thread(ThreadPool& a_owner) :
		RE::BSThread(),
		m_owner(a_owner)
	{
	}

	void ActorProcessorTask::ThreadPool::Thread::wait_until_tasks_complete() noexcept
	{
		std::unique_lock lock(m_mutex);

		m_cond.wait(lock, [this]() noexcept [[msvc::forceinline]] {
			return m_runState == 0;
		});
	}

	void ActorProcessorTask::ThreadPool::Thread::notify_tasks_available() noexcept
	{
		{
			std::unique_lock lock(m_mutex);
			assert(m_runState == 0);
			m_runState = 1;
		}
		m_cond.notify_one();
	}

	DWORD ActorProcessorTask::ThreadPool::Thread::Run()
	{
		gLog.Debug("%s: starting %u", __FUNCTION__, threadID);

		const auto& data      = m_owner.m_shared;
		auto&       processor = m_owner.m_owner;

		while (wait_for_tasks())
		{
			for (const auto& e : m_list)
			{
				processor.DoActorUpdate<true>(
					data.interval,
					*data.stepMuls,
					*data.physUpdData,
					*e,
					data.unpaused);
			}

			tasks_complete();
		}

		gLog.Debug("%s: stopping %u", __FUNCTION__, threadID);

		return 0;
	}

	bool ActorProcessorTask::ThreadPool::Thread::wait_for_tasks() noexcept
	{
		std::unique_lock lock(m_mutex);
		m_cond.wait(lock, [this]() noexcept [[msvc::forceinline]] {
			return m_runState != 0;
		});

		return m_runState != -1;
	}

	void ActorProcessorTask::ThreadPool::Thread::tasks_complete() noexcept
	{
		{
			std::unique_lock lock(m_mutex);
			assert(m_runState == 1);
			m_runState = 0;
		}
		m_cond.notify_one();
	}

	ActorProcessorTask::ThreadPool::ThreadPool(
		ActorProcessorTask& a_owner) :
		m_owner(a_owner)
	{
	}

	ActorProcessorTask::ThreadPool::~ThreadPool()
	{
		Stop();
	}

	void ActorProcessorTask::ThreadPool::Stop()
	{
		for (const auto& e : m_workers)
		{
			{
				std::unique_lock lock(e->m_mutex);
				e->m_cond.wait(lock, [&]() noexcept [[msvc::forceinline]] {
					return e->m_runState == 0;
				});
				e->m_runState = -1;
			}
			e->m_cond.notify_all();
		}

		for (const auto& e : m_workers)
		{
			e->stop_and_wait_for_thread();
		}

		m_workers.clear();
	}

	void ActorProcessorTask::ThreadPool::Start(
		std::uint32_t a_numThreads)
	{
		ASSERT(m_workers.empty());

		m_workers.reserve(a_numThreads);

		for (std::uint32_t i = 0; i < a_numThreads; ++i)
		{
			m_workers.emplace_back(std::make_unique<Thread>(*this));
		}

		for (auto& e : m_workers)
		{
			ASSERT(e->StartThread());
		}
	}

	void ActorProcessorTask::ThreadPool::wait_until_tasks_complete() const noexcept
	{
		for (auto& e : m_workersInUse)
		{
			e->wait_until_tasks_complete();
		}
	}

	void ActorProcessorTask::ThreadPool::notify_tasks_available() const noexcept
	{
		for (auto& e : m_workersInUse)
		{
			e->notify_tasks_available();
		}
	}

	void ActorProcessorTask::ThreadPool::allocate_workers(
		std::size_t a_numTasks) noexcept
	{
		const auto n = std::max<std::size_t>(std::min<std::size_t>(a_numTasks / 2, 2), m_workers.size());

		m_workersInUse.clear();

		for (std::size_t i = 0; i < n; i++)
		{
			auto& thrd = m_workers[i];

			thrd->m_list.clear();
			m_workersInUse.emplace_back(thrd.get());
		}
	}

	void ActorProcessorTask::ThreadPool::distribute_tasks(
		const ActorObjectMap::vector_type& a_data) const noexcept
	{
		assert(m_workersInUse.size() > 0);

		const auto                              maxIndex = m_workersInUse.size() - 1;
		std::remove_const_t<decltype(maxIndex)> curIndex = 0;

		for (auto& e : a_data)
		{
			auto& thrd = m_workersInUse[curIndex];

			thrd->m_list.emplace_back(std::addressof(e->second));

			curIndex = curIndex == maxIndex ? 0 : curIndex + 1;
		}
	}
}