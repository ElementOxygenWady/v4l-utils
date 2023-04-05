#* Author: Wady <iswady@163.com>
#* Date:   Thu Aug 25 12:00:00 2022

cmake_minimum_required(VERSION 3.16.3)
project(V4L2 VERSION 1.24.1 LANGUAGES C CXX)

if(EXTRA_CMAKE)
    include("${EXTRA_CMAKE}")
    message(STATUS "Including extra cmake from ${EXTRA_CMAKE}")
endif()

# Used to configure generated files.
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)

# Basic Information.
MESSAGE(STATUS "CMake version: " ${CMAKE_VERSION})
MESSAGE(STATUS "CMake system name: " ${CMAKE_SYSTEM_NAME})
MESSAGE(STATUS "CMake system processor: " ${CMAKE_SYSTEM_PROCESSOR})

# Specifying a search path for CMake modules to be loaded by the the include() or find_package() commands before checking the default modules that come with CMake.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Provides install directory variables as defined by the GNU Coding Standards.
# GNU Coding Standards Reference 'https://www.gnu.org/prep/standards/html_node/Directory-Variables.html'.
include(GNUInstallDirs)

# Check whether the C compiler supports a given flag.
include(CheckCCompilerFlag)

include(cmake/CMakeUtils.cmake)

# Provides an option that the user can optionally select.
option(ENABLE_SHARED "Build shared library." OFF)
option(ENABLE_STATIC "Build static library." ON)
option(ENABLE_PIC "Enable position-independent code." ON)

if(ENABLE_PIC)
    check_c_compiler_flag("-fpic" SUPPORT_fpic)
    check_c_compiler_flag("-fPIC" SUPPORT_fPIC)
endif()

if((NOT ENABLE_SHARED) AND (NOT ENABLE_STATIC))
    message(FATAL_ERROR "Please enable shared or static library.")
endif()

set(INTERMEDIATE_DIR "${CMAKE_BINARY_DIR}/Intermediate")

set(CONFIGURE "${CMAKE_CURRENT_LIST_DIR}/configure")

set(CONFIGURE_ARCH_TXT "${INTERMEDIATE_DIR}/arch.txt")
set(CONFIGURE_LOG_FILE "${INTERMEDIATE_DIR}/config.log")
set(CONFIGURE_MAKEFILE "${INTERMEDIATE_DIR}/Makefile")

set(CONFIGURE_SUCCESS_INFO "configure: exit 0")

set(COMPILING_ARCH "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")

if(EXISTS "${CONFIGURE_ARCH_TXT}")
    file(READ "${CONFIGURE_ARCH_TXT}" ACTIVATED_ARCH)
endif()

if("${COMPILING_ARCH}" STREQUAL "${ACTIVATED_ARCH}")
    set(ENABLE_CLEAN OFF)

    message(STATUS "The same processor architecture (${COMPILING_ARCH}), checking configure result ...")

    if(EXISTS "${CONFIGURE_LOG_FILE}")
        file(STRINGS "${CONFIGURE_LOG_FILE}" ENABLE_CONFIGURE REGEX "${CONFIGURE_SUCCESS_INFO}")
        if("${ENABLE_CONFIGURE}" STREQUAL "${CONFIGURE_SUCCESS_INFO}")
            set(ENABLE_CONFIGURE OFF)
        else()
            set(ENABLE_CONFIGURE ON)
        endif()
    else()
        set(ENABLE_CONFIGURE ON)
    endif()
else()
    set(ENABLE_CLEAN ON)
    set(ENABLE_CONFIGURE ON)
    file(WRITE "${CONFIGURE_ARCH_TXT}" "${COMPILING_ARCH}")
endif()

file(GLOB SUPPORT_CLEAN "${CONFIGURE_MAKEFILE}")

utils_write_script("#!/bin/bash" TRUNCATE WRITE_ALL)

if(CMAKE_CROSSCOMPILING)
    string(REGEX REPLACE "^(.+)/.+$" "\\1" COMPILER_PATH ${CMAKE_C_COMPILER})
    utils_write_script("export PATH=\"${COMPILER_PATH}:$PATH\"" WRITE_ALL)

    string(REGEX REPLACE "^.+/(.+)-gcc$" "\\1" COMPILER_HOST ${CMAKE_C_COMPILER})
    utils_write_script("export COMPILER_HOST=\"${COMPILER_HOST}\"" WRITE_ALL)

    utils_write_script("export CFLAGS=" WRITE_ALL)
    utils_write_script("export CPPFLAGS=" WRITE_ALL)
    utils_write_script("export CXXFLAGS=" WRITE_ALL)
    utils_write_script("export LDFLAGS=" WRITE_ALL)
endif()

utils_write_script("export shared_extension=\".so\"" WRITE_ALL)

set(MAKE_CLEAN "${CMAKE_MAKE_PROGRAM} -C ${INTERMEDIATE_DIR} distclean")
set(SKIP_CLEAN "# There is nothing to be cleaned.")

if(SUPPORT_CLEAN)
    utils_write_script("${MAKE_CLEAN}" WRITE_CLEAN)
