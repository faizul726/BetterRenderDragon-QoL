#pragma once

#include "mc/_HeaderOutputPredefine.h"

#include "mc/common/options/option_types/Option.h"

class BoolOption : public Option {
public:
    void _set(bool v, bool saveOptionChange) {
        bool va = v;
        if (this->mValue != v) {
            if (mCoerceValueCallback) {
                va = mCoerceValueCallback(v);
            }
            this->mValue = va;
        }
    }

    bool               mValue;               // +16
    bool               mDefaultValue;        // +17
    Option::BoolFilter mCoerceValueCallback; // +32
};