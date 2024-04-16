filegroup(
    name = "tiles_yaml",
    srcs = ["resources/tiles/tiles.yaml"],
)

cc_library(
    name = "dorfai",
    srcs = glob(
        ["src/*.cpp"],
        exclude = ["src/main.cpp"],
    ),
    hdrs = glob(["include/*.h"]),
    data = [":tiles_yaml"],
    includes = ["include"],
    local_defines = ["TILES_YAML=\"$(location tiles_yaml)\""],
    deps = ["@yaml-cpp//:yaml-cpp"],
)

cc_binary(
    name = "main",
    srcs = ["src/main.cpp"],
    deps = [
        ":dorfai",
    ],
)

cc_test(
    name = "unit_test",
    srcs = glob(["tests/test_*.cpp"]),
    data = [":tiles_yaml"],
    local_defines = ["TILES_YAML=\\\"$(location tiles_yaml)\\\""],
    deps = [
        ":dorfai",
        "@catch2//:catch2_main",
    ],
)
