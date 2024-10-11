load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
# load("@bazel_skylib//rules:expand_template.bzl", "expand_template")
#
# -----------------------------------------
# https://docs-legacy.aspect.build/aspect-build/bazel-lib/v0.9.6/docs/expand_make_vars-docgen.html#expand_template
# https://github.com/aspect-build/bazel-lib
load("@aspect_bazel_lib//lib:expand_template.bzl", "expand_template")

package(
    default_visibility = ["//visibility:public"],
)

# FIX EXPANSIONS
#include/cln/config.h
#include/cln/host_cpu.h
#include/cln/intparam.h
#autoconf/cl_config.h
#src/base/cl_base_config.h
#src/base/cl_gmpconfig.h
#src/timing/cl_t_config.h

# ----------------------------------------

expand_template(
    name = "cln_config",
    out = "include/cln/config.h",
    substitutions = {
        "@ALIGNOF_VOIDP@" : "8",
        # "#cmakedefine GMP_DEMANDS_UINTD_LONG_LONG" : "/* #undef GMP_DEMANDS_UINTD_LONG_LONG */", # linux solution
        "#cmakedefine GMP_DEMANDS_UINTD_LONG_LONG" : "$(GMP_TYPE_LL)", # WINDOWS IS LONG LONG, LINUX IS LONG
        "#cmakedefine GMP_DEMANDS_UINTD_INT" : "/* #undef GMP_DEMANDS_UINTD_INT */",
        # "#cmakedefine GMP_DEMANDS_UINTD_LONG" : "#define GMP_DEMANDS_UINTD_LONG",                # linux solution
        "#cmakedefine GMP_DEMANDS_UINTD_LONG" : "$(GMP_TYPE_L)",       # WINDOWS IS LONG LONG, LINUX IS LONG
        
    },
    template = "include/cln/config.h.cmake",
)

cc_library(
    name = "cln_config_generated",
    hdrs = ["include/cln/config.h"],
    include_prefix = ".", 
    visibility = ["//visibility:public"],
    linkstatic=1
)

# ----------------------------------------

expand_template(
    name = "cln_host_cpu",
    out = "include/cln/host_cpu.h",
    substitutions = {
        "__${CMAKE_SYSTEM_PROCESSOR}__" : "__x86_64__",       
    },
    template = "include/cln/host_cpu.h.cmake",
)

cc_library(
    name = "cln_host_cpu_generated",
    hdrs = ["include/cln/host_cpu.h"],
    include_prefix = ".", 
    visibility = ["//visibility:public"],
    linkstatic=1
)

# ----------------------------------------

expand_template(
    name = "cln_intparam",
    out = "include/cln/intparam.h",
    substitutions = {
        "@cl_char_bitsize@" : "8",    
        "@cl_short_bitsize@" : "16",
        "@cl_int_bitsize@" : "32",
        ## "@cl_long_bitsize@" : "64", # 32 is windows, 64 is linux
        "@cl_long_bitsize@" : "$(MY_LONG_SIZE)", # 32 is windows, 64 is linux
        "@cl_long_long_bitsize@" : "64",
        "@cl_pointer_bitsize@" : "64",
        "#cmakedefine short_little_endian" : "#define short_little_endian",
        "#cmakedefine int_little_endian" : "#define int_little_endian",
        "#cmakedefine long_little_endian" : "#define long_little_endian",
        "#cmakedefine long_long_little_endian" : "#define long_long_little_endian",
        "#cmakedefine short_big_endian" :"/* #undef short_big_endian */",
        "#cmakedefine int_big_endian" : "/* #undef int_big_endian */",
        "#cmakedefine long_big_endian" : "/* #undef long_big_endian */",
        "#cmakedefine long_long_big_endian" : "/* #undef long_long_big_endian */"
    },
    template = "include/cln/intparam.h.cmake",
)

cc_library(
    name = "cln_intparam_generated",
    hdrs = ["include/cln/intparam.h"],
    include_prefix = ".", 
    visibility = ["//visibility:public"],
    linkstatic=1
)

