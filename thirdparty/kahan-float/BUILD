load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

package(
    default_visibility = ["//visibility:public"],
)

# thirdparty kahan-float
cc_library(
    name = "kahan-float",
    hdrs = glob(["include/kahan-float/kahan.hpp", "include/kahan-float/neumaier.hpp"]),
    includes = ["include"],
)
