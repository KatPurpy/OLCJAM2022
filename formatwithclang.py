import os
import listsrc

clang_format = 'C:/msys64/mingw64/bin/clang-format.exe --style=file -i "{0}"'

for a in listsrc.getFiles():
	target = a
	
	cmd = clang_format.format(target)
	print(cmd)
	os.system(cmd)