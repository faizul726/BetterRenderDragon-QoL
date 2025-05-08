
#include "Core/Math/Vec4.h"
#include "Core/Resource/ResourceHelper.h"
#include "RenderDragon/Materials/MaterialResourceManager.h"
#include "Renderdragon/Materials/MaterialUniformName.h"
#include "Renderdragon/Materials/ShaderCodePlatform.h"
#include "Renderdragon/Rendering/LightingModels.h"

#include "gui/Options.h"

#include <iostream>
//=====================================================Vanilla2Deferred=====================================================

char globalGraphicsMode = 0;
int MaterialResourceManagerOffset = 0;

using dragon::rendering::LightingModels;

bool shouldForceEnableNewVideoSettings() {
  return Options::vanilla2DeferredAvailable &&
         Options::vanilla2DeferredEnabled &&
         Options::newVideoSettingsAvailable &&
         Options::forceEnableDeferredTechnicalPreview;
}
//======================================================CustomUniforms======================================================

//=====================================================MaterialBinLoader====================================================

typedef bool (*PFN_ResourcePackManager_load)(void *This,
                                             const ResourceLocation &location,
                                             std::string &resourceStream);

//==========================================================================================================================

#include "api/memory/Hook.h"

int NEW_VIDEO_SETTINGS = 0;

SKY_AUTO_STATIC_HOOK(getGameVersionString, memory::HookPriority::Normal,
                     std::initializer_list<const char *>(
                         {// Win 1.21.60
                          "48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 54 41 56 41 57 "
                          "48 83 EC 60 48 8B F9 48 89 4C 24 ? 45 33 E4"}),
                     std::string, std::string *result) {
  auto version = origin(result);
  if (version.find("1.21.6") != std::string::npos) {
    NEW_VIDEO_SETTINGS = 723;
    MaterialResourceManagerOffset = 1008;
  } else if (version.find("1.21.7") != std::string::npos) {
    NEW_VIDEO_SETTINGS = 726;
    MaterialResourceManagerOffset = 1008;
  } else if (version.find("1.21.8") != std::string::npos) {
    NEW_VIDEO_SETTINGS = 730;
    MaterialResourceManagerOffset = 960;
  }
  return version;
}

SKY_AUTO_STATIC_HOOK(
    isDeferredShadingAvailable, memory::HookPriority::Normal,
    std::initializer_list<const char *>(
        {// Win 1.21.60
         "40 53 48 83 EC 20 48 8B 01 48 8B D9 48 8B 40 ? FF 15 ? ? ? ? 84 C0 "
         "74 ? 48 8B 03 48 8B CB 48 8B 40 ? FF 15 ? ? ? ? 84 C0 75 ? 48 8B "
         "03"}),
    bool, int64_t a1) {
  if (shouldForceEnableNewVideoSettings()) {
    return true;
  }
  return origin(a1);
}

SKY_AUTO_STATIC_HOOK(
    RPM_supportsVibrantVisuals, memory::HookPriority::Normal,
    std::initializer_list<const char *>({
        // Win 1.21.80
        "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B 81 ? ? ? ? 48 8B F1",
    }),
    bool, void *a1) {
  return true;
}

/*
SKY_AUTO_STATIC_HOOK(getDeferred, memory::HookPriority::Normal,
                     std::initializer_list<const char *>({
                         // Android 1.21.70
                         "FF 83 01 D1 FD 7B 02 A9 FD 83 00 91 F7 1B 00 F9 F6 "
                         "57 04 A9 F4 4F 05 A9 57 D0 3B D5 C1 5A 80 52",
                     }),
                     bool, void *_this) {
  if (shouldForceEnableNewVideoSettings()) {
    if (globalGraphicsMode == 2) {
      return true;
    }
  }
  return origin(_this);
}

SKY_AUTO_STATIC_HOOK(RayTracingsetGraphicsMode, memory::HookPriority::Normal,
                     std::initializer_list<const char *>({// Win 1.21.70
                                                          "88 51 ? 80 FA 01"}),
                     void, void *a1, unsigned char Graphics) {
  globalGraphicsMode = Graphics;
  origin(a1, Graphics);
}
*/
SKY_AUTO_STATIC_HOOK(RayTracingsetLightingModel, memory::HookPriority::Normal,
                     std::initializer_list<const char *>({// Win 1.21.60
                                                          "89 51 ? 85 D2 74"}),
                     void, void *a1, LightingModels lightingModel) {
  if (shouldForceEnableNewVideoSettings()) {
    if (globalGraphicsMode == 2) {
      if (lightingModel == LightingModels::Vanilla) {
        lightingModel = LightingModels::Deferred;
      }
    }
  }
  origin(a1, lightingModel);
}

