cmake_minimum_required(VERSION 3.10)
project(paragraph-creator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FindPkgConfig)

# zlib
pkg_check_modules(zlib REQUIRED IMPORTED_TARGET zlib)
  get_target_property(
  ZLIB_INC
  PkgConfig::zlib
  INTERFACE_INCLUDE_DIRECTORIES
  )

# tclap
pkg_check_modules(tclap REQUIRED IMPORTED_TARGET tclap)
  get_target_property(
  TCLAP_INC
  PkgConfig::tclap
  INTERFACE_INCLUDE_DIRECTORIES
)
# nlohmann_json
pkg_check_modules(nlohmann_json REQUIRED IMPORTED_TARGET nlohmann_json)
  get_target_property(
  NLOHMANN_JSON_INC
  PkgConfig::nlohmann_json
  INTERFACE_INCLUDE_DIRECTORIES
)

# paragraph
pkg_check_modules(paragraph REQUIRED IMPORTED_TARGET libparagraph)
  get_target_property(
  PARAGRAPH_INC
  PkgConfig::paragraph
  INTERFACE_INCLUDE_DIRECTORIES
  )

# protobuf
find_package(Protobuf REQUIRED)
get_target_property(
  PROTOBUF_INC
  protobuf::libprotobuf
  INTERFACE_INCLUDE_DIRECTORIES
  )

# absl
find_package(absl REQUIRED)
get_target_property(
  ABSL_BASE_INC
  absl::base
  INTERFACE_INCLUDE_DIRECTORIES
  )
list(APPEND ABSL_LIBS
  absl::bad_any_cast_impl
  absl::bad_optional_access
  absl::bad_variant_access
  absl::base
  absl::city
  absl::civil_time
  absl::cord
  absl::debugging_internal
  absl::demangle_internal
  absl::examine_stack
  absl::exponential_biased
  absl::failure_signal_handler
  absl::flags_commandlineflag_internal
  absl::flags_commandlineflag
  absl::flags_config
  absl::flags_internal
  absl::flags_marshalling
  absl::flags_parse
  absl::flags_private_handle_accessor
  absl::flags_program_name
  absl::flags_reflection
  absl::flags
  absl::flags_usage_internal
  absl::flags_usage
  absl::graphcycles_internal
  absl::hash
  absl::hashtablez_sampler
  absl::int128
  absl::leak_check_disable
  absl::leak_check
  absl::log_severity
  absl::malloc_internal
  absl::periodic_sampler
  absl::random_distributions
  absl::random_internal_distribution_test_util
  absl::random_internal_platform
  absl::random_internal_pool_urbg
  absl::random_internal_randen_hwaes_impl
  absl::random_internal_randen_hwaes
  absl::random_internal_randen_slow
  absl::random_internal_randen
  absl::random_internal_seed_material
  absl::random_seed_gen_exception
  absl::random_seed_sequences
  absl::raw_hash_set
  absl::raw_logging_internal
  absl::scoped_set_env
  absl::spinlock_wait
  absl::stacktrace
  absl::statusor
  absl::status
  absl::strerror
  absl::str_format_internal
  absl::strings_internal
  absl::strings
  absl::symbolize
  absl::synchronization
  absl::throw_delegate
  absl::time
  absl::time_zone
  )

# libprim
pkg_check_modules(libprim REQUIRED IMPORTED_TARGET libprim)
  get_target_property(
  LIBPRIM_INC
  PkgConfig::libprim
  INTERFACE_INCLUDE_DIRECTORIES
  )

# libcolhash
pkg_check_modules(libcolhash REQUIRED IMPORTED_TARGET libcolhash)
  get_target_property(
  LIBCOLHASH_INC
  PkgConfig::libcolhash
  INTERFACE_INCLUDE_DIRECTORIES
  )

# libfactory
pkg_check_modules(libfactory REQUIRED IMPORTED_TARGET libfactory)
  get_target_property(
  LIBFACTORY_INC
  PkgConfig::libfactory
  INTERFACE_INCLUDE_DIRECTORIES
  )

# librnd
pkg_check_modules(librnd REQUIRED IMPORTED_TARGET librnd)
  get_target_property(
  LIBRND_INC
  PkgConfig::librnd
  INTERFACE_INCLUDE_DIRECTORIES
  )

# libmut
pkg_check_modules(libmut REQUIRED IMPORTED_TARGET libmut)
  get_target_property(
  LIBMUT_INC
  PkgConfig::libmut
  INTERFACE_INCLUDE_DIRECTORIES
  )

# libbits
pkg_check_modules(libbits REQUIRED IMPORTED_TARGET libbits)
  get_target_property(
  LIBBITS_INC
  PkgConfig::libbits
  INTERFACE_INCLUDE_DIRECTORIES
  )

# libstrop
pkg_check_modules(libstrop REQUIRED IMPORTED_TARGET libstrop)
  get_target_property(
  LIBSTROP_INC
  PkgConfig::libstrop
  INTERFACE_INCLUDE_DIRECTORIES
  )

# libfio
pkg_check_modules(libfio REQUIRED IMPORTED_TARGET libfio)
  get_target_property(
  LIBFIO_INC
  PkgConfig::libfio
  INTERFACE_INCLUDE_DIRECTORIES
  )

# libsettings
pkg_check_modules(libsettings REQUIRED IMPORTED_TARGET libsettings)
  get_target_property(
  LIBSETTINGS_INC
  PkgConfig::libsettings
  INTERFACE_INCLUDE_DIRECTORIES
  )

add_executable(
  paragraph-stencil
  ${PROJECT_SOURCE_DIR}/src/stencil/main.cc
  )

set_target_properties(
  paragraph-stencil
  PROPERTIES
  LINKER_LANGUAGE
  CXX
  )

target_include_directories(
  paragraph-stencil
  PUBLIC
  ${PROJECT_SOURCE_DIR}/src
  ${ZLIB_INC}
  ${TCLAP_INC}
  ${NLOHMANN_JSON_INC}
  ${PARAGRAPH_INC}
  ${PROTOBUF_INC}
  ${ABSL_BASE_INC}
  ${LIBPRIM_INC}
  ${LIBCOLHASH_INC}
  ${LIBFACTORY_INC}
  ${LIBRND_INC}
  ${LIBMUT_INC}
  ${LIBBITS_INC}
  ${LIBSTROP_INC}
  ${LIBFIO_INC}
  ${LIBSETTINGS_INC}
  )

target_link_libraries(
  paragraph-stencil
  PkgConfig::zlib
  PkgConfig::nlohmann_json
  PkgConfig::paragraph
  PkgConfig::protobuf
  PkgConfig::libprim
  PkgConfig::libcolhash
  PkgConfig::libfactory
  PkgConfig::librnd
  PkgConfig::libmut
  PkgConfig::libbits
  PkgConfig::libstrop
  PkgConfig::libfio
  PkgConfig::libsettings
  PkgConfig::tclap
  "${ABSL_LIBS}"
  PkgConfig::protobuf
  PkgConfig::paragraph
  )

include(GNUInstallDirs)

install(
  TARGETS
  paragraph-stencil
  )
