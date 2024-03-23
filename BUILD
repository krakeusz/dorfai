cc_library(
    name = "dorfai",
    srcs = glob(
        ["src/*.cpp"],
        exclude = ["src/main.cpp"],
    ),
    hdrs = glob(["include/*.h"]),
    includes = ["include"],
    deps = ["@yaml-cpp//:yaml-cpp"],
)

cc_binary(
    name = "main",
    srcs = ["src/main.cpp"],
    deps = [
        ":dorfai",
    ],
)
