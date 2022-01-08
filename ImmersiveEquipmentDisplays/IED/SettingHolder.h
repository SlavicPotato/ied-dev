#pragma once

#include "ConfigOverride.h"
#include "ConfigOverrideDefault.h"
#include "NodeMap.h"

#include "Controller/ObjectDatabaseLevel.h"

#include "Fonts/FontInfo.h"
#include "UI/UIData.h"
#include "UI/UIMainCommon.h"

#include "Controller/ImportFlags.h"

namespace IED
{
	namespace Data
	{
		class SettingHolder
		{
		public:

			using EditorPanelCommonFlagsType = std::uint32_t;

			struct EditorPanelCommon
			{
				bool sexSync{ false };
				bool eoPropagation{ false };
				EditorPanelCommonFlagsType flags{ 0 };

				template <
					class T,
					class = std::enable_if_t<
						std::is_enum_v<T> && std::is_same_v<std::underlying_type_t<T>, EditorPanelCommonFlagsType>,
						void>>
				inline constexpr stl::flag<T>& get_flags() noexcept
				{
					return reinterpret_cast<stl::flag<T>&>(flags);
				}

				/*template <class T, class = std::enable_if_t<std::is_enum_v<T>, void>>
				inline constexpr const T& get_flags() const noexcept
				{
					return static_cast<const T&>(flags);
				}*/
			};

			/*struct ObjectDatabase
			{
				std::size_t limit{ 10 };
			};*/

			struct ProfileEditor :
				public EditorPanelCommon
			{
				ConfigSex sex{ ConfigSex::Male };

				UI::UIData::UICollapsibleStates colStates;
			};

			struct EditorPanelActorSettings
			{
				bool autoSelectSex{ true };
				bool showAll{ true };
				ConfigSex sex{ Data::ConfigSex::Male };

				Game::FormID lastActor;
			};

			struct EditorPanelRaceSettings
			{
				bool playableOnly{ false };
				bool showEditorIDs{ true };
				ConfigSex sex{ ConfigSex::Male };
			};

			struct EditorPanel :
				public EditorPanelCommon
			{
				EditorPanelActorSettings actorConfig;
				EditorPanelActorSettings npcConfig;
				EditorPanelRaceSettings raceConfig;

				GlobalConfigType globalType{ GlobalConfigType::Player };
				ConfigSex globalSex{ ConfigSex::Male };

				ConfigClass lastConfigClass{ ConfigClass::Global };

				UI::UIData::UICollapsibleStates colStates[CONFIG_CLASS_MAX];
			};

			struct ImportExport
			{
				stl::flag<Data::ConfigStoreSerializationFlags> exportFlags{
					Data::ConfigStoreSerializationFlags::kAll
				};
				stl::flag<ImportFlags> importFlags{ ImportFlags::kEraseTemporary };
			};

			struct UserInterface
			{
				UserInterface() noexcept
				{
					for (auto& e : logLevels)
					{
						e = true;
					}
				}

				EditorPanel slotEditor;
				EditorPanel customEditor;
				EditorPanel transformEditor;
				ProfileEditor slotProfileEditor;
				ProfileEditor customProfileEditor;
				ProfileEditor transformProfileEditor;
				ImportExport importExport;

				UI::UIData::UICollapsibleStates settingsColStates;
				UI::UIData::UICollapsibleStates statsColStates;

				UI::UIEditorPanel lastPanel;

				ConfigKeyPair toggleKeys;

				bool enableControlLock{ true };
				bool enableRestrictions{ false };
				bool selectCrosshairActor{ true };
				float scale{ 1.0f };

				std::uint32_t logLimit{ 200 };
				bool logLevels[stl::underlying(LogLevel::Max) + 1];

				bool closeOnESC{ false };

				stl::flag<Data::ConfigStoreSerializationFlags> defaultExportFlags{
					Data::ConfigStoreSerializationFlags::kAll
				};

				stl::fixed_string font;
				stl::optional<float> fontSize;
				stl::flag<GlyphPresetFlags> extraGlyphs{ GlyphPresetFlags::kNone };
			};

			struct Settings
			{
				UserInterface ui;

				ConfigKeyPair playerBlockKeys;
				Data::ConfigSound<Game::FormID> sound;

				bool hideEquipped{ false };
				bool toggleKeepLoaded{ false };

				stl::optional<LogLevel> logLevel;

				ObjectDatabaseLevel odbLevel{ ObjectDatabaseLevel::kNone };

				stl::fixed_string language;
			};

			Settings data;

			inline void SetPath(const fs::path& a_path)
			{
				m_path = a_path;
			}

			inline void SetPath(fs::path&& a_path)
			{
				m_path = std::move(a_path);
			}

			inline constexpr const auto& GetPath() const noexcept
			{
				return m_path;
			}

			bool Load();
			bool Save();
			bool SaveIfDirty();

			template <
				class Tm,
				class Tv,
				class = std::enable_if_t<std::is_convertible_v<Tv, Tm>>>
			inline void set(Tm& a_member, Tv&& a_value)
			{
				a_member = std::forward<Tv>(a_value);
				m_dirty = true;
			}

			inline void mark_dirty() noexcept
			{
				m_dirty = true;
			}

			inline bool mark_if(bool a_isTrue) noexcept
			{
				if (a_isTrue)
				{
					m_dirty = true;
				}
				return a_isTrue;
			}

			inline constexpr const auto& GetLastException() const noexcept
			{
				return m_lastException;
			}
			
			inline constexpr auto HasErrors() const noexcept
			{
				return m_loadHasErrors;
			}

		private:
			mutable except::descriptor m_lastException;

			fs::path m_path;
			bool m_dirty{ false };
			bool m_loadHasErrors{ false };
		};

	}  // namespace Data
}  // namespace IED