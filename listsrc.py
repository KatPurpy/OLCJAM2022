import os

def getFiles(): 
	dirs_to_format = ['src', 'TESTS']
	allowed_extensions = ['.cpp','.c','.h','.hpp','.hxx','.tcc']
	ret = []
	for dir in dirs_to_format:
		for path, directories, files in os.walk(dir):
			for file in files:
				for ext in allowed_extensions:
					if file.endswith(ext):
						ret.append(os.path.join(path, file))
						break
	return ret
	
if __name__ == "__main__":
	print(' '.join(map(lambda x: '"{}"'.format(os.path.abspath(x)),getFiles())))