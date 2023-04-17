#pragma once

#include "ObjectDatabaseEntry.h"

#include <ext/IOTask.h>

namespace IED
{
	class ObjectDatabase;

	class ObjectCloningTask :
		public RE::IOTask
	{
	public:
		enum class State : std::uint32_t
		{
			kPending        = 0,
			kProcessing     = 1,
			kCanceled       = 2,
			kDone           = 3,
			kAcquiredForUse = 4
		};

		ObjectCloningTask(
			ObjectDatabase&            a_owner,
			const ObjectDatabaseEntry& a_entry,
			TESModelTextureSwap*       a_textureSwap,
			float                      a_colliderScale,
			std::uint8_t               a_priority = 4);

		~ObjectCloningTask() override;

		void Unk_01() override;
		void Unk_02() override;

		bool Run() override;

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

		[[nodiscard]] constexpr auto& GetClone() noexcept
		{
			return _clone;
		}

		[[nodiscard]] constexpr auto& GetClone() const noexcept
		{
			return _clone;
		}

		[[nodiscard]] inline bool try_acquire_for_use() noexcept
		{
			auto expected = State::kDone;
			return _taskState.compare_exchange_strong(expected, State::kAcquiredForUse);
		}

		[[nodiscard]] inline bool try_acquire_for_processing() noexcept
		{
			auto expected = State::kPending;
			return _taskState.compare_exchange_strong(expected, State::kProcessing);
		}

		inline bool try_cancel_task() noexcept
		{
			auto expected = State::kPending;
			return _taskState.compare_exchange_strong(expected, State::kCanceled);
		}

		[[nodiscard]] inline auto get_task_state() const noexcept
		{
			return _taskState.load();
		}

		static void clone_and_apply_texswap(
			const ObjectDatabaseEntry& a_dbentry,
			TESModelTextureSwap*       a_texSwap,
			float                      a_colliderScale,
			NiPointer<NiNode>&         a_out);

	private:
		ObjectDatabaseEntry        _entry;
		ObjectDatabase&            _owner;
		TESModelTextureSwap* const _texSwap;
		const float                _colliderScale;
		NiPointer<NiNode>          _clone;
		std::atomic<State>         _taskState{ State::kPending };
	};
}