const c = @cImport({
    @cInclude("logs.h");
    @cInclude("player.h");
    @cInclude("render.h");
});

pub export fn main() u8 {
    var window: c.MyWindow = c.createWindow(@as(c_int, 800), @as(c_int, 600));
    _ = &window;
    if (c.initWindow(&window) != @as(c_int, 0)) {
        return 255;
    }
    if (c.loadGL(@as(c.GLADloadfunc, @ptrCast(@alignCast(&c.getProcAddress)))) != @as(c_int, 0)) {
        c.windowClose(&window);
        return 255;
    }
    var world: c.struct_World = c.world_init();
    _ = &world;
    var player: c.Player = c.Player{
        .position = c.Vec3{
            .x = 0.0,
            .y = 2.0,
            .z = 0.0,
        },
        .velocity = c.Vec3{
            .x = 0.0,
            .y = 0.0,
            .z = 0.0,
        },
        .acceleration = c.Vec3{
            .x = 0.0,
            .y = 0.0,
            .z = 0.0,
        },
        .speed = 5.0,
        .direction = c.Vec3{
            .x = 0.0,
            .y = 0.0,
            .z = 0.0,
        },
    };
    _ = &player;
    var camera: c.Camera = c.Camera{
        .position = &player.position,
        .direction = &player.direction,
        .speed = &player.speed,
    };
    _ = &camera;
    var renderer: c.Renderer = c.renderer_init(&window, &camera);
    _ = &renderer;
    c.render_preparation(window.width, window.height);
    c.info("The program has been fully initialized. Starting the game loop...");
    var start: f64 = @as(f64, @floatCast(0.0));
    _ = &start;
    var end: f64 = @as(f64, @floatCast(0.0));
    _ = &end;
    var dt: f64 = @as(f64, @floatCast(0.0));
    _ = &dt;
    while (!c.windowShouldClose(&window)) {
        dt = end - start;
        start = c.getTime();
        var dx: f32 = 0.0;
        _ = &dx;
        var dy: f32 = 0.0;
        _ = &dy;
        while (c.RGFW_window_checkEvent(window.window) != null) {
            if (window.window.*.event.type == c.RGFW_mousePosChanged) {
                dx = @as(f32, @floatFromInt(window.window.*.event.point.x));
                dy = @as(f32, @floatFromInt(window.window.*.event.point.y));
            }
        }
        c.camera_update(&camera, @as([*c]bool, @ptrCast(@alignCast(&window.keys))), dx, dy, @as(f32, @floatCast(dt)));
        c.chunks_load_unload_system(&renderer, &world);
        c.render(&renderer);
        c.swapBuffer(&window);
        end = c.getTime();
    }
    c.world_free(&world);
    c.renderer_free(&renderer);
    c.windowClose(&window);
    c.info("The program has terminated.");
    return 0;
}
