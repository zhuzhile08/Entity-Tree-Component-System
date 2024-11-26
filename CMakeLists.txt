cmake_minimum_required(VERSION 3.24.0)
set(CMAKE_CXX_STANDARD 23)

project(EntityTreeComponentSystem VERSION 2.1.1)

option(ETCS_STATIC "Build ETCS statically" ON)

# Check if LSD is available and if the simplified versions of the unordered sparse set have to be used
if(NOT TARGET LyraStandardLibrary)
	message(WARNING "Please add the Lyra Standard Library as a dependency!")
	add_compile_definitions(USE_STANDARD_LIBRARY)
	include_directories(PRIVATE "${CMAKE_SOURCE_DIR}/include/Deps/")
endif()


option (ETCS_ENABLE_COMPONENTS_EXT "Use the already implemented components as an extension" OFF)

# Check if the components extension in the ETCS/Components folder can be enabled, i.e. if GLM exists
if(ETCS_ENABLE_COMPONENTS_EXT)
	if(TARGET glm::glm)
		set(ETCS_LINKED_LIBRARIES LyraStandardLibrary::Headers glm::glm)

		add_compile_definitions(USE_COMPONENTS_EXT)
	else()
		message(FATAL_ERROR "The components extension cannot be enabled because glm wasn't found as a dependency!")
	endif()
else()
	set(ETCS_LINKED_LIBRARIES LyraStandardLibrary::Headers )
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
	"src/World.cpp"
	"src/Entity.cpp"
	"src/EntityRange.cpp"
	"src/EntityQuery.cpp"
	"src/Detail/ArchetypeManager.cpp"
	"src/Detail/EntityManager.cpp"
	"src/Components/Transform.cpp"
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
		target_link_libraries(EntityTreeComponentSystem-static PRIVATE "${ETCS_LINKED_LIBRARIES}")
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
		target_link_libraries(EntityTreeComponentSystem-static PRIVATE "${ETCS_LINKED_LIBRARIES}")
	endif()

	target_precompile_headers(EntityTreeComponentSystem-shared PUBLIC "${ETCS_PRECOMPILED_HEADERS}")
endif()


# Register headers as a target
add_library(EntityTreeComponenetSystem_Headers INTERFACE)
add_library(ETCS::Headers ALIAS EntityTreeComponenetSystem_Headers)
target_include_directories(EntityTreeComponenetSystem_Headers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
