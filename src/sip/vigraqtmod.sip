%Module VigraQt 1

%Import QtCore/QtCoremod.sip
%Import QtGui/QtGuimod.sip
%If (!WS_WIN)
%Import QtOpenGL/QtOpenGLmod.sip
%End

%Include qimageviewer.sip
%If (!WS_WIN)
%Include qglimageviewer.sip
%End
%Include overlayviewer.sip
%Include colormap.sip
%Include cmgradient.sip
%Include cmeditor.sip
%Include wrapper.sip
