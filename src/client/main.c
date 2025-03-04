#include "logs.h"
#include "player.h"
#include "render.h"
#include "window.h"

#define TITLE "Voxels"
#define WIDTH 800
#define HEIGHT 600

int main(void) {
  if (!logging_init()) {
    return -1;
  }
  window_create(TITLE, WIDTH, HEIGHT);

  if (loadGL((GLADloadfunc)getProcAddress) != 0) {
    window_close();
    return -1;
  }

  struct World *world = world_init();
  Player player = {
      .position = {0.0f, 2.0f, 0.0f},
      .velocity = {0.0f, 0.0f, 0.0f},
      .acceleration = {0.0f, 0.0f, 0.0f},
      .speed = 40.0f,
      .direction = {0.0f, 0.0f, 0.0f},
  };
  Camera camera = {&player.position, &player.direction, &player.speed};
  Renderer renderer = renderer_init(&camera);

  info("The program has been fully initialized. Starting the game loop...");
  while (!window_shouldClose()) {
    float dt = window_clock();

    // EVENTS
    window_mouseAbsorb();
    window_pollEvents();

    // LOGIC
    camera_update(&camera, dt);
    chunks_load_unload_system(&renderer, world);

    // RENDER
    render(&renderer);
    window_swapBuffers();
  }
  world_free(world);
  renderer_free(&renderer);
  window_close();
  info("The program has terminated.");
  logging_deinit();

  return 0;
}

// FIX (RENDER): too many VAO. When the player goes too far the app crashes. I suppose there is a limit of VAO count.

// BUG (SHADERS): voxels' shadows look horrible. Maybe gamma corr or HSV/HSL
// instead of RGB?

// PERFORMANCE (MULTITHREADING): some microfreeze happen. Find a cause and annihilate.

// TODO (DEBUG): hot reloading (hi Zozin)
// TODO (WORLDGEN): world generation using simplex noise
// TODO (PHYSICS): basic physics (gravitation, air resistance)
// TODO (PHYSICS): block breaking by player
// TODO (SHADERS): ambient occlusion
// TODO (SHADERS): a nice sky
// TODO (NEW SYSTEM): addons/plugins/scripts
// TODO (NEW SYSTEM): client-server multiplayer
// TODO (NEW SYSTEM): play some audio using miniaudio
// TODO (NEW SYSTEM): cimgui or header-only alternative (player position, FPS,
// render distance, speed, sensitivity)
// TODO (NEW SYSTEM): level of details (LoD)

// OPTIMIZE (BUILD): fully incremental build system
// OPTIMIZE (WORLDGEN): generate chunks in second thread?
// OPTIMIZE (MESHGEN, SHADERS): optimize vertex size: compress all data in one
// 32 bit integer OPTIMIZE (MESHGEN): mesh generating: create 8 vertex per voxel
// instead of 4 vertex per face (24 vertex per block) upd: after adding normales
// to vertices i'm not sure how to do it :( upd2: if you do "optimize vertex
// size" then you can do instancing and it wouldn't be a problem
// OPTIMIZE (MESHGEN): a blazingly fast greedy mesher xd lol
