#include "yaml_config_loader.h"

#include <iostream>
#include <filesystem>

#include "../environment_variables.h"
#include "../util.h"

namespace trace {

std::vector<instrumentationConfig> LoadConfigsFromEnvironment() {
  std::vector<instrumentationConfig> configs;
  //std::string yamlDirectory = ToString(GetEnvironmentValues(environment::yaml_paths)[0]);
  std::string filePath = __FILE__;
  std::string yamlPath = filePath.substr(0, filePath.size() - 133) +
                         "conf\\rules\\instrumentation\\test.yaml";

  std::vector<std::string> yamlPaths;

  for (auto &p : std::filesystem::recursive_directory_iterator(yamlPath)) {
    if (p.path().extension() == ".yaml") {
      yamlPaths.push_back(p.path().string());
      std::cout << p.path().string() << "\n";
    }
  }

  for (const auto f : yamlPaths) {
    YAML::Node yamlFile = YAML::LoadFile(ToString(f));
    WSTRING name, type;
    bool disabled;
    int priority;
    std::vector<classMethodFilter> classMethodFilters;
    YAML::Node filterSource;

    if (yamlFile["name"]) name = ToWSTRING(yamlFile["name"].as<std::string>());
    if (yamlFile["type"]) type = ToWSTRING(yamlFile["type"].as<std::string>());
    if (yamlFile["disabled"]) disabled = yamlFile["disabled"].as<bool>();
    if (yamlFile["priority"]) priority = yamlFile["priority"].as<int>();

    if (yamlFile["classMethodFilters"])
      filterSource = yamlFile["classMethodFilters"];

    if (!filterSource.IsNull()) {
      for (const auto filter : filterSource) {
        auto flt = FilterFromYaml(filter);
        if (std::get<1>(flt)) classMethodFilters.push_back(std::get<0>(flt));
      }
    }

    configs.push_back({name, type, disabled, priority, classMethodFilters});
  }

  return configs;
}

namespace {
std::pair<classMethodFilter, bool> FilterFromYaml(const YAML::Node& src) {
  WSTRING name, id;
  classMatch classMatch_value;
  methodMatch methodMatch_value;
  wrapper wrapper_value;
  YAML::Node classMatch_src, methodMatch_src, wrapper_src;

  // std::cout << src;

  if (src["name"]) name = ToWSTRING(src["name"].as<std::string>());
  if (src["id"]) id = ToWSTRING(src["id"].as<std::string>());
  if (src["classMatch"]) classMatch_src = src["classMatch"];
  if (src["methodMatch"]) methodMatch_src = src["methodMatch"];
  if (src["wrapper"]) wrapper_src = src["wrapper"];

  if (!classMatch_src.IsNull()) {
    auto cm = ClassMatchFromYaml(classMatch_src);
    if (std::get<1>(cm)) classMatch_value = std::get<0>(cm);
  }

  if (!classMatch_src.IsNull()) {
    auto mm = MethodMatchFromYaml(methodMatch_src);
    if (std::get<1>(mm)) methodMatch_value = std::get<0>(mm);
  }

  if (!wrapper_src.IsNull()) {
    wrapper_value = WrapperFromYaml(wrapper_src);
  }

  if (!classMatch_src.IsNull() && !methodMatch_src.IsNull() &&
      !wrapper_src.IsNull()) {
    return std::make_pair<classMethodFilter, bool>(
        {name, id, classMatch_value, methodMatch_value, wrapper_value}, true);
  }

  else
    return std::make_pair<classMethodFilter, bool>({}, false);
}

std::pair<classMatch, bool> ClassMatchFromYaml(const YAML::Node& src) {
  std::vector<classMatchRule> includes_value;
  std::vector<classMatchRule> excludes_value;
  classMatch obj;
  YAML::Node includes_src, excludes_src;

  if (src["includes"]) includes_src = src["includes"];
  if (src["excludes"]) excludes_src = src["excludes"];

  if (!includes_src.IsNull()) {
    for (const auto rule : includes_src) {
      auto r = ClassMatchRuleFromYaml(rule);
      if (std::get<1>(r)) includes_value.push_back(std::get<0>(r));
    }
  }

  if (!excludes_src.IsNull()) {
    for (const auto rule : excludes_src) {
      auto r = ClassMatchRuleFromYaml(rule);
      if (std::get<1>(r)) excludes_value.push_back(std::get<0>(r));
    }
  }

  if (!includes_src.IsNull() || !excludes_src.IsNull()) {
    return std::make_pair<classMatch, bool>({includes_value, excludes_value},
                                            true);
  }

  else
    return std::make_pair<classMatch, bool>({}, false);
}

std::pair<methodMatch, bool> MethodMatchFromYaml(const YAML::Node& src) {
  std::vector<methodMatchRule> includes_value;
  std::vector<methodMatchRule> excludes_value;
  methodMatch obj;
  YAML::Node includes_src, excludes_src;

  if (src["includes"]) includes_src = src["includes"];
  if (src["excludes"]) excludes_src = src["excludes"];

  if (!includes_src.IsNull()) {
    for (const auto rule : includes_src) {
      auto r = MethodMatchRuleFromYaml(rule);
      if (std::get<1>(r)) includes_value.push_back(std::get<0>(r));
    }
  }

  if (!excludes_src.IsNull()) {
    for (const auto rule : excludes_src) {
      auto r = MethodMatchRuleFromYaml(rule);
      if (std::get<1>(r)) excludes_value.push_back(std::get<0>(r));
    }
  }

  if (!includes_src.IsNull() || !excludes_src.IsNull()) {
    return std::make_pair<methodMatch, bool>({includes_value, excludes_value},
                                             true);
  }

  else
    return std::make_pair<methodMatch, bool>({}, false);
}

std::pair<classMatchRule, bool> ClassMatchRuleFromYaml(const YAML::Node& src) {
  WSTRING type;
  WSTRING matchType;
  std::vector<std::string> matchValue;

  if (src["type"]) type = ToWSTRING(src["type"].as<std::string>());
  if (src["matchType"])
    matchType = ToWSTRING(src["matchType"].as<std::string>());
  if (src["matchValue"])
    matchValue = src["matchValue"].as<std::vector<std::string>>();

  if (src["type"] && src["matchType"])
    return std::make_pair<classMatchRule, bool>({type, matchType, matchValue},
                                                true);

  return std::make_pair<classMatchRule, bool>({}, false);
}

std::pair<methodMatchRule, bool> MethodMatchRuleFromYaml(
    const YAML::Node& src) {
  WSTRING type;
  WSTRING matchType;
  std::vector<std::string> matchValue;
  bool matchParams;
  std::vector<std::string> paramTypes;

  if (src["type"]) type = ToWSTRING(src["type"].as<std::string>());
  if (src["matchType"])
    matchType = ToWSTRING(src["matchType"].as<std::string>());
  if (src["matchValue"])
    matchValue = src["matchValue"].as<std::vector<std::string>>();
  if (src["matchParams"]) matchParams = src["matchParams"].as<bool>();
  if (src["paramTypes"])
    paramTypes = src["paramTypes"].as<std::vector<std::string>>();

  if (src["type"] && src["matchType"])
    return std::make_pair<methodMatchRule, bool>(
        {type, matchType, matchValue, matchParams, paramTypes}, true);

  return std::make_pair<methodMatchRule, bool>({}, false);
}

wrapper WrapperFromYaml(const YAML::Node& src) {
  WSTRING assembly = L"";
  WSTRING type = L"";
  WSTRING method = L"";
  WSTRING signature = L"";
  WSTRING action = L"";

  // std::cout << src;

  if (src["assembly"]) assembly = ToWSTRING(src["assembly"].as<std::string>());
  if (src["method"]) method = ToWSTRING(src["method"].as<std::string>());
  if (src["signature"])
    signature = ToWSTRING(src["signature"].as<std::string>());
  if (src["action"]) action = ToWSTRING(src["action"].as<std::string>());
  if (src["type"]) type = ToWSTRING(src["type"].as<std::string>());

  return wrapper(assembly, type, method, signature, action);
}

}  // namespace

}  // namespace trace