#include "pch.h"

#include "ISkeletonInfo.h"

#include <ext/Node.h>

namespace IED
{
	void ISkeletonInfo::QueueSkeletonInfoLookup(
		Game::FormID             a_actor,
		bool                     a_firstPerson,
		bool                     a_loadedSkeleton,
		SkeletonInfoLookupResult a_result)
	{
		if (!a_result)
		{
			return;
		}

		ITaskPool::AddTask([=] {
			SkeletonInfoLookupImpl(
				a_actor,
				a_firstPerson,
				a_loadedSkeleton,
				a_result);
		});
	}

	void ISkeletonInfo::SkeletonInfoLookupImpl(
		Game::FormID                    a_actor,
		bool                            a_firstPerson,
		bool                            a_loadedSkeleton,
		const SkeletonInfoLookupResult& a_result)
	{
		const stl::lock_guard lock(a_result->lock);

		a_result->actor      = a_actor;
		a_result->lastUpdate = IPerfCounter::Query();

		a_result->initialized |=
			a_result->succeeded =
				CreateSkeletonTree(
					a_actor,
					a_firstPerson,
					a_loadedSkeleton,
					*a_result);
	}

	template <class Tf>
	static void VisitNodeTree(
		NiAVObject*  a_object,
		SI_NiObject& a_data,
		SI_NiObject* a_parent,
		const Tf&    a_func)
	{
		a_func(a_object, a_data, a_parent);

		if (auto node = a_object->AsNode())
		{
			a_data.children.reserve(node->m_children.size());

			for (const auto& object : node->m_children)
			{
				if (object)
				{
					VisitNodeTree(
						object,
						a_data.children.emplace_back(),
						std::addressof(a_data),
						a_func);
				}
			}
		}
	}

	bool ISkeletonInfo::CreateSkeletonTree(
		Game::FormID a_actor,
		bool         a_firstPerson,
		bool         a_loadedSkeleton,
		SI_Root&     a_root)
	{
		auto actor = a_actor.As<Actor>();
		if (!actor)
		{
			return false;
		}

		auto root = GetSkeletonRoot(
			actor,
			a_firstPerson,
			a_loadedSkeleton,
			a_root);

		if (!root)
		{
			return false;
		}

		if (auto node = root->AsNode())
		{
			a_root.skeletonID = std::make_unique<SkeletonID>(node);
		}
		else
		{
			a_root.skeletonID.reset();
		}

		a_root.object = {};

		VisitNodeTree(
			root,
			a_root.object,
			nullptr,
			[](
				NiAVObject*  a_object,
				SI_NiObject& a_data,
				SI_NiObject* a_parent) {
				FillObjectData(a_object, a_data, a_parent);
			});

		return true;
	}

	NiPointer<NiAVObject> ISkeletonInfo::GetSkeletonRoot(
		TESObjectREFR* a_refr,
		bool           a_firstPerson,
		bool           a_loadedSkeleton,
		SI_Root&       a_root)
	{
		if (a_loadedSkeleton)
		{
			a_root.path.clear();

			auto result = a_refr->Get3D1(a_firstPerson);
			if (result)
			{
				a_root.isLoadedData = true;
			}

			return result;
		}
		else
		{
			auto path = ::Util::Model::Get3DPath(a_refr, a_firstPerson);
			if (!path || *path == 0)
			{
				return nullptr;
			}

			char        buffer[MAX_PATH];

			const auto out = ::Util::Model::MakePath("meshes", path, buffer);

			auto result = LoadSkeletonRoot(out);
			if (result)
			{
				a_root.isLoadedData = false;
				a_root.path         = out;
			}
			else
			{
				a_root.path.clear();
			}

			return result;
		}
	}

	NiPointer<NiAVObject> ISkeletonInfo::LoadSkeletonRoot(
		const char* a_path)
	{
		RE::BSModelDB::ModelLoadParams params(3, false, true);

		RE::BSModelDB::ModelEntryAuto entry;

		if (::Util::Model::ModelLoader::NativeLoad(a_path, params, entry))
		{
			return entry->object;
		}
		else
		{
			return {};
		}
	}

	void ISkeletonInfo::FillObjectData(
		NiAVObject*  a_object,
		SI_NiObject& a_data,
		SI_NiObject* a_parent) noexcept
	{
		auto rtti = a_object->GetRTTI();

		if (auto type = rtti ? rtti->name : nullptr)
		{
			a_data.type = type;
		}

		a_data.name   = a_object->m_name.c_str();
		a_data.local  = a_object->m_localTransform;
		a_data.world  = a_object->m_worldTransform;
		a_data.flags  = a_object->m_flags.underlying();
		a_data.parent = a_parent;
	}

}