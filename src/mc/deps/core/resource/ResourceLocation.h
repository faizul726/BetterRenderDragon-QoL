#pragma once

#include "mc/_HeaderOutputPredefine.h"

#include "mc/deps/core/file/Path.h"
#include "mc/deps/core/resource/ResourceFileSystem.h"

class ResourceLocation {
public:
  ResourceLocation(const Core::Path &path) : mPath(path) {
    mFileSystem = ResourceFileSystem::UserPackage;
    _computeHashes();
  }

  ResourceLocation(const Core::Path &path, ResourceFileSystem fileSystem)
      : mPath(path) {
    mFileSystem = fileSystem;
    _computeHashes();
  }

  ResourceLocation() {};

  ResourceFileSystem mFileSystem;
  Core::PathBuffer<::std::string> mPath;
  uint64 mPathHash;
  uint64 mFullHash;

private:
  void _computeHashes() {
    uint64_t pathHash;
    if (!mPath.empty()) {
      pathHash = 0xCBF29CE484222325LL;
      for (auto c : this->mPath.mContainer) {
        pathHash = c ^ (0x100000001B3LL * pathHash);
      }
    } else {
      pathHash = 0;
    }
    this->mPathHash = pathHash;
    this->mFullHash =
        pathHash ^ std::hash<ResourceFileSystem>{}(this->mFileSystem);
  }
};