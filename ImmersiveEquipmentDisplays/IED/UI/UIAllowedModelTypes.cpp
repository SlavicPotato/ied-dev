#include "pch.h"

#include "UIAllowedModelTypes.h"

#include "IED/Controller/IFormDatabase.h"

#pragma warning(disable: 4073)
#pragma init_seg(lib)

namespace IED
{
	namespace UI
	{
		const std::initializer_list<std::uint32_t> g_allowedModelTypes{
			TESObjectWEAP::kTypeID,
			TESObjectARMO::kTypeID,
			TESAmmo::kTypeID,
			TESObjectLIGH::kTypeID,
			TESObjectMISC::kTypeID,
			IngredientItem::kTypeID,
			AlchemyItem::kTypeID,
			TESKey::kTypeID,
			TESObjectBOOK::kTypeID,
			TESSoulGem::kTypeID,
			ScrollItem::kTypeID,
			TESObjectSTAT::kTypeID,
			BGSMovableStatic::kTypeID,
			TESObjectTREE::kTypeID,
			TESFlora::kTypeID,
			TESFurniture::kTypeID,
			BGSArtObject::kTypeID,
			TESObjectACTI::kTypeID,
			BGSTalkingActivator::kTypeID,
			TESGrass::kTypeID,
			TESObjectANIO::kTypeID,
			TESObjectDOOR::kTypeID,
			BGSExplosion::kTypeID,
			BGSMaterialObject::kTypeID,
			BGSProjectile::kTypeID,
			BGSHazard::kTypeID,
			IFormDatabase::EXTRA_TYPE_ARMOR,
			IFormDatabase::EXTRA_TYPE_LIGHT,
		};

	}
}