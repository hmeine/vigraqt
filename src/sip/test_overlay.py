from PyQt4 import QtCore, QtGui
import VigraQt

class ImageCursor(VigraQt.Overlay):
	def __init__(self, parent):
		VigraQt.Overlay.__init__(self, parent)
		self._viewer = parent
		self._pos = None
		self._color = QtCore.Qt.blue
		self._viewer.installEventFilter(self)
		self._mousePressed = False

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
			painter.drawLine(QtCore.QPointF(pixelCenter.x(), viewPort.top()),
							 QtCore.QPointF(pixelCenter.x(), pixel.top()))
			painter.drawLine(QtCore.QPointF(pixelCenter.x(), pixel.bottom()),
							 QtCore.QPointF(pixelCenter.x(), viewPort.bottom()))
			painter.drawLine(QtCore.QPointF(viewPort.left(), pixelCenter.y()),
							 QtCore.QPointF(pixel.left(), pixelCenter.y()))
			painter.drawLine(QtCore.QPointF(pixel.right(), pixelCenter.y()),
							 QtCore.QPointF(viewPort.right(), pixelCenter.y()))
			painter.drawEllipse(pixel)

	def position(self):
		return self._pos

	def setPosition(self, pos):
		if pos != self._pos:
			self._pos = pos
			self.emit(QtCore.SIGNAL("positionChanged(const QPoint&)"),
					  pos)
			self._viewer.update()

	def eventFilter(self, watched, event):
		if event.type() == QtCore.QEvent.MouseButtonPress:
			if event.modifiers() != QtCore.Qt.NoModifier \
				   or event.button() != QtCore.Qt.LeftButton:
				return False
			pos = self._viewer.imageCoordinate(event.pos())
			self.setPosition(pos)
			self._mousePressed = True
			return True
		elif event.type() == QtCore.QEvent.MouseMove:
			if self._mousePressed:
				pos = self._viewer.imageCoordinate(event.pos())
				self.setPosition(pos)
		elif event.type() == QtCore.QEvent.MouseButtonRelease:
			if self._mousePressed and event.button() == QtCore.Qt.LeftButton:
				self._mousePressed = False
				return True
		elif event.type() == QtCore.QEvent.MouseButtonDblClick:
			if event.modifiers() != QtCore.Qt.NoModifier \
				   or event.button() != QtCore.Qt.LeftButton:
				return False
			pos = self._viewer.imageCoordinate(event.pos())
			self._viewer.centerOn(pos)
			return True
		return False

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
