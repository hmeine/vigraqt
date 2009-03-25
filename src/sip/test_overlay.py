from PyQt4 import QtCore, QtGui
import VigraQt

class ImageCursor(VigraQt.Overlay):
	def __init__(self, parent):
		VigraQt.Overlay.__init__(self, parent)
		self._viewer = parent
#		self._pos = None
		self._pos = QtCore.QPoint(269, 253) # FIXME
		self._color = QtCore.Qt.blue

	def draw(self, painter, rect):
		if self._pos != None:
			painter.setRenderHint(QtGui.QPainter.Antialiasing)

			pixel = QtCore.QRectF(0, 0, 1.5, 1.5)
			pixel.moveCenter(QtCore.QPointF(self._pos.x(), self._pos.y()))
			viewPort = self._viewer.imageCoordinatesF(
				self._viewer.contentsRect())
			pixelCenter = pixel.center()

			painter.setPen(QtGui.QPen(self._color,
									  1.2 / self._viewer.zoomFactor()))
			painter.drawLine(QtCore.QPointF(pixelCenter.x(), 0.0),
							 QtCore.QPointF(pixelCenter.x(), pixel.top()))
			painter.drawLine(QtCore.QPointF(pixelCenter.x(), pixel.bottom()),
							 QtCore.QPointF(pixelCenter.x(), self._viewer.originalHeight()-1))
			painter.drawLine(QtCore.QPointF(0, pixelCenter.y()),
							 QtCore.QPointF(pixel.left(), pixelCenter.y()))
			painter.drawLine(QtCore.QPointF(pixel.right(), pixelCenter.y()),
							 QtCore.QPointF(self._viewer.originalWidth()-1, pixelCenter.y()))
			painter.drawEllipse(pixel)
			
if __name__ == "__main__":
	import sys
	a = QtGui.QApplication(sys.argv)
	fn, = sys.argv[1:]
	img = QtGui.QImage(fn)
	v = VigraQt.OverlayViewer()
	v.setImage(img)
	c = ImageCursor(v)
	v.addOverlay(c)
	v.show()
	sys.exit(a.exec_())
