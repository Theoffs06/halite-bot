cmake_minimum_required(VERSION 3.16)

project(MyBot LANGUAGES CXX)

set(EXECUTABLE_NAME ${PROJECT_NAME})
set(LIB_NAME hlt)

add_executable(${EXECUTABLE_NAME})

target_compile_features(${EXECUTABLE_NAME} PRIVATE cxx_std_20)

target_sources(${EXECUTABLE_NAME} PRIVATE 
	include/util.hpp
	include/behavior_tree.hpp
	include/spot_manager.hpp
	include/move_manager.hpp
	include/shipyard_ai.hpp
	include/ship_ai.hpp
	src/spot_manager.cpp
	src/move_manager.cpp
	src/shipyard_ai.cpp
	src/ship_ai.cpp
	MyBot.cpp
)

target_include_directories(${EXECUTABLE_NAME} PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_library(${LIB_NAME} STATIC)

target_compile_features(${LIB_NAME} PUBLIC cxx_std_20)

target_sources(${LIB_NAME} PRIVATE
	${LIB_NAME}/command.cpp
	${LIB_NAME}/command.hpp
	${LIB_NAME}/constants.cpp
	${LIB_NAME}/constants.hpp
	${LIB_NAME}/direction.hpp
	${LIB_NAME}/dropoff.cpp
	${LIB_NAME}/dropoff.hpp
	${LIB_NAME}/entity.hpp
	${LIB_NAME}/game.cpp
	${LIB_NAME}/game.hpp
	${LIB_NAME}/game_map.cpp
	${LIB_NAME}/game_map.hpp
	${LIB_NAME}/input.hpp
	${LIB_NAME}/log.cpp
	${LIB_NAME}/log.hpp
	${LIB_NAME}/map_cell.hpp
	${LIB_NAME}/player.cpp
	${LIB_NAME}/player.hpp
	${LIB_NAME}/position.hpp
	${LIB_NAME}/ship.cpp
	${LIB_NAME}/ship.hpp
	${LIB_NAME}/shipyard.hpp
	${LIB_NAME}/types.hpp
)

target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_NAME})
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${LIB_NAME})

foreach(tgt ${EXECUTABLE_NAME} ${LIB_NAME})
    target_compile_options(${tgt} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>
    )
endforeach()

if (MINGW)
    goal_link_libraries(${EXECUTABLE_NAME} -static)
endif()