SKY_AUTO_STATIC_HOOK(
    bgfxsetLightingModel, memory::HookPriority::Normal,
    std::initializer_list<const char *>({// Win 1.21.70
                                         "39 91 ? ? ? ? 89 91"}),
    char, void *a1, LightingModels lightingModel, char *a3) {
  if (shouldForceEnableNewVideoSettings()) {
    if (globalGraphicsMode == 2) {
      if (lightingModel == LightingModels::Vanilla) {
        lightingModel = LightingModels::Deferred;
      }
    }
  }
  return origin(a1, lightingModel, a3);
}

/*
SKY_AUTO_STATIC_HOOK(
    getGraphicsMode, memory::HookPriority::Normal,
    std::initializer_list<const char *>(
        {// Win 1.21.60
         "40 53 48 83 EC 20 48 8B 01 48 8B D9 48 8B 80 ? ? ? "
         "? FF 15 ? ? ? ? 84 C0 74 ? B0 02",
         // Android 1.21.60
         "? ? ? A9 ? ? ? F9 FD 03 00 91 ? ? ? F9 F3 03 00 AA ? ? ? F9 00 01 3F "
         "D6 ? ? ? 36 ? ? ? 52 ? ? ? F9 ? ? ? A8 C0 03 5F D6 ? ? ? F9"}),
    char, void *a1) {
  char result = origin(a1);
  globalGraphicsMode = result;
  return result;
}
*/

// BaseOptions::get
SKY_AUTO_STATIC_HOOK(
    Option_get, memory::HookPriority::Normal,
    std::initializer_list<const char *>(
        {// Win 1.21.50
         "48 89 5C 24 ? 48 89 74 24 ? 57 48 81 EC 80 00 00 00 33 C0"}),
    void *, void *This, int a3) {
  auto res = origin(This, a3);
  if (a3 == NEW_VIDEO_SETTINGS) {
    Options::newVideoSettingsAvailable = true;
    if (shouldForceEnableNewVideoSettings()) {
      *(bool *)((uintptr_t)res + 0x10) = true;
      *(bool *)((uintptr_t)res + 0x11) = true;
    }
  }
  return res;
}

// GeneralSettingsScreenController::_registerControllerCallbacks
SKY_AUTO_STATIC_HOOK(
    GeneralSettingsScreenControllerHOOK, memory::HookPriority::Normal,
    std::initializer_list<const char *>(
        {// Win 1.21.50
         "48 89 5C 24 ? 57 48 83 EC 50 48 8B D9 C7 44 24",
         // Win 1.21.80
         "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 60 C7 44 24 ? ? ? ? ? 48 8B "
         "39"}),
    bool, void *a1) {
  if (shouldForceEnableNewVideoSettings()) {
    return true;
  }
  return origin(a1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void *resourcePackManager = nullptr;
PFN_ResourcePackManager_load ResourcePackManager_load;
// ResourcePackManager::ResourcePackManager
SKY_AUTO_STATIC_HOOK(
    ResourcePackManagerConstructor, memory::HookPriority::Normal,
    std::initializer_list<const char *>(
        {// 1.21.50
         "4C 8B DC 53 55 56 57 41 54 41 56 41 57 48 81 EC A0 00 00 00 41 0F B6 "
         "E9",
         // 1.21.60
         "4C 8B DC 49 89 5B ? 49 89 53 ? 49 89 4B ? 55 56 57 41 56",
         // 1.21.80
         "4C 8B DC 49 89 5B ? 49 89 6B ? 49 89 53 ? 49 89 4B"

        }),
    void *, void *This, uintptr_t a2, uintptr_t a3, bool needsToInitialize) {

  void *result = origin(This, a2, a3, needsToInitialize);
  if (needsToInitialize && !resourcePackManager) {
    resourcePackManager = This;
    void **vptr = *(void ***)resourcePackManager;
    ResourcePackManager_load = (PFN_ResourcePackManager_load) * (vptr + 3);
  }
  return result;
}

#include "materialbin.h"
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
         "48 89 5C 24 ? 48 89 7C 24 ? 55 48 8D 6C 24 ? 48 81 EC 60 01 00 00 48 "
         "8B 05 ? ? ? ? 48 33 C4 48 89 45 ? 48 8B FA"}),
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
      if (success && !out.empty()) {
        bool successful_update = true;
        struct Buffer outbufdata = {0, 0};
        if (update_file(out.length(), (const uint8_t *)out.c_str(),
                        &outbufdata) != 0) {
          // printf("Updating failed!");
          successful_update = false;
          free_buf(outbufdata);
        }

        if (!successful_update) {
          retstr->assign(out);
        } else {
          retstr->assign((const char *)outbufdata.data, outbufdata.len);
          free_buf(outbufdata);
        }
      }
      // printf("ResourcePackManager::load ret=%d\n", success);
    }
  }
  return result;
}

