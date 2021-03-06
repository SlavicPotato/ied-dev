#include "pch.h"

#include "ISkeletonInfo.h"

#include <ext/Node.h>

namespace IED
{
	void ISkeletonInfo::QueueSkeletonInfoLookup(
		Game::FormID             a_actor,
		bool                     a_firstPerson,
		bool                     a_loadSkeleton,
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
				a_loadSkeleton,
				a_result);
		});
	}

	void ISkeletonInfo::SkeletonInfoLookupImpl(
		Game::FormID                    a_actor,
		bool                            a_firstPerson,
		bool                            a_loadSkeleton,
		const SkeletonInfoLookupResult& a_result)
	{
		stl::scoped_lock lock(a_result->lock);

		a_result->actor      = a_actor;
		a_result->lastUpdate = IPerfCounter::Query();

		a_result->initialized |=
			a_result->succeeded =
				CreateSkeletonTree(
					a_actor,
					a_firstPerson,
					a_loadSkeleton,
					*a_result);
	}

	template <class Tf>
	static void VisitNodeTree(
		NiAVObject*  a_object,
		SI_NiObject& a_data,
		Tf           a_func)
	{
		a_func(a_object, a_data);

		if (auto node = a_object->AsNode())
		{
			a_data.children.reserve(node->m_children.m_size);

			for (auto& object : node->m_children)
			{
				if (object)
				{
					VisitNodeTree(
						object,
						a_data.children.emplace_back(),
						a_func);
				}
			}
		}
	}

	bool ISkeletonInfo::CreateSkeletonTree(
		Game::FormID a_actor,
		bool         a_firstPerson,
		bool         a_loadSkeleton,
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
			a_loadSkeleton,
			a_root);

		if (!root)
		{
			return false;
		}

		a_root.object = {};

		VisitNodeTree(
			root,
			a_root.object,
			[](NiAVObject* a_object, SI_NiObject& a_data) {
				FillObjectData(a_object, a_data);
			});

		return true;
	}

	NiPointer<NiAVObject> ISkeletonInfo::GetSkeletonRoot(
		TESObjectREFR* a_refr,
		bool           a_firstPerson,
		bool           a_loadSkeleton,
		SI_Root&       a_root)
	{
		if (a_loadSkeleton)
		{
			if (!a_root.path.empty())
			{
				a_root.path.clear();
			}

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
			const char* out;

			if (!::Util::Model::MakePath("meshes", path, buffer, out))
			{
				return nullptr;
			}

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
		BSResourceNiBinaryStream binaryStream(a_path);
		if (!binaryStream.IsValid())
		{
			return nullptr;
		}

		::Util::Stream::NiStreamWrapper stream;

		if (!stream->LoadStream(std::addressof(binaryStream)))
		{
			return nullptr;
		}

		if (!stream->m_rootObjects.m_data)
		{
			return nullptr;
		}

		for (auto& e : stream->m_rootObjects)
		{
			if (!e)
			{
				continue;
			}

			if (auto object = NRTTI<NiAVObject>()(e))
			{
				return object;
			}
		}

		return nullptr;
	}

	void ISkeletonInfo::FillObjectData(
		NiAVObject*  a_object,
		SI_NiObject& a_data) noexcept
	{
		a_data.name  = a_object->m_name.c_str();
		a_data.local = a_object->m_localTransform;
		a_data.world = a_object->m_worldTransform;
		a_data.flags = a_object->m_flags;
	}

}