#define NOB_IMPLEMENTATION
#include "nob.h"

void append_warnings(Nob_Cmd *cmd) {
	nob_cmd_append(cmd, "-Wall", "-Wextra", "-Wpedantic");
}

void append_common(Nob_Cmd *cmd) {
	nob_cmd_append(cmd, "src/common/camera.c", "src/common/chunk.c",
				   "src/common/logs.c", "src/common/player.c",
				   "src/common/utils.c", "src/common/world.c");
}

void append_output(Nob_Cmd *cmd, const char *name) {
	Nob_String_Builder output = {0};
	nob_sb_append_cstr(&output, "./build/");
	nob_sb_append_cstr(&output, name);
	nob_sb_append_null(&output);
	nob_cmd_append(cmd, "-o", nob_temp_sv_to_cstr(nob_sb_to_sv(output)));
}

void append_compiler(Nob_Cmd *cmd, char *CC, char *CFLAGS) {
	if (CC != NULL) {
		nob_cmd_append(cmd, CC);
	} else {
		nob_cmd_append(cmd, "cc");
	}

	if (CFLAGS != NULL) {
		nob_cmd_append(cmd, CFLAGS);
	}
}

void include_common(Nob_Cmd *cmd) { nob_cmd_append(cmd, "-I./include"); }

void link_math(Nob_Cmd *cmd) { nob_cmd_append(cmd, "-lm"); }

bool for_windows(char *CC) {
	return (CC != NULL && strcmp(CC, "x86_64-w64-mingw32-gcc"));
}

int build_client(void) {
	Nob_Cmd cmd = {0};

	char *CC = getenv("CC");
	char *CFLAGS = getenv("CFLAGS");

	append_compiler(&cmd, CC, CFLAGS);
	append_warnings(&cmd);
	append_common(&cmd);

	// SOURCE FILES
	nob_cmd_append(&cmd, "src/client/main.c", "src/client/mesh.c",
				   "src/client/render.c", "src/client/shader.c",
				   "src/client/window.c");

	// LINKING
	include_common(&cmd);

	DIR *cglm_dir = opendir("cglm/include");
	if (!cglm_dir) {
		nob_log(NOB_ERROR, "The cglm submodule is not initialized."
						   "Please, run the following:\n"
						   "$ git submodule init cglm\n"
						   "$ git submodule update");
		return -1;
	}
	closedir(cglm_dir);

	nob_cmd_append(&cmd, "-I./cglm/include");

	// MinGW
	if (for_windows(CC)) {
		DIR *glfw_dir = opendir("glfw");
		if (!glfw_dir) {
			nob_log(NOB_ERROR, "You should download and unpack pre-compiled "
							   "GLFW 3 as ./glfw/");
			return -1;
		}
		closedir(glfw_dir);

		nob_cmd_append(&cmd, "-lglfw3", "-lopengl32", "-lgdi32");
		link_math(&cmd);
		nob_cmd_append(&cmd, "-I./glfw/include");
		nob_cmd_append(&cmd, "-L./glfw/lib-mingw-w64/");

		append_output(&cmd, "client.exe");
	}
	// Non-MinGW
	else {
		nob_cmd_append(&cmd, "-lglfw", "-lGL");
		link_math(&cmd);

		append_output(&cmd, "client");
	}

	if (!nob_cmd_run_sync(cmd))
		return 1;

	return 0;
}

int build_server(void) {
	Nob_Cmd cmd = {0};

	char *CC = getenv("CC");
	char *CFLAGS = getenv("CFLAGS");

	append_compiler(&cmd, CC, CFLAGS);
	append_warnings(&cmd);
	append_common(&cmd);

	// SOURCE FILES
	nob_cmd_append(&cmd, "src/server/main.c");

	// LINKING
	include_common(&cmd);
	link_math(&cmd);

	if (for_windows(CC)) {
		append_output(&cmd, "server.exe");
	} else {
		append_output(&cmd, "server");
	}

	if (!nob_cmd_run_async(cmd)) {
		return 1;
	}

	return 0;
}

int main(int argc, char **argv) {
	NOB_GO_REBUILD_URSELF(argc, argv);

	if (!nob_mkdir_if_not_exists("build/"))
		return 1;

	if (build_client() != 0) {
		return -1;
	}

	if (build_server() != 0) {
		return -1;
	}

	return 0;
}
