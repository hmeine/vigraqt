VigraQt4
========

This package contains Qt_ bindings for VIGRA_, in particular for Qt
version 4.x.  This includes:

- Header files (i.e. ``vigraqimage.hxx``) for letting VIGRA algorithms
  work directly on Qt images.

- An image viewer class (QImageViewer) that includes panning and
  zooming, a derived OverlayViewer class that adds overlay support and
  an OpenGL-based viewer.

- A color map for displaying images or analysis results in false
  colors, including a Qt widget for interactively editing the color
  map.

- Python bindings and a designer plugin for including the widgets in
  your own (C++ or Python-based) applications.

.. _Qt: http://www.trolltech.com/products/qt/
.. _VIGRA: http://kogs-www.informatik.uni-hamburg.de/~koethe/vigra/

You can always find the current release at
 http://kogs-www.informatik.uni-hamburg.de/~meine/software/vigraqt/

Installation
------------

In contrast to earlier versions, starting with vigraqt4-0.4, the three
components (the VigraQt library, the designer plugin, and the python
bindings) are compiled and installed separately (the old,
autotools-based build system was ditched, which accounts for the big
difference in the download's filesizes..).  This is not unlike with
other, similar software packages (e.g. qwt) and probably makes sense:

- You don't have to care about the paths for Python or the designer
  plugins when you only want to use the VigraQt library, or even only
  its header files.  It is likely that most users are not interested
  in all three components, and distributions would probably also want
  to create separate subpackages for dependency reasons.

- The install processes (especially the typical target directories) are
  quite different.  This way, each subpackage can use the build tools
  best suited and the user can focus on installing the part he/she is
  currently interested in.

For the VigraQt library itself, use qmake::

  qmake # optionally, add INSTALLBASE=/some/where
  make
  # copies the library and header files to subdirectores of
  # INSTALLBASE (default: /usr/local):
  make install

Similarly, you can install the designer plugin with::

  cd src/designer-plugin
  qmake
  make
  # copies the plugin into ${QT_PLUGINS}/designer
  make install

Finally, the Python bindings require PyQt_ and use its python-based
build system::

  cd src/sip
  # optionally, add -m /your/python/site-packages:
  python configure.py
  make
  make install # installs VigraQt.so into the module target dir (see above)

.. _PyQt: http://www.riverbankcomputing.co.uk/pyqt/

Using CMake
-----------

Alternatively, you may use CMake for configuring the build, for instance::

  mkdir build
  cd build
  cmake -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=$HOME/inst \
        -DPYTHON_SITE_PACKAGES_DIR=$HOME/inst/lib/python2.6/site-packages \
        ..
