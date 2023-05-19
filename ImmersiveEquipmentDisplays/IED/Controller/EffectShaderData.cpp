#include "pch.h"

#include "EffectShaderData.h"

#include "ESF_Pulse.h"
#include "ESF_UVLinearMotion.h"

#include "ActorObjectHolder.h"

namespace IED
{
	std::atomic<bool> EffectShaderData::_backgroundLoad{ false };

	EffectShaderData::EffectShaderData(
		const ActorObjectHolder&                a_owner,
		const Data::configEffectShaderHolder_t& a_data) noexcept
	{
		UpdateImpl(a_owner, a_data);
	}

	EffectShaderData::EffectShaderData(
		const ActorObjectHolder&                a_owner,
		BIPED_OBJECT                            a_bipedObject,
		NiNode*                                 a_sheathNode,
		NiNode*                                 a_sheathNode1p,
		const Data::configEffectShaderHolder_t& a_data) noexcept :
		bipedObject(a_bipedObject),
		sheathNode(a_sheathNode),
		sheathNode1p(a_sheathNode1p)
	{
		UpdateImpl(a_owner, a_data);
	}

	EffectShaderData::~EffectShaderData()
	{
		CancelPendingTasks();
	}

	bool EffectShaderData::UpdateIfChanged(
		const ActorObjectHolder&                a_owner,
		NiAVObject*                             a_root,
		const Data::configEffectShaderHolder_t& a_data) noexcept
	{
		if (tag == a_data)
		{
			return false;
		}
		else
		{
			Update(a_owner, a_root, a_data);
			return true;
		}
	}

	bool EffectShaderData::UpdateIfChanged(
		const ActorObjectHolder&                a_holder,
		const Data::configEffectShaderHolder_t& a_data) noexcept
	{
		if (tag == a_data)
		{
			return false;
		}
		else
		{
			Update(a_holder, a_data);
			return true;
		}
	}

	void EffectShaderData::Update(
		const ActorObjectHolder&                a_holder,
		NiAVObject*                             a_root,
		const Data::configEffectShaderHolder_t& a_data) noexcept
	{
		ClearEffectShaderDataFromTree(a_root);

		UpdateImpl(a_holder, a_data);
	}

	void EffectShaderData::Update(
		const ActorObjectHolder&                a_holder,
		const Data::configEffectShaderHolder_t& a_data) noexcept
	{
		ClearEffectShaderDataFromTree(a_holder.Get3D());
		ClearEffectShaderDataFromTree(a_holder.Get3D1p());

		UpdateImpl(a_holder, a_data);
	}

	void EffectShaderData::UpdateImpl(
		const ActorObjectHolder&                a_owner,
		const Data::configEffectShaderHolder_t& a_data) noexcept
	{
		CancelPendingTasks();

		data.clear();
		data.reserve(a_data.data.size());

		pending.clear();
		pending.reserve(a_data.data.size());

		for (auto& [i, e] : a_data.data)
		{
			auto shaderData = e.create_shader_data();

			if (_backgroundLoad.load(std::memory_order_relaxed) &&
			    e.has_custom_texture())
			{
				auto thrd = RE::BackgroundProcessThread::GetSingleton();
				assert(thrd);

				auto task = thrd->QueueTask<ShaderTextureLoadTask>(a_owner, e);

				pending.emplace_back(i, std::move(shaderData), std::move(task), e);
			}
			else
			{
				shaderData->baseTexture = e.baseTexture.load_texture(true);

				if (!shaderData->baseTexture)
				{
					continue;
				}

				shaderData->paletteTexture  = e.paletteTexture.load_texture(false);
				shaderData->blockOutTexture = e.blockOutTexture.load_texture(false);

				if (shaderData->paletteTexture)
				{
					shaderData->grayscaleToColor = e.flags.test(Data::EffectShaderDataFlags::kGrayscaleToColor);
					shaderData->grayscaleToAlpha = e.flags.test(Data::EffectShaderDataFlags::kGrayscaleToAlpha);
				}

				data.emplace_back(i, std::move(shaderData), e);
			}
		}

		if (pending.empty())
		{
			pending.shrink_to_fit();
		}

		tag = a_data;
	}

	void EffectShaderData::CancelPendingTasks() noexcept
	{
		for (const auto& e : pending)
		{
			e.task->try_cancel_task();
		}
	}

