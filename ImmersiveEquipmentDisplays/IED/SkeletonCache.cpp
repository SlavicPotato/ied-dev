#include "pch.h"

#include "SkeletonCache.h"

#include <ext/Model.h>
#include <ext/Node.h>

namespace IED
{
	SkeletonCache SkeletonCache::m_Instance;

	auto SkeletonCache::Get(
		TESObjectREFR* a_refr,
		bool           a_firstPerson)
		-> ActorEntry
	{
		const auto key = make_key(a_refr, a_firstPerson);
		if (!key.empty())
		{
			if (auto result = try_get(key))
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
		const std::shared_lock lock(m_lock);

		return m_data.size();
	}

	std::size_t SkeletonCache::GetTotalEntries() const
	{
		const std::shared_lock lock(m_lock);

		std::size_t result = 0;

		for (auto& e : m_data)
		{
			result += e.second.ptr->data.size();
		}

		return result;
	}

	stl::fixed_string SkeletonCache::make_key(
		TESObjectREFR* a_refr,
		bool           a_firstPerson)
	{
		auto path = ::Util::Model::Get3DPath(a_refr, a_firstPerson);
		if (!path || *path == 0)
		{
			return {};
		}

		char        buffer[MAX_PATH];
		const char* out;

		if (::Util::Model::MakePath("meshes", path, buffer, out))
		{
			return out;
		}
		else
		{
			return {};
		}
	}

	auto SkeletonCache::get_or_create(
		const stl::fixed_string& a_key)
		-> ActorEntry
	{
		const std::unique_lock lock(m_lock);

		return m_data.try_emplace(a_key, a_key).first->second;
	}

	auto SkeletonCache::try_get(
		const stl::fixed_string& a_key) const
		-> ActorEntry
	{
		const std::shared_lock lock(m_lock);

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
		const stl::fixed_string& a_key)
	{
		BSResourceNiBinaryStream binaryStream(a_key.c_str());
		if (!binaryStream.IsValid())
		{
			return;
		}

		::Util::Stream::NiStreamWrapper stream;

		if (!stream->LoadStream(std::addressof(binaryStream)))
		{
			return;
		}

		if (!stream->m_rootObjects.initialized())
		{
			return;
		}

		for (const auto& e : stream->m_rootObjects)
		{
			if (!e)
			{
				continue;
			}

			if (auto object = ::NRTTI<NiAVObject>()(e.get()))
			{
				::Util::Node::Traverse(
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

			break;
		}
	}

	SkeletonCache::ActorEntry::ActorEntry(
		const stl::fixed_string& a_key) :
		ptr(stl::make_smart<actor_entry_data>(a_key))
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