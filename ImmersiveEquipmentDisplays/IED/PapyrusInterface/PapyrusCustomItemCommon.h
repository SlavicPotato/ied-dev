#pragma once

#include "IED/ConfigCustom.h"
#include "IED/NodeDescriptor.h"

namespace IED
{
	namespace Papyrus
	{
		namespace Custom
		{
			struct keyPair_t
			{
				stl::fixed_string key;
				stl::fixed_string name;

				inline constexpr explicit operator bool() const noexcept
				{
					return !key.empty() && !name.empty();
				}
			};

			static inline constexpr Data::ConfigSex GetSex(bool a_female) noexcept
			{
				return a_female ? Data::ConfigSex::Female : Data::ConfigSex::Male;
			}

			template <class T>
			static inline constexpr auto GetConfigClass() noexcept
			{
				Data::ConfigClass cl;

				if constexpr (std::is_same_v<T, Actor>)
				{
					cl = Data::ConfigClass::Actor;
				}
				else if constexpr (std::is_same_v<T, TESNPC>)
				{
					cl = Data::ConfigClass::NPC;
				}
				else if constexpr (std::is_same_v<T, TESRace>)
				{
					cl = Data::ConfigClass::Race;
				}
				else
				{
					//static_assert(false);
					HALT("fixme");
				}

				return cl;
			}

			void QueueReset(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name);

			void QueueReset(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key);

			void QueueReset(
				Game::FormID      a_target,
				Data::ConfigClass a_class);

			void QueueTransformUpdate(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name);

			void QueueEvaluate(
				Game::FormID      a_target,
				Data::ConfigClass a_class);

			keyPair_t GetKeys(
				const BSFixedString& a_key,
				const BSFixedString& a_name);

			Data::NodeDescriptor GetOrCreateNodeDescriptor(const BSFixedString& a_node);

			Data::configMapCustom_t& GetConfigMap(Data::ConfigClass a_class) noexcept;

			Data::configCustomEntry_t* LookupConfig(
				Game::FormID             a_target,
				Data::ConfigClass        a_class,
				const stl::fixed_string& a_key,
				const stl::fixed_string& a_name);

			std::optional<NiPoint3> GetNiPoint3(VMArray<float>& a_in);

			enum class TransformClearFlags : std::uint8_t
			{
				Position = 1i8 << 0,
				Rotation = 1i8 << 1,
				Scale    = 1i8 << 1,

				All = Position | Rotation | Scale
			};

			DEFINE_ENUM_CLASS_BITWISE(TransformClearFlags)
		}
	}
}