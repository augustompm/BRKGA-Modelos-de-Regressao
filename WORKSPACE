# Carregando a definição de repositórios HTTP do Bazel
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

local_repository(
    name = "manydeps-gmp",
    path = ".",  
)

new_local_repository(
    name = "gmp",
    path = "thirdparty/gmp",  
    build_file = "//thirdparty/gmp:BUILD",  
)

new_local_repository(
    name = "cln",
    path = "thirdparty/cln",  
    build_file = "//thirdparty/cln:BUILD",  
)

new_local_repository(
    name = "ginac",
    path = "thirdparty/ginac", 
    build_file = "//thirdparty/ginac:BUILD",  
)

new_local_repository(
    name = "Scanner",
    path = "thirdparty/Scanner", 
    build_file = "//thirdparty/Scanner:BUILD",  
)

new_local_repository(
    name = "kahan-float",
    path = "thirdparty/kahan-float",  
    build_file = "//thirdparty/kahan-float:BUILD",  
)

# Arquivo de arquivamento HTTP para Catch2
http_archive(
    name = "catch2",
    urls = ["https://github.com/catchorg/Catch2/archive/v3.4.0.tar.gz"],
    strip_prefix = "Catch2-3.4.0",
    sha256 = "122928b814b75717316c71af69bd2b43387643ba076a6ec16e7882bfb2dfacbb",
)

# Comentários sobre possíveis queries do Bazel, descomente se necessário
# bazel query "@vcpkg-build//include:*"
# bazel query 'labels(hdrs, ...)'

# Repositório local comentado para 'vcpkg-build', descomente se necessário
# local_repository(
#     name = "vcpkg-build",
#     path = "./build/vcpkg_installed/x64-linux",
# )

# Carregando novamente para evitar duplicação, pode ser removido se já carregado acima
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Configuração do Hedron Compile Commands Extractor para Bazel
# https://github.com/hedronvision/bazel-compile-commands-extractor
http_archive(
    name = "hedron_compile_commands",
    url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/ed994039a951b736091776d677f324b3903ef939.tar.gz",
    strip_prefix = "bazel-compile-commands-extractor-ed994039a951b736091776d677f324b3903ef939",
    # Recomenda-se atualizar o sha256 conforme instruções no repositório
    #sha256 = "SUBSTITUA_COM_O_SHA256_CORRETO",  # Atualize com o hash correto
)

# Carregando e configurando o Hedron Compile Commands
load("@hedron_compile_commands//:workspace_setup.bzl", "hedron_compile_commands_setup")

hedron_compile_commands_setup()

# Adicionando o Skylib necessário para o Catch2
http_archive(
    name = "bazel_skylib",
    sha256 = "66ffd9315665bfaafc96b52278f57c7e2dd09f5ede279ea6d39b2be471e7e3aa",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.4.2/bazel-skylib-1.4.2.tar.gz",
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.4.2/bazel-skylib-1.4.2.tar.gz",
    ],
)

# Carregando e configurando o Skylib
load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")

bazel_skylib_workspace()

# Adicionando o Aspect Bazel Lib
http_archive(
    name = "aspect_bazel_lib",
    sha256 = "09b51a9957adc56c905a2c980d6eb06f04beb1d85c665b467f659871403cf423",
    strip_prefix = "bazel-lib-1.34.5",
    url = "https://github.com/aspect-build/bazel-lib/releases/download/v1.34.5/bazel-lib-v1.34.5.tar.gz",
)

# Carregando e configurando o Aspect Bazel Lib
load("@aspect_bazel_lib//lib:repositories.bzl", "aspect_bazel_lib_dependencies")

aspect_bazel_lib_dependencies()
