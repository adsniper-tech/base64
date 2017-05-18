cmake_minimum_required( VERSION 3.4.0 )

project(libbase64)

set(LIB base64)

include_directories(include)


set(LIB_HEADERS
    lib/config.h
    lib/codecs.h
    include/libbase64.h
    lib/arch/sse2/compare_macros.h
   )

set(LIB_SOURCES
    lib/codec_choose.c
    lib/lib.c
    lib/arch/generic/codec.c
    lib/arch/neon32/codec.c
    lib/arch/neon64/codec.c
    lib/arch/avx/codec.c
    lib/arch/avx2/codec.c
    lib/arch/ssse3/codec.c
    lib/arch/sse41/codec.c
    lib/arch/sse42/codec.c
   )


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(SniperC_CXX_FLAGS)
include(SniperC++17)


#Library
add_library(${LIB} STATIC ${LIB_HEADERS} ${LIB_SOURCES})
target_compile_options(${LIB} PRIVATE -Wno-implicit-fallthrough)


#Tests
include(CTest)
enable_testing()

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

find_package(GMock REQUIRED)
include_directories(${GMOCK_INCLUDE_DIRS})

add_executable (${LIB}Test unittest/Base64Test.cpp)
target_link_libraries(${LIB}Test ${LIB})
target_link_libraries(${LIB}Test ssl crypto)
target_link_libraries(${LIB}Test ${GTEST_LIBRARIES} ${GMOCK_BOTH_LIBRARIES})
add_test(NAME ${LIB}Test COMMAND ${LIB}Test --gtest_output=xml:gtestresults.xml --gtest_color=yes)


#Install
install(TARGETS ${LIB} ARCHIVE DESTINATION lib CONFIGURATIONS Release)
install(FILES include/libbase64.h DESTINATION include)
