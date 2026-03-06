cmake_minimum_required(VERSION 3.17)

add_link_options(/IGNORE:4099)

project(softManager)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /Zi /Zc:threadSafeInit-")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd /Zi /Zc:threadSafeInit-")

# 为release添加pdb输出文件
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")

# set up options for debugging under the release
# ADD_DEFINITIONS( /Zi )
ADD_DEFINITIONS( /Od )

set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)
add_definitions(-DUILIB_STATIC)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DUILIB_EXPORTS)
add_definitions(-DBUILDING_LIBCURL)
add_definitions(-DHTTP_ONLY)
# add_definitions(-DWIN32_LEAN_AND_MEAN)

IF(DEFINED ENV{GITHUB_RUN_NUMBER})
	SET(BUILD_VERSION $ENV{GITHUB_RUN_NUMBER})
ELSE()
	SET(BUILD_VERSION 0)
ENDIF()

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

include(cmake/get_cpm.cmake)

CPMAddPackage("gh:danielaparker/jsoncons@1.2.0")
CPMAddPackage("gh:wqking/eventpp@0.1.3")
CPMAddPackage("gh:sammycage/lunasvg@3.2.0")

add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/DuiLib)

add_subdirectory(softManager)

add_subdirectory(downloadTool)
