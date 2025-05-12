#pragma once

#include "mc/_HeaderOutputPredefine.h"

#include "mc/deps/core/file/Path.h"
#include "mc/deps/core/image/Image.h"

namespace mce {
namespace ImageUtils {

bool loadImageFromFile(mce::Image &a1, const Core::Path &a2);
} // namespace ImageUtils
} // namespace mce