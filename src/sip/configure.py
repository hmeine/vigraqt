#!/usr/bin/env python
import os, sys

srcdir = "."
if len(sys.argv) > 1:
	srcdir = sys.argv[1]

print "checking SIP configuration...",
import pyqtconfig
config = pyqtconfig.Configuration()
print config

print "running SIP..."
build_file = "vigraqt.sbf"
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
makefile.extra_include_dirs = [os.path.join(srcdir, "..", "vigraqt")]
makefile.extra_lib_dirs = [os.path.join(srcdir, "..", "vigraqt")]
makefile.generate()

print "done (you can run 'make' now)."
