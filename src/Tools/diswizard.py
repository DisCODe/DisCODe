from PySide import QtGui  
from PySide import QtCore
from PySide import QtUiTools

import sys
import os
import re

def replace_words(text, word_dic):
	"""
	take a text and replace words that match a key in a dictionary with :
	the associated value, return the changed text
	"""
	rc = re.compile('|'.join(map(re.escape, word_dic)))
	def translate(match):
		return word_dic[match.group(0)]
	return rc.sub(translate, text)


def configure_file(in_name, out_name, word_dic):
	"""
	take template file, replace words according to given dictionary and write result to another file
	"""
	# Read template file
	fin = open(in_name, "r")
	str = fin.read()
	fin.close()
	
	# call the function and get the changed text
	str = replace_words(str, word_dic)
	
	# write changed text back out
	fout = open(out_name, "w")
	fout.write(str)
	fout.close()




class MyWidget(QtGui.QMainWindow):
    def __init__(self, *args):  
       apply(QtGui.QMainWindow.__init__, (self,) + args)

       loader = QtUiTools.QUiLoader()
       file = QtCore.QFile("diswizard.ui")
       file.open(QtCore.QFile.ReadOnly)
       self.ui = loader.load(file, self)
       file.close()

       self.setCentralWidget(self.ui)

    
class DisCODeWizard(object):
	def __init__(self):
		self.app = QtGui.QApplication(sys.argv)  
		self.win = MyWidget()
		
		QtCore.QObject.connect(self.win.ui.btnGenerate, QtCore.SIGNAL('clicked()'), self.generate)
		QtCore.QObject.connect(self.win.ui.btnAddProp, QtCore.SIGNAL('clicked()'), self.addProp)
		QtCore.QObject.connect(self.win.ui.btnRemoveProp, QtCore.SIGNAL('clicked()'), self.remProp)
		
		self.app.connect(self.app, QtCore.SIGNAL("lastWindowClosed()"), self.app, QtCore.SLOT("quit()"))
		
	def run(self):
		self.win.show()
		self.app.exec_()
		
		
	def generate(self):
		# Absolute path of directory DisCODe was installed in
		DISCODE_PATH="@CMAKE_INSTALL_PREFIX@"

		fullpath = os.path.abspath(os.getcwd())
		dcl_name = self.win.ui.cbDCL.text()
		cmp_name = self.win.ui.edName.text()

		dir = fullpath+'/'+dcl_name+'/src/Components/'+cmp_name
		if !os.path.exists(fullpath+'/'+dcl_name):
			sys.exit("DCL doesn't exist!")
		if os.path.exists(dir):
			sys.exit("Folder already exists! Choose different name of component.")
		else:
			print "Creating component", cmp_name
			os.makedirs(dir)
		
		
		
		print "Properties:\n"
		props_header = ""
		props_initializer = ""
		props_constructor = ""
		
		tbl = self.win.ui.tblProps
		allRows = tbl.rowCount()
		for row in xrange(0,allRows):
			prop_name = tbl.item(row,0)
			prop_type = tbl.item(row,1)
			prop_default = tbl.item(row,2)
			prop_display = tbl.item(row,3)
			if (prop_display):
				prop_display = ", " + prop_display.text()
			else:
				prop_display = ""
			props_header += "\t\tBase::Property<" + prop_type.text() + "> " + prop_name.text() + ";\n";
			props_initializer += ", \n\t\t" + prop_name.text() + "(\"" + prop_name.text() + "\", " + prop_default.text() + prop_display + ")"
			props_constructor += "\t\tregisterProperty(" + prop_name.text() + ");\n"
			
		print props_header
		print props_initializer
		print props_constructor
		
		#===============================================================================
		# Preparing component source files
		#===============================================================================
		 
		dic = {
		'Example' : cmp_name,
		'EXAMPLE' : cmp_name.upper()
		}

		configure_file(DISCODE_PATH+'/share/DisCODe/Templates/src/Components/Component/Component.hpp', dir+'/'+cmp_name+'.hpp', dic)
		configure_file(DISCODE_PATH+'/share/DisCODe/Templates/src/Components/Component/Component.cpp', dir+'/'+cmp_name+'.cpp', dic)
		configure_file(DISCODE_PATH+'/share/DisCODe/Templates/src/Components/Component/CMakeLists.txt', dir+'/CMakeLists.txt', dic)

		#===============================================================================
		# Adding new component to DCL CMakeLists file
		#===============================================================================
		cmakefile = open(fullpath+'/src/Components/CMakeLists.txt', "a")
		cmakefile.write("\nADD_COMPONENT("+cmp_name+")\n")
		cmakefile.close()
		
		
		
		
		
		
	def addProp(self):
		self.win.ui.tblProps.insertRow(self.win.ui.tblProps.rowCount())
		
	def remProp(self):
		self.win.ui.tblProps.removeRow(self.win.ui.tblProps.currentRow())

if __name__ == '__main__':  
	import sys  
	import os
	print("Running in " + os.getcwd() + " .\n")
	app = DisCODeWizard()
	app.run()

