#include "pch.h"

#include "EffectShaderData.h"

#include "ESF_Pulse.h"
#include "ESF_UVLinearMotion.h"

#include "ActorObjectHolder.h"

namespace IED
{
	EffectShaderData::EffectShaderData(
		const Data::configEffectShaderHolder_t& a_data)
	{
		Update(a_data);
	}

	EffectShaderData::EffectShaderData(
		BIPED_OBJECT                            a_bipedObject,
		NiNode*                                 a_sheathNode,
		const Data::configEffectShaderHolder_t& a_data) :
		bipedObject(a_bipedObject),
		sheathNode(a_sheathNode)
	{
		Update(a_data);
	}

	bool EffectShaderData::UpdateIfChanged(
		NiAVObject*                             a_root,
		const Data::configEffectShaderHolder_t& a_data)
	{
		if (tag == a_data)
		{
			return false;
		}
		else
		{
			Update(a_root, a_data);
			return true;
		}
	}

	bool EffectShaderData::UpdateIfChanged(
		const ActorObjectHolder&                a_holder,
		const Data::configEffectShaderHolder_t& a_data)
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
		NiAVObject*                             a_root,
		const Data::configEffectShaderHolder_t& a_data)
	{
		ClearEffectShaderDataFromTree(a_root);

		Update(a_data);
	}

	void EffectShaderData::Update(
		const ActorObjectHolder&                a_holder,
		const Data::configEffectShaderHolder_t& a_data)
	{
		ClearEffectShaderDataFromTree(a_holder.Get3D());
		ClearEffectShaderDataFromTree(a_holder.Get3D1p());

		Update(a_data);
	}

	void EffectShaderData::Update(
		const Data::configEffectShaderHolder_t& a_data)
	{
		data.clear();

		for (auto& [i, e] : a_data.data)
		{
			RE::BSTSmartPointer<BSEffectShaderData> shaderData;

			if (!e.create_shader_data(shaderData))
			{
				continue;
			}

			auto& r = data.emplace_back(i, std::move(shaderData));

			r.create_function_list(e.functions);

			r.flags.set(
				static_cast<EffectShaderData::EntryFlags>(
					e.flags & Data::EffectShaderDataFlags::EntryMask));

			for (auto& f : e.targetNodes)
			{
				r.targetNodes.emplace(f.c_str());
			}
		}

		tag = a_data;
	}

	bool EffectShaderData::UpdateConfigValues(
		const Data::configEffectShaderHolder_t& a_data)
	{
		for (auto& e : data)
		{
			auto it = a_data.data.find(e.key);
			if (it == a_data.data.end())
			{
				return false;
			}

			e.shaderData->fillColor     = it->second.fillColor;
			e.shaderData->rimColor      = it->second.rimColor;
			e.shaderData->uOffset       = it->second.uvo.uOffset;
			e.shaderData->vOffset       = it->second.uvo.vOffset;
			e.shaderData->uScale        = it->second.uvp.uScale;
			e.shaderData->vScale        = it->second.uvp.vScale;
			e.shaderData->edgeExponent  = it->second.edgeExponent;
			e.shaderData->baseFillScale = it->second.baseFillScale;
			e.shaderData->baseFillAlpha = it->second.baseFillAlpha;
			e.shaderData->baseRimAlpha  = it->second.baseRimAlpha;
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
		}

		return true;
	}

	void EffectShaderData::ClearEffectShaderDataFromTree(NiAVObject* a_object)
	{
		if (!a_object || data.empty())
		{
			return;
		}

		Util::Node::TraverseGeometry(a_object, [&](BSGeometry* a_geometry) {
			if (auto& effect = a_geometry->m_spEffectState)
			{
				if (auto shaderProp = NRTTI<BSShaderProperty>()(effect.get()))
				{
					auto it = std::find_if(
						data.begin(),
						data.end(),
						[&](auto& a_v) {
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

	void EffectShaderData::Entry::update_effect_data(float a_step) const
	{
		auto sdata = shaderData.get();
		assert(sdata);

		for (auto& e : functions)
		{
			e->Run(sdata, a_step);
		}
	}

	void EffectShaderData::Entry::create_function_list(
		const Data::configEffectShaderFunctionList_t& a_data)
	{
		functions.clear();

		for (auto& e : a_data)
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
	}

}