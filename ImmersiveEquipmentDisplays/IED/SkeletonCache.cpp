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
		-> const const_actor_entry_type
	{
		const auto key = mk_key(a_refr, a_firstPerson);
		if (!key.empty())
		{
			return get_or_create(key);
		}
		else
		{
			return {};
		}
	}

	std::size_t SkeletonCache::GetSize() const noexcept
	{
		const boost::lock_guard lock(m_lock);

		return m_data.size();
	}

	std::size_t SkeletonCache::GetTotalEntries() const noexcept
	{
		const boost::lock_guard lock(m_lock);

		std::size_t result = 0;

		for (auto& e : m_data)
		{
			result += e.second->size();
		}

		return result;
	}

	stl::fixed_string SkeletonCache::mk_key(
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
		-> const const_actor_entry_type
	{
		const boost::lock_guard lock(m_lock);

		const auto r = m_data.try_emplace(a_key);

		if (r.second)
		{
			r.first->second = std::make_shared<actor_entry_type::element_type>();

			fill(a_key, *r.first->second);
		}

		return r.first->second;
	}

	void SkeletonCache::fill(
		const stl::fixed_string&        a_key,
		actor_entry_type::element_type& a_entry)
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

		if (!stream->m_rootObjects.m_data)
		{
			return;
		}

		for (auto& e : stream->m_rootObjects)
		{
			if (!e)
			{
				continue;
			}

			if (auto object = NRTTI<NiAVObject>()(e))
			{
				::Util::Node::Traverse(
					object,
					[&](const NiAVObject* a_object) [[msvc::forceinline]] {
						const auto& name = a_object->m_name;

						if (!name.empty())
						{
							a_entry.emplace(name.data(), a_object->m_localTransform);
						}

						return ::Util::Node::VisitorControl::kContinue;
					});
			}

			break;
		}
	}
}