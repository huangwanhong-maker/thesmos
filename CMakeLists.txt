# thesmos: institutions computed with. Parties of declared kinds and their origins; mandates whose chains only narrow, with
# envelopes charged along the chain; acts of declared force with a typed envelope and a free body; commitments and their
# life, set-off and novation; constitutions as data in Ostrom's seven kinds of rules, admitted by form; decision rules with
# their unit of count; the chronicle, a sealed record whose fold gives the relational state; the facets of a norm; a community's
# holdings and the norms of access to them, and the briefing a party reads from the record; passports issued by a home and verified
# anywhere from the document (decision 0007); a taxonomy of behaviour, its declared forms of extension and the behaviour journal
# joined to the chronicle (decision 0008), the taxonomy admitted and revised on kategoria's ontologies; presentation and recognition
# (decision 0009); the taxonomy in levels, disclosure agreements and epistemic behaviour recorded by agreement (decision 0010); items, routes and rulings (decision 0011); posted work (decision 0012); transmission by operatives, counts by provider and by weight, and compacts with reservations and withdrawal (decision 0013).
#
# Every module is a header-only target thesmos::<module>. thesmos uses archeion (the sealed journal), symploke (JSON, read
# exactly), morphe (exact rationals), apodeixis (verdicts), calx (the report of its checks), and kategoria in its checks (the
# schema of the ground stated as categories): inside the infrastructure, the siblings under foundations/, each added here
# when no larger project has added it.

cmake_minimum_required(VERSION 3.20)
project(thesmos VERSION 0.1.0 LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(THESMOS_TOP_LEVEL ON)
else()
  set(THESMOS_TOP_LEVEL OFF)
endif()
option(THESMOS_CHECKS "Build thesmos's checks" ${THESMOS_TOP_LEVEL})

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

# thesmos_sibling(<directory> <a target it defines>): the sibling library, added once.
function(thesmos_sibling directory target)
  if(NOT TARGET ${target})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../${directory}/CMakeLists.txt)
      add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../${directory} ${CMAKE_BINARY_DIR}/${directory})
    else()
      message(FATAL_ERROR "thesmos needs ${directory}: build it inside the infrastructure (foundations/${directory}).")
    endif()
  endif()
endfunction()

thesmos_sibling(calx calx::linalg)
thesmos_sibling(morphe morphe::formal)
thesmos_sibling(apodeixis apodeixis::kernel)
thesmos_sibling(archeion archeion::fixity)
thesmos_sibling(symploke symploke::jsonld)
thesmos_sibling(kategoria kategoria::script)
if(THESMOS_CHECKS)
  enable_testing()
endif()

# thesmos_module(<name> [LIBRARIES ...] [CHECK_LIBRARIES ...]): the target thesmos::<name> and, with the checks,
# check_thesmos_<stem> for each tests/check_<stem>.cpp, told where the schema is kept (THESMOS_DATA).
function(thesmos_module name)
  cmake_parse_arguments(ARG "" "" "LIBRARIES;CHECK_LIBRARIES" ${ARGN})
  add_library(thesmos_${name} INTERFACE)
  add_library(thesmos::${name} ALIAS thesmos_${name})
  target_include_directories(thesmos_${name} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/${name}/include)
  target_compile_features(thesmos_${name} INTERFACE cxx_std_20)
  target_link_libraries(thesmos_${name} INTERFACE ${ARG_LIBRARIES})
  if(THESMOS_CHECKS)
    file(GLOB checks CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${name}/tests/check_*.cpp)
    foreach(source ${checks})
      get_filename_component(stem ${source} NAME_WE)
      string(REPLACE "check_" "" stem ${stem})
      calx_add_check(check_thesmos_${stem} SOURCES ${source} LIBRARIES thesmos::${name} calx::random ${ARG_CHECK_LIBRARIES})
      target_compile_definitions(check_thesmos_${stem} PRIVATE THESMOS_DATA="${CMAKE_CURRENT_SOURCE_DIR}")
    endforeach()
  endif()
endfunction()

# The modules in their order (decision 0003).
thesmos_module(kernel LIBRARIES apodeixis::kernel)
thesmos_module(mandates LIBRARIES thesmos::kernel)
thesmos_module(acts LIBRARIES thesmos::mandates)
thesmos_module(behaviour LIBRARIES thesmos::acts archeion::fixity symploke::jsonld morphe::formal kategoria::ontologies  # decision 0008
               CHECK_LIBRARIES thesmos::chronicle)
thesmos_module(commitments LIBRARIES thesmos::kernel)
thesmos_module(procedures LIBRARIES thesmos::kernel morphe::arithmetic)
thesmos_module(holdings LIBRARIES thesmos::kernel archeion::fixity symploke::jsonld)  # decision 0006
thesmos_module(work LIBRARIES symploke::jsonld)  # decision 0012
thesmos_module(compacts LIBRARIES archeion::fixity symploke::jsonld)  # decision 0013
thesmos_module(disclosure LIBRARIES thesmos::holdings archeion::fixity symploke::jsonld)  # decision 0010
thesmos_module(passports LIBRARIES thesmos::acts thesmos::holdings archeion::fixity symploke::jsonld apodeixis::kernel)  # decisions 0007, 0009
thesmos_module(constitutions LIBRARIES thesmos::kernel thesmos::holdings thesmos::behaviour thesmos::work thesmos::procedures thesmos::compacts symploke::jsonld)
thesmos_module(chronicle LIBRARIES thesmos::acts thesmos::commitments thesmos::constitutions thesmos::procedures thesmos::passports thesmos::disclosure archeion::fixity
               CHECK_LIBRARIES kategoria::script)
thesmos_module(norms LIBRARIES thesmos::acts morphe::arithmetic)
thesmos_module(crossings LIBRARIES thesmos::chronicle archeion::fixity)
thesmos_module(statements LIBRARIES thesmos::norms thesmos::constitutions morphe::formal CHECK_LIBRARIES thesmos::chronicle)