else()
    utils_write_script("${SKIP_CLEAN}" WRITE_CLEAN)
endif()

if(SUPPORT_CLEAN AND ENABLE_CLEAN)
    utils_write_script("${MAKE_CLEAN}" WRITE_COMPILE)
else()
    utils_write_script("${SKIP_CLEAN}" WRITE_COMPILE)
endif()

if(ENABLE_PIC)
    if(SUPPORT_fpic)
        utils_write_script("export CFLAGS=\"$CFLAGS -fpic\"" WRITE_COMPILE)
    endif()
    if(SUPPORT_fPIC)
        utils_write_script("export CFLAGS=\"$CFLAGS -fPIC\"" WRITE_COMPILE)
    endif()
endif()

if(ENABLE_CONFIGURE)
    utils_write_script("cd ${INTERMEDIATE_DIR}" WRITE_COMPILE)

    set(CONFIGURE_COMMAND "${CONFIGURE}")

    list(APPEND CONFIGURE_COMMAND "--prefix=${CMAKE_INSTALL_PREFIX}")
    list(APPEND CONFIGURE_COMMAND "--includedir=${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME_LOWER}")
    list(APPEND CONFIGURE_COMMAND "--libdir=${INTERMEDIATE_DIR}/install")

    if(CMAKE_CROSSCOMPILING)
        list(APPEND CONFIGURE_COMMAND "--host=$COMPILER_HOST")
    endif()

    list(APPEND CONFIGURE_COMMAND "--enable-shared=yes")
    list(APPEND CONFIGURE_COMMAND "--enable-dyn-libv4l=yes")
    list(APPEND CONFIGURE_COMMAND "--enable-static=yes")

    list(APPEND CONFIGURE_COMMAND "--enable-doxygen-doc=no")
    list(APPEND CONFIGURE_COMMAND "--enable-doxygen-dot=no")
    list(APPEND CONFIGURE_COMMAND "--enable-doxygen-html=no")
    list(APPEND CONFIGURE_COMMAND "--enable-doxygen-ps=no")
    list(APPEND CONFIGURE_COMMAND "--enable-doxygen-pdf=no")
    list(APPEND CONFIGURE_COMMAND "--enable-libdvbv5=no")
    list(APPEND CONFIGURE_COMMAND "--enable-v4l-utils=no")
    list(APPEND CONFIGURE_COMMAND "--enable-v4l2-compliance-libv4l=no")
    list(APPEND CONFIGURE_COMMAND "--enable-v4l2-tracer=no")
    list(APPEND CONFIGURE_COMMAND "--enable-v4l2-ctl-libv4l=no")
    list(APPEND CONFIGURE_COMMAND "--enable-v4l2-ctl-stream-to=no")
    list(APPEND CONFIGURE_COMMAND "--enable-qv4l2=no")
    list(APPEND CONFIGURE_COMMAND "--enable-qvidcap=no")
    list(APPEND CONFIGURE_COMMAND "--enable-bpf=no")

    string(REPLACE ";" " " CONFIGURE_COMMAND "${CONFIGURE_COMMAND}")

    utils_write_script("${CONFIGURE_COMMAND}" WRITE_COMPILE)
else()
    utils_write_script("# There is nothing to be configured." WRITE_COMPILE)
endif()

utils_write_script("${CMAKE_MAKE_PROGRAM} -C ${INTERMEDIATE_DIR}" WRITE_COMPILE)

utils_write_script("${CMAKE_MAKE_PROGRAM} -C ${INTERMEDIATE_DIR} install" WRITE_INSTALL)

add_custom_target(Customize_compile
    COMMAND "/bin/bash" "${COMPILE_SCRIPT}"
    COMMENT "Running `compile` ..."
    VERBATIM
)

add_custom_target(Customize_clean
    COMMAND "/bin/bash" "${CLEAN_SCRIPT}"
    COMMENT "Running `clean` ..."
    VERBATIM
)

add_custom_target(Customize_install
    COMMAND "/bin/bash" "${INSTALL_SCRIPT}"
    COMMAND "${CMAKE_COMMAND}" --build ${CMAKE_BINARY_DIR} --target install
    COMMENT "Running `install` ..."
    VERBATIM
)

set(CONFIG_PACKAGE_LOCATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/cmake")

set(SHARED_LIBRARY_NAMES "v4l2;v4lconvert")
set(STATIC_LIBRARY_NAMES "${SHARED_LIBRARY_NAMES}")

configure_file(cmake/Config.cmake.in "${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake" @ONLY)
install(FILES
    "${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    DESTINATION ${CONFIG_PACKAGE_LOCATION}
)

if(ENABLE_SHARED)
    foreach(LIB ${SHARED_LIBRARY_NAMES})
        install(FILES
            "${INTERMEDIATE_DIR}/install/lib${LIB}.so"
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        )
    endforeach()
endif()

if(ENABLE_STATIC)
    foreach(LIB ${STATIC_LIBRARY_NAMES})
        install(FILES
            "${INTERMEDIATE_DIR}/install/lib${LIB}.a"
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        )
    endforeach()
endif()
