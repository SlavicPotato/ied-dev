#pragma once

#include "NodeDescriptor.h"

namespace IED
{
	namespace Data
	{
		class NodeMap
		{
		public:
			using value_type = NodeDescriptor;

			using data_type = stl::vectormap<stl::fixed_string, value_type>;
			using map_type  = std::unordered_map<stl::fixed_string, value_type>;

			struct initializer_type
			{
				const char*         node;
				const char*         desc;
				NodeDescriptorFlags flags;
			};

			static inline constexpr auto& GetSingleton() noexcept
			{
				return m_Instance;
			}

			inline constexpr auto& GetData() noexcept
			{
				return m_data;
			}

			inline constexpr const auto& GetData() const noexcept
			{
				return m_data;
			}

			inline constexpr const auto& GetExtraData() const noexcept
			{
				return m_extraData;
			}

			void Merge(std::initializer_list<initializer_type> a_init);
			void Merge(std::initializer_list<map_type::value_type> a_init);

			std::pair<map_type::iterator, bool> Add(
				const char*                    a_node,
				const char*                    a_desc,
				stl::flag<NodeDescriptorFlags> a_flags);

			bool RemoveExtra(const stl::fixed_string& a_node);

			void SortIfDirty();

			inline void SetDirty()
			{
				m_dirty = true;
			}

			bool SaveExtra(const fs::path& a_path) const;
			bool LoadExtra(const fs::path& a_path);

			inline constexpr const auto& GetLastException() const noexcept
			{
				return m_lastException;
			}

			template <std::size_t _Size>
			static const char* GetNodeDescription(
				const char*                    a_desc,
				stl::flag<NodeDescriptorFlags> a_flags,
				char (&a_out)[_Size]);

			static bool ValidateNodeName(const std::string& a_name) noexcept;

		private:
			NodeMap();

			void Sort();

			data_type m_data;
			map_type  m_extraData;

			bool m_dirty{ false };

			mutable FastSpinLock       m_rwLock;
			mutable except::descriptor m_lastException;

			static NodeMap m_Instance;
		};

		template <std::size_t _Size>
		const char* NodeMap::GetNodeDescription(
			const char*                    a_desc,
			stl::flag<NodeDescriptorFlags> a_flags,
			char (&a_out)[_Size])
		{
			char          buf[10];
			std::uint32_t index = 0;

			if (a_flags.test(NodeDescriptorFlags::kWeapon))
			{
				buf[index] = 'W';
				index++;
			}

			if (a_flags.test(NodeDescriptorFlags::kDefault))
			{
				buf[index] = 'D';
				index++;
			}

			if (a_flags.test(NodeDescriptorFlags::kManaged))
			{
				buf[index] = 'M';
				index++;
			}

			if (a_flags.test(NodeDescriptorFlags::kXP32))
			{
				buf[index] = 'X';
				index++;
			}

			if (a_flags.test(NodeDescriptorFlags::kExtra))
			{
				buf[index] = 'E';
				index++;
			}

			if (a_flags.test(NodeDescriptorFlags::kDefaultCopy))
			{
				buf[index] = 'C';
				index++;
			}

			if (a_flags.test(NodeDescriptorFlags::kPapyrus))
			{
				buf[index] = 'P';
				index++;
			}

			if (index)
			{
				buf[index] = 0;

				stl::snprintf(a_out, "%s [%s]", a_desc, buf);
				return a_out;
			}
			else
			{
				return a_desc;
			}
		}

	}
}