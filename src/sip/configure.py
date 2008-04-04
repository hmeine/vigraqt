#!/usr/bin/env python
import os, sys, subprocess

from optparse import OptionParser

print "checking SIP/PyQt4 configuration..."
import PyQt4.pyqtconfig as pyqt4
config = pyqt4.Configuration()
#print config

op = OptionParser(usage = "python %prog [options]")
op.add_option("-m", "--moddir", action = "store",
			  dest = "moddir", default = config.pyqt_mod_dir,
			  help = "install directory for the python module (default %r)"
			  % config.pyqt_mod_dir)
options, args = op.parse_args()

buildfile = "VigraQt.sbf"
command = [config.sip_bin, "-c", ".", "-b", buildfile] + \
		  config.pyqt_sip_flags.split() + \
		  ["-I", config.pyqt_sip_dir, "VigraQtmod.sip"]

print "running SIP (%s)..." % " ".join(command)
subprocess.call(command)

print "generating Makefile..."
makefile = pyqt4.QtOpenGLModuleMakefile(
	config, buildfile,
	install_dir = options.moddir)
# for i in dir(makefile):
# 	print i
makefile.extra_libs = ["VigraQt"]
makefile.extra_include_dirs = ["."]
makefile.extra_lib_dirs = [os.path.join("..", "vigraqt")]
makefile.generate()

print "done (you can run 'make' now)."