# ----------------------------------------

expand_template(
    name = "cln_autoconf_cl_config",
    out = "autoconf/cl_config.h",
    # DISABLED GMP!
    substitutions = {
        # "#cmakedefine CL_USE_GMP 1" : "#undef CL_USE_GMP",  # IF NOT USING GMP!
        "#cmakedefine CL_USE_GMP 1" : "#define CL_USE_GMP 1", # IF USING GMP!
        "#cmakedefine ASM_UNDERSCORE" : "/* #undef ASM_UNDERSCORE */",
        # "#cmakedefine CL_HAVE_ATTRIBUTE_FLATTEN" : "#define CL_HAVE_ATTRIBUTE_FLATTEN",    # LINUX
        "#cmakedefine CL_HAVE_ATTRIBUTE_FLATTEN" : "/* #undef CL_HAVE_ATTRIBUTE_FLATTEN */", # WINDOWS
        # "#cmakedefine HAVE_UNISTD_H" : "#define HAVE_UNISTD_H",    # LINUX
        "#cmakedefine HAVE_UNISTD_H" : "/* #undef HAVE_UNISTD_H */", # WINDOWS
    },
    template = "autoconf/cl_config.h.cmake",
)

cc_library(
    name = "cln_autoconf_cl_config_generated",
    hdrs = ["autoconf/cl_config.h"],
    include_prefix = ".", 
    visibility = ["//visibility:public"],
    linkstatic=1
)

# ----------------------------------------
# src/base/cl_base_config.h

expand_template(
    name = "cln_base_cl_base_config",
    out = "src/base/cl_base_config.h",
    substitutions = { 
        "#cmakedefine HAVE_GETTIMEOFDAY": "$(TIME_UNIX_WIN)",
        # "#cmakedefine HAVE_GETTIMEOFDAY" : "#define HAVE_GETTIMEOFDAY",    # linux ok: HAVE_GETTIMEOFDAY
        # "#cmakedefine HAVE_GETTIMEOFDAY" : "/* #undef HAVE_GETTIMEOFDAY */", # WINDOWS DOES NOT HAVE THIS!
        "#cmakedefine GETTIMEOFDAY_DOTS" : "/* #undef GETTIMEOFDAY_DOTS */",
        "#cmakedefine GETTIMEOFDAY_TZP_T" : "/* #undef GETTIMEOFDAY_TZP_T */",
        "#cmakedefine HAVE_TIMES_CLOCK" : "/* #undef HAVE_TIMES_CLOCK */",
    },
    template = "src/base/cl_base_config.h.cmake",
)

cc_library(
    name = "cln_base_cl_base_config_generated",
    hdrs = ["src/base/cl_base_config.h"],
    include_prefix = ".", 
    visibility = ["//visibility:public"],
    linkstatic=1
)

# ----------------------------------------
#src/base/cl_gmpconfig.h

expand_template(
    name = "cln_base_cl_gmpconfig",
    out = "src/base/cl_gmpconfig.h",
    # DISABLED GMP!
    substitutions = {
        # "#cmakedefine CL_USE_GMP 1" : "#undef CL_USE_GMP",  # IF NOT USING GMP!
        "#cmakedefine CL_USE_GMP 1" : "#define CL_USE_GMP 1", # IF USING GMP!
    },
    template = "src/base/cl_gmpconfig.h.cmake",
)

cc_library(
    name = "cln_base_cl_gmpconfig_generated",
    hdrs = ["src/base/cl_gmpconfig.h"],
    include_prefix = ".", 
    visibility = ["//visibility:public"],
    linkstatic=1
)

# ----------------------------------------

