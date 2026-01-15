cmake_minimum_required(VERSION 3.16)
project(arkanoid)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SOURCES
    
    libs/glad/src/glad.cpp
    libs/imgui/imgui.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_widgets.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/backends/imgui_impl_glfw.cpp
    libs/imgui/backends/imgui_impl_opengl3.cpp

)

set(HEADERS
    libs/glad/include/KHR/khrplatform.h
    libs/glad/include/glad/glad.h
)


include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/backends
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS}

	src/main.cpp
    src/utils/stats.h
    src/utils/stats.cpp
    src/utils/math.h
    src/utils/math.cpp
    src/config.h
    src/utils/random.h
    src/utils/random.cpp

    src/gl/shader.h
    src/gl/shader.cpp
    src/gl/buffer.h
    src/gl/buffer.cpp
    src/gl/vertexarray.h
    src/gl/vertexarray.cpp
    src/geometry/mesh.h
    src/geometry/mesh.cpp
    src/geometry/meshfactory.h

    src/oop/gameobjects.h
    src/oop/gameobjects.cpp
    src/oop/oopgame.h
    src/oop/oopgame.cpp

    src/ecs/ecsgame.h
    src/ecs/ecsgame.cpp
    src/ecs/component/components.h
    src/ecs/system/inputsystem.h
    src/ecs/system/inputsystem.cpp
    src/ecs/entity/entitymanager.h 
    src/ecs/entity/entitymanager.cpp
    src/ecs/system/powerupsystem.h 
    src/ecs/system/powerupsystem.cpp
    src/ecs/system/physicssystem.h 
    src/ecs/system/physicssystem.cpp
    src/ecs/system/gamelogicsystem.h 
    src/ecs/system/gamelogicsystem.cpp
    src/ecs/system/rendersystem.h 
    src/ecs/system/rendersystem.cpp
)

if(WIN32)
# DLLs to copy
    set(DLL_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw/glfw3.dll
    
    )
    foreach(dll ${DLL_FILES})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${dll} $<TARGET_FILE_DIR:${PROJECT_NAME}>

    )
    endforeach()
    # Windows linking
    target_link_libraries(${PROJECT_NAME}
        ${CMAKE_CURRENT_BINARY_DIR}/../../../libs/glfw/glfw3dll.lib
 
        opengl32
        glu32
    )

    

else() # Unix/Linux/Mac
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)
 

    target_link_libraries(${PROJECT_NAME}
        glfw
        ${OPENGL_LIBRARIES}

    )
endif()

set(DISTFILES
    src/shaders/arkanoid.vert
    src/shaders/arkanoid.frag
)


file(COPY
    src/shaders/
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders
)