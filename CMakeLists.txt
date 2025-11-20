cmake_minimum_required(VERSION 3.10)
Project(MyTinyRender VERSION 1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(Qt5_DIR "D:/Program/QT/QT/5.15.2/msvc2019_64/lib/cmake/Qt5")
set(glm_DIR Dependence/glm/share/glm)
set(Eigen3_DIR ${PROJECT_SOURCE_DIR}/Dependence/eigen3)
set(TBB_DIR D:/Dependence/TBBmy_installed_onetbb/lib/cmake/TBB)

find_package(Eigen3 REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
if(NOT Qt5_FOUND)
    message(FATAL_ERROR "Qt5 not found!")
endif()
file(GLOB  UI_FILES ${PROJECT_SOURCE_DIR}/TinyRenderUI/*.ui)
#message(${UI_FILES})
qt5_wrap_ui(UI_HEADERS ${UI_FILES})
#message(${UI_HEADERS})
#message(${PROJECT_BINARY_DIR})
# 添加这些行来诊断
message(STATUS "项目名: ${PROJECT_NAME}")
message(STATUS "当前源码目录: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "当前二进制目录: ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "生成器: ${CMAKE_GENERATOR}")
file(GLOB SOURCE_DIR ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/*.h
					 ${PROJECT_SOURCE_DIR}/TinyRenderSrc/*.cpp  
					 ${PROJECT_SOURCE_DIR}/TinyRenderSrc/*.h  
					 ${PROJECT_SOURCE_DIR}/TinyRenderUI/*.h  
					 ${PROJECT_SOURCE_DIR}/TinyRenderUI/*.cpp  
)
message(${SOURCE_DIR})
message(${Qt5_DIR})

add_subdirectory(${PROJECT_SOURCE_DIR}/Dependence/eigen3)
add_executable(${PROJECT_NAME} ${SOURCE_DIR} ${UI_FILES} ${UI_HEADERS} "TinyRenderSrc/Triangle.h" "TinyRenderSrc/Triangle.cpp" "TinyRenderSrc/MVPTransformer.h" "TinyRenderSrc/Shader.h")
find_package(glm CONFIG REQUIRED)
find_package(TBB CONFIG REQUIRED)



target_include_directories(${PROJECT_NAME} PUBLIC "D:/Download/include")
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME}  PRIVATE
"D:/Download/lib/assimp-vc143-mt.lib"
Qt5::Core
Qt5::Widgets
Qt5::Gui  # 显式
glm::glm
eigen
)
target_link_libraries(${PROJECT_NAME} PUBLIC TBB::tbb)
