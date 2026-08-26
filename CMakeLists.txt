cmake_minimum_required(VERSION 3.10)
Project(MyTinyRender VERSION 1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(CMAKE_AUTOMOC ON)

set(glm_DIR Dependence/glm/share/glm)
set(Eigen3_DIR ${PROJECT_SOURCE_DIR}/Dependence/eigen3)


find_package(Eigen3 REQUIRED)
file(GLOB  UI_FILES ${PROJECT_SOURCE_DIR}/TinyRenderUI/*.ui)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGLWidgets Quick Qml)


#target_link_libraries(demo PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
# ������Щ�������
message(STATUS "��Ŀ��: ${PROJECT_NAME}")
message(STATUS "��ǰԴ��Ŀ¼: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "��ǰ������Ŀ¼: ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "������: ${CMAKE_GENERATOR}")
file(GLOB SOURCE_DIR ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/*.h
					 ${PROJECT_SOURCE_DIR}/TinyRenderSrc/*.cpp  
					 ${PROJECT_SOURCE_DIR}/TinyRenderSrc/*.h  
					 ${PROJECT_SOURCE_DIR}/TinyRenderUI/*.h  
					 ${PROJECT_SOURCE_DIR}/TinyRenderUI/*.cpp  
					 ${PROJECT_SOURCE_DIR}/TwoWayRender/src/*.cpp  
					 ${PROJECT_SOURCE_DIR}/TwoWayRender/include/*.h  

)
message(${SOURCE_DIR})
message("PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR})

#add_subdirectory(${PROJECT_SOURCE_DIR}/Dependence/eigen3)
add_executable(${PROJECT_NAME} ${SOURCE_DIR} ${UI_FILES} ${UI_HEADERS} "TinyRenderSrc/Triangle.h" "TinyRenderSrc/Triangle.cpp" "TinyRenderSrc/MVPTransformer.h" "TinyRenderSrc/Shader.h")
find_package(glm CONFIG REQUIRED)

target_include_directories(${PROJECT_NAME} PUBLIC "D:/Download/include")
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME}  PRIVATE
"D:/Download/lib/assimp-vc143-mt.lib"
Qt6::Core
Qt6::Widgets
Qt6::Gui  # ��ʽ 
Qt6::OpenGLWidgets   
glm::glm
Eigen3::Eigen
)

# CPU + OpenGL 双模式改造项目（新项目）
qt_add_resources(TWO_WAY_RENDER_SOURCES TwoWayRender/qml.qrc)

add_executable(TwoWayRender
    TwoWayRender/main.cpp
    TwoWayRender/RenderController.cpp
    TwoWayRender/CpuRenderView.cpp
    TwoWayRender/src/threadpool.cpp
    TinyRenderSrc/DataLoader.cpp
    TinyRenderSrc/Triangle.cpp
    TinyRenderSrc/RasterizationDevice.cpp
    TinyRenderSrc/MVPTransformer.cpp
    #TinyRenderSrc/src/threadpool.cpp
    ${TWO_WAY_RENDER_SOURCES}
)
message("PROJECT_SOURCE_DIR=" ${PROJECT_SOURCE_DIR})
target_include_directories(TwoWayRender PRIVATE
    ${PROJECT_SOURCE_DIR}/TinyRenderSrc
    ${PROJECT_SOURCE_DIR}/TwoWayRender
    #${PROJECT_SOURCE_DIR}/TwoWayRender/include
#    ${PROJECT_SOURCE_DIR}/TinyRenderSrc/include
    "D:/Download/include"
)

target_link_libraries(TwoWayRender PRIVATE
    Qt6::Quick
    Qt6::Qml
    "D:/Download/lib/assimp-vc143-mt.lib"
    glm::glm
    Eigen3::Eigen
)

