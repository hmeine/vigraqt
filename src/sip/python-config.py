import PyQt4.pyqtconfig, distutils.sysconfig, sys

c = PyQt4.pyqtconfig.Configuration()
s = distutils.sysconfig

if sys.argv[1] == "includepath":
	pi1 = s.get_python_inc()
	pi2 = s.get_python_inc(True)

	print c.sip_inc_dir, pi1
	if pi1 != pi2:
		print pi2
else:
	print "-L" + s.get_config_var("LIBDIR")
	print "-lpython" + s.get_config_var("VERSION")
