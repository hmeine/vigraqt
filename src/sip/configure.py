#!/usr/bin/env python
build_file = "vigraqt.sbf"

print "checking SIP configuration..."
import pyqtconfig
config = pyqtconfig.Configuration()

print "running SIP..."
import os
os.system(" ".join([
	config.sip_bin,
	"-c", ".",
	"-b", build_file,
	"-I", config.pyqt_sip_dir,
	config.pyqt_qt_sip_flags,
	"vigraqtmod.sip"]))

print "generating Makefile..."
makefile = pyqtconfig.QtModuleMakefile(config, build_file)
# for i in dir(makefile):
# 	print i
makefile.extra_libs = ["vigraqt"]
makefile.extra_include_dirs = ["../vigraqt"]
makefile.extra_lib_dirs = ["../vigraqt"]
makefile.generate()

print "done (you can run 'make' now)."
