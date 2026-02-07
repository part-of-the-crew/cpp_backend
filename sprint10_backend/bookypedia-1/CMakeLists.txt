cmake_minimum_required(VERSION 3.11)
project(bookypedia CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Conan Setup (Standard 'cmake' generator)
include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

# 2. Find Threads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# My library
add_library(mylib STATIC
	src/menu/menu.cpp
	src/menu/menu.h
	src/ui/view.cpp
	src/ui/view.h
	src/app/use_cases.h
	src/app/use_cases_impl.cpp
	src/app/use_cases_impl.h
	src/domain/author.cpp
	src/domain/author.h
	src/domain/author_fwd.h
	src/util/tagged.h
	src/util/tagged_uuid.cpp
	src/util/tagged_uuid.h
	src/postgres/postgres.cpp
	src/postgres/postgres.h
)

target_include_directories(mylib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(mylib PUBLIC
    CONAN_PKG::boost
    Threads::Threads
    CONAN_PKG::libpqxx  # Boost is needed by json_loader
)

# game_server
add_executable(bookypedia
	src/bookypedia.cpp
	src/bookypedia.h
	src/main.cpp
)

target_link_libraries(bookypedia PRIVATE 
    Threads::Threads
    CONAN_PKG::boost
    CONAN_PKG::libpqxx
    mylib
)

target_compile_options(bookypedia PRIVATE
    -Wall -Wextra -Wpedantic
    #-ftime-report
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CXX_FLAGS}")

#Tests
enable_testing()

add_executable(tests
	tests/use_case_tests.cpp
	tests/tagged_uuid_tests.cpp
)

target_link_libraries(tests PRIVATE 
    CONAN_PKG::boost
    CONAN_PKG::catch2
    CONAN_PKG::gtest
    mylib
) 

target_compile_options(tests PRIVATE
    -Wall -Wextra -Wpedantic
    #-ftime-report
)

#include(CTest) creates dir Testing

# Add Conan's module path so we can find 'Catch.cmake' or similar scripts
list(APPEND CMAKE_MODULE_PATH ${CONAN_BUILD_DIRS_CATCH2})

# Try to use Catch2's automatic test discovery
if(EXISTS "${CONAN_BUILD_DIRS_CATCH2}/Catch.cmake")
    include(${CONAN_BUILD_DIRS_CATCH2}/Catch.cmake)
    catch_discover_tests(tests)
else()
    # Fallback if the CMake script isn't found
    message(STATUS "Catch.cmake not found. Registering simple test.")
    add_test(NAME AllTests COMMAND tests)
endif()