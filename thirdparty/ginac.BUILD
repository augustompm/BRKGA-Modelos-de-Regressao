load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("@bazel_skylib//rules:expand_template.bzl", "expand_template")

package(
    default_visibility = ["//visibility:public"],
)

# GENERATE FILES
# function.h
# function.cpp

genrule(
    name = 'ginac_function_h',
    outs = ['ginac/function.h'],
    srcs = ["ginac/function.py", "ginac/function.hppy", "scripts/yaptu.py"],
    cmd = 'python3 $(location ginac/function.py) -o $@ $(location ginac/function.hppy)'
)

genrule(
    name = 'ginac_function_cpp',
    outs = ['ginac/function.cpp'],
    srcs = ["ginac/function.py", "ginac/function.cppy", "scripts/yaptu.py"],
    cmd = 'python3 $(location ginac/function.py) -o $@ $(location ginac/function.cppy)'
)

cc_library(
    name = "ginac_function_h_generated",
    hdrs = ["ginac/function.h"],  # GENERATED
    include_prefix = ".", 
    visibility = ["//visibility:public"],
    # include ONLY basic headers needed for function.h
    deps = [":lib_basic_headers"]
)

cc_library(
    name = "ginac_function_cpp_generated",
    srcs = ["ginac/function.cpp",  # GENERATED
            "ginac/remember.cpp"], # link dependency for function.cpp
    include_prefix = ".", 
    visibility = ["//visibility:public"],
    # generate .h before this .cpp
    deps = [":ginac_function_h_generated"],
)

# ====================================

cc_library(
    name = "ginac_generated",
    deps = [
        ":ginac_function_h_generated",
        ":ginac_function_cpp_generated",
        ],
    visibility = ["//visibility:public"],
)

# =============

# GINAC lib basic headers
# THIS IS SEPARATED BECAUSE function.h/.cpp helper will need this in pre-build!
cc_library(
    name = "lib_basic_headers",
    hdrs = glob([
        "ginac/**/*.h",
    ]),
    includes = ["ginac/"],
    deps = ["@cln//:cln"],
    visibility = ["//visibility:public"],
)

# =============

# GINAC lib
cc_library(
    name = "ginac",
    srcs = glob(
        ["ginac/**/*.cpp",
        "src/**/*.cc"],
        exclude = [
        ],
    ),
    hdrs = glob([
        "ginac/**/*.h",
        "autoconf/**/*.h",
    ]),
    includes = ["ginac/", "autoconf/", "ginac/parser/"],
    deps = [ ":ginac_generated", "@cln//:cln"],
    visibility = ["//visibility:public"],
    #linkstatic = 1
)

# bazel run @ginac//:ginac_check_exam_factor
cc_test(
    name = "ginac_check_exam_factor",
    srcs = ["check/exam_factor.cpp"],
    deps = [":ginac"],
    visibility = ["//visibility:public"],
)

# bazel run @ginac//:ginac_check_exam_paranoia
cc_test(
    name = "ginac_check_exam_paranoia",
    srcs = ["check/exam_paranoia.cpp"],
    deps = [":ginac"],
    visibility = ["//visibility:public"],
)

# bazel run @ginac//:ginac_check_exam_pgcd
cc_test(
    name = "ginac_check_exam_pgcd",
    srcs = ["check/exam_pgcd.cpp"],
    deps = [":ginac"],
    visibility = ["//visibility:public"],
)