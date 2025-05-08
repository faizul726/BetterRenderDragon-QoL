#include "api/memory/Memory.h"

#include <Windows.h>
namespace memory {
void ReplaceVtable(void *_vptr, size_t index, void **outOldFunc,
                   void *newFunc) {
  void **vptr = (void **)_vptr;
  void *oldFunc = vptr[index];
  if (oldFunc == newFunc) {
    return;
  }
  if (outOldFunc != nullptr) {
    *outOldFunc = oldFunc;
  }

  DWORD oldProtect, tmp;
  VirtualProtect(vptr + index, sizeof(void *), PAGE_READWRITE, &oldProtect);
  vptr[index] = newFunc;
  VirtualProtect(vptr + index, sizeof(void *), oldProtect, &tmp);
}
} // namespace memory