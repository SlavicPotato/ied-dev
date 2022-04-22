#include "pch.h"

#include "EffectShaderData.h"

#include "ESF_Pulse.h"
#include "ESF_UVLinearMotion.h"

namespace IED
{
	void EffectShaderData::clear()
	{
		if (!data.empty())
		{
			visit_nodes([](auto& a_entry, auto& a_prop) {
				if (a_prop.prop->effectData == a_entry.shaderData)
				{
					a_prop.prop->ClearEffectShaderData();
				}
			});

			data.clear();
		}

		tag.reset();
	}

	bool EffectShaderData::UpdateIfChanged(
		NiNode*                                 a_object,
		const Data::configEffectShaderHolder_t& a_data)
	{
		if (tag == a_data)
		{
			return false;
		}
		else
		{
			Update(a_object, a_data);
			return true;
		}
	}

	void EffectShaderData::Update(
		NiNode*                                 a_object,
		const Data::configEffectShaderHolder_t& a_data)
	{
		clear();

		for (auto& [i, e] : a_data.data)
		{
			Entry tmp(i);

			std::optional<stl::set<BSFixedString>> tset;

			Util::Node::TraverseGeometry(a_object, [&](BSGeometry* a_geometry) {
				if (auto& effect = a_geometry->m_spEffectState)
				{
					if (auto shaderProp = NRTTI<BSShaderProperty>()(effect.get()))
					{
						if (!e.targetNodes.empty())
						{
							if (!tset)
							{
								tset.emplace();

								for (auto& f : e.targetNodes)
								{
									tset->emplace(f.c_str());
								}
							}

							if (!tset->contains(a_geometry->m_name))
							{
								return Util::Node::VisitorControl::kContinue;
							}
						}

						tmp.nodes.emplace_back(shaderProp);
					}
				}

				return Util::Node::VisitorControl::kContinue;
			});

			if (tmp.nodes.empty())
			{
				continue;
			}

			if (!e.create_shader_data(tmp.shaderData))
			{
				continue;
			}

			if (e.flags.test(Data::EffectShaderDataFlags::kForce))
			{
				tmp.flags.set(EffectShaderData::EntryFlags::kForce);
			}

			auto& r = data.emplace_back(std::move(tmp));

			r.create_function_list(e.functions);
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

			e.shaderData->fillColor    = it->second.fillColor;
			e.shaderData->rimColor     = it->second.rimColor;
			e.shaderData->uOffset      = it->second.uvo.uOffset;
			e.shaderData->vOffset      = it->second.uvo.vOffset;
			e.shaderData->uScale       = it->second.uvp.uScale;
			e.shaderData->vScale       = it->second.uvp.vScale;
			e.shaderData->edgeExponent = it->second.edgeExponent;
			//e.shaderData->boundDiameter = it->second.boundDiameter;

			for (auto& f : e.functions)
			{
				auto& d = it->second.functions;

				auto itf = std::find_if(
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

	void EffectShaderData::Entry::update_effect_data(float a_step) const
	{
		if (functions.empty())
		{
			return;
		}

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
		assert(functions.empty());

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