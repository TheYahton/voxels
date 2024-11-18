#define NOB_IMPLEMENTATION
#include "nob.h"

#include <stdlib.h>

// -lglfw3 -lglew32 -lglu32 -lopengl32 -lgdi32 -lm
// -I./include -I./glew-2.1.0/include/ -I./glfw-3.4.bin.WIN64/include -I./cglm/
// -L./glfw-3.4.bin.WIN64/lib-mingw-w64/ -L./glew-2.1.0/lib/Release/x64/

int main(int argc, char **argv) {
	NOB_GO_REBUILD_URSELF(argc, argv);

	char *CC = getenv("CC");
	char *CFLAGS = getenv("CFLAGS");

	Nob_Cmd cmd = {0};

	if (CC != NULL) {
		nob_cmd_append(&cmd, CC);
	} else {
		nob_cmd_append(&cmd, "cc");
	}

	if (CFLAGS != NULL) {
		nob_cmd_append(&cmd, CFLAGS);
	}

	nob_cmd_append(&cmd, "-std=c2x", "-Wall", "-Wextra", "-Wpedantic");
	nob_cmd_append(&cmd, "src/main.c", "src/camera.c", "src/chunk.c",
				   "src/logs.c", "src/mesh.c", "src/player.c", "src/render.c",
				   "src/shader.c", "src/utils.c", "src/window.c",
				   "src/world.c");

	// MinGW
	if (CC != NULL && strcmp(CC, "x86_64-w64-mingw32-gcc") == 0) {
		DIR *glfw_dir = opendir("glfw");
		if (!glfw_dir) {
			nob_log(NOB_ERROR, "You should download and unpack pre-compiled "
							   "GLFW 3 as ./glfw/");
			return -1;
		}
		closedir(glfw_dir);

		DIR *glew_dir = opendir("glew");
		if (!glew_dir) {
			nob_log(
				NOB_ERROR,
				"You should download and unpack pre-compiled GLEW as ./glew/");
			return -1;
		}
		closedir(glew_dir);

		DIR *cglm_dir = opendir("cglm");
		if (!cglm_dir) {
			Nob_Cmd download_cglm = {0};
			nob_cmd_append(&download_cglm, "git", "clone",
						   "https://github.com/recp/cglm.git");
			nob_cmd_run_sync(download_cglm);
		}
		closedir(cglm_dir);

		nob_cmd_append(&cmd, "-lglfw3", "-lglew32", "-lglu32", "-lopengl32",
					   "-lgdi32", "-lm");
		nob_cmd_append(&cmd, "-I./include", "-I./glew/include/",
					   "-I./glfw/include", "-I./cglm/include");
		nob_cmd_append(&cmd, "-L./glfw/lib-mingw-w64/",
					   "-L./glew/lib/Release/x64/");

		nob_cmd_append(&cmd, "-o", "./build/voxels.exe");
	}
	// Non-MinGW
	else {
		nob_cmd_append(&cmd, "-lglfw", "-lGL", "-lm", "-I./include");

		nob_cmd_append(&cmd, "-o", "./build/voxels");
	}

	if (!nob_mkdir_if_not_exists("build"))
		return 1;

	if (!nob_cmd_run_sync(cmd))
		return 1;

	return 0;
}
