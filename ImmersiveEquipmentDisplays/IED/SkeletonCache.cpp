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
		-> std::optional<data_type::value_type>
	{
		const std::lock_guard lock(m_lock);

		auto key = mk_key(a_refr, a_firstPerson);
		if (key.empty())
		{
			return {};
		}

		return *get_or_create(key);
	}

	auto SkeletonCache::Get2(
		TESObjectREFR* a_refr,
		bool           a_firstPerson)
		-> actor_entry_type
	{
		const std::lock_guard lock(m_lock);

		auto key = mk_key(a_refr, a_firstPerson);
		if (key.empty())
		{
			return {};
		}

		return get_or_create(key)->second;
	}

	std::size_t SkeletonCache::GetTotalEntries() const noexcept
	{
		const std::lock_guard lock(m_lock);

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
		-> data_type::const_iterator
	{
		auto r = m_data.try_emplace(a_key);

		if (!r.first->second)
		{
			r.first->second = std::make_unique<actor_entry_type::element_type>();

			fill(a_key, r.first);
		}

		return r.first;
	}

	void SkeletonCache::fill(
		const stl::fixed_string& a_key,
		data_type::iterator      a_it)
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

			auto object = NRTTI<NiAVObject>()(e);
			if (!object)
			{
				continue;
			}

			::Util::Node::Traverse(object, [&](NiAVObject* a_object) {
				auto& name = a_object->m_name;

				if (!name.empty())
				{
					a_it->second->try_emplace(name.data(), a_object->m_localTransform);
				}

				return ::Util::Node::VisitorControl::kContinue;
			});
		}
	}
}