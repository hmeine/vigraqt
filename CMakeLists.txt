PROJECT(VigraQt)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/config)

find_package(Qt4 REQUIRED)
find_package(Vigra REQUIRED)
find_package(GLU REQUIRED)

include_directories(
    ${VIGRA_INCLUDE_DIR}
    ${QT_INCLUDE_DIR}
    ${QT_QTCORE_INCLUDE_DIR}
    ${QT_QTGUI_INCLUDE_DIR}
    ${QT_QTOPENGL_INCLUDE_DIR}
    ${GLU_INCLUDE_PATH}
)

add_subdirectory(src)
