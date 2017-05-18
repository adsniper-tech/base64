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


#Library
add_library(${LIB} STATIC ${LIB_HEADERS} ${LIB_SOURCES})
target_compile_options(${LIB} PRIVATE -Wno-implicit-fallthrough)


#Install
install(TARGETS ${LIB} ARCHIVE DESTINATION lib CONFIGURATIONS Release)
install(FILES include/libbase64.h DESTINATION include)
