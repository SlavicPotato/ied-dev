#include "pch.h"

#include "IJSONSerialization.h"
#include "IMaintenance.h"

#include "IED/Parsers/JSONConfigStoreParser.h"
#include "Serialization/Serialization.h"

#include "IED/Data.h"
#include "IED/StringHolder.h"

namespace IED
{
	using namespace Serialization;

	template <class T>
	constexpr void EraseTemporary(T& a_in)
	{
		for (auto it = a_in.begin(); it != a_in.end();)
		{
			if (it->first.IsTemporary())
			{
				it = a_in.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	static void CopyCustomPapyrusEntries(
		const Data::configCustomPluginMap_t& a_src,
		Data::configCustomPluginMap_t&       a_dst)
	{
		for (auto& e : a_src)
		{
			if (e.first == StringHolder::GetSingleton().IED)
			{
				continue;
			}

			a_dst.emplace(e.first, e.second);
		}
	}

	static void CopyCustomPapyrusEntries(
		const Data::configMapCustom_t& a_src,
		Data::configMapCustom_t&       a_dst)
	{
		for (auto& e : a_src)
		{
			for (auto& f : e.second)
			{
				if (f.first != StringHolder::GetSingleton().IED)
				{
					a_dst
						.try_emplace(e.first)
						.first->second
						.emplace(f.first, f.second);
				}
			}
		}
	}

	bool IJSONSerialization::ImportData(
		const Data::configStore_t&                     a_in,
		stl::flag<ImportFlags>                         a_flags,
		stl::flag<Data::ConfigStoreSerializationFlags> a_serFlags)
	{
		auto data = CreateFilteredConfigStore(a_in, ExportFlags::kNone, a_serFlags);

		if (a_flags.test(ImportFlags::kEraseTemporary))
		{
			EraseTemporary(data->slot.GetActorData());
			EraseTemporary(data->slot.GetNPCData());
			EraseTemporary(data->custom.GetActorData());
			EraseTemporary(data->custom.GetNPCData());
			EraseTemporary(data->transforms.GetActorData());
			EraseTemporary(data->transforms.GetNPCData());
		}

		if (a_flags.test(ImportFlags::kMerge))
		{
			return DoImportMerge(std::move(*data), a_flags);
		}
		else
		{
			return DoImportOverwrite(std::move(*data), a_flags);
		}
	}

	bool IJSONSerialization::DoImportOverwrite(
		Data::configStore_t&&  a_in,
		stl::flag<ImportFlags> a_flags)
	{
		const stl::lock_guard lock(JSGetLock());

		auto& store = JSGetConfigStore();

		const auto& fm = store.custom.GetFormMaps();

		for (std::size_t i = 0; i < std::size(fm); i++)
		{
			CopyCustomPapyrusEntries(
				fm[i],
				a_in.custom.GetFormMaps()[i]);
		}

		const auto& gd = store.custom.GetGlobalData();

		for (std::size_t i = 0; i < std::size(gd); i++)
		{
			CopyCustomPapyrusEntries(
				gd[i],
				a_in.custom.GetGlobalData()[i]);
		}

		store = std::move(a_in);

		JSOnDataImport();

		return true;
	}

	static void MergeConfig(
		Data::configFormMap_t<Data::configCustomPluginMap_t>&& a_in,
		Data::configFormMap_t<Data::configCustomPluginMap_t>&  a_out)
	{
		for (auto& e : a_in)
		{
			if (auto it = e.second.find(StringHolder::GetSingleton().IED);
			    it != e.second.end())
			{
				a_out.try_emplace(e.first)
					.first->second
					.insert_or_assign(it->first, std::move(it->second));
			}
		}
	}

	static void MergeConfig(
		Data::configCustomPluginMap_t&& a_in,
		Data::configCustomPluginMap_t&  a_dst)
	{
		if (auto it = a_in.find(StringHolder::GetSingleton().IED);
		    it != a_in.end())
		{
			a_dst.insert_or_assign(it->first, std::move(it->second));
		}
	}

	template <class T>
	static void MergeConfig(
		Data::configFormMap_t<T>&& a_in,
		Data::configFormMap_t<T>&  a_out)
	{
		for (auto& e : a_in)
		{
			a_out.insert_or_assign(e.first, std::move(e.second));
		}
	}

	template <class Ti, class To>
	static void MergeConfig(
		Ti&& a_in,
		To&  a_out)
	{
		auto& fm = a_in.GetFormMaps();

		for (std::size_t i = 0; i < std::size(fm); i++)
		{
			MergeConfig(
				std::move(fm[i]),
				a_out.GetFormMaps()[i]);
		}

		auto& gd = a_in.GetGlobalData();

		for (std::size_t i = 0; i < std::size(gd); i++)
		{
			if constexpr (std::is_same_v<
							  Ti,
							  Data::configStoreCustom_t>)
			{
				MergeConfig(
					std::move(gd[i]),
					a_out.GetGlobalData()[i]);
			}
			else
			{
				a_out.GetGlobalData()[i] = std::move(gd[i]);
			}
		}
	}

	bool IJSONSerialization::DoImportMerge(
		Data::configStore_t&&  a_in,
		stl::flag<ImportFlags> a_flags)
	{
		const stl::lock_guard lock(JSGetLock());

		auto& store = JSGetConfigStore();

		MergeConfig(std::move(a_in.slot), store.slot);
		MergeConfig(std::move(a_in.custom), store.custom);
		MergeConfig(std::move(a_in.transforms), store.transforms);
		store.condvars = std::move(a_in.condvars);
		store.outfit   = std::move(a_in.outfit);
		store.keybinds = std::move(a_in.keybinds);

		JSOnDataImport();

		return true;
	}

	bool IJSONSerialization::ExportData(
		const fs::path&                                a_path,
		stl::flag<ExportFlags>                         a_exportFlags,
		stl::flag<Data::ConfigStoreSerializationFlags> a_flags)
	{
		try
		{
			ParserState state;

			Parser<Data::configStore_t> parser(state);
			Json::Value                 root;

			std::unique_ptr<Data::configStore_t> tmp;

			{
				const stl::lock_guard lock(JSGetLock());

				tmp = CreateFilteredConfigStore(JSGetConfigStore(), a_exportFlags, a_flags);
			}

			parser.Create(*tmp, root);

			WriteData(a_path, root);

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

	bool IJSONSerialization::LoadConfigStore(
		const fs::path&      a_path,
		Data::configStore_t& a_out) const
	{
		Serialization::ParserState state;

		return LoadConfigStore(a_path, a_out, state);
	}

	bool IJSONSerialization::LoadConfigStore(
		const fs::path&             a_path,
		Data::configStore_t&        a_out,
		Serialization::ParserState& a_state) const
	{
		using namespace Serialization;

		try
		{
			Json::Value root;

			ReadData(a_path, root);

			Parser<Data::configStore_t> parser(a_state);

			auto tmp = std::make_unique_for_overwrite<Data::configStore_t>();

			if (!parser.Parse(root, *tmp))
			{
				throw std::exception("parse failed");
			}

			if (parser.HasErrors())
			{
				Warning(
					"%s: [%s] parser errors occured",
					__FUNCTION__,
					SafeGetPath(a_path).c_str());
			}

			a_out = std::move(*tmp);

			return true;
		}
		catch (const std::exception& e)
		{
			Exception(
				e,
				"%s: [%s]",
				__FUNCTION__,
				SafeGetPath(a_path).c_str());

			m_lastException = e;

			return false;
		}
		catch (...)
		{
			Error(
				"%s: [%s] unknown",
				__FUNCTION__,
				SafeGetPath(a_path).c_str());

			m_lastException.clear();

			return false;
		}
	}

	bool IJSONSerialization::SaveConfigStore(
		const fs::path&            a_path,
		const Data::configStore_t& a_data) const
	{
		using namespace Serialization;

		try
		{
			ParserState                 state;
			Parser<Data::configStore_t> parser(state);
			Json::Value                 root;

			parser.Create(
				a_data,
				root);

			WriteData(a_path, root);

			return true;
		}
		catch (const std::exception& e)
		{
			m_lastException = e;

			Exception(
				e,
				"%s: [%s]",
				__FUNCTION__,
				SafeGetPath(a_path).c_str());

			return false;
		}
		catch (...)
		{
			m_lastException.clear();

			Error(
				"%s: [%s] unknown",
				__FUNCTION__,
				SafeGetPath(a_path).c_str());

			return false;
		}
	}

	std::unique_ptr<Data::configStore_t> IJSONSerialization::CreateFilteredConfigStore(
		const Data::configStore_t&                     a_data,
		stl::flag<ExportFlags>                         a_exportFlags,
		stl::flag<Data::ConfigStoreSerializationFlags> a_flags)
	{
		using namespace Data;

		auto result = std::make_unique<Data::configStore_t>();

		for (std::size_t i = 0; i < std::size(a_data.slot.GetGlobalData()); i++)
		{
			auto type = static_cast<GlobalConfigType>(i);
			if (type == GlobalConfigType::Player)
			{
				if (!a_flags.test(ConfigStoreSerializationFlags::kSlotGlobalPlayer))
				{
					continue;
				}
			}
			else if (type == GlobalConfigType::NPC)
			{
				if (!a_flags.test(ConfigStoreSerializationFlags::kSlotGlobalNPC))
				{
					continue;
				}
			}

			result->slot.GetGlobalData()[i] = a_data.slot.GetGlobalData()[i];
		}

		if (a_flags.test(ConfigStoreSerializationFlags::kSlotActor))
		{
			result->slot.GetActorData() = a_data.slot.GetActorData();
		}

		if (a_flags.test(ConfigStoreSerializationFlags::kSlotNPC))
		{
			result->slot.GetNPCData() = a_data.slot.GetNPCData();
		}

		if (a_flags.test(ConfigStoreSerializationFlags::kSlotRace))
		{
			result->slot.GetRaceData() = a_data.slot.GetRaceData();
		}

		//

		for (std::size_t i = 0; i < std::size(a_data.transforms.GetGlobalData()); i++)
		{
			auto type = static_cast<GlobalConfigType>(i);
			if (type == GlobalConfigType::Player)
			{
				if (!a_flags.test(ConfigStoreSerializationFlags::kSlotGlobalPlayer))
				{
					continue;
				}
			}
			else if (type == GlobalConfigType::NPC)
			{
				if (!a_flags.test(ConfigStoreSerializationFlags::kSlotGlobalNPC))
				{
					continue;
				}
			}

			result->transforms.GetGlobalData()[i] = a_data.transforms.GetGlobalData()[i];
		}

		if (a_flags.test(ConfigStoreSerializationFlags::kNodeOverrideActor))
		{
			result->transforms.GetActorData() = a_data.transforms.GetActorData();
		}
		else if (a_flags.test(ConfigStoreSerializationFlags::kNodeOverridePlayer))
		{
			auto& data = a_data.transforms.GetActorData();
			if (auto it = data.find(IData::GetPlayerRefID()); it != data.end())
			{
				result->transforms.GetActorData().emplace(*it);
			}
		}

		if (a_flags.test(ConfigStoreSerializationFlags::kNodeOverrideNPC))
		{
			result->transforms.GetNPCData() = a_data.transforms.GetNPCData();
		}

		if (a_flags.test(ConfigStoreSerializationFlags::kNodeOverrideRace))
		{
			result->transforms.GetRaceData() = a_data.transforms.GetRaceData();
		}

		if (!a_exportFlags.test(ExportFlags::kKeepGenerated))
		{
			IMaintenance::ClearConfigStoreRand(*result);
		}

		//

		if (a_flags.test(ConfigStoreSerializationFlags::kCustomGlobal))
		{
			auto& sh   = StringHolder::GetSingleton();
			auto& data = a_data.custom.GetGlobalData()[0];

			if (auto it = data.find(sh.IED); it != data.end())
			{
				result->custom.GetGlobalData()[0].emplace(*it);
			}
		}

		if (a_flags.test(ConfigStoreSerializationFlags::kCustomActor))
		{
			auto& sh = StringHolder::GetSingleton();

			for (auto& e : a_data.custom.GetActorData())
			{
				if (auto it = e.second.find(sh.IED); it != e.second.end())
				{
					result->custom.GetActorData().try_emplace(e.first).first->second.emplace(*it);
				}
			}
		}
		else if (a_flags.test(ConfigStoreSerializationFlags::kCustomPlayer))
		{
			auto& data = a_data.custom.GetActorData();

			if (auto it1 = data.find(IData::GetPlayerRefID()); it1 != data.end())
			{
				auto& sh = StringHolder::GetSingleton();
				if (auto it2 = it1->second.find(sh.IED); it2 != it1->second.end())
				{
					result->custom.GetActorData().try_emplace(IData::GetPlayerRefID()).first->second.emplace(*it2);
				}
			}
		}

		if (a_flags.test(ConfigStoreSerializationFlags::kCustomNPC))
		{
			auto& sh = StringHolder::GetSingleton();

			for (auto& e : a_data.custom.GetNPCData())
			{
				if (auto it = e.second.find(sh.IED); it != e.second.end())
				{
					result->custom.GetNPCData().try_emplace(e.first).first->second.emplace(*it);
				}
			}
		}

		if (a_flags.test(ConfigStoreSerializationFlags::kCustomRace))
		{
			auto& sh = StringHolder::GetSingleton();

			for (auto& e : a_data.custom.GetRaceData())
			{
				if (auto it = e.second.find(sh.IED); it != e.second.end())
				{
					result->custom.GetRaceData().try_emplace(e.first).first->second.emplace(*it);
				}
			}
		}

		//

		if (a_flags.test(ConfigStoreSerializationFlags::kConditionalVariables))
		{
			result->condvars = a_data.condvars;
		}

		//

		if (a_flags.test(ConfigStoreSerializationFlags::kOutfit))
		{
			result->outfit = a_data.outfit;
		}

		//

		if (a_flags.test(ConfigStoreSerializationFlags::kKeyBinds))
		{
			result->keybinds = a_data.keybinds;
		}

		return result;
	}

}