cmake_minimum_required(VERSION 2.8.12)
project(libtermkey)

find_package(PkgConfig)
pkg_check_modules(UNIBILIUM REQUIRED IMPORTED_TARGET unibilium)

add_definitions(-D _CRT_SECURE_NO_WARNINGS)
add_definitions(-DHAVE_UNIBILIUM)
if(NOT MSVC)
  add_compile_options(-std=c99)
endif()

include_directories(${PROJECT_BINARY_DIR}/t)
include_directories(SYSTEM ${UNIBILIUM_INCLUDE_DIRS})

file(STRINGS Makefile Makefile)
foreach(line IN LISTS Makefile)
  if (line MATCHES "VERSION_MAJOR=([0-9]+)")
    set(VERSION_MAJOR ${CMAKE_MATCH_1})
  elseif(line MATCHES "VERSION_MINOR=([0-9]+)")
    set(VERSION_MINOR ${CMAKE_MATCH_1})
  endif()
endforeach(line)

configure_file(termkey.h.in ${PROJECT_SOURCE_DIR}/termkey.h @ONLY)

add_library(termkey-static termkey.c driver-csi.c driver-ti.c)
set_target_properties(termkey-static PROPERTIES
  PUBLIC_HEADER ${PROJECT_SOURCE_DIR}/termkey.h
  OUTPUT_NAME termkey)
target_link_libraries(termkey-static PkgConfig::UNIBILIUM)

add_library(termkey-shared SHARED termkey.c driver-csi.c driver-ti.c)
set_target_properties(termkey-shared PROPERTIES
  PUBLIC_HEADER ${PROJECT_SOURCE_DIR}/termkey.h
  VERSION "${VERSION_MAJOR}.${VERSION_MINOR}"
  OUTPUT_NAME termkey)
target_link_libraries(termkey-shared PkgConfig::UNIBILIUM)

set(LIBDIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(INCDIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR})
configure_file(termkey.pc.in termkey.pc @ONLY)

include(GNUInstallDirs)
install(FILES ${PROJECT_BINARY_DIR}/termkey.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
install(TARGETS termkey-static termkey-shared
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

enable_testing()
file(GLOB TESTSOURCES "t/[0-9]*.c")
foreach(f ${TESTSOURCES})
  get_filename_component(t ${f} NAME_WE)
  if(${t} STREQUAL 05read)
    continue()
  endif()

  add_executable("test_${t}" ${f} t/taplib.c)
  target_link_libraries("test_${t}" termkey-static)
  add_test("${t}" "test_${t}")
endforeach()
