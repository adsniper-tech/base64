message ("[+] Use custom C/C++ Flags")


# SPDLOG config
add_definitions(-DSPDLOG_FMT_EXTERNAL -DSPDLOG_ENABLE_SYSLOG)


#    -flto=8 -fuse-linker-plugin
#    -Wl,--eh-frame-hdr

set(COMMON_FLAGS "\
    -march=corei7-avx \
    -m64 \
    -mavx \
    -msse4.1 \
    -msse4.2 \
    -Wall \
    -Wextra \
    -Werror \
    -Wpedantic \
    -Wformat-security \
    -fno-builtin-malloc \
    -fno-builtin-calloc \
    -fno-builtin-realloc \
    -fno-builtin-free \
    ")


set(COMMON_FLAGS_DEBUG "\
    -O0 \
    -g \
    ")

set(COMMON_FLAGS_RELEASE "\
    -DNDEBUG \
    -Ofast \
    -funroll-loops \
    -fomit-frame-pointer \
    -Wno-misleading-indentation \
    ")


#if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(COMMON_FLAGS "\
        ${COMMON_FLAGS} \
         -pthread \
        ")

    set(COMMON_FLAGS_DEBUG "\
        ${COMMON_FLAGS_DEBUG} \
        -fsanitize=address \
        -fsanitize=undefined \
        -fsanitize=leak \
        -fstack-protector \
        -fuse-ld=gold \
        ")
endif()



set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "${COMMON_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_RELEASE "${COMMON_FLAGS_RELEASE}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${COMMON_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${COMMON_FLAGS_RELEASE}")
