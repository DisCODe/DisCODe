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
    take a text and <strong class="highlight">replace</strong> words that match a key <strong class="highlight">in</strong> a dictionary with
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


# Absolute path of directory DisCODe was installed in
DISCODE_PATH="@CMAKE_INSTALL_PREFIX@"

if len(sys.argv) != 3:
    # stop the program and print an error message
    sys.exit("Usage: " + sys.argv[0] + " PATH_TO_DCL COMPONENT_NAME")

fullpath = os.path.abspath(sys.argv[1])
dcl_name = os.path.basename(fullpath)
cmp_name = sys.argv[2]

dir = fullpath+'/src/Components/'+cmp_name
if os.path.exists(dir):
    sys.exit("Folder already exists! Choose different name of component.")
else:
    os.makedirs(dir)
    

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