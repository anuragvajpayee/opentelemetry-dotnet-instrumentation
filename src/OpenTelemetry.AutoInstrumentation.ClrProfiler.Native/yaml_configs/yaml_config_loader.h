#ifndef INCEPTION_CLR_PROFILER_YAML_CONFIG_LOADER_H
#define INCEPTION_CLR_PROFILER_YAML_CONFIG_LOADER_H

#include <yaml-cpp/yaml.h>

#include <utility>
#include <vector>

#include "yaml_config.h"

namespace trace {

std::vector<instrumentationConfig> LoadConfigsFromEnvironment();

std::wstring GetFilterIDFromConfigCache(mdToken key);

void AddFilterIDToConfigCache(mdToken key, std::wstring value);
}  // namespace trace

#endif  // !INCEPTION_CLR_PROFILER_YAML_CONFIG_LOADER_H
