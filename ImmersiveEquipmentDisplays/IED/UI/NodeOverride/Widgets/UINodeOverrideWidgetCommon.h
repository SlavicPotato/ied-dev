#pragma once

#include "IED/UI/Widgets/UIWidgetsCommon.h"

#include "IED/ConfigCommon.h"
#include "IED/ConfigNodeOverride.h"

namespace IED
{
	namespace UI
	{
		using entryNodeOverrideData_t = Data::configNodeOverrideHolderCopy_t;

		enum class NodeOverrideCommonAction : std::uint8_t
		{
			None,
			Insert,
			Delete,
			Clear,
			Swap,
			Reset,
			Rename,
			Copy,
			Paste,
			PasteOver
		};

		struct NodeOverrideCommonResult
		{
			NodeOverrideCommonAction        action{ NodeOverrideCommonAction::None };
			stl::fixed_string               str;
			std::string                     desc;
			Data::NodeOverrideConditionType matchType;
			Game::FormID                    form;
			SwapDirection                   dir{ SwapDirection::None };

			union
			{
				BIPED_OBJECT             biped;
				Data::ExtraConditionType excond;
				Data::ObjectSlotExtra    type;
			};

			bool isGroup{ false };
		};

		template <class T>
		struct SingleNodeOverrideUpdateParams
		{
			using data_type = typename T;

			Data::ConfigSex         sex;
			stl::fixed_string       name;
			data_type::mapped_type& entry;
		};

		template <class T>
		struct profileSelectorParamsNodeOverride_t
		{
			T                        handle;
			entryNodeOverrideData_t& data;
		};

		struct NodeOverrideUpdateParams
		{
			entryNodeOverrideData_t& data;
		};

		struct ClearNodeOverrideUpdateParams
		{
			stl::fixed_string        name;
			entryNodeOverrideData_t& entry;
		};

		struct ClearAllNodeOverrideUpdateParams
		{
			entryNodeOverrideData_t& entry;
		};

		using SingleNodeOverrideTransformUpdateParams = SingleNodeOverrideUpdateParams<entryNodeOverrideData_t::transform_data_type>;
		using SingleNodeOverridePlacementUpdateParams = SingleNodeOverrideUpdateParams<entryNodeOverrideData_t::placement_data_type>;
		using SingleNodeOverridePhysicsUpdateParams   = SingleNodeOverrideUpdateParams<entryNodeOverrideData_t::physics_data_type>;

		enum class NodeOverrideDataType
		{
			kTransform,
			kPlacement,
			kPhysics
		};

		template <class T>
		inline consteval NodeOverrideDataType GetNodeOverrideDataTypeFromParams()
		{
			if constexpr (std::is_same_v<T, SingleNodeOverrideTransformUpdateParams>)
			{
				return NodeOverrideDataType::kTransform;
			}
			else if constexpr (std::is_same_v<T, SingleNodeOverridePlacementUpdateParams>)
			{
				return NodeOverrideDataType::kPlacement;
			}
			else if constexpr (std::is_same_v<T, SingleNodeOverridePhysicsUpdateParams>)
			{
				return NodeOverrideDataType::kPhysics;
			}
			else
			{
				static_assert(false);
			}
		}
	}
}