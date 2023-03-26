const std = @import("std");
const Self = @This();
const CrossTarget = std.zig.CrossTarget;

pub fn build(b: *std.Build) !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer {
        if (gpa.deinit()) {
            @panic("Memory leak!");
        }
    }
    var allocator = std.heap.ArenaAllocator.init(gpa.allocator());
    defer allocator.deinit();

    const target = b.standardTargetOptions(.{});
    const mode = b.standardOptimizeOption(.{});

    const dxlib_options: std.build.SharedLibraryOptions = .{
        .name = "DxPortLib",
        .target = target,
        .optimize = mode,
    };

    const dxLib: *std.build.CompileStep = b.addSharedLibrary(dxlib_options);

    dxLib.linkLibC();
    dxLib.linkLibCpp();

    dxLib.addIncludePath("DxPortLib/vorbis/include");
    dxLib.addIncludePath("DxPortLib/ogg/include");

    dxLib.linkSystemLibrary("SDL2");
    dxLib.linkSystemLibrary("SDL2_ttf");
    dxLib.linkSystemLibrary("SDL2_image");
    dxLib.linkSystemLibrary("vorbisfile");

    dxLib.addIncludePath("DxPortLib/src");
    dxLib.addIncludePath("DxPortLib/include");

    var dxlib_sources = try discover_dxportlib_sources(allocator.allocator());

    dxLib.addCSourceFiles(dxlib_sources.c, &.{});
    dxLib.addCSourceFiles(dxlib_sources.cpp, &.{"-std=c++11"});

    dxLib.install();

    const executable_options: std.build.ExecutableOptions = .{
        .name = "UTyping",
        .target = target,
        .optimize = mode,
    };

    var executable: *std.build.CompileStep = b.addExecutable(executable_options);

    executable.linkLibrary(dxLib);

    executable.linkLibC();
    executable.linkLibCpp();

    executable.addIncludePath(root_path ++ "DxPortLib/include");

    // executable.addCSourceFiles(srcs, &.{"-std=c++11"});
    executable.addCSourceFile(root_path ++ "UTyping.cpp", &.{"-std=c++11"});

    executable.install();
}

fn discover_dxportlib_sources(allocator: std.mem.Allocator) !struct { c: []const []const u8, cpp: []const []const u8 } {
    var c_list = std.ArrayList([]const u8).init(allocator);
    var cpp_list = std.ArrayList([]const u8).init(allocator);

    const dxportlib_source_root: []const u8 = root_path ++ "DxPortLib/src/";

    var dir = try std.fs.openIterableDirAbsolute(dxportlib_source_root, .{});
    defer dir.close();

    var walker: std.fs.IterableDir.Walker = try dir.walk(allocator);
    defer walker.deinit();

    var itr_next: ?std.fs.IterableDir.Walker.WalkerEntry = try walker.next();
    while (itr_next != null) {
        var next: std.fs.IterableDir.Walker.WalkerEntry = itr_next.?;

        //if the file is a c source file
        if (std.mem.endsWith(u8, next.path, ".c")) {
            var item = try allocator.alloc(u8, next.path.len + dxportlib_source_root.len);

            //copy the root first
            std.mem.copy(u8, item, dxportlib_source_root);

            //copy the filepath next
            std.mem.copy(u8, item[dxportlib_source_root.len..], next.path);

            try c_list.append(item);
        }

        //if the file is a cpp source file
        if (std.mem.endsWith(u8, next.path, ".cpp")) {
            var item = try allocator.alloc(u8, next.path.len + dxportlib_source_root.len);

            //copy the root first
            std.mem.copy(u8, item, dxportlib_source_root);

            //copy the filepath next
            std.mem.copy(u8, item[dxportlib_source_root.len..], next.path);

            try cpp_list.append(item);
        }

        itr_next = try walker.next();
    }

    return .{ .c = try c_list.toOwnedSlice(), .cpp = try cpp_list.toOwnedSlice() };
}

fn root() []const u8 {
    return std.fs.path.dirname(@src().file) orelse ".";
}

const root_path = root() ++ "/";

const srcs = &.{
    root_path ++ "ututil.hpp",
    root_path ++ "utscore.hpp",
    root_path ++ "utkeyinput.hpp",
    root_path ++ "utconfig.hpp",
    root_path ++ "utcheck.hpp",
    root_path ++ "utchallenge.hpp",
};
