#pragma once

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	namespace UI
	{
		class UIFileSelector :
			public virtual UILocalizationInterface
		{
			using storage_type = std::map<stl::fixed_string, fs::path>;

			class SelectedFile
			{
			public:
				SelectedFile() = default;

				SelectedFile(
					const fs::path& a_root,
					const storage_type::value_type& a_filename);

				void UpdateInfo();

				fs::path m_fullpath;
				fs::path m_filename;
				stl::fixed_string m_key;
			};

		public:
			bool UpdateFileList(bool a_createPath, bool a_select = true);

		protected:
			UIFileSelector(Localization::ILocalization &a_loc, const fs::path& a_root);

			void DrawFileSelector();

			bool DeleteSelected();

			bool DeleteItem(
				const SelectedFile& a_item);

			bool RenameItem(
				const SelectedFile& a_item,
				const fs::path& a_newFileName);

			void SelectItem(
				const stl::fixed_string& a_itemDesc);

			inline constexpr const auto& GetSelected() const { return m_selected; }

			inline constexpr const auto& GetLastException() const { return m_lastExcept; }

			bool HasFile(const stl::fixed_string& a_itemDesc) const
			{
				return m_files.contains(a_itemDesc);
			}

		private:
			bool Delete(const fs::path& a_file);

			stl::optional<SelectedFile> m_selected;
			storage_type m_files;
			fs::path m_root;

			except::descriptor m_lastExcept;
		};
	}  // namespace UI
}  // namespace IED