#ifdef _WIN32
#include <windows.h>
#endif

#define NOB_IMPLEMENTATION
#include "nob.h"

#define WARNINGS "-std=c99", "-Wall", "-Wextra"

const char* COMMON_FILES[] = {"chunk", "logs", "player", "utils", "world"};
const size_t COMMON_N = sizeof(COMMON_FILES) / sizeof(char*);

const char* CLIENT_FILES[] = {"main", "camera", "mesh", "render", "shader", "window"};
const size_t CLIENT_N = sizeof(CLIENT_FILES) / sizeof(char*);

bool for_windows(char *CC) {
#ifdef _WIN32
  return 1;
#else
  return strcmp(CC, "x86_64-w64-mingw32-gcc") == 0; // You can compile an executable for Windows on
                                                    // a non-Windows system if you use MinGW
#endif
}

bool for_macos(void) {
#ifdef __APPLE__
  return 1;
#else
  return 0;
#endif
}

bool build_common(Nob_Cmd *cmd, char *cc) {
  for (size_t i = 0; i < COMMON_N; i++) {
    nob_cmd_append(cmd, cc, WARNINGS);
    nob_cmd_append(cmd, nob_temp_sprintf("src/common/%s.c", COMMON_FILES[i]));
    nob_cmd_append(cmd, "-I./include/", "-I./cglm/include/");
    nob_cmd_append(cmd, "-c", "-o", nob_temp_sprintf("./build/common/%s.o", COMMON_FILES[i]));
    if (!nob_cmd_run_sync_and_reset(cmd)) return 0;
    nob_temp_reset();
  }
  return 1;
}

bool build_rgfw(Nob_Cmd *cmd, char *cc) {
  nob_cmd_append(cmd, cc, "-c", "-x", "c", "-DRGFW_IMPLEMENTATION", "./RGFW/RGFW.h", "-o", "./build/rgfw.o");
  if (!nob_cmd_run_sync_and_reset(cmd)) return 0;
  return 1;
}

bool build_client(Nob_Cmd *cmd, char *cc) {
  // COMPILE
  for (size_t i = 0; i < CLIENT_N; i++) {
    nob_cmd_append(cmd, cc, WARNINGS);
    nob_cmd_append(cmd, nob_temp_sprintf("src/client/%s.c", CLIENT_FILES[i]));
    nob_cmd_append(cmd, "-I./include/", "-I./cglm/include/", "-I./RGFW/");
    nob_cmd_append(cmd, "-c", "-o", nob_temp_sprintf("./build/client/%s.o", CLIENT_FILES[i]));
    if (!nob_cmd_run_sync_and_reset(cmd)) return 0;
    nob_temp_reset();
  }

  // LINK
  nob_cmd_append(cmd, cc);
  for (size_t i = 0; i < CLIENT_N; i++) {
    nob_cmd_append(cmd, nob_temp_sprintf("./build/client/%s.o", CLIENT_FILES[i]));
  }
  for (size_t i = 0; i < COMMON_N; i++) {
    nob_cmd_append(cmd, nob_temp_sprintf("./build/common/%s.o", COMMON_FILES[i]));
  }
  nob_cmd_append(cmd, "./build/rgfw.o");
  nob_cmd_append(cmd, "-lm");

  // Windows
  if (for_windows(cc)) {
    nob_cmd_append(cmd, "-lopengl32", "-lshell32", "-lgdi32", "-lwinmm");
    nob_cmd_append(cmd, "-o", "./build/voxels.exe");
  }
  // MacOS
  else if (for_macos()) {
    nob_cmd_append(cmd, "-framework", "Foundation", "-framework", "AppKit",
                   "-framework", "OpenGL", "-framework", "CoreVideo");
    nob_cmd_append(cmd, "-o", "./build/voxels");
  }
  // Linux
  else {
    nob_cmd_append(cmd, "-lX11", "-lGL", "-lXrandr");
    nob_cmd_append(cmd, "-o", "./build/voxels");
  }

  if (!nob_cmd_run_sync_and_reset(cmd)) {
    nob_temp_reset();
    return 0;
  }

  nob_temp_reset();
  return 1;
}

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);

  Nob_Cmd cmd = {0};
  
  char *cc = getenv("CC");
  if (cc == NULL) cc = "cc";

  if (!nob_mkdir_if_not_exists("build/")) return 1;
  if (!nob_mkdir_if_not_exists("build/client")) return 1;
  if (!nob_mkdir_if_not_exists("build/common")) return 1;

  if (!build_common(&cmd, cc)) return 1;
  if (!build_rgfw(&cmd, cc)) return 1;
  if (!build_client(&cmd, cc)) return 1;

  return 0;
}
