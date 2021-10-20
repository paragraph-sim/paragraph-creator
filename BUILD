package(licenses = ["notice"])

exports_files([
    "LICENSE",
    "NOTICE",
])

cc_binary(
    name = "stencil",
    srcs = ["src/stencil/main.cc"],
    copts = ["-UNDEBUG"],
    deps = [
        "@libprim//:prim",
        "@paragraph//paragraph/graph",
        "@tclap//:tclap",
    ],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "allreduce",
    srcs = ["src/allreduce/main.cc"],
    copts = ["-UNDEBUG"],
    deps = [
        "@paragraph//paragraph/graph",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_protobuf//:protobuf",
    ],
    visibility = ["//visibility:public"],
)

genrule(
    name = "lint",
    srcs = glob([
        "src/**/*.cc",
        "src/**/*.h",
        "src/**/*.tcc",
    ]),
    outs = ["linted"],
    cmd = """
    python $(location @cpplint//:cpplint) \
      --root=$$(pwd) \
      --headers=h,tcc \
      --extensions=cc,h,tcc \
      --quiet $(SRCS) > $@
    echo // $$(date) > $@
    """,
    tools = [
        "@cpplint",
    ],
    visibility = ["//visibility:public"],
)

genrule(
    name = "format_check",
    srcs = glob([
        "src/**/*.cc",
        "src/**/*.h",
        "src/**/*.tcc",
    ]),
    outs = ["format_checked"],
    cmd = """
    cp $(location @clang_format//file) .clang-format
    clang-format --style=file --dry-run --Werror $(SRCS)
    echo // $$(date) > $@
    """,
    tools = [
        "@clang_format//file",
    ],
    visibility = ["//visibility:public"],
)

sh_test(
    name = "stencil_test",
    srcs = ["test/stencil.sh"],
    data = [
        ":stencil",
        "test/unidir_ring_push_translation.json",
        "@paragraph//paragraph/simulator:simulator",
        "@paragraph//paragraph/translation:graph_translator",
    ],
    visibility = ["//visibility:public"],
)

sh_test(
    name = "allreduce_test",
    srcs = ["test/allreduce.sh"],
    data = [
        ":allreduce",
        "test/unidir_ring_push_translation.json",
        "@paragraph//paragraph/simulator:simulator",
        "@paragraph//paragraph/translation:graph_translator",
    ],
    visibility = ["//visibility:public"],
)
