cmake_minimum_required(VERSION 3.10)
Project(MyTinyRender )
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(Qt5_DIR "D:/Program/QT/QT/5.15.2/msvc2019_64/lib/cmake/Qt5")
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
if(NOT Qt5_FOUND)
    message(FATAL_ERROR "Qt5 not found!")
endif()
file(GLOB  UI_FILES ${PROJECT_SOURCE_DIR}/TinyRenderUI/*.ui)
#message(${UI_FILES})
qt5_wrap_ui(UI_HEADERS ${UI_FILES})
#message(${UI_HEADERS})
#message(${PROJECT_BINARY_DIR})

file(GLOB SOURCE_DIR ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/*.h
					 ${PROJECT_SOURCE_DIR}/TinyRenderSrc/*.cpp  
					 ${PROJECT_SOURCE_DIR}/TinyRenderSrc/*.h  
					 ${PROJECT_SOURCE_DIR}/TinyRenderUI/*.h  
					 ${PROJECT_SOURCE_DIR}/TinyRenderUI/*.cpp  
)
message(${SOURCE_DIR})
message(${Qt5_DIR})

add_executable(MyTinyRender ${SOURCE_DIR} ${UI_FILES} ${UI_HEADERS})


target_include_directories(${PROJECT_NAME} PUBLIC "D:/Download/include")
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME}  PRIVATE
"D:/Download/lib/assimp-vc143-mt.lib"
Qt5::Core
Qt5::Widgets
Qt5::Gui  # œ‘ Ω
)
