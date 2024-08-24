cmake_minimum_required(VERSION 3.24.0)
set(CMAKE_CXX_STANDARD 23)

project(EntityTreeComponentSystem VERSION 1.0.0)


option(BUILD_STATIC "Build ETCS statically" ON)


if(NOT TARGET LyraStandardLibrary)
	message(WARNING "Please add the Lyra Standard Library as a dependency!")
	set(PRIVATE USE_STANDARD_LIBRARY)
	include_directories(PRIVATE "${CMAKE_SOURCE_DIR}/include/Deps/")
endif()


set (ETCS_PRECOMPILED_HEADERS
	<type_traits>
	<algorithm>
	<cstdint>
	<cstdlib>
	<initializer_list>
	<variant>
	<tuple>
	<set>
	<stddef.h>
	<stdexcept>
	<utility>
)

set (ETCS_SOURCE_FILES
	"src/ECS.cpp"
	"src/Detail/Archetype.cpp"
	"src/Detail/Managers.cpp"
)

if(BUILD_STATIC)
	add_library(EntityTreeComponentSystem-static STATIC "${ETCS_SOURCE_FILES}")
	add_library(ETCS::ETCS-static ALIAS EntityTreeComponentSystem-static)

	if (WIN32) 
		target_compile_options(EntityTreeComponentSystem-static PRIVATE /WX)
	else () 
		target_compile_options(EntityTreeComponentSystem-static PRIVATE -Wall -Wextra -Wpedantic)
	endif ()
	
	
	if(TARGET LyraStandardLibrary)
		target_link_libraries(EntityTreeComponentSystem-static PRIVATE LyraStandardLibrary::Headers)
	endif()

	target_precompile_headers(EntityTreeComponentSystem-static PUBLIC "${ETCS_PRECOMPILED_HEADERS}")
else()
	add_library(EntityTreeComponentSystem-shared SHARED "${ETCS_SOURCE_FILES}")
	add_library(ETCS::ETCS-shared ALIAS EntityTreeComponentSystem-shared)

	if (WIN32) 
		target_compile_options(EntityTreeComponentSystem-shared PRIVATE /WX)
	else () 
		target_compile_options(EntityTreeComponentSystem-shared PRIVATE -Wall -Wextra -Wpedantic)
	endif ()
	
	
	if(TARGET LyraStandardLibrary)
		target_link_libraries(EntityTreeComponentSystem-shared PRIVATE LyraStandardLibrary::Headers)
	endif()

	target_precompile_headers(EntityTreeComponentSystem-shared PUBLIC "${ETCS_PRECOMPILED_HEADERS}")
endif()


add_library(EntityTreeComponenetSystem_Headers INTERFACE)
add_library(ETCS::Headers ALIAS EntityTreeComponenetSystem_Headers)
target_include_directories(EntityTreeComponenetSystem_Headers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
