#include "AppConfigStore.h"

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>

#include <um-crypto/utils/base64.h>

#include "../types.h"
#include "../utils/AppDataPath.h"
#include "../utils/JSONExtension.h"

#include <fstream>

namespace json = umd::utils::json;

namespace umd::config {

AppConfigStore* AppConfigStore::instance_ = new AppConfigStore();

AppConfigStore::AppConfigStore() {
  config_file_path_ = umd::utils::GetUserDataDirectory() / "config.json5";
}

constexpr int kModeReadConfigFromJSON = 1;
constexpr int kModeSaveConfigToJSON = 2;

template <int MODE>
inline void JSONManipulate(AppConfig& config, rapidjson::Document& doc) {
  using namespace rapidjson;
  if (!doc.IsObject()) {
    doc.SetObject();
  }

#define BEGIN_MANIP_NAMESPACED_VALUE(PARENT, NAMESPACE)                      \
  {                                                                          \
    if (!PARENT.HasMember(#NAMESPACE)) {                                     \
      PARENT.AddMember(#NAMESPACE, Value().SetObject(), doc.GetAllocator()); \
    }                                                                        \
    auto& childConf = config.NAMESPACE;                                      \
    auto& childJSON = PARENT[#NAMESPACE];                                    \
    if (!childJSON.IsObject()) {                                             \
      childJSON.SetObject();                                                 \
    }
#define END_MANIP_NAMESPACED_VALUE() }

#define MANIP_JSON_ITEM(KEY, DEFAULT_VAL)                         \
  if (MODE == kModeReadConfigFromJSON) {                          \
    json::ReadValue(childJSON, #KEY, childConf.KEY, DEFAULT_VAL); \
  } else {                                                        \
    json::WriteValue(doc, childJSON, #KEY, childConf.KEY);        \
  }

  Vec<u8> empty_bytes;

  // General config
  BEGIN_MANIP_NAMESPACED_VALUE(doc, general) {
    MANIP_JSON_ITEM(thread_count, (int)4);
  }
  END_MANIP_NAMESPACED_VALUE()

  // Kugou config
  BEGIN_MANIP_NAMESPACED_VALUE(doc, kugou) {
    MANIP_JSON_ITEM(t1, empty_bytes);
    MANIP_JSON_ITEM(t2, empty_bytes);
    MANIP_JSON_ITEM(v2, empty_bytes);
    MANIP_JSON_ITEM(vpr_key, empty_bytes);
  }
  END_MANIP_NAMESPACED_VALUE()

  // Ximalaya config
  BEGIN_MANIP_NAMESPACED_VALUE(doc, xmly) {
    MANIP_JSON_ITEM(x2m_content_key, empty_bytes);
    MANIP_JSON_ITEM(x3m_content_key, empty_bytes);
    MANIP_JSON_ITEM(x3m_scramble_indexes, empty_bytes);
  }
  END_MANIP_NAMESPACED_VALUE()

#undef BEGIN_MANIP_NAMESPACED_VALUE
#undef END_MANIP_NAMESPACED_VALUE
#undef MANIP_JSON_ITEM
}

bool AppConfigStore::LoadConfigFromDisk() {
  std::ifstream config_file(config_file_path_, std::fstream::binary);

  using namespace rapidjson;
  IStreamWrapper json_stream(config_file);
  Document d;

  if (config_file.is_open()) {
    d.ParseStream<kParseCommentsFlag | kParseTrailingCommasFlag>(json_stream);
  }

  config_ = {};
  JSONManipulate<kModeReadConfigFromJSON>(config_, d);

  return true;
}

bool AppConfigStore::SaveConfigToDisk() {
  std::ofstream config_file(config_file_path_,
                            std::fstream::out | std::fstream::binary);

  using namespace rapidjson;
  Document d;
  JSONManipulate<kModeSaveConfigToJSON>(config_, d);

  OStreamWrapper json_stream(config_file);
  PrettyWriter<OStreamWrapper> writer(json_stream);
  writer.SetIndent(' ', 2);
  d.Accept(writer);

  return true;
}

}  // namespace umd::config
