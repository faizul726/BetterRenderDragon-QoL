#include "ImageUtils.h"
#include "api/memory/Hook.h"
#include "api/memory/win/Memory.h"
bool mce::ImageUtils::loadImageFromFile(mce::Image &a1, const Core::Path &a2) {
  return CC_CALL("loadImageFromFile",
                 "48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? "
                 "? ? ? 48 81 EC 50 02 00 00 48 8B 05 ? ? ? ? 48 "
                 "33 C4 48 89 85 ? ? ? ? 48 8B FA",
                 bool, mce::Image &, const Core::Path &)(a1, a2);
}