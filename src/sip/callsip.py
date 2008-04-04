#!/usr/bin/python
import PyQt4.pyqtconfig, subprocess, os

config = PyQt4.pyqtconfig.Configuration()

command = [config.sip_bin] + \
		  config.pyqt_sip_flags.split() + \
		  ["-I", config.pyqt_sip_dir, "VigraQtmod.sip"]
print " ".join(command)
subprocess.call(command)
