#pragma once

#include <memory>
#include <mutex>
#include <unordered_map>

namespace dragon::materials {
struct MaterialLocation {
  uint64_t mHash;
};
} // namespace dragon::materials

namespace std {
template <> class hash<dragon::materials::MaterialLocation> {
public:
  std::size_t operator()(
      const dragon::materials::MaterialLocation &location) const noexcept {
    return location.mHash;
  }
};
}; // namespace std

#include <unordered_set>

namespace dragon::materials {
struct MaterialResource {
  std::shared_ptr<void> mMaterial;
  bool mUsed;
};
class ResolvedMaterialResource {
  std::unique_ptr<void> materialPtr;
  std::shared_ptr<void> mCompiledMaterial;
};
struct MaterialResourceManager {
  std::unordered_map<MaterialLocation, MaterialResource> mCache;
  std::unordered_set<MaterialLocation> mMaterialsToReload;
  std::vector<std::shared_ptr<void>> mMaterialsToTrim;
  std::mutex mMutex;

  void forceTrim() {
    std::lock_guard<std::mutex> lock(mMutex);
    for (auto it = mCache.begin(); it != mCache.end();) {
      MaterialResource &res = it->second;
      res.mUsed = false;
      it = mCache.erase(it);
    }
    for (auto &ptr : mMaterialsToTrim) {
      ptr.reset();
    }
    mMaterialsToTrim.clear();
  }
};
} // namespace dragon::materials
