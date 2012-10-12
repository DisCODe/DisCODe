#!/usr/bin/env python


import re
import sys
import os
 
path = '.'
listing = os.listdir(path)

dict = {}

for infile in listing:
    file = open(infile)
    lines = file.readlines()
    p = re.compile('^\\\\page[ \\t]+([^ ]*)[ \\t]*([^\\n\\r]*)')
    for l in lines:
		m = p.match(l)
		if m:
			dict[m.group(1)] = m.group(2)
			
#print dict

fname = sys.argv[1]
file=open(fname)
text=file.readlines()


p = re.compile('(.*)\\\\ref[ \\t]*([^ \\t<]*)(.*)')
for f in text:
	f = re.sub('\\\\page [^ ]*' , '---+ ', f)
	f = re.sub('\\\\section [^ ]* ' , '---++ ', f)
	f = re.sub('/\\*![ \\t\\n]*' , '', f)
	f = re.sub('\\*/' , '', f)
	f = re.sub('\\\\beginlinklist' , '<div class="linklist">', f)
	f = re.sub('\\\\endlinklist' , '</div>', f)
	f = re.sub('^[ \t-]*-[ \t]+' , '   * ', f)
	f = re.sub('\\\\n' , '<br />', f)
	m = p.match(f)
	if m:
		f = "" + m.group(1) + "[[" + dict[m.group(2)] + "]]" + m.group(3)

	if len(f)>0:
		print f,


