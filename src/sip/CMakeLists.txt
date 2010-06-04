find_package(SIP REQUIRED)
find_package(PyQt4 REQUIRED)

if(SIP_FOUND)
    include_directories(${SIP_INCLUDE_DIR})
    include(${PROJECT_SOURCE_DIR}/config/SIPMacros.cmake)	

    set(SIP_INCLUDES ${PYQT4_SIP_DIR})
    set(SIP_EXTRA_OPTIONS "${PYQT4_SIP_FLAGS} -b ${CMAKE_CURRENT_SOURCE_DIR}/VigraQt.sbf")
    ADD_SIP_PYTHON_MODULE(VigraQt VigraQtmod.sip VigraQt)
else()
    message(STATUS "VigraQt python bindings are NOT built")
endif()