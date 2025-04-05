#include "memory.h"
#include <sstream>
#include <string>

namespace memory {
void WriteEx(uintptr_t fovAddr, const std::vector<uint16_t>& bytes, const std::string& patchName);
void WriteEx(memory::FuncPtr fovAddr, const std::vector<uint16_t>& bytes, const std::string& patchName);
void WriteEx(uintptr_t fovAddr, const std::string& byteStr, const std::string& patchName);
void WriteEx(memory::FuncPtr fovAddr, const std::string& byteStr, const std::string& patchName);

std::vector<uint8_t> ReadEx(uintptr_t fovAddr, size_t count);
std::vector<uint8_t> ReadEx(memory::FuncPtr fovAddr, size_t count);

void RevertPatch(const std::string& patchName);
void RevertAllPatches();
} // namespace memory