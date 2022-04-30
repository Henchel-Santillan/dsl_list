cmake_minimum_required(VERSION 3.3)
project(dsl_list VERSION 1.0.0 LANGUAGES CXX)

include(GNUInstallDirs)

add_library(dsl_list INTERFACE)

# Use dsl::list as target
add_library(dsl::list ALIAS dsl_list)
target_include_directories(dsl_list INTERFACE 
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

list(APPEND headers "${CMAKE_CURRENT_SOURCE_DIR}/include/doubly_linked_list.h"
                    "${CMAKE_CURRENT_SOURCE_DIR}/include/list.h"
                    "${CMAKE_CURRENT_SOURCE_DIR}/include/list_base.h"
                    "${CMAKE_CURRENT_SOURCE_DIR}/include/singly_linked_list.h")
target_sources(dsl_list INTERFACE "$<BUILD_INTERFACE:${headers}>")

# Add GoogleTest
include(FetchContent)
FetchContent_Declare(googletest 
                     URL https://github.com/google/googletest/archive/8ded48c37be09d8cc3665af1b414c5d53c0862e7.zip)     # Commit 3893, Apr. 28

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


if (${CMAKE_VERSION} VERSION_GREATER "3.2")
        include (CMakePackageConfigHelpers)
        install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include"
                DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

        configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/dsl_listConfig.cmake.in"
                                      "${CMAKE_CURRENT_BINARY_DIR}/dsl_listConfig.cmake"
                                      INSTALL_DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/cmake/dsl_list")

        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/dsl_listConfig.cmake"
                DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/cmake/dsl_list")

        export(TARGETS dsl_list NAMESPACE dsl:: FILE "${CMAKE_CURRENT_BINARY_DIR}/dsl_listTargets.cmake")

        install(TARGETS dsl_list 
                EXPORT dsl_listTargets)
        install(EXPORT dsl_listTargets
                NAMESPACE dsl::
                DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/cmake/dsl_list")
        
        set(CMAKE_SIZEOF_VOID_P_BACKUP ${CMAKE_SIZEOF_VOID_P})
        unset(CMAKE_SIZEOF_VOID_P)

        write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/dsl_listConfigVersion.cmake"
                                        COMPATIBILITY SameMajorVersion)
        set(CMAKE_SIZEOF_VOID_P ${CMAKE_SIZEOF_VOID_P_BACKUP})

        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/dsl_listConfigVersion.cmake"
                DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/cmake/dsl_list")
endif()
