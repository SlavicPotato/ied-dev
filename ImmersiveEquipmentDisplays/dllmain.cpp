#include "pch.h"

#include "IED/Main.h"
#include "IED/Util/Logging.h"

#include "gitparams.h"

static bool Initialize(const SKSEInterface* a_skse)
{
	auto& skse = ISKSE::GetSingleton();

	bool result = false;

	try
	{
		gLog.Debug("Querying SKSE interfaces..");

		if (!skse.QueryInterfaces(a_skse))
		{
			gLog.FatalError("Could not query SKSE interfaces");
			return false;
		}

		result = IED::Initializer::GetSingleton().Run(a_skse);

		if (result)
		{
			auto usageBranch = skse.GetTrampolineUsage(TrampolineID::kBranch);
			auto usageLocal  = skse.GetTrampolineUsage(TrampolineID::kLocal);

			gLog.Message(
				"Loaded, trampolines: branch:[%zu/%zu] codegen:[%zu/%zu] IAL:[%lld ms]",
				usageBranch.used,
				usageBranch.total,
				usageLocal.used,
				usageLocal.total,
				IPerfCounter::delta_us(
					IAL::GetLoadStart(),
					IAL::GetLoadEnd()) /
					1000ll);
		}
	}
	catch (const std::exception& e)
	{
		WinApi::MessageBoxErrorFmtLog(
			PLUGIN_NAME,
			"An exception occured during initialization:\n\n%s",
			e.what());

		result = false;
	}
	catch (...)
	{
		WinApi::MessageBoxErrorFmtLog(
			PLUGIN_NAME,
			"An exception occured during initialization");

		result = false;
	}

	return result;
}

extern "C" {
	bool SKSEPlugin_Query(const SKSEInterface* a_skse, PluginInfo* a_info)
	{
		return ISKSE::GetSingleton().Query(a_skse, a_info);
	}

	bool SKSEPlugin_Load(const SKSEInterface* a_skse)
	{
#if defined(SKMP_AE_POST_629)
		if (a_skse->runtimeVersion < RUNTIME_VERSION_1_6_629)
		{
			WinApi::MessageBoxErrorLog(
				PLUGIN_NAME,
				"Not supported on game versions prior to 1.6.629");
			return false;
		}
#endif

		if (IAL::IsAE())
		{
			auto& skse = ISKSE::GetSingleton();

			skse.SetPluginHandle(a_skse->GetPluginHandle());
			skse.OpenLog();
		}

		gLog.Message(
			"%s %s [%.8x-%s] [runtime %u.%u.%u.%u]",
			PLUGIN_NAME_FULL,
			PLUGIN_VERSION_VERSTRING,
			MK_GIT_COMMIT(GIT_CUR_COMMIT),
			MK_STRING(GIT_BRANCH),
			GET_EXE_VERSION_MAJOR(a_skse->runtimeVersion),
			GET_EXE_VERSION_MINOR(a_skse->runtimeVersion),
			GET_EXE_VERSION_BUILD(a_skse->runtimeVersion),
			GET_EXE_VERSION_SUB(a_skse->runtimeVersion));

#if defined(IED_MIMALLOC_IN_USE)
		gLog.Message(
			"mimalloc %u.%u, boost %u.%u.%u, JsonCpp %u.%u.%u, ImGui %s (%u)",
			MI_MALLOC_VERSION / 100,
			MI_MALLOC_VERSION % 100,
			BOOST_VERSION / 100000,
			BOOST_VERSION / 100 % 1000,
			BOOST_VERSION % 100,
			JSONCPP_VERSION_MAJOR,
			JSONCPP_VERSION_MINOR,
			JSONCPP_VERSION_PATCH,
			IMGUI_VERSION,
			IMGUI_VERSION_NUM);
#else

		gLog.Message(
			"boost %u.%u.%u, JsonCpp %u.%u.%u, ImGui %s (%u)",
			BOOST_VERSION / 100000,
			BOOST_VERSION / 100 % 1000,
			BOOST_VERSION % 100,
			JSONCPP_VERSION_MAJOR,
			JSONCPP_VERSION_MINOR,
			JSONCPP_VERSION_PATCH,
			IMGUI_VERSION,
			IMGUI_VERSION_NUM);
#endif

		if (!IAL::IsLoaded())
		{
			WinApi::MessageBoxErrorLog(
				PLUGIN_NAME,
				"Could not load the address library");
			return false;
		}

		if (IAL::HasBadQuery())
		{
			WinApi::MessageBoxErrorLog(
				PLUGIN_NAME,
				"One or more addresses could not be retrieved from the address library");
			return false;
		}

		bool ret = Initialize(a_skse);

		if (!ret)
		{
			WinApi::MessageBoxError(
				PLUGIN_NAME,
				"Plugin initialization failed, see log for more info");
		}

		IAL::Release();

		return ret;
	}

	SKSEPluginVersionData SKSEPlugin_Version = {
		SKSEPluginVersionData::kVersion,
		MAKE_PLUGIN_VERSION(
			PLUGIN_VERSION_MAJOR,
			PLUGIN_VERSION_MINOR,
			PLUGIN_VERSION_REVISION),
		PLUGIN_NAME,
		PLUGIN_AUTHOR,
		"n/a",
		SKSEPluginVersionData::kVersionIndependentEx_None,
		SKSEPluginVersionData::kVersionIndependent_AddressLibraryPostAE
#if defined(SKMP_AE_POST_629)
			| SKSEPluginVersionData::kVersionIndependent_StructsPost629
#endif
		,
		{ RUNTIME_VERSION_1_6_318, RUNTIME_VERSION_1_6_323, 0 },
		0,
	};
};