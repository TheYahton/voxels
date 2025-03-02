#define RGFW_EXPORT

#include "logs.h"
#include "player.h"
#include "render.h"

int main(void) {
  MyWindow window = createWindow(800, 600);
  if (initWindow(&window) != 0) {
    return -1;
  }

  if (loadGL((GLADloadfunc)getProcAddress) != 0) {
    windowClose(&window);
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
  Renderer renderer = renderer_init(&window, &camera);

  render_preparation(window.width, window.height);

  info("The program has been fully initialized. Starting the game loop...");

  double start = 0.0f;
  double end = 0.0f;
  double dt = 0.0f;
  while (!windowShouldClose(&window)) {
    dt = end - start;
    start = getTime();

    // EVENTS
    window.dx = 0, window.dy = 0;
    RGFW_window_checkEvents(window.window, 0);

    // LOGIC
    camera_update(&camera, window.keys, window.dx, window.dy, dt);
    chunks_load_unload_system(&renderer, world);

    // RENDER
    render(&renderer);
    swapBuffer(&window);

    end = getTime();
  }
  world_free(world);
  renderer_free(&renderer);
  windowClose(&window);
  info("The program has terminated.");

  return 0;
}

// FIX (RENDER): too many VAO. When the player goes too far the app crashes. I suppose there is a limit of VAO count.

// BUG (SHADERS): voxels' shadows look horrible. Maybe gamma corr or HSV/HSL
// instead of RGB?

// PERFORMANCE (MULTITHREADING): some microfreeze happen. Find a cause and annihilate.

// TODO (LOGGING): write the logs to a file in the `logs` folder
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

// OPTIMIZE (BUILD): build libs to obj file ONCE, then link with other stuff (it
// will improve build speed) OPTIMIZE (BUILD): fully incremental build system
// OPTIMIZE (WORLDGEN): generate chunks in second thread?
// OPTIMIZE (MESHGEN, SHADERS): optimize vertex size: compress all data in one
// 32 bit integer OPTIMIZE (MESHGEN): mesh generating: create 8 vertex per voxel
// instead of 4 vertex per face (24 vertex per block) upd: after adding normales
// to vertices i'm not sure how to do it :( upd2: if you do "optimize vertex
// size" then you can do instancing and it wouldn't be a problem
// OPTIMIZE (MESHGEN): a blazingly fast greedy mesher xd lol
