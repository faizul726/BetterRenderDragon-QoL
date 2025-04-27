#include "gui/Options.h"

#include "Core/Math/Vec4.h"
#include "Core/Resource/ResourceHelper.h"
#include "RenderDragon/Materials/MaterialResourceManager.h"
#include "Renderdragon/Materials/MaterialUniformName.h"
#include "Renderdragon/Materials/ShaderCodePlatform.h"
#include "Renderdragon/Rendering/LightingModels.h"
#include "gsl/span"

#include <iostream>
//=====================================================Vanilla2Deferred=====================================================

bool shouldForceEnableDeferredRendering() {
  return Options::vanilla2DeferredAvailable &&
         Options::vanilla2DeferredEnabled &&
         !Options::newVideoSettingsAvailable &&
         Options::deferredRenderingEnabled;
}

bool shouldForceEnableNewVideoSettings() {
  return Options::vanilla2DeferredAvailable &&
         Options::vanilla2DeferredEnabled &&
         Options::newVideoSettingsAvailable &&
         Options::forceEnableDeferredTechnicalPreview;
}

bool (*RayTracingOptions_isDeferredShadingAvailable)(void *This) = nullptr;
bool RayTracingOptions_isDeferredShadingAvailable_Hook(void *This) {
  // printf("RayTracingOptions::isDeferredShadingAvailable");
  if (shouldForceEnableNewVideoSettings()) {
    return true;
  }
  return RayTracingOptions_isDeferredShadingAvailable(This);
}

using dragon::rendering::LightingModels;

LightingModels (*RayTracingOptions_getLightingModel)(void *This) = nullptr;
LightingModels RayTracingOptions_getLightingModel_Hook(void *This) {
  LightingModels result = RayTracingOptions_getLightingModel(This);
  // printf("RayTracingOptions::getLightingModel result=%d\n", result);

  if (shouldForceEnableDeferredRendering() &&
      result == LightingModels::Vanilla) {
    result = LightingModels::Deferred;
  }
  return result;
}

void (*RayTracingOptions_setLightingModel)(
    void *This, LightingModels lightingModel) = nullptr;
void RayTracingOptions_setLightingModel_Hook(void *This,
                                             LightingModels lightingModel) {
  if (shouldForceEnableDeferredRendering() &&
      lightingModel == LightingModels::Vanilla) {
    lightingModel = LightingModels::Deferred;
  }

  RayTracingOptions_setLightingModel(This, lightingModel);
}

void *newVideoSettingsOptionPtr = nullptr;

//======================================================CustomUniforms======================================================

//=====================================================MaterialBinLoader====================================================

typedef bool (*PFN_ResourcePackManager_load)(void *This,
                                             const ResourceLocation &location,
                                             std::string &resourceStream);

//==========================================================================================================================

void initMCHooks() {}

#include "api/memory/Hook.h"
// RayTracingOptions::isRayTracingAvailable
SKY_AUTO_STATIC_HOOK(
    RayTracingOptions_isRayTracingAvailable, memory::HookPriority::Normal,
    std::initializer_list<const char *>(
        {// 1.21.50
         "40 53 48 83 EC 20 48 8B 01 48 8B D9 48 8B 40 ? FF 15 ? ? ? ? 84 "
         "C0 74 ? 48 8B 03 48 8B CB 48 8B 40 ? FF 15 ? ? ? ? 84 C0 74"}),
    bool, void *This) {
  memory::ReplaceVtable(*(void **)This, 8,
                        (void **)&RayTracingOptions_isDeferredShadingAvailable,
                        RayTracingOptions_isDeferredShadingAvailable_Hook);
  memory::ReplaceVtable(*(void **)This, 9,
                        (void **)&RayTracingOptions_getLightingModel,
                        RayTracingOptions_getLightingModel_Hook);
  memory::ReplaceVtable(*(void **)This, 10,
                        (void **)&RayTracingOptions_setLightingModel,
                        RayTracingOptions_setLightingModel_Hook);
  bool result = origin(This);
  unhook();
  return result;
}

