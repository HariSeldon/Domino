#! /bin/bash

output="cscope.files"
rm $output
find src -name '*.cpp' >> $output
find include -name '*.h' >> $output
cscope -b
