load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

package(default_visibility = ["//visibility:public"])


cc_test(
    name = "Evaluator_test",
    srcs = glob([
        "Evaluator_test.cpp",
    ]),
    defines = ["CATCH_CONFIG_MAIN"],
    deps = ["//include:brkgp-lib", "@Scanner//:Scanner", "@gmp//:gmp", "@cln//:cln", "@ginac//:ginac",":catch2_thirdparty"],
)


cc_library(
    name = "catch2_thirdparty",
    hdrs = ["thirdparty/catch2/catch_amalgamated.hpp"],
    srcs = ["thirdparty/catch2/catch_amalgamated.cpp"],
    strip_include_prefix="thirdparty/",
    
)

test_suite(
    name = "all-tests",
    tests = [
        "Evaluator_test"
    ]
)