	bool EffectShaderData::UpdateConfigValues(
		const Data::configEffectShaderHolder_t& a_data) noexcept
	{
		const auto func = [&](Entry& e) noexcept -> bool {
			auto it = a_data.data.find(e.key);
			if (it == a_data.data.end())
			{
				return false;
			}

			e.shaderData->fillColor     = it->second.fillColor;
			e.shaderData->rimColor      = it->second.rimColor;
			e.shaderData->baseFillScale = it->second.baseFillScale;
			e.shaderData->baseFillAlpha = it->second.baseFillAlpha;
			e.shaderData->baseRimAlpha  = it->second.baseRimAlpha;
			e.shaderData->uOffset       = it->second.uvo.uOffset;
			e.shaderData->vOffset       = it->second.uvo.vOffset;
			e.shaderData->uScale        = it->second.uvp.uScale;
			e.shaderData->vScale        = it->second.uvp.vScale;
			e.shaderData->edgeExponent  = it->second.edgeExponent;
			//e.shaderData->boundDiameter = it->second.boundDiameter;

			for (const auto& f : e.functions)
			{
				const auto& d = it->second.functions;

				const auto itf = std::find_if(
					d.begin(),
					d.end(),
					[&](auto& a_v) {
						return *f == a_v.get_unique_id();
					});

				if (itf == d.end())
				{
					return false;
				}

				if (!f->UpdateConfig(*itf))
				{
					return false;
				}
			}

			return true;
		};

		for (auto& e : data)
		{
			if (!func(e))
			{
				return false;
			}
		}

		for (auto& e : pending)
		{
			if (!func(e))
			{
				return false;
			}
		}

		return true;
	}

	void EffectShaderData::ClearEffectShaderDataFromTree(NiAVObject* a_object) noexcept
	{
		if (!a_object || data.empty())
		{
			return;
		}

		Util::Node::TraverseGeometry(a_object, [&](BSGeometry* a_geometry) noexcept {
			if (auto& effect = a_geometry->m_spEffectState)
			{
				if (auto shaderProp = ::NRTTI<BSShaderProperty>()(effect.get()))
				{
					auto it = std::find_if(
						data.begin(),
						data.end(),
						[&](auto& a_v) noexcept {
							return shaderProp->effectData == a_v.shaderData;
						});

					if (it != data.end())
					{
						shaderProp->ClearEffectShaderData();
					}
				}
			}

			return Util::Node::VisitorControl::kContinue;
		});
	}

	bool EffectShaderData::ProcessPendingLoad(
		const NiPointer<ShaderTextureLoadTask>& a_task) noexcept
	{
		auto it = std::find_if(
			pending.begin(),
			pending.end(),
			[&](const auto& a_entry) {
				return a_entry.task == a_task;
			});

		if (it != pending.end())
		{
			switch (it->task->get_state())
			{
			case ShaderTextureLoadTask::State::kLoaded:

				assert(it->task->_baseTexture.second);

				it->shaderData->baseTexture     = std::move(it->task->_baseTexture.second);
				it->shaderData->paletteTexture  = std::move(it->task->_paletteTexture.second);
				it->shaderData->blockOutTexture = std::move(it->task->_blockOutTexture.second);

				if (it->shaderData->paletteTexture)
				{
					it->shaderData->grayscaleToColor = it->flags.test(Data::EffectShaderDataFlags::kGrayscaleToColor);
					it->shaderData->grayscaleToAlpha = it->flags.test(Data::EffectShaderDataFlags::kGrayscaleToAlpha);
				}

				data.emplace_back(std::move(*it));

				[[fallthrough]];
			case ShaderTextureLoadTask::State::kCancelled:
			case ShaderTextureLoadTask::State::kError:
				it = pending.erase(it);
				break;
			}

			if (pending.empty())
			{
				pending.shrink_to_fit();
			}

			return true;
		}
		else
		{
			return false;
		}
	}

	void EffectShaderData::Entry::UpdateEffectData(
		float a_step) const noexcept
	{
		auto sdata = shaderData.get();
		assert(sdata);

		for (auto& e : functions)
		{
			e->Run(sdata, a_step);
		}
	}

	EffectShaderData::Entry::Entry(
		const stl::fixed_string&                  a_key,
		RE::BSTSmartPointer<BSEffectShaderData>&& a_shaderData,
		const Data::configEffectShaderData_t&     a_config) :
		key(a_key),
		shaderData(std::move(a_shaderData)),
		flags(a_config.flags)
	{
		functions.reserve(a_config.functions.size());

		for (auto& e : a_config.functions)
		{
			switch (e.type)
			{
			case Data::EffectShaderFunctionType::UVLinearMotion:
				functions.emplace_back(
					make_effect_shader_function<ESF::UVLinearMotion>(e));
				break;
			case Data::EffectShaderFunctionType::Pulse:
				functions.emplace_back(
					make_effect_shader_function<ESF::Pulse>(e));
				break;
			}
		}

		targetNodes.reserve(a_config.targetNodes.size());

		for (auto& f : a_config.targetNodes)
		{
			targetNodes.emplace(f.c_str());
		}
	}
}