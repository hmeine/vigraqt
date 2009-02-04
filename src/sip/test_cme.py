from PyQt4 import QtGui
import VigraQt

if __name__ == "__main__":
	import sys
	a = QtGui.QApplication(sys.argv)
	cm = VigraQt.createColorMap(VigraQt.CMFire)
	g = VigraQt.ColorMapEditor()
	g.setColorMap(cm)
	g.show()
	sys.exit(a.exec_())
