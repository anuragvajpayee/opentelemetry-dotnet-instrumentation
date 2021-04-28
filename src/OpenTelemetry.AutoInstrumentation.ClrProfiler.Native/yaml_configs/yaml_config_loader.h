#ifndef INCEPTION_CLR_PROFILER_YAML_CONFIG_LOADER_H
#define INCEPTION_CLR_PROFILER_YAML_CONFIG_LOADER_H

#include <yaml-cpp/yaml.h>

#include <utility>
#include <vector>

#include "yaml_config.h"

namespace trace {

std::vector<instrumentationConfig> LoadConfigsFromEnvironment();

WSTRING GetFilterIDFromConfigCache(mdToken key);

void AddFilterIDToConfigCache(mdToken key, WSTRING value);
}  // namespace trace

#endif  // !INCEPTION_CLR_PROFILER_YAML_CONFIG_LOADER_H
