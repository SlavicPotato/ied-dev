#include "pch.h"

#include "KeyBindDataHolder.h"

#include "Parsers/JSONKeyToggleStateEntryHolderParser.h"

namespace IED
{
	namespace KB
	{
		void KeyBindDataHolder::ResetKeyToggleStates() noexcept
		{
			const stl::lock_guard lock(m_lock);

			for (auto& e : m_data.entries)
			{
				e.second.SetState(false);
			}
		}

		KeyToggleStateEntryHolder::state_data KeyBindDataHolder::GetKeyToggleStates() const
		{
			const stl::lock_guard lock(m_lock);

			return m_data.make_state_data();
		}

		void KeyBindDataHolder::InitializeKeyToggleStates(
			const KeyToggleStateEntryHolder::state_data& a_states) noexcept
		{
			const stl::lock_guard lock(m_lock);

			auto& entries = m_data.entries;

			for (auto& e : entries)
			{
				e.second.SetState(false);
			}

			for (auto& e : a_states)
			{
				const auto it = entries.find(e.first);

				if (it != entries.end())
				{
					it->second.SetState(e.second);
				}
			}
		}

		bool KeyBindDataHolder::GetKeyState(const stl::fixed_string& a_id) const noexcept
		{
			const stl::lock_guard lock(m_lock);

			auto& entries = m_data.entries;

			const auto it = entries.find(a_id);

			return it != entries.end() ? it->second.GetState() : false;
		}

		bool KeyBindDataHolder::Save(const fs::path& a_path) const
		{
			using namespace Serialization;

			try
			{
				const stl::lock_guard lock(m_lock);

				ParserState                       state;
				Parser<KeyToggleStateEntryHolder> parser(state);
				Json::Value                       root;

				parser.Create(m_data, root);

				WriteData(a_path, root);

				m_dirty = false;

				return true;
			}
			catch (const std::exception& e)
			{
				m_lastException = e;

				return false;
			}
			catch (...)
			{
				m_lastException.clear();

				return false;
			}
		}

		bool KeyBindDataHolder::Load(const fs::path& a_path)
		{
			using namespace Serialization;

			try
			{
				const stl::lock_guard lock(m_lock);

				ParserState                       state;
				Parser<KeyToggleStateEntryHolder> parser(state);

				Json::Value root;

				ReadData(a_path, root);

				auto tmp = std::make_unique_for_overwrite<KeyToggleStateEntryHolder>();

				if (!parser.Parse(root, *tmp))
				{
					throw std::exception("parse failed");
				}

				m_data  = std::move(*tmp);
				m_dirty = false;

				return true;
			}
			catch (const std::exception& e)
			{
				m_lastException = e;

				return false;
			}
			catch (...)
			{
				m_lastException.clear();

				return false;
			}
		}
	}
}