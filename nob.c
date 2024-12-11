#ifdef _WIN32
#include <windows.h>
#endif

#define NOB_IMPLEMENTATION
#include "nob.h"


void append_warnings(Nob_Cmd *cmd) {
	nob_cmd_append(cmd, "-Wall", "-Wextra", "-Werror");
}

void append_common(Nob_Cmd *cmd) {
	nob_cmd_append(cmd, "src/common/chunk.c",
				   "src/common/logs.c", "src/common/player.c",
				   "src/common/utils.c", "src/common/world.c");
}

void append_output(Nob_Cmd *cmd, const char *name) {
	Nob_String_Builder output = {0};
	nob_sb_append_cstr(&output, "./build/");
	nob_sb_append_cstr(&output, name);
	nob_sb_append_null(&output);
	nob_cmd_append(cmd, "-o", nob_temp_sv_to_cstr(nob_sb_to_sv(output)));
	nob_sb_free(output);
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
	#ifdef _WIN32
	return 1;
	#else
	return (CC != NULL && strcmp(CC, "x86_64-w64-mingw32-gcc") == 0);
	#endif
}

int build_client(void) {
	Nob_Cmd cmd = {0};

	char *CC = getenv("CC");
	char *CFLAGS = getenv("CFLAGS");

	append_compiler(&cmd, CC, CFLAGS);
	append_warnings(&cmd);
	append_common(&cmd);

	// SOURCE FILES
	nob_cmd_append(&cmd, "src/client/main.c", "src/client/camera.c", "src/client/mesh.c",
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

	DIR *rgfw_dir = opendir("cglm/include");
	if (!rgfw_dir) {
		nob_log(NOB_ERROR, "The RGFW submodule is not initialized."
						   "Please, run the following:\n"
						   "$ git submodule init RGFW\n"
						   "$ git submodule update");
		return -1;
	}
	closedir(rgfw_dir);

	nob_cmd_append(&cmd, "-I./cglm/include");
	nob_cmd_append(&cmd, "-I./RGFW/");

	// MinGW
	if (for_windows(CC)) {
		nob_cmd_append(&cmd, "-lopengl32", "-lshell32", "-lgdi32", "-lwinmm");
		link_math(&cmd);

		append_output(&cmd, "client.exe");
	}
	// Non-MinGW
	else {
		nob_cmd_append(&cmd, "-lX11", "-lGL", "-lXrandr");
		link_math(&cmd);

		append_output(&cmd, "client");
	}

	if (!nob_cmd_run_sync(cmd))
		return 1;

	nob_cmd_free(cmd);

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

	nob_cmd_free(cmd);

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
