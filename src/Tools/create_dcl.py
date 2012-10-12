#! /usr/bin/env python

# Copyright (c) 2010 Warsaw Univeristy of Technology
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import sys
import os
import re


def replace_words(text, word_dic):
    """
    take a text and <strong class="highlight">replace</strong> words that match 
    a key <strong class="highlight">in</strong> a dictionary with the associated
    value, return the changed text
    """
    rc = re.compile('|'.join(map(re.escape, word_dic)))
    def translate(match):
        return word_dic[match.group(0)]
    return rc.sub(translate, text)


def configure_file(in_name, out_name, word_dic):
    """
    take template file, replace words according to given dictionary and write 
    result to another file
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


# Absolute path of directory DisCODe was installed in
DISCODE_PATH="@CMAKE_INSTALL_PREFIX@"

if len(sys.argv) != 2:
    # stop the program and print an error message
    sys.exit("Usage: " + sys.argv[0] + " PATH_TO_DCL")

fullpath = os.path.abspath(sys.argv[1])
dcl_name = os.path.basename(fullpath)

# Create directory if it doesn't exist
if not os.path.exists(fullpath):
    os.makedirs(fullpath)

# Check if provided directory contains CMakeLists.txt file
# which means, that it is probably already an DCL
if os.path.exists(fullpath + "/CMakeLists.txt"):
    sys.exit("There is already DCL named " + dcl_name + " in current location!")


print "Creating DCL:" , dcl_name , "in" , fullpath 


#===============================================================================
# Preparing README file
#===============================================================================
readme_header = dcl_name + " - DisCODe Component Library"
readme_header += "\n" + "=" * len(readme_header)
 
readme_dic = {
'TEMPLATE_ARG_DCL_NAME' : readme_header
}

configure_file(DISCODE_PATH+'/share/DisCODe/Templates/README.md', fullpath+'/README.md', readme_dic)

#===============================================================================
# Preparing CMakeLists.txt file
#===============================================================================
cmakelists_dic = {
'TEMPLATE_ARG_DCL_NAME' : dcl_name
}

configure_file(DISCODE_PATH+'/share/DisCODe/Templates/CMakeLists.txt', fullpath+'/CMakeLists.txt', cmakelists_dic)

#===============================================================================
# Preparing 'src' directory. If it already exists, then all components
# inside are converted and inserted to CMakeLists.txt
#===============================================================================

# create src directory if it doesn't exist
srcpath = os.path.join(fullpath, "src")
if not os.path.exists(srcpath):
    os.makedirs(srcpath)

# create CMakeLists.txt in src directory
srccmakepath = os.path.join(srcpath, "CMakeLists.txt")
if not os.path.exists(srccmakepath):
    configure_file(DISCODE_PATH+'/share/DisCODe/Templates/src/CMakeLists.txt', srccmakepath, cmakelists_dic)


# create DCLConfig.txt in src directory
srcconfigpath = os.path.join(srcpath, dcl_name+"Config.cmake.in")
if not os.path.exists(srcconfigpath):
    configure_file(DISCODE_PATH+'/share/DisCODe/Templates/src/DCLConfig.cmake.in', srcconfigpath, cmakelists_dic)


# create Types directory if it doesn't exist
srctypespath = os.path.join(srcpath, "Types")
if not os.path.exists(srctypespath):
    os.makedirs(srctypespath)

# create CMakeLists.txt in Components directory
srctypescmakepath = os.path.join(srctypespath, "CMakeLists.txt")
if not os.path.exists(srctypescmakepath):
    configure_file(DISCODE_PATH+'/share/DisCODe/Templates/src/Types/CMakeLists.txt', srctypescmakepath, cmakelists_dic)



# create Components directory if it doesn't exist
srccomponentspath = os.path.join(srcpath, "Components")
if not os.path.exists(srccomponentspath):
    os.makedirs(srccomponentspath)

# create CMakeLists.txt in Components directory
srccomponentscmakepath = os.path.join(srccomponentspath, "CMakeLists.txt")
if not os.path.exists(srccomponentscmakepath):
    configure_file(DISCODE_PATH+'/share/DisCODe/Templates/src/Components/CMakeLists.txt', srccomponentscmakepath, cmakelists_dic)

# prepare dictionary for components conversion
cmakelists_dic = {
'INSTALL_PROCESSOR' : 'INSTALL_COMPONENT',
'INSTALL_SOURCE' : 'INSTALL_COMPONENT',
'INSTALL_SINK' : 'INSTALL_COMPONENT',
'INSTALL_PROXY' : 'INSTALL_COMPONENT'
}

cmakefile = open(srccomponentscmakepath, "a")

# iterate through all subdirectories in Components folder
for f in os.listdir(srccomponentspath):
    newpath = os.path.join(srccomponentspath, f)
    if os.path.isdir(newpath):
        cmakepath = os.path.join(newpath, "CMakeLists.txt")
        if os.path.exists(cmakepath):
            print "Converting component: " + f
            configure_file(cmakepath, cmakepath, cmakelists_dic)
            cmakefile.write("\nADD_COMPONENT("+f+")\n")
            
cmakefile.close()



#===============================================================================
# Print message and finish
#===============================================================================
print dcl_name , "DCL created."
print "Please edit README file and provide all necessary information."
print "You can create new components by executing create_component command."