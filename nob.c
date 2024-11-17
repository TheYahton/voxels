#define NOB_IMPLEMENTATION
#include "nob.h"

#include <stdlib.h>

int main(int argc, char **argv) {
	NOB_GO_REBUILD_URSELF(argc, argv);

	char *CC = getenv("CC");
	char *CFLAGS = getenv("CFLAGS");

	Nob_Cmd cmd = {0};
	if (CC != NULL)
		nob_cmd_append(&cmd, CC);
	else
		nob_cmd_append(&cmd, "cc");
	if (CFLAGS != NULL)
		nob_cmd_append(&cmd, CFLAGS);
	nob_cmd_append(&cmd, "-std=c2x", "-Wall", "-Wextra", "-Wpedantic", "-o",
				   "./build/voxels.out");
	nob_cmd_append(&cmd, "src/main.c", "src/camera.c", "src/chunk.c",
				   "src/logs.c", "src/mesh.c", "src/player.c", "src/render.c",
				   "src/shader.c", "src/utils.c", "src/window.c",
				   "src/world.c");
	nob_cmd_append(&cmd, "-lGLEW", "-lglfw", "-lGL", "-lm", "-I./include");
	if (!nob_cmd_run_sync(cmd))
		return 1;
	return 0;
}
