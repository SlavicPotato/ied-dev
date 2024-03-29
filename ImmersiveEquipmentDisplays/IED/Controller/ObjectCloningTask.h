#pragma once

#include "IED/IOTaskBase.h"
#include "ObjectDatabaseEntry.h"

namespace IED
{
	class IObjectManager;

	class ObjectCloningTask :
		public IOTaskBase<ObjectCloningTask>
	{
		class PostRunTask :
			public TaskDelegate
		{
		public:
			PostRunTask(
				ObjectCloningTask* a_task);

			void Run() override;
			void Dispose() override;

		private:
			NiPointer<ObjectCloningTask> task;
		};

	public:
		enum class State : std::uint32_t
		{
			kPending    = 0,
			kProcessing = 1,
			kCancelled  = 2,
			kCompleted  = 3,
		};

		ObjectCloningTask(
			IObjectManager&            a_owner,
			Game::FormID               a_actor,
			const ObjectDatabaseEntry& a_entry,
			TESModelTextureSwap*       a_textureSwap,
			float                      a_colliderScale,
			std::uint8_t               a_priority = 3);

		~ObjectCloningTask() override;

		void RunTask();

		[[nodiscard]] constexpr auto& GetActor() const noexcept
		{
			return _actor;
		}

		[[nodiscard]] constexpr auto& GetDBEntry() const noexcept
		{
			return _entry;
		}

		[[nodiscard]] constexpr auto GetSwap() const noexcept
		{
			return _texSwap;
		}

		[[nodiscard]] constexpr auto GetColliderScale() const noexcept
		{
			return _colliderScale;
		}

		[[nodiscard]] constexpr auto& GetClone() const noexcept
		{
			return _clone;
		}

		[[nodiscard]] constexpr auto HasGraphHolder() noexcept
		{
			return static_cast<bool>(_graphHolder.second);
		}

		[[nodiscard]] constexpr auto& GetAnimPath() const noexcept
		{
			return _graphHolder.first;
		}

		[[nodiscard]] constexpr auto& GetGraphHolderManager() noexcept
		{
			return _graphHolder.second;
		}

		[[nodiscard]] inline bool try_acquire_for_processing() noexcept
		{
			auto expected = State::kPending;
			return _taskState.compare_exchange_strong(expected, State::kProcessing);
		}

		inline bool try_cancel_task() noexcept
		{
			auto expected = State::kPending;
			return _taskState.compare_exchange_strong(expected, State::kCancelled);
		}

		[[nodiscard]] inline auto get_task_state() const noexcept
		{
			return _taskState.load();
		}

		static void CloneAndApplyTexSwap(
			const ObjectDatabaseEntry& a_dbentry,
			TESModelTextureSwap*       a_texSwap,
			float                      a_colliderScale,
			NiPointer<NiNode>&         a_out);

	private:
		ObjectDatabaseEntry                                                _entry;
		Game::FormID                                                       _actor;
		IObjectManager&                                                    _owner;
		TESModelTextureSwap*                                               _texSwap;
		float                                                              _colliderScale;
		NiPointer<NiNode>                                                  _clone;
		std::pair<BSFixedString, RE::WeaponAnimationGraphManagerHolderPtr> _graphHolder;
		std::atomic<State>                                                 _taskState{ State::kPending };
	};
}