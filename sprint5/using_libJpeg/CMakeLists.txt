# напишите CMakeLists.txt для сборки трёх программ
# он должен иметь три цели: hmirror, vmirror и sobel

cmake_minimum_required(VERSION 3.11)

project(MyApp CXX)
set(CMAKE_CXX_STANDARD 20)

# воспользуйтесь этой переменной при создании цели
set(IMGLIB_FILES img_lib.h img_lib.cpp
            ppm_image.h ppm_image.cpp)

add_executable(hmirror ${IMGLIB_FILES}
            hmirr.cpp)
add_executable(vmirror  ${IMGLIB_FILES}
            vmirr.cpp)
add_executable(sobel  ${IMGLIB_FILES}
            sobel.cpp)

message(STATUS "Good luck")

#if(CMAKE_SYSTEM_NAME MATCHES "^MINGW")
#    set(SYSTEM_LIBS -lstdc++)
#else()
#    set(SYSTEM_LIBS)
#endif()