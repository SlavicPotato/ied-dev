#pragma once

#include "ActorSkeletonTreeData.h"

namespace IED
{
	using SkeletonInfoLookupResult = stl::smart_ptr<SI_Root>;

	class ISkeletonInfo
	{
	public:
		static void QueueSkeletonInfoLookup(
			Game::FormID             a_actor,
			bool                     a_firstPerson,
			bool                     a_loadedSkeleton,
			SkeletonInfoLookupResult a_result);

	private:
		static void SkeletonInfoLookupImpl(
			Game::FormID                    a_actor,
			bool                            a_firstPerson,
			bool                            a_loadedSkeleton,
			const SkeletonInfoLookupResult& a_result);

		static bool CreateSkeletonTree(
			Game::FormID a_actor,
			bool         a_firstPerson,
			bool         a_loadedSkeleton,
			SI_Root&     a_root);

		static NiPointer<NiAVObject> GetSkeletonRoot(
			TESObjectREFR* a_refr,
			bool           a_firstPerson,
			bool           a_loadedSkeleton,
			SI_Root&       a_root);

		static NiPointer<NiAVObject> LoadSkeletonRoot(
			const char* a_path);

		static void FillObjectData(
			NiAVObject*  a_object,
			SI_NiObject& a_data,
			SI_NiObject* a_parent) noexcept;
	};
}