expand_template(
    name = "cln_timing_config",
    out = "src/timing/cl_t_config.h",
    substitutions = {
        "#cmakedefine HAVE_GETTIMEOFDAY": "$(TIME_UNIX_WIN)",
        #"#cmakedefine HAVE_GETTIMEOFDAY": "#define HAVE_GETTIMEOFDAY",       # LINUX OK
        #"#cmakedefine HAVE_GETTIMEOFDAY" : "/* #undef HAVE_GETTIMEOFDAY */", # WINDOWS DOES NOT HAVE THIS!
    },
    template = "src/timing/cl_t_config.h.cmake",
)

cc_library(
    name = "cln_timing_config_generated",
    hdrs = ["src/timing/cl_t_config.h"],
    include_prefix = ".", 
    visibility = ["//visibility:public"],
    linkstatic=1
)

# ====================================

cc_library(
    name = "cln_generated",
    deps = [
        ":cln_config_generated",
        ":cln_host_cpu_generated",
        ":cln_intparam_generated",
        ":cln_autoconf_cl_config_generated",
        ":cln_base_cl_base_config_generated",
        ":cln_base_cl_gmpconfig_generated",
        ":cln_timing_config_generated",
        ],
    visibility = ["//visibility:public"],
    linkstatic=1
)

# =================================================

# CLN lib
cc_library(
    name = "cln", # OUTPUT: cln.lib ?
    srcs = glob(
        ["src/**/*.c",
        "src/**/*.cc"],
        exclude = [
            "src/base/digitseq/cl_asm_hppa_.cc",
            "src/base/digitseq/cl_asm_i386_.cc",
            "src/base/digitseq/cl_asm_arm_.cc",
            "src/base/digitseq/cl_asm_mips_.cc",
            "src/base/digitseq/cl_asm_mipsel_.cc",
            "src/base/digitseq/cl_asm_m68k_.cc",
            "src/base/digitseq/cl_asm_sparc64_.cc",
            "src/base/digitseq/cl_asm_sparc_.cc",
            "src/polynomial/elem/cl_asm_sparc_GF2.cc",
        ],
    ),
    hdrs = glob([
        "include/**/*.h",
        "src/**/*.h",
        "autoconf/**/*.h",
        "src/float/sfloat/**/*.cc",
        "src/float/ffloat/**/*.cc",
        "src/float/dfloat/**/*.cc",
        "src/float/lfloat/**/*.cc",
        "src/rational/misc/**/*.cc",
        "src/integer/misc/**/*.cc",
        "src/integer/conv/**/*.cc",
        "src/complex/algebraic/cl_C_abs_aux.cc",
        "src/real/conv/cl_F_from_R_def.cc",
        "src/integer/misc/cl_I_signum.cc",
        "src/rational/misc/cl_RA_signum.cc",
        "src/real/algebraic/cl_RA_sqrt.cc",
    ]),
    includes = ["include/", "src/", "autoconf/",],
    
    deps = [":cln_generated", "@gmp//:gmp"],
    #deps = [":cln_generated"], # DISABLED GMP!
    visibility = ["//visibility:public"],
)

cc_library(
    name="cln_shared",
    linkstatic = False,
    # linkstatic False means to generate .so / .dll
    #
    ###linkshared=1 # NOT AVAILABLE IN cc_library
    # CANNOT USE linkshared HERE
    #
    alwayslink = True,   # important (According to...)
    #
    # alwayslink ON will generate libcln.lo and libcln.so (NOT libcln.a)
    #
    # https://stackoverflow.com/questions/51689092/playing-with-bazel-c-tutorials-build-does-not-create-use-shared-libraries
    # copts=["-fPIC"],
    deps = [":cln"]
)

# to use as dynamic_deps = [":cln_shared_experimental"]
#cc_shared_library(
#    name="cln_shared_experimental",
#    # shared_lib_name = "cln.so" # BUT, IN WINDOWS?
#    deps=[":cln"]
#)

# bazel run @cln//:cln_example_fibonacci 10
cc_binary(
    name = "cln_example_fibonacci",
    srcs = ["examples/fibonacci.cc"],
    deps = [":cln"],
    visibility = ["//visibility:public"],
    linkstatic=1
)
