#include "pch.h"

#include "ShaderTextureLoadTask.h"

#include "IED/Controller/IObjectManager.h"

namespace IED
{
	ShaderTextureLoadTask::ShaderTextureLoadTask(
		const ActorObjectHolder&                       a_owner,
		const RE::BSTSmartPointer<BSEffectShaderData>& a_data,
		const Data::configEffectShaderData_t&          a_config,
		std::uint8_t                                   a_priority) :
		_baseTexture(a_config.baseTexture),
		_paletteTexture(a_config.paletteTexture),
		_blockOutTexture(a_config.blockOutTexture),
		_flags(a_config.flags),
		_data(a_data),
		_owner(a_owner.GetOwner()),
		_actor(a_owner.GetActorFormID())
	{
		SetPriority(a_priority);
	}

	void ShaderTextureLoadTask::RunTask()
	{
		if (try_acquire_for_load())
		{
			_data->baseTexture = _baseTexture.load_texture(true);

			if (_data->baseTexture)
			{
				_data->paletteTexture  = _paletteTexture.load_texture(false);
				_data->blockOutTexture = _blockOutTexture.load_texture(false);

				if (_data->paletteTexture)
				{
					_data->grayscaleToColor = _flags.test(Data::EffectShaderDataFlags::kGrayscaleToColor);
					_data->grayscaleToAlpha = _flags.test(Data::EffectShaderDataFlags::kGrayscaleToAlpha);
				}

				_taskState.store(State::kLoaded);
			}
			else
			{
				_taskState.store(State::kError);
			}

			ITaskPool::AddPriorityTask<PostRunTask>(_owner, _actor, this);
		}
	}

	ShaderTextureLoadTask::PostRunTask::PostRunTask(
		IObjectManager&        a_owner,
		Game::FormID           a_actor,
		ShaderTextureLoadTask* a_task) :
		_owner(a_owner),
		_actor(a_actor),
		_task(a_task)
	{
	}

	void ShaderTextureLoadTask::PostRunTask::Run()
	{
		const stl::lock_guard lock(_owner.GetLock());

		const auto& data = _owner.GetActorMap();

		if (const auto it = data.find(_actor); it != data.end())
		{
			it->second.visit2([&task = _task](auto& a_e) noexcept {
				if (const auto& esdata = a_e.data.effectShaderData)
				{
					return esdata->ProcessPendingLoad(task);
				}
				else
				{
					return false;
				}
			});
		}
	}

	void ShaderTextureLoadTask::PostRunTask::Dispose()
	{
		delete this;
	}

}