SKY_AUTO_STATIC_HOOK(
    makeBoolOption, memory::HookPriority::Normal,
    std::initializer_list<const char *>(
        {// 1.21.50
         "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 54 41 55 41 56 "
         "41 57 48 8D 6C 24 ? 48 81 EC C0 00 00 00 4D 8B E1 4C 89 4D ? "
         "4C 89 45 ? 48 89 55 ? 48 8B F9 48 89 4D ? 48 89 4D ? 4C 8B 7D "
         "? 4C 89 7D ? 4C 8B 6D ? 45 33 F6",
         // 1.21.60
         "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 54 41 55 41 56 "
         "41 57 48 8D 6C 24 ? 48 81 EC C0 00 00 00 4C 89 4D ? 4C 89 45 ? "
         "48 89 55 ? 4C 8B E9 48 89 4D ? 48 89 4D ? 4C 8B 65 ? 4C 89 65 "
         "? 4C 8B 7D ? 4C 89 7D ? 33 FF"}),
    void **, void **a1, uintptr_t a2, uintptr_t a3, uintptr_t a4,
    const char *a5, const char *a6, uintptr_t a7) {
  void **result = origin(a1, a2, a3, a4, a5, a6, a7);
  if (!newVideoSettingsOptionPtr && a1 && a5 && a6 &&
      strcmp(a5, "options.newVideoSettings") == 0 &&
      strcmp(a6, "new_video_settings") == 0) {
    // printf("options.newVideoSettings\n");
    newVideoSettingsOptionPtr = *a1;
    // std::cout << "newVideoSettingsOptionPtr: " << newVideoSettingsOptionPtr
    //           << std::endl;
    Options::newVideoSettingsAvailable = true;
  }
  return result;
}

SKY_AUTO_STATIC_HOOK(
    Option_getBool, memory::HookPriority::Normal,
    std::initializer_list<const char *>(
        {// 1.21.50
         "48 8B 41 ? 48 8B 90 ? ? ? ? 48 85 D2 74 ? 48 8B 4A"}),
    bool, void *This) {
  if (shouldForceEnableNewVideoSettings() &&
      This == newVideoSettingsOptionPtr) {
    // printf("Option::getBool\n");
    return true;
  }
  return origin(This);
}

SKY_AUTO_STATIC_HOOK(
    Option_get, memory::HookPriority::Normal,
    std::initializer_list<const char *>(
        {// 1.21.50
         "40 53 48 83 EC 20 48 8B DA 41 8B D0 E8 ? ? ? ? 48 85 C0"}),
    void **, void *This, void **result, int a3) {
  auto res = origin(This, result, a3);
  if (shouldForceEnableNewVideoSettings() &&
      *res == newVideoSettingsOptionPtr) {
    if (newVideoSettingsOptionPtr) {
      *(bool *)((uintptr_t)newVideoSettingsOptionPtr + 0x10) = true;
      *(bool *)((uintptr_t)newVideoSettingsOptionPtr + 0x11) = true;
    } else {
      *(bool *)((uintptr_t)newVideoSettingsOptionPtr + 0x10) = false;
      *(bool *)((uintptr_t)newVideoSettingsOptionPtr + 0x11) = false;
    }
  }
  return res;
}

SKY_AUTO_STATIC_HOOK(
    Option_set_bool, memory::HookPriority::Normal,
    std::initializer_list<const char *>({// 1.21.50
                                         "40 53 48 83 EC 30 48 8B 05 ? ? ? ? "
                                         "48 33 C4 48 89 44 24 ? 48 8B D9 48 "
                                         "8B 49 ? 48 83 B9 ? ? ? ? ? 0F 85"}),
    void, void *This, bool *value) {
  if (shouldForceEnableNewVideoSettings() &&
      This == newVideoSettingsOptionPtr) {
    // printf("Option::set<bool>\n");
    *value = true;
  }
  origin(This, value);
}

SKY_AUTO_STATIC_HOOK(
    BoolOption_set, memory::HookPriority::Normal,
    std::initializer_list<const char *>(
        {// 1.21.50
         "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 30 48 8B 05 ? ? ? ? 48 33 C4 "
         "48 89 44 24 ? 48 8B D9 41 0F B6 F0"}),
    void, void *This, bool value, char a3) {
  if (shouldForceEnableNewVideoSettings() &&
      This == newVideoSettingsOptionPtr) {
    // printf("BoolOption::set\n");
    value = true;
  }
  origin(This, value, a3);
}

