#pragma once

#include "IED/ConfigStore.h"

namespace IED
{
	namespace UI
	{
		class UIClipboard
		{
		public:
			enum class DataType : std::uint8_t
			{
				None,
				ConfigBaseValues,
				ConfigBase,
				ConfigSlot,
				ConfigSlotPriority,
				ConfigCustom,
				EquipmentOverride,
				EquipmentOverrideCondition,
				EquipmentOverrideConditionList,
				FormSet,
				FormList,
				NodeOverrideHolder,
				NodeOverrideTransform,
				NodeOverridePlacement,
				NodeOverridePlacementOverride,
				NodeOverridePlacementOverrideList,
				NodeOverridePlacementValues,
				NodeOverridePhysics,
				NodeOverridePhysicsOverride,
				NodeOverridePhysicsOverrideList,
				NodePhysicsValues,
				NodeOverrideTransformValues,
				NodeOverrideOffset,
				NodeOverrideOffsetList,
				NodeOverrideOffsetConditionList,
				ConfigTransform,
				FormFilter,
				EffectShaderList,
				EffectShaderHolder,
				EquipmentOverrideList,
				EffectShaderData,
				FixedStringSet,
				FixedStringList,
				EffectShaderFunction,
				EffectShaderFunctionList,
				BipedObjectList,
				ConditionalVariablesList,
				ConditionalVariablesEntry,
				ConditionalVariable,
			};

			struct entry_t
			{
				DataType type{ DataType::None };
				void*    data{ nullptr };
			};

			template <class T, class data_type = stl::strip_type<T>>
			static constexpr const data_type* Get() noexcept;

			template <class T, class = std::enable_if_t<std::is_copy_constructible_v<T>, void>>
			static constexpr void Set(const T& a_data);

			template <class T, class... Args>
			static constexpr void Set(Args&&... a_data);

			static void clear();

		private:
			UIClipboard() = default;
			~UIClipboard();

			template <class T>
			static constexpr void erase();

			template <class T>
			static constexpr void set_type();

			entry_t m_data;

			static UIClipboard m_Instance;
		};

