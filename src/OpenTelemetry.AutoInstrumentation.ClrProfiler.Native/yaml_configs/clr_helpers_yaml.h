#include <vector>
#include "yaml_config.h";
#include "../clr_helpers.h"

namespace trace {

struct ValidIntegration {
  ValidIntegration() {}
  ValidIntegration(mdMethodDef methodToken, wrapper integrationWrapper,
                   std::wstring filterIDs)
      : methodDef(methodToken),
        wrapperMethod(integrationWrapper),
        filters(filterIDs) {}
  mdMethodDef methodDef;
  wrapper wrapperMethod;
  std::wstring filters;
};

bool TestRule(const WSTRING&, const std::vector<WSTRING>&, const WSTRING&);

std::pair<std::vector<ValidIntegration>, bool> QualifyRules(
    wchar_t*, wchar_t*, const std::vector<std::wstring>&,
    const std::vector<FunctionInfo>&, const std::vector<instrumentationConfig>&,
    const ComPtr<IMetaDataImport2>& metadata_import);
} // namespace trace