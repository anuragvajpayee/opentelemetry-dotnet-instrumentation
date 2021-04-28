#include <vector>
#include "yaml_config.h"
#include "../clr_helpers.h"

namespace trace {

struct ValidIntegration {
  ValidIntegration() {}
  ValidIntegration(mdMethodDef methodToken, wrapper integrationWrapper,
                   WSTRING filterIDs)
      : methodDef(methodToken),
        wrapperMethod(integrationWrapper),
        filters(filterIDs) {}
  mdMethodDef methodDef;
  wrapper wrapperMethod;
  WSTRING filters;
};

std::pair<std::vector<ValidIntegration>, bool> QualifyRules(
    WCHAR*, WCHAR*, const std::vector<WSTRING>&,
    const std::vector<FunctionInfo>&, const std::vector<instrumentationConfig>&,
    const ComPtr<IMetaDataImport2>& metadata_import);
} // namespace trace
