#!/bin/sh
cd doc
doxygen Doxyfile
cd ..
scp -r doc gerstrong,clonekeenplus@web.sourceforge.net:/home/groups/c/cl/clonekeenplus/htdocs/

