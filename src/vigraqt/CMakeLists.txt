include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(VigraQt_MOC_HDRS
    imagecaption.hxx
    cmeditor.hxx
    cmgradient.hxx
    fimageviewer.hxx
    overlayviewer.hxx
    qimageviewer.hxx
)
QT4_WRAP_CPP(VigraQt_MOC_SRCS ${VigraQt_MOC_HDRS})

add_library(VigraQt SHARED
    ${VigraQt_MOC_SRCS}
    cmeditor.cxx
    cmgradient.cxx
    colormap.cxx
    fimageviewer.cxx
    imagecaption.cxx
    linear_colormap.cxx
    overlayviewer.cxx
    qglimageviewer.cxx
    qimageviewer.cxx
)
target_link_libraries(VigraQt ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} ${VIGRA_IMPEX_LIBRARY} ${GLU_LIBRARY})

file(GLOB header_files "*.hxx")
install(FILES ${header_files} DESTINATION include/VigraQt)
install(TARGETS VigraQt DESTINATION lib)
