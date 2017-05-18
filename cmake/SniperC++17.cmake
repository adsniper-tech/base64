enable_language(CXX)
include(CheckCXXCompilerFlag)


check_cxx_compiler_flag("-std=c++17" COMPILER_KNOWS_CXX17)
if(COMPILER_KNOWS_CXX17)
    message ("[+] Use C++17")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
else()
    message("Compiler does not support -std=c++17")

    check_cxx_compiler_flag("-std=c++1z" COMPILER_KNOWS_CXX1Z)
    if(COMPILER_KNOWS_CXX1Z)
        message ("[+] Use C++1z")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
    else()
        message(FATAL_ERROR "Compiler does not support -std=c++1z")
    endif()
endif()
