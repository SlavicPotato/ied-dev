#pragma once

#define PLUGIN_NAME      "ImmersiveEquipmentDisplays"
#define PLUGIN_NAME_FULL "Immersive Equipment Displays"
#define PLUGIN_AUTHOR    "SlavicPotato"
#define PLUGIN_URL       "nexusmods.com/skyrimspecialedition/mods/62001"
#define PLUGIN_URL_DEV   "github.com/SlavicPotato/ied-dev"

#include "version.h"

#define MIN_RUNTIME_VERSION RUNTIME_VERSION_1_5_39

#define PLUGIN_LOG_PATH       PLUGIN_NAME ".log"
#define PLUGIN_INI_FILE_NOEXT "Data\\SKSE\\Plugins\\" PLUGIN_NAME
#define PLUGIN_INI_FILE       PLUGIN_INI_FILE_NOEXT ".ini"

#define DEP_URL_MIMALLOC "https://github.com/microsoft/mimalloc"
#define DEP_URL_BOOST    "https://www.boost.org/"
#define DEP_URL_JSONCPP  "https://github.com/open-source-parsers/jsoncpp"
#define DEP_URL_IMGUI    "https://github.com/ocornut/imgui"
