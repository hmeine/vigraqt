import sys
from PyQt4 import QtCore, QtGui
import VigraQt

app = QtGui.QApplication(sys.argv)

try:
	from qimage2ndarray import rgb_view, array2qimage
	import numpy
except ImportError, e:
	sys.stderr.write("This test program requires the numpy and qimage2ndarray projects to be installed.\n")
	raise

def getWidgetImage(w):
	result = QtGui.QImage(w.size(), QtGui.QImage.Format_RGB32)
	result.fill(0)
	r = QtCore.QRect(QtCore.QPoint(0,0), w.size())
	p = QtGui.QPainter(result)
	v.paintImage(p, r)
	return result

def showImage(array):
	qimg = array2qimage(array)
	result = VigraQt.QImageViewer()
	result.setImage(qimg)
	result.show()
	return result

# --------------------------------------------------------------------
# 				 test viewer reproducing input @100%
# --------------------------------------------------------------------

qimg = QtGui.QImage("../../examples/example.png")
reference = qimg.convertToFormat(QtGui.QImage.Format_RGB32)
v = VigraQt.QImageViewer()
v.setImage(qimg)
v.resize(qimg.size())
v.show()

out = getWidgetImage(v)

assert numpy.all(rgb_view(out) == rgb_view(reference))

# --------------------------------------------------------------------
#                       test zoom factor 200%
# --------------------------------------------------------------------

v.setZoomLevel(1)
v.resize(qimg.size()*2)

out = getWidgetImage(v)

for yo in (0, 1):
	for xo in (0, 1):
		assert numpy.all(rgb_view(out)[xo::2,yo::2] ==
						 rgb_view(reference))

# --------------------------------------------------------------------
#                   test zoom factor 400%, cropped
# --------------------------------------------------------------------

v.setZoomLevel(3)
assert v.zoomFactor() == 4.0

out = getWidgetImage(v)

for yo in range(4):
	for xo in range(4):
		assert numpy.all(rgb_view(out)[xo::4,yo::4] ==
						 rgb_view(reference)[32:-32,32:-32])

# --------------------------------------------------------------------

v.setZoomLevel(-1)
assert v.zoomFactor() == 0.5

out = getWidgetImage(v)

assert numpy.all(rgb_view(out)[96:-96,96:-96] ==
				 rgb_view(reference)[::2,::2])

