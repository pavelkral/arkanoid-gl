cmake_minimum_required(VERSION 3.16)
project(arkanoid)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SOURCES
    main.cpp
    ../libs/glad/src/glad.cpp
    ../libs/imgui/imgui.cpp
    ../libs/imgui/imgui_draw.cpp
    ../libs/imgui/imgui_widgets.cpp
    ../libs/imgui/imgui_tables.cpp
    ../libs/imgui/backends/imgui_impl_glfw.cpp
    ../libs/imgui/backends/imgui_impl_opengl3.cpp

)

set(HEADERS
    ../libs/glad/include/KHR/khrplatform.h
    ../libs/glad/include/glad/glad.h
)

set(DISTFILES
    ../.gitignore
    ../LICENSE
    ../README.md
    shaders/arkanoid.frag
    shaders/arkanoid.vert
 
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/../libs/imgui/backends
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS}
    utils/stats.h
    utils/stats.cpp
    utils/math.h
    utils/math.cpp
    config.h
    utils/random.h
    utils/random.cpp

    gl/shader.h
    gl/shader.cpp
    gl/buffer.h
    gl/buffer.cpp
    gl/vertexarray.h
    gl/vertexarray.cpp
    geometry/mesh.h
    geometry/mesh.cpp
    geometry/meshfactory.h

    oop/gameobjects.h
    oop/gameobjects.cpp
    oop/oopgame.h
    oop/oopgame.cpp

    ecs/ecsgame.h
    ecs/ecsgame.cpp
    ecs/component/components.h
    ecs/system/inputsystem.h
    ecs/system/inputsystem.cpp
    ecs/entity/entitymanager.h ecs/entity/entitymanager.cpp
    ecs/system/powerupsystem.h ecs/system/powerupsystem.cpp
    ecs/system/physicssystem.h ecs/system/physicssystem.cpp
    ecs/system/gamelogicsystem.h ecs/system/gamelogicsystem.cpp
    ecs/system/rendersystem.h ecs/system/rendersystem.cpp

)

if(WIN32)
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

    # DLLs to copy
    set(DLL_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../libs/glfw/glfw3.dll
    
    )

else() # Unix/Linux/Mac
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)
 

    target_link_libraries(${PROJECT_NAME}
        glfw
        ${OPENGL_LIBRARIES}

    )
endif()

# Copy shader files to output
foreach(file IN LISTS DISTFILES)
    configure_file(${file} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${file} COPYONLY)
endforeach()
