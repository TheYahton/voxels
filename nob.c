#define NOB_IMPLEMENTATION
#include "nob.h"

void append_warnings(Nob_Cmd *cmd) {
	nob_cmd_append(cmd, "-Wall", "-Wextra", "-Wpedantic");

}

void append_common(Nob_Cmd *cmd) {
	nob_cmd_append(cmd, "src/common/camera.c", "src/common/chunk.c", "src/common/logs.c", "src/common/player.c", "src/common/utils.c", "src/common/world.c");
}

int build_client(void) {
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

	append_warnings(&cmd);

	// SOURCE FILES
	append_common(&cmd);
	nob_cmd_append(&cmd, "src/client/main.c", "src/client/mesh.c", "src/client/render.c", "src/client/shader.c", "src/client/window.c");

	// LINKING
	DIR *cglm_dir = opendir("cglm/include");
	if (!cglm_dir) {
		nob_log(NOB_ERROR,
		        "The cglm submodule is not initialized."
		        "Please, run the following:\n"
		        "$ git submodule init cglm\n"
		        "$ git submodule update");
		return -1;
	}
	closedir(cglm_dir);

	nob_cmd_append(&cmd, "-I./include", "-I./cglm/include");

	// MinGW
	if (CC != NULL && strcmp(CC, "x86_64-w64-mingw32-gcc") == 0) {
		DIR *glfw_dir = opendir("glfw");
		if (!glfw_dir) {
			nob_log(NOB_ERROR, "You should download and unpack pre-compiled "
							   "GLFW 3 as ./glfw/");
			return -1;
		}
		closedir(glfw_dir);

		nob_cmd_append(&cmd, "-lglfw3", "-lopengl32", "-lgdi32", "-lm");
		nob_cmd_append(&cmd, "-I./glfw/include");
		nob_cmd_append(&cmd, "-L./glfw/lib-mingw-w64/");

		nob_cmd_append(&cmd, "-o", "./build/client.exe");
	}
	// Non-MinGW
	else {
		nob_cmd_append(&cmd, "-lglfw", "-lGL", "-lm");

		nob_cmd_append(&cmd, "-o", "./build/client");
	}

	if (!nob_cmd_run_sync(cmd))
		return 1;

	return 0;
}

int main(int argc, char **argv) {
	NOB_GO_REBUILD_URSELF(argc, argv);

	if (!nob_mkdir_if_not_exists("build/"))
		return 1;

	if (build_client() != 0) {
		return -1;
	}

	return 0;
}