		template <class T, class data_type>
		inline constexpr const data_type* UIClipboard::Get() noexcept
		{
			auto& data = m_Instance.m_data;

			if (!data.data)
			{
				return nullptr;
			}

			if constexpr (std::is_same_v<data_type, Data::configBaseValues_t>)
			{
				switch (data.type)
				{
				case DataType::ConfigCustom:
					{
						auto v = static_cast<Data::configCustomNameValue_t*>(data.data);
						return static_cast<data_type*>(std::addressof(v->data(v->sex)));
					}
				case DataType::ConfigSlot:
				case DataType::EquipmentOverride:
				case DataType::ConfigBase:
				case DataType::ConfigBaseValues:
					return static_cast<data_type*>(data.data);
				default:
					return nullptr;
				}
			}
			else if constexpr (std::is_same_v<data_type, Data::configBase_t>)
			{
				switch (data.type)
				{
				case DataType::ConfigCustom:
					return static_cast<data_type*>(std::addressof(static_cast<Data::configCustomNameValue_t*>(data.data)->second));
				case DataType::ConfigSlot:
				case DataType::ConfigBase:
					return static_cast<data_type*>(data.data);
				default:
					return nullptr;
				}
			}
			else if constexpr (std::is_same_v<data_type, Data::configCustomNameValue_t>)
			{
				return data.type == DataType::ConfigCustom ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configSlot_t>)
			{
				return data.type == DataType::ConfigSlot ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configSlotPriority_t>)
			{
				return data.type == DataType::ConfigSlotPriority ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::equipmentOverride_t>)
			{
				return data.type == DataType::EquipmentOverride ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::equipmentOverrideCondition_t>)
			{
				return data.type == DataType::EquipmentOverrideCondition ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::equipmentOverrideConditionList_t>)
			{
				return data.type == DataType::EquipmentOverrideConditionList ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configFormSet_t>)
			{
				return data.type == DataType::FormSet ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configFormList_t>)
			{
				return data.type == DataType::FormList ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configNodeOverrideTransform_t>)
			{
				return data.type == DataType::NodeOverrideTransform ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configNodeOverrideHolderClipboardData_t>)
			{
				return data.type == DataType::NodeOverrideHolder ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configNodeOverridePlacement_t>)
			{
				return data.type == DataType::NodeOverridePlacement ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configNodeOverridePlacementOverride_t>)
			{
				return data.type == DataType::NodeOverridePlacementOverride ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configNodeOverridePlacementOverrideList_t>)
			{
				return data.type == DataType::NodeOverridePlacementOverrideList ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configNodeOverridePhysics_t>)
			{
				return data.type == DataType::NodeOverridePhysics ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configNodeOverridePhysicsOverride_t>)
			{
				return data.type == DataType::NodeOverridePhysicsOverride ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configNodeOverridePhysicsOverrideList_t>)
			{
				return data.type == DataType::NodeOverridePhysicsOverrideList ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configNodeOverrideOffset_t>)
			{
				return data.type == DataType::NodeOverrideOffset ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configNodeOverrideOffsetList_t>)
			{
				return data.type == DataType::NodeOverrideOffsetList ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configNodeOverrideConditionList_t>)
			{
				return data.type == DataType::NodeOverrideOffsetConditionList ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configTransform_t>)
			{
				return data.type == DataType::ConfigTransform ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configFormFilter_t>)
			{
				return data.type == DataType::FormFilter ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configNodeOverrideTransformValues_t>)
			{
				switch (data.type)
				{
				case DataType::NodeOverrideOffset:
				case DataType::NodeOverrideTransformValues:
				case DataType::NodeOverrideTransform:
					return static_cast<data_type*>(data.data);
				default:
					return nullptr;
				}
			}
			else if constexpr (std::is_same_v<data_type, Data::configNodeOverridePlacementValues_t>)
			{
				switch (data.type)
				{
				case DataType::NodeOverridePlacementOverride:
				case DataType::NodeOverridePlacementValues:
				case DataType::NodeOverridePlacement:
					return static_cast<data_type*>(data.data);
				default:
					return nullptr;
				}
			}
			else if constexpr (std::is_same_v<data_type, Data::configNodePhysicsValues_t>)
			{
				switch (data.type)
				{
				case DataType::NodeOverridePhysicsOverride:
				case DataType::NodePhysicsValues:
				case DataType::NodeOverridePhysics:
					return static_cast<data_type*>(data.data);
				default:
					return nullptr;
				}
			}
			else if constexpr (std::is_same_v<data_type, Data::effectShaderList_t>)
			{
				return data.type == DataType::EffectShaderList ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configEffectShaderHolder_t>)
			{
				return data.type == DataType::EffectShaderHolder ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::equipmentOverrideList_t>)
			{
				return data.type == DataType::EquipmentOverrideList ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configEffectShaderData_t>)
			{
				return data.type == DataType::EffectShaderData ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configFixedStringSet_t>)
			{
				return data.type == DataType::FixedStringSet ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configFixedStringList_t>)
			{
				return data.type == DataType::FixedStringList ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configEffectShaderFunction_t>)
			{
				return data.type == DataType::EffectShaderFunction ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configEffectShaderFunctionList_t>)
			{
				return data.type == DataType::EffectShaderFunctionList ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configBipedObjectList_t>)
			{
				return data.type == DataType::BipedObjectList ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configConditionalVariablesEntryListValue_t>)
			{
				return data.type == DataType::ConditionalVariablesEntry ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configConditionalVariablesList_t>)
			{
				return data.type == DataType::ConditionalVariablesList ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else if constexpr (std::is_same_v<data_type, Data::configConditionalVariable_t>)
			{
				return data.type == DataType::ConditionalVariable ?
				           static_cast<data_type*>(data.data) :
                           nullptr;
			}
			else
			{
				static_assert(false);
			}
		}

		template <class T, class>
		inline constexpr void UIClipboard::Set(const T& a_data)
		{
			clear();

			m_Instance.m_data.data = new T(a_data);

			set_type<T>();
		}

		template <class T, class... Args>
		inline constexpr void UIClipboard::Set(Args&&... a_data)
		{
			clear();

			m_Instance.m_data.data = new T(std::forward<Args>(a_data)...);

			set_type<T>();
		}

		template <class T>
		inline constexpr void UIClipboard::erase()
		{
			auto& data = m_Instance.m_data;

			if (data.data)
			{
				delete static_cast<T*>(data.data);

				data.data = nullptr;
			}

			data.type = DataType::None;
		}

		template <class T>
		inline constexpr void UIClipboard::set_type()
		{
			auto& data = m_Instance.m_data;

			if constexpr (std::is_same_v<T, Data::configBaseValues_t>)
			{
				data.type = DataType::ConfigBaseValues;
			}
			else if constexpr (std::is_same_v<T, Data::configBase_t>)
			{
				data.type = DataType::ConfigBase;
			}
			else if constexpr (std::is_same_v<T, Data::configCustomNameValue_t>)
			{
				data.type = DataType::ConfigCustom;
			}
			else if constexpr (std::is_same_v<T, Data::configSlot_t>)
			{
				data.type = DataType::ConfigSlot;
			}
			else if constexpr (std::is_same_v<T, Data::configSlotPriority_t>)
			{
				data.type = DataType::ConfigSlotPriority;
			}
			else if constexpr (std::is_same_v<T, Data::equipmentOverride_t>)
			{
				data.type = DataType::EquipmentOverride;
			}
			else if constexpr (std::is_same_v<T, Data::equipmentOverrideCondition_t>)
			{
				data.type = DataType::EquipmentOverrideCondition;
			}
			else if constexpr (std::is_same_v<T, Data::equipmentOverrideConditionList_t>)
			{
				data.type = DataType::EquipmentOverrideConditionList;
			}
			else if constexpr (std::is_same_v<T, Data::configFormSet_t>)
			{
				data.type = DataType::FormSet;
			}
			else if constexpr (std::is_same_v<T, Data::configFormList_t>)
			{
				data.type = DataType::FormList;
			}
			else if constexpr (std::is_same_v<T, Data::configNodeOverrideHolderClipboardData_t>)
			{
				data.type = DataType::NodeOverrideHolder;
			}
			else if constexpr (std::is_same_v<T, Data::configNodeOverrideTransform_t>)
			{
				data.type = DataType::NodeOverrideTransform;
			}
			else if constexpr (std::is_same_v<T, Data::configNodeOverridePlacement_t>)
			{
				data.type = DataType::NodeOverridePlacement;
			}
			else if constexpr (std::is_same_v<T, Data::configNodeOverridePlacementOverride_t>)
			{
				data.type = DataType::NodeOverridePlacementOverride;
			}
			else if constexpr (std::is_same_v<T, Data::configNodeOverridePlacementOverrideList_t>)
			{
				data.type = DataType::NodeOverridePlacementOverrideList;
			}
			else if constexpr (std::is_same_v<T, Data::configNodeOverridePlacementValues_t>)
			{
				data.type = DataType::NodeOverridePlacementValues;
			}
			else if constexpr (std::is_same_v<T, Data::configNodePhysicsValues_t>)
			{
				data.type = DataType::NodePhysicsValues;
			}
			else if constexpr (std::is_same_v<T, Data::configNodeOverridePhysicsOverride_t>)
			{
				data.type = DataType::NodeOverridePhysicsOverride;
			}
			else if constexpr (std::is_same_v<T, Data::configNodeOverridePhysicsOverrideList_t>)
			{
				data.type = DataType::NodeOverridePhysicsOverrideList;
			}
			else if constexpr (std::is_same_v<T, Data::configNodeOverridePhysics_t>)
			{
				data.type = DataType::NodeOverridePhysics;
			}
			else if constexpr (std::is_same_v<T, Data::configNodeOverrideOffset_t>)
			{
				data.type = DataType::NodeOverrideOffset;
			}
			else if constexpr (std::is_same_v<T, Data::configNodeOverrideTransformValues_t>)
			{
				data.type = DataType::NodeOverrideTransformValues;
			}
			else if constexpr (std::is_same_v<T, Data::configNodeOverrideOffsetList_t>)
			{
				data.type = DataType::NodeOverrideOffsetList;
			}
			else if constexpr (std::is_same_v<T, Data::configNodeOverrideConditionList_t>)
			{
				data.type = DataType::NodeOverrideOffsetConditionList;
			}
			else if constexpr (std::is_same_v<T, Data::configTransform_t>)
			{
				data.type = DataType::ConfigTransform;
			}
			else if constexpr (std::is_same_v<T, Data::configFormFilter_t>)
			{
				data.type = DataType::FormFilter;
			}
			else if constexpr (std::is_same_v<T, Data::effectShaderList_t>)
			{
				data.type = DataType::EffectShaderList;
			}
			else if constexpr (std::is_same_v<T, Data::configEffectShaderHolder_t>)
			{
				data.type = DataType::EffectShaderHolder;
			}
			else if constexpr (std::is_same_v<T, Data::equipmentOverrideList_t>)
			{
				data.type = DataType::EquipmentOverrideList;
			}
			else if constexpr (std::is_same_v<T, Data::configEffectShaderData_t>)
			{
				data.type = DataType::EffectShaderData;
			}
			else if constexpr (std::is_same_v<T, Data::configFixedStringSet_t>)
			{
				data.type = DataType::FixedStringSet;
			}
			else if constexpr (std::is_same_v<T, Data::configFixedStringList_t>)
			{
				data.type = DataType::FixedStringList;
			}
			else if constexpr (std::is_same_v<T, Data::configEffectShaderFunction_t>)
			{
				data.type = DataType::EffectShaderFunction;
			}
			else if constexpr (std::is_same_v<T, Data::configEffectShaderFunctionList_t>)
			{
				data.type = DataType::EffectShaderFunctionList;
			}
			else if constexpr (std::is_same_v<T, Data::configBipedObjectList_t>)
			{
				data.type = DataType::BipedObjectList;
			}
			else if constexpr (std::is_same_v<T, Data::configConditionalVariablesEntryListValue_t>)
			{
				data.type = DataType::ConditionalVariablesEntry;
			}
			else if constexpr (std::is_same_v<T, Data::configConditionalVariablesList_t>)
			{
				data.type = DataType::ConditionalVariablesList;
			}
			else if constexpr (std::is_same_v<T, Data::configConditionalVariable_t>)
			{
				data.type = DataType::ConditionalVariable;
			}
			else
			{
				static_assert(false);
			}
		}
	}
}