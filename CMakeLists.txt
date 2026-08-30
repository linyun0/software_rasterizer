cmake_minimum_required(VERSION 3.10)
Project(MyTinyRender VERSION 1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(CMAKE_AUTOMOC ON)

set(glm_DIR Dependence/glm/share/glm)
set(Eigen3_DIR ${PROJECT_SOURCE_DIR}/Dependence/eigen3)

find_package(Eigen3 REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGLWidgets Quick Qml)
find_package(glm CONFIG REQUIRED)

file(GLOB  UI_FILES ${PROJECT_SOURCE_DIR}/TinyRenderUI/*.ui)
file(GLOB SOURCE_DIR ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/*.h
					 ${PROJECT_SOURCE_DIR}/TinyRenderSrc/*.cpp  
					 ${PROJECT_SOURCE_DIR}/TinyRenderSrc/*.h  
					 ${PROJECT_SOURCE_DIR}/TinyRenderUI/*.h  
					 ${PROJECT_SOURCE_DIR}/TinyRenderUI/*.cpp  
					 ${PROJECT_SOURCE_DIR}/TwoWayRender/src/*.cpp  
					 ${PROJECT_SOURCE_DIR}/TwoWayRender/include/*.h  

)

add_executable(${PROJECT_NAME} ${SOURCE_DIR} ${UI_FILES} ${UI_HEADERS} )

target_include_directories(${PROJECT_NAME} PUBLIC "D:/Download/include"  ${PROJECT_SOURCE_DIR}/TinyRenderSrc/  ${PROJECT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}  PRIVATE
"D:/Download/lib/assimp-vc143-mt.lib"
Qt6::Core
Qt6::Widgets
Qt6::Gui  # ��ʽ 
Qt6::OpenGLWidgets   
glm::glm
Eigen3::Eigen
)


