#include "clr_helpers_yaml.h"
#include <iostream>
#include <string>
#include <regex>

namespace trace {

/*******************************/

bool StartsWith(const WSTRING& matchValue, const WSTRING& target) {
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

bool MatchesRegex(const WSTRING& matchValue, const WSTRING& target) {
  //std::wcout << "matchValue -> " << matchValue << "\n";
  //std::wcout << "target -> " << target << "\n";

  return std::regex_match(ToString(target), std::regex(ToString(matchValue)));
}

bool TestRule(const WSTRING& matchType, const std::vector<WSTRING>& matchValues,
              const WSTRING& target) {
  /*std::wcout << "Test rule for matchType - " << matchType
             << " matchValues size " << matchValues.size() << " target - "
             << target << "\n";*/
  for (auto matchValue : matchValues) {
    // std::wcout << "matchValue - " << matchValue << " matchType - " <<
    // matchType << " target - " << target << "\n";
    if (matchType == L"eq") {
      if (matchValue == target) return true;
    } else if (matchType == L"startswith") {
      if (StartsWith(matchValue, target)) return true;
    } else if (matchType == L"endswith") {
      if (EndsWith(matchValue, target)) return true;
    } else if (matchType == L"regex") {
      if (MatchesRegex(matchValue, target)) return true;
    } else if (matchType == L"isNull") {
      if (target.empty()) return true;
    } else if (matchType == L"isNotNull") {
      if (!target.empty()) return true;
    } else if (matchType == L"contains") {
      if (target.find(matchValue)) return true;
    }
  }
  return false;
}

/*******************************/

bool checkParameters(const FunctionInfo& methodInfo,
                     const methodMatchRule& rule,
                     const ComPtr<IMetaDataImport2>& metadata_import) {

  if (!rule.matchParams) return true;

  auto fInfo = new FunctionInfo(methodInfo);
  HRESULT hr = fInfo->method_signature.TryParse();
  if (FAILED(hr)) {
    delete fInfo;
    return false;
  }

  std::vector<WSTRING> args;
  for (auto i : fInfo->method_signature.GetMethodArguments())
    args.push_back(i.GetTypeTokName(metadata_import));

  if (args.size() != rule.m_paramTypes.size()) {
    delete fInfo;
    return false;
  }

  for (int i = 0; i < args.size(); i++)
    if (args[i] != rule.m_paramTypes[i]) {
      delete fInfo;
      return false;
    }

  return true;
}

bool QualifyMethod(const FunctionInfo& functionInfo,
                   const std::vector<classMethodFilter>& filters,
                   wrapper& methodWrapper, std::wstring& filterIDs,
                   const ComPtr<IMetaDataImport2>& metadata_import) {
  filterIDs = L"";
  for (auto filter : filters) { // Going over all the selected CMFs that are valid for this method's class
    bool skipThisCMF = false;
    for (auto exclude : filter.m_methodMatch.excludes) { // All method excludes in this CMF
      if (TestRule(exclude.matchType, exclude.m_matchValue,
                   functionInfo.name) &&
          checkParameters(functionInfo, exclude, metadata_import)) {
        skipThisCMF = true;
        break;
      }
    }

    if (skipThisCMF) continue; // This CMF is to be excluded for this method

    // This CMF is not to be excluded for this method. Checking include rules
    for (auto include : filter.m_methodMatch.includes) { // All method includes in this CMF
      if (TestRule(include.matchType, include.m_matchValue,
                   functionInfo.name) &&
          checkParameters(functionInfo, include, metadata_import)) {
        if (!filterIDs.empty()) filterIDs.append(L"; ");
        filterIDs.append(filter.m_id);
        methodWrapper = filter.m_wrapper;
        break;
      }
    }
  }
  return (filterIDs.empty()) ? false : true;
}

std::pair<std::vector<ValidIntegration>, bool> QualifyRules(
    wchar_t* className, wchar_t* parentClassName,
    const std::vector<std::wstring>& interfaceList,
    const std::vector<FunctionInfo>& methods,
    const std::vector<instrumentationConfig>& all_configs,
    const ComPtr<IMetaDataImport2>& metadata_import) {

  std::vector<classMethodFilter> methodMatchRules;
  std::vector<ValidIntegration> finalMatches;

  for (auto config :
       all_configs) {  // Filtering configs based on class match rules
    if (!config.disabled) {
      for (auto cmf :
           config.m_classMethodFilters) {  // All CMFs inside this config
        bool skipThisCMF = false;
        for (auto exclude :
             cmf.m_classMatch.excludes) {  // All excludes inside this CMF

          if (exclude.type == L"matchClass") {
            if (TestRule(exclude.matchType, exclude.m_matchValue,
                         std::wstring(className))) {
              skipThisCMF = true;
              break;
            }
          }

          else if (exclude.type == L"matchSuperClass") {
            if (TestRule(exclude.matchType, exclude.m_matchValue,
                         std::wstring(parentClassName))) {
              skipThisCMF = true;
              break;
            }
          }

          else if (exclude.type == L"matchInterface") {
            bool foundInterfaceExclude = true;
            for (auto interf : interfaceList) {
              if (TestRule(exclude.matchType, exclude.m_matchValue, interf)) {
                foundInterfaceExclude = true;
                skipThisCMF = true;
                break;
              }
            }
            if (foundInterfaceExclude) break;
          }

          else
            continue;
        }

        if (skipThisCMF) continue; // This CMF has to be excluded for this class

        // This CMF is not to be excluded for this class. Checking include rules
        for (auto include :
             cmf.m_classMatch.includes) {  // All includes inside this CMF

          if (include.type == L"matchClass") {
            if (TestRule(include.matchType, include.m_matchValue,
                         std::wstring(className))) {
              methodMatchRules.push_back(cmf);
              break;
            }
          }

          else if (include.type == L"matchSuperClass") {
            //std::wcout << "Yes found superclass for " << className << " "
            //          << parentClassName << "\n";
            if (TestRule(include.matchType, include.m_matchValue,
                         std::wstring(parentClassName))) {
              methodMatchRules.push_back(cmf);
              break;
            }
          }

          else if (include.type == L"matchInterface") {
            bool found = false;
            for (auto interf : interfaceList) {
              if (TestRule(include.matchType, include.m_matchValue, interf)) {
                methodMatchRules.push_back(cmf);
                found = true;
                break;
              }
            }
            if (found) break;
          }

          else
            continue;
        }
      }
    }
  }
  
  if (methodMatchRules.size() == 0)
    return std::make_pair<std::vector<ValidIntegration>, bool>({}, false);
  
  for (auto method : methods) {
    std::wstring filterIDs = L"";
    wrapper methodWrapper;
    if (!QualifyMethod(method, methodMatchRules, methodWrapper, filterIDs,
                       metadata_import))
      continue;
    finalMatches.push_back({method.id, methodWrapper, filterIDs});
    /*std::wcout << "For " << method.name << " filter ID's - " << filterIDs
               << " Wrapper - " << methodWrapper.assembly << "\n";*/
  }

  return (finalMatches.empty())
             ? std::make_pair<std::vector<ValidIntegration>, bool>({}, false)
             : std::make_pair(finalMatches, true);
}
} //namespace trace