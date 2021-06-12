cmake_minimum_required(VERSION 3.5)

project(pathfinding_visualizer_qt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

include_directories(include)

set(SOURCES
  src/main.cpp
  src/mainwindow.cpp
  src/grid.cpp
  src/grid_map.cpp
  src/planner/planner.cpp
  src/planner/search/bfs/bfs.cpp
  src/planner/search/dfs/dfs.cpp
  src/planner/search/dijkstra/dijkstra.cpp
  src/planner/search/astar/astar.cpp
)

set(MOC_HEADERS
  include/mainwindow.h
  include/grid.h
  include/grid_map.h
  include/planner/planner.h
  include/planner/search/bfs/bfs.h
  include/planner/search/dfs/dfs.h
  include/planner/search/dijkstra/dijkstra.h
  include/planner/search/astar/astar.h
)

set(CMAKE_AUTOUIC_SEARCH_PATHS ui)
qt5_wrap_ui(UI_HEADERS
  ui/mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(pathfinding_visualizer_qt
        MANUAL_FINALIZATION
        ${SOURCES}
        ${MOC_HEADERS}
        ${UI_HEADERS}
    )
else()
    if(ANDROID)
        add_library(pathfinding_visualizer_qt SHARED
          ${SOURCES}
          ${MOC_HEADERS}
          ${UI_HEADERS}
        )
    else()
        add_executable(pathfinding_visualizer_qt
          ${SOURCES}
          ${MOC_HEADERS}
          ${UI_HEADERS}
        )
    endif()
endif()

target_link_libraries(pathfinding_visualizer_qt PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(pathfinding_visualizer_qt PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)
