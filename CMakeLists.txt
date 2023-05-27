cmake_minimum_required(VERSION 3.0.0)
project(Midas VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(scripts/minhook.cmake)

add_library(Midas SHARED src/main/main.cpp src/wiz_structs/common.h)

set_target_properties(Midas PROPERTIES OUTPUT_NAME "WizardCommandLineCSR")

target_link_libraries(Midas PRIVATE minhook)
target_include_directories(Midas PRIVATE ${minhook_INCLUDE_DIRS})
target_include_directories(Midas PRIVATE src)
target_include_directories(Midas PRIVATE deps)
target_include_directories(Midas PRIVATE deps/LuaBridge3/Tests/Lua)
target_include_directories(Midas PRIVATE deps/LuaBridge3/Source)

target_include_directories(Midas PUBLIC deps/LuaBridge3/ThirdParty/luau/VM/include)
target_include_directories(Midas PUBLIC deps/LuaBridge3/ThirdParty/luau/VM/src)

add_subdirectory(deps/LuaBridge3/ThirdParty/luau)
target_link_libraries(Midas PRIVATE Luau.Compiler)
target_link_libraries(Midas PRIVATE Luau.VM)
target_link_libraries(Midas PRIVATE Luau.Ast)

add_custom_command(
    TARGET Midas
    POST_BUILD
    COMMAND cmd /c ${CMAKE_CURRENT_SOURCE_DIR}/scripts/post-build.bat
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    
    COMMENT "Launching W101"
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)