using dragon::materials::MaterialResourceManager;

typedef void (*PFN_mce_framebuilder_BgfxFrameBuilder_discardFrame)(
    uintptr_t This, bool waitForPreviousFrame);
typedef void (*PFN_dragon_materials_CompiledMaterialManager_freeShaderBlobs)(
    uintptr_t This);
PFN_mce_framebuilder_BgfxFrameBuilder_discardFrame discardFrame = nullptr;
PFN_dragon_materials_CompiledMaterialManager_freeShaderBlobs freeShaderBlobs =
    nullptr;

bool discardFrameAndClearShaderCaches(uintptr_t bgfxFrameBuilder) {
  uintptr_t compiledMaterialManager =
      *(uintptr_t *)(*(uintptr_t *)(bgfxFrameBuilder + 40) + 16) + 776;
  uintptr_t mExtractor = *(uintptr_t *)(bgfxFrameBuilder + 32);
  MaterialResourceManager *mMaterialsManager = (MaterialResourceManager *)*(
      uintptr_t *)(mExtractor + MaterialResourceManagerOffset);
  if (discardFrame && freeShaderBlobs && mMaterialsManager) {
    discardFrame(bgfxFrameBuilder, true);
    mMaterialsManager->forceTrim();
    freeShaderBlobs(compiledMaterialManager);
    freeShaderBlobs(compiledMaterialManager);
    return true;
  }
  return false;
}
// mce::framebuilder::BgfxFrameBuilder::endFrame
SKY_AUTO_STATIC_HOOK(
    mce_framebuilder_BgfxFrameBuilder_endFrame, memory::HookPriority::Normal,
    std::initializer_list<const char *>(
        {// 1.21.60
         "48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? "
         "B8 10 29 00 00",
         // 1.21.70
         "48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? B8 00 32 00 00",
         // 1.21.80
         "48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? B8 60 33 00 00"}),
    void, uintptr_t This, uintptr_t frameBuilderContext) {
  std::cout << 11 << std::endl;
  bool clear = false;
  if (Options::reloadShadersAvailable && Options::reloadShaders) {
    Options::reloadShaders = false;
    clear = true;
  }
  if (clear && discardFrameAndClearShaderCaches(This)) {
    return;
  }
  origin(This, frameBuilderContext);
}

void initMCHooks() {

  discardFrame = (PFN_mce_framebuilder_BgfxFrameBuilder_discardFrame)
      memory::resolveIdentifier(
          {"48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 "
           "57 48 81 EC 90 00 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? "
           "? ? 44 0F B6 EA"});
  if (!discardFrame) {
    printf("mce::framebuilder::BgfxFrameBuilder::discardFrame not found\n");
  }

  freeShaderBlobs =
      (PFN_dragon_materials_CompiledMaterialManager_freeShaderBlobs)
          memory::resolveIdentifier(
              {"48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 "
               "41 57 48 83 EC 20 4C 8B E9 48 83 C1 40"});
  if (!freeShaderBlobs) {
    printf("dragon::materials::CompiledMaterialManager::freeShaderBlobs not "
           "found\n");
  }
}