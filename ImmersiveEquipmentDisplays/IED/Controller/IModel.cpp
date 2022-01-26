#include "pch.h"

#include "IModel.h"

namespace IED
{
	template <class T>
	static bool ExtractModelParams(
		T* a_ptr,
		IModel::modelParams_t& a_out,
		ModelType a_type = ModelType::kMisc)
	{
		TESModelTextureSwap* texSwap{ nullptr };
		const char* path;

		if constexpr (std::is_same_v<T, TESModel>)
		{
			path = a_ptr->GetModelName();
			//texSwap = a_ptr->GetAsModelTextureSwap();
		}
		else
		{
			texSwap = std::addressof(a_ptr->texSwap);
			path = texSwap->GetModelName();
		}

		if (!path || path[0] == 0)
		{
			return false;
		}
		else
		{
			a_out = { a_type, path, texSwap };
			return true;
		}
	}

	template <class T, class>
	bool IModel::ExtractFormModelParams(
		TESForm* a_form,
		modelParams_t& a_out,
		ModelType a_type)
	{
		return ExtractModelParams(static_cast<T*>(a_form), a_out, a_type);
	}

	bool IModel::GetModelParams(
		Actor* a_actor,
		TESForm* a_form,
		TESRace* a_race,
		bool a_isFemale,
		bool a_1pWeap,
		bool a_useArma,
		modelParams_t& a_out)
	{
		switch (a_form->formType)
		{
		case TESSoulGem::kTypeID:
		case TESKey::kTypeID:
		case TESObjectMISC::kTypeID:
			return ExtractFormModelParams<TESObjectMISC>(a_form, a_out);
		case TESAmmo::kTypeID:
			return ExtractFormModelParams<TESAmmo>(a_form, a_out);
		case TESObjectLIGH::kTypeID:
			{
				auto carryFlag = static_cast<TESObjectLIGH*>(a_form)->CanCarry();
				return ExtractFormModelParams<TESObjectLIGH>(
					a_form,
					a_out,
					carryFlag ? ModelType::kLight : ModelType::kMisc);
			}
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
		case TESFlora::kTypeID:
		case TESFurniture::kTypeID:
		case BGSTalkingActivator::kTypeID:
			return ExtractFormModelParams<TESObjectACTI>(a_form, a_out);
		case TESObjectSTAT::kTypeID:
			return ExtractFormModelParams<TESObjectSTAT>(a_form, a_out);
		case BGSMovableStatic::kTypeID:
			return ExtractFormModelParams<BGSMovableStatic>(a_form, a_out);
		case BGSStaticCollection::kTypeID:
			return ExtractFormModelParams<BGSStaticCollection>(a_form, a_out);
		case TESObjectTREE::kTypeID:
			return ExtractModelParams(std::addressof(static_cast<TESObjectTREE*>(a_form)->model), a_out);
		case TESObjectWEAP::kTypeID:
			{
				if (a_actor == *g_thePlayer || a_1pWeap)
				{
					if (auto weapon = static_cast<TESObjectWEAP*>(a_form); weapon->model)
					{
						// 1p model
						if (ExtractFormModelParams<TESObjectSTAT>(
								weapon->model,
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

				if (armor->IsShield())
				{
					for (auto arma : armor->armorAddons)
					{
						if (!arma)
						{
							continue;
						}

						if (!arma->isValidRace(a_race))
						{
							continue;
						}

						auto texSwap = std::addressof(arma->models[0][a_isFemale ? 1 : 0]);
						auto path = texSwap->GetModelName();

						if (!path || path[0] == 0)
						{
							texSwap = std::addressof(arma->models[0][a_isFemale ? 0 : 1]);
							path = texSwap->GetModelName();
						}

						if (!path || path[0] == 0)
						{
							return false;
						}
						else
						{
							a_out = {
								ModelType::kArmor,
								path,
								texSwap,
								true,
								arma
							};

							return true;
						}
					}
				}
				else
				{
					auto texSwap = std::addressof(armor->bipedModel.textureSwap[a_isFemale ? 1 : 0]);
					auto path = texSwap->GetModelName();

					if (!path || path[0] == 0)
					{
						texSwap = std::addressof(armor->bipedModel.textureSwap[a_isFemale ? 0 : 1]);
						path = texSwap->GetModelName();
					}

					if (!path || path[0] == 0)
					{
						return false;
					}
					else
					{
						a_out = {
							ModelType::kArmor,
							path,
							texSwap,
							false,
							nullptr
						};

						return true;
					}
				}
			}
			break;
		}

		return false;
	}

}  // namespace IED