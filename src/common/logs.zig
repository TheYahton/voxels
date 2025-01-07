const std = @import("std");

const c = @cImport({
    @cInclude("stdio.h");
    @cInclude("time.h");
});

fn formatted_time(buffer: *[26]u8) void {
    const timer = c.time(null);
    const tm_info = c.localtime(timer);
    _ = c.strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
}

export fn info(text: *c_char) void {
    var buffer: [26]u8 = undefined;
    formatted_time(&buffer);
    std.debug.print("[{s}] INFO: {s}", .{ buffer, text });
}

export fn warning(text: *c_char) void {
    var buffer: [26]u8 = undefined;
    formatted_time(&buffer);
    std.debug.print("[{s}] WARNING: {s}", .{ buffer, text });
}

export fn @"error"(text: *c_char) void {
    var buffer: [26]u8 = undefined;
    formatted_time(&buffer);
    std.debug.print("[{s}] ERROR: {s}", .{ buffer, text });
}
