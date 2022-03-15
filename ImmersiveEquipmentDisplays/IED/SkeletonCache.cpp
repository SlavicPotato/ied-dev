#include "pch.h"

#include "SkeletonCache.h"

#include <ext/Model.h>
#include <ext/Node.h>

namespace IED
{
	SkeletonCache SkeletonCache::m_Instance;

	auto SkeletonCache::GetNode(
		TESObjectREFR*           a_refr,
		const stl::fixed_string& a_name)
		-> const Entry*
	{
		auto key = mk_key(a_refr);
		if (key.empty())
		{
			return nullptr;
		}

		auto it1 = get_or_create(key);

		if (auto it2 = it1->second.find(a_name);
		    it2 != it1->second.end())
		{
			return std::addressof(it2->second);
		}
		else
		{
			return nullptr;
		}
	}

	stl::fixed_string SkeletonCache::mk_key(
		TESObjectREFR* a_refr)
	{
		auto path = ::Util::Model::Get3DPath(a_refr, false);
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

		if (r.second)
		{
			fill(a_key, r.first);
		}

		return r.first;
	}

	void SkeletonCache::fill(
		const stl::fixed_string& a_key,
		data_type::iterator      a_it)
	{
		::Util::Stream::NiStreamWrapper stream;

		BSResourceNiBinaryStream binaryStream(a_key.c_str());
		if (!binaryStream.IsValid())
		{
			return;
		}

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

			auto object = ni_cast(e, NiNode);
			if (!object)
			{
				continue;
			}

			::Util::Node::Traverse(object, [&](NiAVObject* a_object) {
				if (auto node = a_object->GetAsNiNode())
				{
					auto& name = node->m_name;

					if (*name.data() != 0)
					{
						a_it->second.try_emplace(name.data(), node->m_localTransform);
					}
				}

				return ::Util::Node::VisitorControl::kContinue;
			});

			break;
		}
	}
}