load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

package(
    default_visibility = ["//visibility:public"],
)

# thirdparty Scanner
cc_library(
    name = "Scanner",
    hdrs = glob(["File.h", "Scanner.hpp"]),
    #includes = ["Scanner"]
    include_prefix = "Scanner/",
)
