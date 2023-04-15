#include "pch.h"

#include "SkeletonCache.h"

#include <ext/Model.h>
#include <ext/Node.h>

namespace IED
{
	using namespace ::Util::Model;
	using namespace ::Util::Node;

	SkeletonCache SkeletonCache::m_Instance;

	auto SkeletonCache::Get(
		TESObjectREFR* a_refr,
		bool           a_firstPerson)
		-> ActorEntry
	{
		const auto key = make_key(a_refr, a_firstPerson);
		if (!key.first.empty())
		{
			if (auto result = try_get(key.first))
			{
				return result;
			}
			else
			{
				return get_or_create(key);
			}
		}
		else
		{
			return {};
		}
	}

	std::size_t SkeletonCache::GetSize() const
	{
		const stl::read_lock_guard lock(m_lock);

		return m_data.size();
	}

	std::size_t SkeletonCache::GetTotalEntries() const
	{
		const stl::read_lock_guard lock(m_lock);

		std::size_t result = 0;

		for (auto& e : m_data)
		{
			result += e.second.ptr->data.size();
		}

		return result;
	}

	auto SkeletonCache::make_key(
		TESObjectREFR* a_refr,
		bool           a_firstPerson)
		-> KeyPathPair
	{
		auto path = Get3DPath(a_refr, a_firstPerson);
		if (!path || *path == 0)
		{
			return {};
		}

		char buffer[MAX_PATH];

		path = MakePath("meshes", path, buffer);

		return KeyPathPair(path, path);
	}

	auto SkeletonCache::get_or_create(
		const KeyPathPair& a_key)
		-> ActorEntry
	{
		const stl::write_lock_guard lock(m_lock);

		return m_data.try_emplace(
						 a_key.first,
						 m_useNativeLoader.load(std::memory_order_relaxed),
						 a_key.second.c_str())
		    .first->second;
	}

	auto SkeletonCache::try_get(
		const stl::fixed_string& a_key) const
		-> ActorEntry
	{
		const stl::read_lock_guard lock(m_lock);

		auto it = m_data.find(a_key);
		if (it != m_data.end())
		{
			return it->second;
		}
		else
		{
			return {};
		}
	}

	SkeletonCache::actor_entry_data::actor_entry_data(
		bool        a_nativeLoader,
		const char* a_modelPath)
	{
		RE::BSModelDB::ModelLoadParams params(3, false, true);

		NiPointer<NiAVObject>         object;
		RE::BSModelDB::ModelEntryAuto entry;

		bool result;

		if (a_nativeLoader)
		{
			result = ModelLoader::NativeLoad(a_modelPath, params, entry);

			if (result)
			{
				object = entry->object;

				ASSERT(object->m_parent == nullptr);
			}
		}
		else
		{
			result = ModelLoader::Load(a_modelPath, params, object);
		}

		if (!result)
		{
			return;
		}

		Traverse(
			object,
			[&](const NiAVObject* a_object) [[msvc::forceinline]] {
				const auto& name = a_object->m_name;

				if (!name.empty())
				{
					data.emplace(name.data(), a_object->m_localTransform);
				}

				return ::Util::Node::VisitorControl::kContinue;
			});
	}

	SkeletonCache::ActorEntry::ActorEntry(
		bool        a_nativeLoader,
		const char* a_modelPath) :
		ptr(stl::make_smart<actor_entry_data>(a_nativeLoader, a_modelPath))
	{
	}

	NiTransform SkeletonCache::ActorEntry::GetCachedOrZeroTransform(
		const stl::fixed_string& a_name) const
	{
		if (auto& cache = ptr)
		{
			auto it = cache->data.find(a_name);
			if (it != cache->data.end())
			{
				return it->second;
			}
		}

		return {};
	}

	NiTransform SkeletonCache::ActorEntry::GetCachedOrCurrentTransform(
		const stl::fixed_string& a_name,
		NiAVObject*              a_object) const
	{
		if (auto& cache = ptr)
		{
			auto it = cache->data.find(a_name);
			if (it != cache->data.end())
			{
				return it->second;
			}
		}

		return a_object->m_localTransform;
	}

	std::optional<NiTransform> SkeletonCache::ActorEntry::GetCachedTransform(
		const stl::fixed_string& a_name) const
	{
		if (auto& cache = ptr)
		{
			auto it = cache->data.find(a_name);
			if (it != cache->data.end())
			{
				return it->second;
			}
		}

		return {};
	}

}