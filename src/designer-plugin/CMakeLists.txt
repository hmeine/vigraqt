include_directories(${PROJECT_SOURCE_DIR}/src/VigraQt)
include_directories(${PROJECT_BINARY_DIR}/src/VigraQt)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt4_generate_moc(vigraqtplugins.cxx ${CMAKE_CURRENT_BINARY_DIR}/vigraqtplugins.moc)
add_library(vigraqtplugins SHARED vigraqtplugins.cxx ${CMAKE_CURRENT_BINARY_DIR}/vigraqtplugins.moc)
target_link_libraries(vigraqtplugins VigraQt)

install(TARGETS vigraqtplugins DESTINATION lib)
