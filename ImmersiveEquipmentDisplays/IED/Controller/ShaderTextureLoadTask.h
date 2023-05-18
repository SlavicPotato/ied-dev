#pragma once

#include "ObjectDatabaseEntry.h"

#include "IED/ConfigEffectShader.h"

namespace IED
{
	class IObjectManager;
	class ActorObjectHolder;

	class ShaderTextureLoadTask :
		public IOTaskBase<ShaderTextureLoadTask>
	{
		class PostRunTask :
			public TaskDelegate
		{
		public:
			PostRunTask(
				IObjectManager&        a_owner,
				Game::FormID           a_actor,
				ShaderTextureLoadTask* a_data);

			void Run() override;
			void Dispose() override;

		private:
			IObjectManager&                  _owner;
			Game::FormID                     _actor;
			NiPointer<ShaderTextureLoadTask> _task;
		};

	public:
		enum class State : std::uint32_t
		{
			kPending   = 0,
			kLoading   = 1,
			kCancelled = 2,
			kError     = 3,
			kLoaded    = 4,
		};

		ShaderTextureLoadTask(
			const ActorObjectHolder&                       a_owner,
			const RE::BSTSmartPointer<BSEffectShaderData>& a_data,
			const Data::configEffectShaderData_t&          a_config,
			std::uint8_t                                   a_priority = 4);

		void RunTask();

		[[nodiscard]] inline bool try_acquire_for_load() noexcept
		{
			auto expected = State::kPending;
			return _taskState.compare_exchange_strong(expected, State::kLoading);
		}

		inline bool try_cancel_task() noexcept
		{
			auto expected = State::kPending;
			return _taskState.compare_exchange_strong(expected, State::kCancelled);
		}

		inline auto get_state() const noexcept
		{
			return _taskState.load();
		}

	private:
		Data::configEffectShaderTexture_t      _baseTexture;
		Data::configEffectShaderTexture_t      _paletteTexture;
		Data::configEffectShaderTexture_t      _blockOutTexture;
		stl::flag<Data::EffectShaderDataFlags> _flags;
		Game::FormID                           _actor;
		IObjectManager&                        _owner;

		RE::BSTSmartPointer<BSEffectShaderData> _data;
		std::atomic<State>                      _taskState{ State::kPending };
	};
}