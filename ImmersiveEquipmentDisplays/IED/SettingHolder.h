#pragma once

#include "ConfigSerializationFlags.h"
#include "ConfigStore.h"
#include "NodeMap.h"

#include "Controller/ObjectDatabaseLevel.h"

#include "Fonts/FontInfo.h"
#include "UI/UIData.h"
#include "UI/UIMainCommon.h"

#include "Controller/ImportFlags.h"

#include "ImGui/Styles/StylePreset.h"

namespace IED
{
	namespace Data
	{
		enum class DefaultConfigType : std::uint8_t
		{
			kDefault = 0,
			kUser    = 1
		};

		class SettingHolder
		{
		public:
			using EditorPanelCommonFlagsType = std::uint32_t;

			struct EditorPanelCommon
			{
				bool                       sexSync{ false };
				bool                       eoPropagation{ false };
				EditorPanelCommonFlagsType flags{ 0 };

				template <
					class T,
					class = std::enable_if_t<
						std::is_enum_v<T> &&
							std::is_same_v<
								std::underlying_type_t<T>,
								EditorPanelCommonFlagsType>,
						void>>
				inline constexpr stl::flag<T&> get_flags() noexcept
				{
					return reinterpret_cast<T&>(flags);
				}
			};

			struct ProfileEditor :
				public EditorPanelCommon
			{
				ConfigSex sex{ ConfigSex::Male };

				UI::UIData::UICollapsibleStates colStates;
			};

			struct EditorPanelActorSettings
			{
				bool      autoSelectSex{ true };
				bool      showAll{ true };
				ConfigSex sex{ Data::ConfigSex::Male };

				Game::FormID lastActor;
			};

			struct EditorPanelRaceSettings
			{
				bool      playableOnly{ false };
				bool      showEditorIDs{ true };
				ConfigSex sex{ ConfigSex::Male };
			};

			struct EditorPanel :
				public EditorPanelCommon
			{
				EditorPanelActorSettings actorConfig;
				EditorPanelActorSettings npcConfig;
				EditorPanelRaceSettings  raceConfig;

				GlobalConfigType globalType{ GlobalConfigType::Player };
				ConfigSex        globalSex{ ConfigSex::Male };
				ConfigClass      lastConfigClass{ ConfigClass::Global };

				UI::UIData::UICollapsibleStates colStates[CONFIG_CLASS_MAX];
			};

			struct ImportExport
			{
				stl::flag<ConfigStoreSerializationFlags> serializationFlags{
					ConfigStoreSerializationFlags::kAll
				};
				stl::flag<ImportFlags> importFlags{ ImportFlags::kEraseTemporary };
			};

			struct SkeletonExplorer
			{
				EditorPanelActorSettings        actorSettings;
				UI::UIData::UICollapsibleStates colStates;

				bool firstPerson{ false };
				bool filterShowChildNodes{ false };
				bool showLoadedSkeleton{ true };
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

				EditorPanel      slotEditor;
				EditorPanel      customEditor;
				EditorPanel      transformEditor;
				ProfileEditor    slotProfileEditor;
				ProfileEditor    customProfileEditor;
				ProfileEditor    transformProfileEditor;
				ImportExport     importExport;
				SkeletonExplorer skeletonExplorer;

				UI::UIData::UICollapsibleStates settingsColStates;
				UI::UIData::UICollapsibleStates statsColStates;

				UI::UIEditorPanel lastPanel{ UI::UIEditorPanel::Slot };

				stl::optional<ConfigKeyPair> openKeys;

				bool  enableControlLock{ true };
				bool  enableFreezeTime{ false };
				bool  enableRestrictions{ false };
				bool  selectCrosshairActor{ true };
				float scale{ 1.0f };

				std::uint32_t logLimit{ 500 };
				bool          logShowTimestamps{ true };
				bool          logLevels[stl::underlying(LogLevel::Max) + 1];

				bool closeOnESC{ true };
				bool showIntroBanner{ true };

				stl::flag<Data::ConfigStoreSerializationFlags> defaultExportFlags{
					Data::ConfigStoreSerializationFlags::kAll
				};

				stl::fixed_string           font;
				stl::optional<float>        fontSize;
				stl::flag<GlyphPresetFlags> extraGlyphs{ GlyphPresetFlags::kNone };
				bool                        releaseFontData{ false };

				DefaultConfigType selectedDefaultConfImport{ DefaultConfigType::kUser };

				UIStylePreset        stylePreset{ UIStylePreset::Dark };
				float                alpha{ 1.0f };
				stl::optional<float> bgAlpha;
			};

			struct Settings
			{
				UserInterface ui;

				stl::optional<ConfigKeyPair>    playerBlockKeys;
				Data::ConfigSound<Game::FormID> sound;

				bool hideEquipped{ false };
				bool toggleKeepLoaded{ false };
				bool disableNPCSlots{ false };
				bool removeFavRestriction{ false };
				bool enableXP32AA{ false };
				bool XP32AABowIdle{ false };
				bool XP32AABowAtk{ false };
				bool placementRandomization{ false };
				bool hkWeaponAnimations{ false };
				bool hkWeaponAnimationsWarned{ false };
				bool animEventForwarding{ false };
				bool effectShaderParallelUpdates{ false };

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
				class Tv>
			inline constexpr void set(Tm& a_member, Tv&& a_value)  //
				noexcept(std::is_nothrow_assignable_v<Tm&, Tv&&>)  //
				requires(std::is_assignable_v<Tm&, Tv&&>)
			{
				a_member = std::forward<Tv>(a_value);
				m_dirty  = true;
			}

			inline constexpr void mark_dirty() noexcept
			{
				m_dirty = true;
			}

			inline constexpr bool mark_if(bool a_isTrue) noexcept
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
			bool     m_dirty{ false };
			bool     m_loadHasErrors{ false };
		};

	}
}