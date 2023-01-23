#pragma once

#include "ConfigSerializationFlags.h"
#include "ConfigStore.h"
#include "NodeMap.h"

#include "Controller/ObjectDatabaseLevel.h"

#include "Fonts/FontInfo.h"

#include "UI/UIChildWindowID.h"
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
			//kNone = static_cast<std::underlying_type_t<DefaultConfigType>>(-1),

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
				constexpr stl::flag<T&> get_flags() noexcept
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

				Game::FormID lastSelected;
			};

			struct EditorPanelRaceSettings
			{
				bool      playableOnly{ false };
				bool      showEditorIDs{ true };
				ConfigSex sex{ ConfigSex::Male };

				Game::FormID lastSelected;
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

			struct ActorInfo
			{
				EditorPanelActorSettings        actorSettings;
				UI::UIData::UICollapsibleStates colStates;
			};

			struct ConditionalVariablesEditor
			{
				UI::UIData::UICollapsibleStates colStates;
			};

			struct I3DI
			{
				bool enableWeapons{ false };
			};

			struct UserInterface
			{
				UserInterface() noexcept
				{
					for (auto& e : logLevels)
					{
						e = true;
					}

					windowOpenStates[stl::underlying(UI::ChildWindowID::kUIDisplayManagement)] = true;
				}

				EditorPanel                slotEditor;
				EditorPanel                customEditor;
				EditorPanel                transformEditor;
				ProfileEditor              slotProfileEditor;
				ProfileEditor              customProfileEditor;
				ProfileEditor              transformProfileEditor;
				ImportExport               importExport;
				SkeletonExplorer           skeletonExplorer;
				ActorInfo                  actorInfo;
				ConditionalVariablesEditor condVarEditor;
				ConditionalVariablesEditor condVarProfileEditor;
				I3DI                       i3di;

				UI::UIData::UICollapsibleStates settingsColStates;
				UI::UIData::UICollapsibleStates statsColStates;

				UI::UIDisplayManagementEditorPanel lastPanel{ UI::UIDisplayManagementEditorPanel::Slot };

				stl::optional<ConfigKeyPair> openKeys;
				stl::optional<ConfigKeyPair> releaseLockKeys;
				float                        releaseLockAlpha{ 0.33f };
				bool                         releaseLockUnfreezeTime{ false };

				bool  enableControlLock{ true };
				bool  enableFreezeTime{ false };
				bool  enableRestrictions{ false };
				bool  selectCrosshairActor{ true };
				float scale{ 1.0f };

				std::uint32_t logLimit{ 500 };
				bool          logShowTimestamps{ true };
				bool          logLevels[stl::underlying(LogLevel::Max) + 1]{ false };

				bool closeOnESC{ true };
				bool exitOnLastWindowClose{ true };
				bool showIntroBanner{ true };
				bool enableNotifications{ false };

				LogLevel notificationThreshold{ LogLevel::Message };

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

				/*[[nodiscard]] constexpr bool GetChildWindowOpen(UI::ChildWindowID a_id) const noexcept
				{
					assert(a_id < UI::ChildWindowID::kMax);
					return windowOpenStates[stl::underlying(a_id)];
				}

				constexpr void SetChildWindowOpen(UI::ChildWindowID a_id, bool a_state) noexcept
				{
					assert(a_id < UI::ChildWindowID::kMax);
					windowOpenStates[stl::underlying(a_id)] = a_state;
				}*/

				bool windowOpenStates[stl::underlying(UI::ChildWindowID::kMax)]{ false };
			};

			struct Physics
			{
				float maxDiff{ 1024.0f };
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
				bool hkWeaponAnimations{ true };
				bool syncTransformsToFirstPersonSkeleton{ false };
				bool enableEquipmentPhysics{ false };
				bool enableEffectShaders{ false };
				bool lightEnableNPCUpdates{ false };
				bool lightNPCUpdateFix{ false };
				bool lightNPCCellAttachFix{ false };
				bool apParallelUpdates{ true };

				stl::optional<LogLevel> logLevel;

				ObjectDatabaseLevel odbLevel{ ObjectDatabaseLevel::kLow };

				Physics physics;

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

			[[nodiscard]] constexpr const auto& GetPath() const noexcept
			{
				return m_path;
			}

			bool Load();
			bool Save();
			bool SaveIfDirty();

			template <
				class Tm,
				class Tv>
			constexpr void set(Tm& a_member, Tv&& a_value)  //
				noexcept(std::is_nothrow_assignable_v<Tm&, Tv&&>)  //
				requires(std::is_assignable_v<Tm&, Tv &&>)
			{
				a_member = std::forward<Tv>(a_value);
				m_dirty  = true;
			}

			constexpr void mark_dirty() noexcept
			{
				m_dirty = true;
			}

			constexpr bool mark_if(bool a_isTrue) noexcept
			{
				if (a_isTrue)
				{
					m_dirty = true;
				}
				return a_isTrue;
			}

			[[nodiscard]] constexpr const auto& GetLastException() const noexcept
			{
				return m_lastException;
			}

			[[nodiscard]] constexpr auto HasErrors() const noexcept
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