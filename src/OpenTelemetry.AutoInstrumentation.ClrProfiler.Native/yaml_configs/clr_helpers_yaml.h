#include <vector>
#include "yaml_config.h";
#include "../clr_helpers.h"

namespace trace {

std::vector<classMethodFilter> FilterByClass(
    const std::vector<instrumentationConfig>& all_configs,
    const AssemblyInfo assembly);

std::vector<classMethodFilter> FilterByMethod(
    const std::vector<classMethodFilter>& filted_cmf,
    const WSTRING& methodName);

bool TestRule(const WSTRING&, const WSTRING&, const WSTRING&);
bool CheckForOverload(const classMethodFilter& cmf,
                      const std::vector<WSTRING>& MethodArgs,
                      WSTRING functionName);

bool TestParameters(const std::vector<WSTRING>& MethodArgs, methodMatchRule m);

bool StartsWith(const WSTRING&, const WSTRING&);
bool EndsWith(const WSTRING&, const WSTRING&);
bool MatchesRegex(const WSTRING&, const WSTRING&);
} // namespace trace