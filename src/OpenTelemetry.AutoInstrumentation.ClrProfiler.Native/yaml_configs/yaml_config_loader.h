#ifndef INCEPTION_CLR_PROFILER_YAML_CONFIG_LOADER_H
#define INCEPTION_CLR_PROFILER_YAML_CONFIG_LOADER_H

#include <yaml-cpp/yaml.h>

#include <utility>
#include <vector>

#include "yaml_config.h"

namespace trace {

std::vector<instrumentationConfig> LoadConfigsFromEnvironment();

namespace {

std::pair<classMethodFilter, bool> FilterFromYaml(const YAML::Node& src);
std::pair<classMatch, bool> ClassMatchFromYaml(const YAML::Node& src);
std::pair<methodMatch, bool> MethodMatchFromYaml(const YAML::Node& src);
std::pair<classMatchRule, bool> ClassMatchRuleFromYaml(const YAML::Node& src);
std::pair<methodMatchRule, bool> MethodMatchRuleFromYaml(const YAML::Node& src);
wrapper WrapperFromYaml(const YAML::Node& src);

}  // namespace

}  // namespace trace

#endif  // !INCEPTION_CLR_PROFILER_YAML_CONFIG_LOADER_H
