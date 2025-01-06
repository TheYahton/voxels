const c = @cImport({
    @cInclude("logs.h");
    @cInclude("player.h");
    @cInclude("render.h");
});

const Camera = @import("camera.zig");

pub export fn main() u8 {
    var window: c.MyWindow = c.createWindow(@as(c_int, 800), @as(c_int, 600));
    if (c.initWindow(&window) != @as(c_int, 0)) {
        return 255;
    }
    if (c.loadGL(@as(c.GLADloadfunc, @ptrCast(@alignCast(&c.getProcAddress)))) != @as(c_int, 0)) {
        c.windowClose(&window);
        return 255;
    }
    var world: c.struct_World = c.world_init();
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
    var camera: Camera.Camera = Camera.Camera{
        .position = @ptrCast(&player.position),
        .direction = @ptrCast(&player.direction),
        .speed = @ptrCast(&player.speed),
    };
    var renderer: c.Renderer = c.renderer_init(&window, @ptrCast(&camera));
    c.render_preparation(window.width, window.height);
    c.info("The program has been fully initialized. Starting the game loop...");
    var start: f64 = @as(f64, @floatCast(0.0));
    var end: f64 = @as(f64, @floatCast(0.0));
    var dt: f64 = @as(f64, @floatCast(0.0));
    while (!c.windowShouldClose(&window)) {
        dt = end - start;
        start = c.getTime();
        var dx: f32 = 0.0;
        var dy: f32 = 0.0;
        while (c.RGFW_window_checkEvent(window.window) != null) {
            if (window.window.*.event.type == c.RGFW_mousePosChanged) {
                dx = @as(f32, @floatFromInt(window.window.*.event.point.x));
                dy = @as(f32, @floatFromInt(window.window.*.event.point.y));
            }
        }
        Camera.camera_update(&camera, @ptrCast(@alignCast(&window.keys)), dx, dy, @as(f32, @floatCast(dt)));
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
