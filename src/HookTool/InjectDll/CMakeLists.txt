# CMakeList.txt : CMake project for ScanSaveTool, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(project_name "InjectDll") ## rename your project here

project(${project_name})

set(CMAKE_CXX_STANDARD "11")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS OFF)

include_directories(
"${CMAKE_CURRENT_SOURCE_DIR}"
"${CMAKE_SOURCE_DIR}/vendor/ui"
"${CMAKE_SOURCE_DIR}/vendor/log"
"${CMAKE_SOURCE_DIR}/vendor/json"
"${CMAKE_SOURCE_DIR}/vendor/common"
)
IF(CMAKE_CL_64)
    set(CMAKE_PLATFORM_TYPE x64)
ELSE(CMAKE_CL_64)
    set(CMAKE_PLATFORM_TYPE x86)
ENDIF(CMAKE_CL_64)

message(STATUS "platform_type=${CMAKE_PLATFORM_TYPE},bulid_type=${CMAKE_BUILD_TYPE}")

# 把第三方库的.a文件和.so文件放到代码的lib文件夹
link_directories(
"${CMAKE_CURRENT_BINARY_DIR}"
)
# Add source to this project's executable.

if(MSVC)
set(PPS_RC_FILE "")
else(MSVC)
set(PPS_RC_FILE "")
endif(MSVC)
file(GLOB SRCS "*.cpp" "*.c")
add_library(${project_name} SHARED ${SRCS} ${PPS_RC_FILE})

if (MSVC)
	list(APPEND project_name_cflags /W4)
	list(APPEND project_name_defines WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0601 _CRT_SECURE_NO_WARNINGS)
	MESSAGE(STATUS "CMAKE_BUILD_TYPE=" ${CMAKE_BUILD_TYPE})

	set(CompilerFlags
		CMAKE_CXX_FLAGS
		CMAKE_CXX_FLAGS_DEBUG
		CMAKE_CXX_FLAGS_RELEASE
		CMAKE_C_FLAGS
		CMAKE_C_FLAGS_DEBUG
		CMAKE_C_FLAGS_RELEASE
		)
	foreach(CompilerFlag ${CompilerFlags})
		string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
	endforeach()
	
	target_compile_definitions(${project_name}
							PRIVATE ${project_name_defines}
							)
	target_compile_options(${project_name}
							PRIVATE ${project_name_cflags}
	)
endif(MSVC)

target_link_libraries(${project_name}
        PRIVATE wsock32
        PRIVATE ws2_32
)

if(MSVC)
SET_TARGET_PROPERTIES(${project_name} PROPERTIES LINK_FLAGS ${ADMIN_LEVEL})
endif(MSVC)
# TODO: Add tests and install targets if needed.