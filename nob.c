#ifdef _WIN32
#include <windows.h>
#endif

#define NOB_IMPLEMENTATION
#include "nob.h"


void append_warnings(Nob_Cmd *cmd) {
	nob_cmd_append(cmd, "-std=c99", "-Wall", "-Wextra");
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
	return (CC != NULL && strcmp(CC, "x86_64-w64-mingw32-gcc") == 0); // You can compile an executable for Windows on a non-Windows system if you use MinGW
	#endif
}

bool for_macos(void) {
	#ifdef __APPLE__
	return 1;
	#else
	return 0;
	#endif
}

int build_client_zig(void) {
	Nob_Cmd cmd = {0};

	char *CC = getenv("CC");
	char *CFLAGS = getenv("CFLAGS");

	append_compiler(&cmd, "zig", CFLAGS);
	// append_warnings(&cmd);

	nob_cmd_append(&cmd, "build-exe");

	// SOURCE FILES
	append_common(&cmd);
	nob_cmd_append(&cmd, "src/client/main.zig", "src/client/mesh.c",
				   "src/client/render.c", "src/client/shader.c",
				   "src/client/window.c");

	// LINKING
	include_common(&cmd);

	DIR *cglm_dir = opendir("cglm/include");
	if ((!cglm_dir) || (!nob_file_exists("RGFW/RGFW.h"))) {
		closedir(cglm_dir);

		Nob_Cmd autoinit = {0};
		nob_cmd_append(&autoinit, "git", "submodule", "init");
		if (!nob_cmd_run_sync(autoinit)) return -1;

		Nob_Cmd autoupdate = {0};
		nob_cmd_append(&autoupdate, "git", "submodule", "update", "--depth=1");
		if (!nob_cmd_run_sync(autoupdate)) return -1;
	}

	nob_cmd_append(&cmd, "-I./cglm/include");
	nob_cmd_append(&cmd, "-I./RGFW/");

	link_math(&cmd);

	// Windows
	if (for_windows(CC)) {
		nob_cmd_append(&cmd, "-lopengl32", "-lshell32", "-lgdi32", "-lwinmm");
		// append_output(&cmd, "zig-client.exe");
		nob_cmd_append(&cmd, "-femit-bin=./build/zig-client.exe");
		nob_cmd_append(&cmd, "-target", "x86_64-windows-gnu");
	}
	// MacOS
	else if (for_macos()) {
		nob_cmd_append(&cmd, "-framework", "Foundation", "-framework", "AppKit", "-framework", "OpenGL", "-framework", "CoreVideo");
		// append_output(&cmd, "zig-client");
		nob_cmd_append(&cmd, "-femit-bin=./build/zig-client");
	}
	// Linux
	else {
		nob_cmd_append(&cmd, "-lX11", "-lGL", "-lXrandr");
		// append_output(&cmd, "zig-client");
		nob_cmd_append(&cmd, "-femit-bin=./build/zig-client");
	}

	if (!nob_cmd_run_sync(cmd))
		return 1;

	nob_cmd_free(cmd);

	return 0;
}

int build_client(void) {
	Nob_Cmd cmd = {0};

	char *CC = getenv("CC");
	char *CFLAGS = getenv("CFLAGS");

	append_compiler(&cmd, CC, CFLAGS);
	append_warnings(&cmd);

	// SOURCE FILES
	append_common(&cmd);
	nob_cmd_append(&cmd, "src/client/main.c", "src/client/camera.c", "src/client/mesh.c",
				   "src/client/render.c", "src/client/shader.c",
				   "src/client/window.c");

	// LINKING
	include_common(&cmd);

	DIR *cglm_dir = opendir("cglm/include");
	if ((!cglm_dir) || (!nob_file_exists("RGFW/RGFW.h"))) {
		closedir(cglm_dir);

		Nob_Cmd autoinit = {0};
		nob_cmd_append(&autoinit, "git", "submodule", "init");
		if (!nob_cmd_run_sync(autoinit)) return -1;

		Nob_Cmd autoupdate = {0};
		nob_cmd_append(&autoupdate, "git", "submodule", "update", "--depth=1");
		if (!nob_cmd_run_sync(autoupdate)) return -1;
	}

	nob_cmd_append(&cmd, "-I./cglm/include");
	nob_cmd_append(&cmd, "-I./RGFW/");

	link_math(&cmd);

	// Windows
	if (for_windows(CC)) {
		nob_cmd_append(&cmd, "-lopengl32", "-lshell32", "-lgdi32", "-lwinmm");
		append_output(&cmd, "client.exe");
	}
	// MacOS
	else if (for_macos()) {
		nob_cmd_append(&cmd, "-framework", "Foundation", "-framework", "AppKit", "-framework", "OpenGL", "-framework", "CoreVideo");
		append_output(&cmd, "client");
	}
	// Linux
	else {
		nob_cmd_append(&cmd, "-lX11", "-lGL", "-lXrandr");
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

	// SOURCE FILES
	append_common(&cmd);
	nob_cmd_append(&cmd, "src/server/main.c");

	// LINKING
	include_common(&cmd);
	link_math(&cmd);

	// Windows
	if (for_windows(CC)) {
		append_output(&cmd, "server.exe");
	}
	// Other
	else {
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

	if (argc == 2 && strcmp(argv[1], "zig") == 0) {
		if (build_client_zig() != 0) return -1;
	} else {
		if (build_client() != 0) return -1;
	}

	if (build_server() != 0) return -1;

	return 0;
}
