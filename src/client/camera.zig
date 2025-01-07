const c = @cImport({
    @cInclude("RGFW.h");
});

const std = @import("std");
const math = std.math;

const Vec3 = extern struct { x: f32, y: f32, z: f32 };

pub const Camera = extern struct {
    position: *Vec3,
    direction: *Vec3,
    speed: *f32,
};

pub export fn camera_update(camera: *Camera, keys: [*]c_char, dx: f32, dy: f32, dt: f32) void {
    // Update camera rotation
    {
        const up: f32 = @as(f32, @floatFromInt(keys[c.RGFW_Up] - keys[c.RGFW_Down]));
        const left: f32 = @as(f32, @floatFromInt(keys[c.RGFW_Left] - keys[c.RGFW_Right]));

        camera.direction.y += up * dt;
        camera.direction.x += left * dt;
        camera.direction.y += -dy * dt * 0.15;
        camera.direction.x += -dx * dt * 0.15;

        camera.direction.y = @max(@min(camera.direction.y, math.pi / 2.0), -math.pi / 2.0);
    }

    // Update camera position
    {
        const forward: f32 = @as(f32, @floatFromInt(keys[c.RGFW_w] - keys[c.RGFW_s]));
        const left: f32 = @as(f32, @floatFromInt(keys[c.RGFW_a] - keys[c.RGFW_d]));
        const up: f32 = @as(f32, @floatFromInt(keys[c.RGFW_Space] - keys[c.RGFW_ShiftL]));

        camera.position.z += (forward * math.cos(camera.direction.x) - left * math.sin(camera.direction.x)) * camera.speed.* * dt;
        camera.position.x += (left * math.cos(camera.direction.x) + forward * math.sin(camera.direction.x)) * camera.speed.* * dt;
        camera.position.y += up * camera.speed.* * dt;
    }
}
