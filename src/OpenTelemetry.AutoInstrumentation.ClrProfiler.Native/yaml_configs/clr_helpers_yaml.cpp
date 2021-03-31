#include "clr_helpers_yaml.h"
#include <iostream>
#include <string>
#include <regex>

namespace trace {

std::vector<classMethodFilter> FilterByClass(
        const std::vector<instrumentationConfig>& all_configs,
        const AssemblyInfo assembly) {
  std::vector<classMethodFilter> enabled;

  for (auto i : all_configs) {
    if (!i.disabled) {
      for (auto j : i.m_classMethodFilters) {
        bool check = false;
        for (auto k : j.m_classMatch.includes) {
          check |= TestRule(k.matchType, k.m_matchValue[0], assembly.name);
        }
        if(check) enabled.push_back(j);
      }
    }
  }

  return enabled;
}

std::vector<classMethodFilter> FilterByMethod(
    const std::vector<classMethodFilter>& filtered_cmf,
    const WSTRING& methodName) {
     
  std::vector<classMethodFilter> enabled;
  for (auto i : filtered_cmf) {
    bool check = false;
    for (auto j : i.m_methodMatch.includes) {
      check = TestRule(j.matchType, j.m_matchValue[0], methodName);
      if (check) continue;
    }
    if (check) enabled.push_back(i);
  }
  return enabled;
}

bool CheckForOverload(const classMethodFilter& cmf, const std::vector<WSTRING>& MethodArgs, WSTRING functionName) {
  bool toWrap = true;
  for (auto m : cmf.m_methodMatch.includes) {
    //toWrap = TestRule(m.matchType, m.m_matchValue[0], functionName);
    if (m.matchParams) toWrap = TestParameters(MethodArgs, m);
    if (toWrap) return true;
  }
  return false;
}

bool TestParameters(const std::vector<WSTRING>& MethodArgs, methodMatchRule m) {
  if (m.m_paramTypes.size() != MethodArgs.size()) return false;
  for (int i = 0; i < MethodArgs.size(); i++) {
    if (m.m_paramTypes[i] != MethodArgs[i]) return false;
  }
  return true;
}

bool TestRule(const WSTRING& matchType,
                             const WSTRING& matchValue, const WSTRING& target) {
  if (matchType == L"eq")
    return (matchValue == target);
  else if (matchType == L"startswith")
    return StartsWith(matchValue, target);
  else if (matchType == L"endswith")
    return EndsWith(matchValue, target);
  else if (matchType == L"regex")
    return MatchesRegex(matchValue, target);
  else {
    return false;
  }
}

bool StartsWith(const WSTRING& matchValue,
                               const WSTRING& target) {
  //std::wcout << "matchValue - " << matchValue << "\n";
  //std::wcout << "target - " << target << "\n";

  return (target.rfind(matchValue, 0) == 0) ? 1 : 0;
}

bool EndsWith(const WSTRING& matchValue, const WSTRING& target) {
  //std::wcout << "matchValue = " << matchValue << "\n";
  //std::wcout << "target = " << target << "\n";

  if (matchValue.size() > target.size()) return false;

  return (target.compare(target.size() - matchValue.size(), matchValue.size(), matchValue) == 0);
}

bool MatchesRegex(const WSTRING& matchValue,
                                 const WSTRING& target) {
  //std::wcout << "matchValue -> " << matchValue << "\n";
  //std::wcout << "target -> " << target << "\n";

  return std::regex_match(ToString(target), std::regex(ToString(matchValue)));
}

} //namespace trace