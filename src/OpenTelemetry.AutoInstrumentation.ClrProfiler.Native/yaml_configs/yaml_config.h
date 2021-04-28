#ifndef INCEPTION_CLR_PROFILER_YAML_CONFIG_H
#define INCEPTION_CLR_PROFILER_YAML_CONFIG_H

#include <stdexcept>
#include <vector>

#include "../string.h"

namespace trace {

struct classMatchRule {
  WSTRING type;
  WSTRING matchType;
  std::vector<WSTRING> m_matchValue;

  classMatchRule() {}

  classMatchRule(const WSTRING& type, const WSTRING& matchType,
                 const std::vector<std::string>& matchValue)
      : type(type), matchType(matchType) {
    for (auto i : matchValue) {
      m_matchValue.push_back(ToWSTRING(i));
    }
  }

  inline bool operator==(const classMatchRule& other) const {
    return type == other.type && m_matchValue == other.m_matchValue &&
           matchType == other.matchType;
  }
};

struct methodMatchRule {
  WSTRING type;
  WSTRING matchType;
  std::vector<WSTRING> m_matchValue;
  bool matchParams;
  std::vector<WSTRING> m_paramTypes;

  methodMatchRule() : type(WStr("")), matchType(WStr("")), matchParams(false) {}

  methodMatchRule(const WSTRING& type, const WSTRING& matchType,
                  const std::vector<std::string>& matchValue,
                  const bool& matchParams,
                  const std::vector<std::string>& paramTypes)
      : type(type), matchType(matchType), matchParams(matchParams) {
    for (auto i : matchValue) m_matchValue.push_back(ToWSTRING(i));
    for (auto i : paramTypes) m_paramTypes.push_back(ToWSTRING(i));
  }

  inline bool operator==(const methodMatchRule& other) const {
    return type == other.type && matchType == other.matchType &&
           m_matchValue == other.m_matchValue &&
           matchParams == other.matchParams &&
           m_paramTypes == other.m_paramTypes;
  }
};

struct wrapper {
  WSTRING assembly;
  WSTRING type;
  WSTRING method;
  WSTRING signature;
  WSTRING action;

  wrapper()
      : assembly(WStr("DEFAULT")), type(WStr("DEFAULT")), method(WStr("DEFAULT")), signature(WStr("DEFAULT")), action(WStr("DEFAULT")) {}

  wrapper(const WSTRING& assembly, const WSTRING& type, const WSTRING& method,
          const WSTRING& signature, const WSTRING& action)
      : assembly(assembly),
        type(type),
        method(method),
        signature(signature),
        action(action) {}

  inline bool operator==(const wrapper& other) {
    return assembly == other.assembly && type == other.type &&
           method == other.method && signature == other.signature &&
           action == other.action;
  }
};

struct classMatch {
  std::vector<classMatchRule> includes;
  std::vector<classMatchRule> excludes;

  classMatch() : includes({}), excludes({}) {}
  classMatch(std::vector<classMatchRule> includes,
             std::vector<classMatchRule> excludes)
      : includes(includes), excludes(excludes) {}
  classMatch(const classMatch& other)
      : includes(other.includes), excludes(other.excludes) {}

  void addInclude(const classMatchRule& add) { includes.push_back(add); }

  void addExclude(const classMatchRule& add) { excludes.push_back(add); }

  inline bool operator==(const classMatch& other) const {
    return includes == other.includes && excludes == other.excludes;
  }
};

struct methodMatch {
  std::vector<methodMatchRule> includes;
  std::vector<methodMatchRule> excludes;

  methodMatch() : includes({}), excludes({}) {}
  methodMatch(std::vector<methodMatchRule> includes,
              std::vector<methodMatchRule> excludes)
      : includes(includes), excludes(excludes) {}

  void addInclude(const methodMatchRule& add) { includes.push_back(add); }

  void addExclude(const methodMatchRule& add) { excludes.push_back(add); }

  inline bool operator==(const methodMatch& other) const {
    return includes == other.includes && excludes == other.excludes;
  }
};

struct classMethodFilter {
  WSTRING m_name;
  WSTRING m_id;
  classMatch m_classMatch;
  methodMatch m_methodMatch;
  wrapper m_wrapper;

  classMethodFilter() {}

  classMethodFilter(const WSTRING& name, const WSTRING& id,
                    const classMatch& classMatch,
                    const methodMatch& methodMatch, const wrapper& wrapper)
      : m_name(name),
        m_id(id),
        m_classMatch(classMatch),
        m_methodMatch(methodMatch),
        m_wrapper(wrapper) {}

  classMethodFilter(const WSTRING& name, const WSTRING& id,
                    const wrapper& wrapper)
      : m_name(name),
        m_id(id),
        m_classMatch(),
        m_methodMatch(),
        m_wrapper(wrapper) {}

  classMethodFilter(const WSTRING& name, const WSTRING& id,
                    const std::vector<classMatchRule> classMatchIncludes,
                    const std::vector<classMatchRule> classMatchExcludes,
                    const std::vector<methodMatchRule> methodMatchIncludes,
                    const std::vector<methodMatchRule> methodMatchExcludes,
                    const wrapper& wrapper)
      : m_name(name),
        m_id(id),
        m_classMatch(classMatchIncludes, classMatchExcludes),
        m_methodMatch(methodMatchIncludes, methodMatchIncludes),
        m_wrapper(wrapper) {}
};

struct instrumentationConfig {
  WSTRING name;
  WSTRING type;
  bool disabled;
  int priority;
  std::vector<classMethodFilter> m_classMethodFilters;

  instrumentationConfig(
      const WSTRING& name, const WSTRING& type, const bool& disabled,
      const int& priority,
      const std::vector<classMethodFilter>& classMethodFilters)
      : name(name),
        type(type),
        disabled(disabled),
        priority(priority),
        m_classMethodFilters(classMethodFilters) {}
};

}  // namespace trace

#endif  // !INCEPTION_CLR_PROFILER_YAML_CONFIG_H
