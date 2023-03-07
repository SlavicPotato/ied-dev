#pragma once

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	namespace UI
	{
		class UIFileSelector
		{
			using storage_type = stl::map<
				stl::fixed_string,
				fs::path,
				stl::fixed_string::less_str>;

			class SelectedFile
			{
			public:
				SelectedFile() = default;

				SelectedFile(
					const fs::path&                 a_root,
					const storage_type::value_type& a_filename);

				void UpdateInfo();

				fs::path          m_fullpath;
				fs::path          m_filename;
				stl::fixed_string m_key;
			};

		public:
			bool UpdateFileList();

		protected:
			UIFileSelector(
				const fs::path& a_root,
				const fs::path& a_ext);

			void DrawFileSelector();

			bool DeleteSelected();

			bool DeleteItem(
				const SelectedFile& a_item);

			bool RenameItem(
				const SelectedFile& a_item,
				const fs::path&     a_newFileName);

			void SelectItem(
				const stl::fixed_string& a_itemDesc);

			constexpr const auto& GetSelected() const noexcept
			{
				return m_selected;
			}

			constexpr const auto& GetLastException() const noexcept
			{
				return m_lastExcept;
			}

			inline bool HasFile(const stl::fixed_string& a_itemDesc) const
			{
				return m_files.contains(a_itemDesc);
			}

		private:
			bool Delete(const fs::path& a_file);

			static std::string get_key(const fs::path& a_filename);

			std::optional<SelectedFile> m_selected;
			storage_type                m_files;
			fs::path                    m_root;
			fs::path                    m_ext;

			except::descriptor m_lastExcept;
		};
	}
}