cmake_minimum_required(VERSION 3.28)
project(OpenBongoCat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# BS workaround for stupid Glib using "signals" that QT uses as well
#add_definitions(-DQT_NO_KEYWORDS)

find_package(X11 REQUIRED)
find_package(Qt5 REQUIRED
    COMPONENTS
        Core
        Gui
        Widgets
        Multimedia
)

find_library(XI_LIB Xi REQUIRED)


include_directories(
    ${X11_INCLUDE_DIR}
)


add_executable(${PROJECT_NAME} 
    main.cpp
    keylogger.cpp
)


target_link_libraries(${PROJECT_NAME} PRIVATE
    ${X11_LIBRARIES} 
    ${XI_LIB}

    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Multimedia
)
