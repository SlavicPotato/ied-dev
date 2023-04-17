#include "pch.h"

#include "IModel.h"

namespace IED
{
	static constexpr bool ExtractModelParams(
		TESModel*              a_ptr,
		IModel::modelParams_t& a_out,
		ModelType              a_type = ModelType::kMisc)  //
		noexcept
	{
		const char* path = a_ptr->GetModelName();

		if (!path || path[0] == 0)
		{
			return false;
		}
		else
		{
			a_out = {
				a_type,
				path,
				a_ptr->GetAsModelTextureSwap()
			};

			return true;
		}
	}

	template <class T>
	static constexpr bool ExtractFormModelParams(
		TESForm*               a_form,
		IModel::modelParams_t& a_out,
		ModelType              a_type = ModelType::kMisc)  //
		noexcept                              //
		requires(std::is_convertible_v<T*, TESModel*>)
	{
		return ExtractModelParams(static_cast<T*>(a_form), a_out, a_type);
	}

	struct ModelData
	{
		const char*          path;
		TESModelTextureSwap* swap;
	};

	template <class T>
	static std::optional<ModelData> GetModelData(
		T*   a_form,
		bool a_female)  //
		noexcept        //
		requires(
			std::is_base_of_v<TESObjectARMO, T> ||
			std::is_base_of_v<TESObjectARMA, T>)
	{
		auto texSwap = std::addressof(a_form->GetBipedModelTextureSwap(a_female));
		auto path    = texSwap->GetModelName();

		if (!path || path[0] == 0)
		{
			texSwap = std::addressof(a_form->GetBipedModelTextureSwap(!a_female));
			path    = texSwap->GetModelName();
		}

		if (!path || path[0] == 0)
		{
			return {};
		}
		else
		{
			return std::make_optional<ModelData>(path, texSwap);
		}
	}

	bool IModel::GetModelParams(
		Actor*         a_actor,
		TESForm*       a_form,
		TESRace*       a_race,
		bool           a_isFemale,
		bool           a_1pWeap,
		bool           a_useWorld,
		modelParams_t& a_out) noexcept
	{
		switch (a_form->formType)
		{
		case TESSoulGem::kTypeID:
			return ExtractFormModelParams<TESSoulGem>(a_form, a_out);
		case TESKey::kTypeID:
			return ExtractFormModelParams<TESKey>(a_form, a_out);
		case TESObjectMISC::kTypeID:
			return ExtractFormModelParams<TESObjectMISC>(a_form, a_out);
		case TESAmmo::kTypeID:
			return ExtractFormModelParams<TESAmmo>(a_form, a_out, ModelType::kAmmo);
		case TESObjectLIGH::kTypeID:
			return ExtractFormModelParams<TESObjectLIGH>(a_form, a_out, ModelType::kLight);
		case BGSArtObject::kTypeID:
			return ExtractFormModelParams<BGSArtObject>(a_form, a_out);
		case TESObjectBOOK::kTypeID:
			return ExtractFormModelParams<TESObjectBOOK>(a_form, a_out);
		case IngredientItem::kTypeID:
			return ExtractFormModelParams<IngredientItem>(a_form, a_out);
		case AlchemyItem::kTypeID:
			return ExtractFormModelParams<AlchemyItem>(a_form, a_out);
		case ScrollItem::kTypeID:
			return ExtractFormModelParams<ScrollItem>(a_form, a_out);
		case TESObjectACTI::kTypeID:
			return ExtractFormModelParams<TESObjectACTI>(a_form, a_out);
		case TESFlora::kTypeID:
			return ExtractFormModelParams<TESFlora>(a_form, a_out);
		case TESFurniture::kTypeID:
			return ExtractFormModelParams<TESFurniture>(a_form, a_out);
		case BGSTalkingActivator::kTypeID:
			return ExtractFormModelParams<BGSTalkingActivator>(a_form, a_out);
		case TESObjectSTAT::kTypeID:
			return ExtractFormModelParams<TESObjectSTAT>(a_form, a_out);
		case BGSMovableStatic::kTypeID:
			return ExtractFormModelParams<BGSMovableStatic>(a_form, a_out);
		case BGSStaticCollection::kTypeID:
			return ExtractFormModelParams<BGSStaticCollection>(a_form, a_out);
		case TESObjectANIO::kTypeID:
			return ExtractFormModelParams<TESObjectANIO>(a_form, a_out);
		case TESObjectDOOR::kTypeID:
			return ExtractFormModelParams<TESObjectDOOR>(a_form, a_out);
		case TESObjectTREE::kTypeID:
			return ExtractFormModelParams<TESObjectTREE>(a_form, a_out);
		case TESGrass::kTypeID:
			return ExtractFormModelParams<TESGrass>(a_form, a_out);
		case BGSExplosion::kTypeID:
			return ExtractFormModelParams<BGSExplosion>(a_form, a_out);
		case BGSMaterialObject::kTypeID:
			return ExtractFormModelParams<BGSMaterialObject>(a_form, a_out);
		case BGSProjectile::kTypeID:
			return ExtractFormModelParams<BGSProjectile>(a_form, a_out, ModelType::kProjectile);
		case BGSHazard::kTypeID:
			return ExtractFormModelParams<BGSHazard>(a_form, a_out, ModelType::kHazard);
		case TESObjectWEAP::kTypeID:
			{
				if (a_actor == *g_thePlayer || a_1pWeap)
				{
					if (auto weapon = static_cast<TESObjectWEAP*>(a_form); weapon->firstPersonModelObject)
					{
						// 1p model
						if (ExtractModelParams(
								weapon->firstPersonModelObject,
								a_out,
								ModelType::kWeapon))
						{
							return true;
						}
					}
				}

				return ExtractFormModelParams<TESObjectWEAP>(a_form, a_out, ModelType::kWeapon);
			}
		case TESObjectARMO::kTypeID:
			{
				auto armor = static_cast<TESObjectARMO*>(a_form);

				const bool isShield = armor->IsShield();

				if (!a_useWorld && isShield)
				{
					for (auto& arma : armor->armorAddons)
					{
						if (!arma)
						{
							continue;
						}

						if (!arma->isValidRace(a_race))
						{
							continue;
						}

						if (const auto modelData = GetModelData(arma, a_isFemale))
						{
							a_out = {
								ModelType::kShield,
								modelData->path,
								modelData->swap,
								arma
							};

							return true;
						}
						else
						{
							return false;
						}
					}
				}
				else
				{
					if (const auto modelData = GetModelData(armor, a_isFemale))
					{
						a_out = {
							isShield ?
								ModelType::kShield :
								ModelType::kArmor,
							modelData->path,
							modelData->swap
						};

						return true;
					}
					else
					{
						return false;
					}
				}
			}
			break;
		}

		return false;
	}

}