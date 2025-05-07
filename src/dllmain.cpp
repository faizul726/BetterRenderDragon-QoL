#include <windows.h>
#include <wrl.h>

#include "MCPatches.h"

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

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call,
                      LPVOID lpReserved) {
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH: {
    openConsole();
    if (FAILED(Windows::Foundation::Initialize(RO_INIT_MULTITHREADED))) {
      printf("Windows::Foundation::Initialize failed\n");
      return TRUE;
    }

    initMCPatches();
    DisableThreadLibraryCalls(hModule);
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
