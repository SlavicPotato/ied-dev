#pragma once

#include "SkeletonID.h"

namespace IED
{
	struct SI_Transform
	{
		SI_Transform() = default;
		SI_Transform(const NiTransform& a_in) :
			pos(a_in.pos),
			scale(a_in.scale)
		{
			a_in.rot.GetEulerAngles(
				std::addressof(rot.x),
				std::addressof(rot.y),
				std::addressof(rot.z));
		}

		NiPoint3 pos;
		NiPoint3 rot;
		float    scale{ 1.0f };
	};

	struct SI_NiObject
	{
		stl::string              name;
		SI_Transform             local;
		SI_Transform             world;
		std::uint32_t            flags;
		stl::vector<SI_NiObject> children;
		SI_NiObject*             parent{ nullptr };
	};

	struct SI_Root
	{
		Game::FormID                actor;
		std::string                 path;
		SI_NiObject                 object;
		std::unique_ptr<SkeletonID> skeletonID;
		bool                        initialized{ false };
		bool                        succeeded{ false };
		bool                        isLoadedData{ true };
		long long                   lastUpdate{ 0 };
		std::recursive_mutex        lock;

		SKMP_REDEFINE_NEW_PREF();
	};
}