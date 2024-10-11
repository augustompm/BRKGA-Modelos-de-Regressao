load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

package(
    default_visibility = ["//visibility:public"],
)

# ------------------
# thirdparty Scanner
# ------------------

# BEWARE THAT THIS ORGANIZATION IS BAD, USE NEXT ONE!
# IT WORKS... BUT IT BREAKS ON compile_commands.json
# VERY STRANGE!

#cc_library(
#    name = "Scanner",
#    hdrs = glob(["File.h", "Scanner.hpp"]),
#    include_prefix = "Scanner/",
#    # WORKS WITH (thirdparty/): 
#    # Scanner/
#    #         File.h
#    #         Scanner.hpp
#)

cc_library(
    name = "Scanner",
    hdrs = glob(["include/Scanner/File.h", "include/Scanner/Scanner.hpp"]),
    includes = ["include"],
    # WORKS WITH (thirdparty/): 
    # Scanner/
    #   include/
    #      Scanner/
    #         File.h
    #         Scanner.hpp
)
