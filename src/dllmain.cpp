#include <windows.h>
#include <wrl.h>

#include "MCPatches.h"
#include "gui/Options.h"
#include "imgui/ImGuiHooks.h"

#include <cstdio>
#include <fcntl.h>
#include <io.h>

void openConsole() {
  if (AllocConsole()) {
    SetConsoleTitleA("PreLoader Debug Console");

    FILE *stream;
    freopen_s(&stream, "CONOUT$", "w", stdout);
    freopen_s(&stream, "CONOUT$", "w", stderr);
    freopen_s(&stream, "CONIN$", "r", stdin);

    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);
  }
}
void initMCHooks();

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call,
                      LPVOID lpReserved) {
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH: {
    openConsole();

    Options::init();
    Options::load();

    initMCHooks();
    initMCPatches();
    initImGuiHooks();

    break;
  }
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
    break;
  case DLL_PROCESS_DETACH:
    Windows::Foundation::Uninitialize();
    break;
  }
  return TRUE;
}