SKY_AUTO_STATIC_HOOK(
    isLightingModelSupportedHOOK, memory::HookPriority::Normal,
    std::initializer_list<const char *>({// 1.21.50
                                         "83 F9 01 75 ? 84 D2 75"}),
    bool, void *a1, LightingModels lightingModel, char a3) {
  if (shouldForceEnableNewVideoSettings() &&
      lightingModel == LightingModels::Deferred) {
    return true;
  }
  return origin(a1, lightingModel, a3);
}

// GeneralSettingsScreenController::_registerControllerCallbacks
SKY_AUTO_STATIC_HOOK(GeneralSettingsScreenControllerHOOK,
                     memory::HookPriority::Normal,
                     std::initializer_list<const char *>(
                         {// 1.21.50
                          "48 89 5C 24 ? 57 48 83 EC 50 48 8B D9 C7 44 24"}),
                     bool, void *a1) {
  bool result = origin(a1);
  if (shouldForceEnableNewVideoSettings()) {
    // std::cout << "graphicsModeRadioDeferredEnabledCallback " << std::endl;
    result = true;
  }
  return result;
}

void *resourcePackManager;
PFN_ResourcePackManager_load ResourcePackManager_load;
// ResourcePackManager::ResourcePackManager
SKY_AUTO_STATIC_HOOK(
    ResourcePackManagerConstructor, memory::HookPriority::Normal,
    std::initializer_list<const char *>(
        {// 1.21.50
         "4C 8B DC 53 55 56 57 41 54 41 56 41 57 48 81 EC A0 00 00 00 41 0F B6 "
         "E9",
         // 1.21.60
         "4C 8B DC 49 89 5B ? 49 89 53 ? 49 89 4B ? 55 56 57 41 56"}),
    void *, void *This, uintptr_t a2, uintptr_t a3, bool needsToInitialize) {

  void *result = origin(This, a2, a3, needsToInitialize);
  if (needsToInitialize && !resourcePackManager) {
    resourcePackManager = This;
    void **vptr = *(void ***)resourcePackManager;
    ResourcePackManager_load = (PFN_ResourcePackManager_load) * (vptr + 3);
  }
  return result;
}

// AppPlatform::readAssetFile
SKY_AUTO_STATIC_HOOK(
    readAssetFileHOOK, memory::HookPriority::Normal,
    std::initializer_list<const char *>(
        {// 1.21.50
         "48 89 5C 24 ? 55 56 57 48 8D 6C 24 ? 48 81 EC 50 01 00 00 48 8B "
         "05 ? ? ? ? 48 33 C4 48 89 45 ? 49 8B F0 48 8B FA 48 89 55 ? 0F "
         "57 C9 F3 0F 7F 4D ? 0F B6 5C 24 ? 80 E3 A1 80 CB 21 BA 28 00 00 "
         "00 65 48 8B 04 25 ? ? ? ? 48 8B 08 8B 04 0A 39 05 ? ? ? ? 0F 8F "
         "? ? ? ? 48 8B 05 ? ? ? ? C7 44 24 ? ? ? ? ? 88 5C 24 ? 4C 8D",
         // 1.21.60
         "48 89 5C 24 ? 48 89 7C 24 ? 55 48 8D 6C 24 ? 48 81 EC 60 01 00 "
         "00 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 ? 48 8B FA"}),
    std::string *, void *This, std::string *retstr, Core::Path &path) {
  std::string *result = origin(This, retstr, path);
  if (Options::materialBinLoaderEnabled && Options::redirectShaders &&
      resourcePackManager) {
    const std::string &p = path.getUtf8StdString();
    if (p.find("/data/renderer/materials/") != std::string::npos &&
        strncmp(p.c_str() + p.size() - 13, ".material.bin", 13) == 0) {
      std::string binPath =
          "renderer/materials/" + p.substr(p.find_last_of('/') + 1);
      ResourceLocation location(binPath);
      std::string out;
      // printf("ResourcePackManager::load path=%s\n", binPath.c_str());

      bool success =
          ResourcePackManager_load(resourcePackManager, location, out);
      if (success) {
        retstr->assign(out);
      }
      // printf("ResourcePackManager::load ret=%d\n", success);
    }
  }
  return result;
}