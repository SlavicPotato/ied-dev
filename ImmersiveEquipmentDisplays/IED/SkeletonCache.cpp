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
		Actor* a_refr,
		bool   a_firstPerson)
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

	void SkeletonCache::MakeFrom(
		Actor*      a_refr,
		NiAVObject* a_root,
		bool        a_firstPerson)
	{
		const auto key = make_key(a_refr, a_firstPerson);
		if (!key.first.empty())
		{
			if (!has(key.first))
			{
				const stl::write_lock_guard lock(m_lock);

				m_data.try_emplace(
					key.first,
					m_useNativeLoader.load(std::memory_order_relaxed),
					key.second.c_str());
			}
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
			result += e.second->size();
		}

		return result;
	}

	void SkeletonCache::OnLoad3D(Actor* a_actor, NiAVObject* a_root, bool a_firstPerson)
	{
		if (m_makeOnLoad)
		{
			MakeFrom(a_actor, a_root, a_firstPerson);
		}
	}

	auto SkeletonCache::make_key(
		Actor* a_refr,
		bool   a_firstPerson)
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

	bool SkeletonCache::has(const stl::fixed_string& a_key) const
	{
		const stl::read_lock_guard lock(m_lock);

		return m_data.contains(a_key);
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
				if (entry->object->m_parent != nullptr)
				{
					params = RE::BSModelDB::ModelLoadParams(3, false, true);
					result = ModelLoader::Load(a_modelPath, params, object);
				}
				else
				{
					object = entry->object;
				}
			}
		}
		else
		{
			result = ModelLoader::Load(a_modelPath, params, object);
		}

		if (!result)
		{
			gLog.Warning(__FUNCTION__ ": [%s] could not load skeleton mesh", a_modelPath);
			return;
		}

		make_node_entries(object);
	}

	SkeletonCache::actor_entry_data::actor_entry_data(NiAVObject* a_root)
	{
		make_node_entries(a_root);
	}

	void SkeletonCache::actor_entry_data::make_node_entries(NiAVObject* a_root)
	{
		Traverse(
			a_root,
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

	SkeletonCache::ActorEntry::ActorEntry(NiAVObject* a_root) :
		ptr(stl::make_smart<actor_entry_data>(a_root))
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