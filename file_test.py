from os import environ
from os.path import exists
import sys
import cgi

try:
	form = cgi.FieldStorage()

	uploaded_file = form['file']
	filename = uploaded_file.filename

	# extension = filename[filename.rfind("."):]
	# filename = filename[:filename.rfind(".")]
	fd = uploaded_file.file

	# print(len(uploaded_file.value))

	fn = environ['PATH_INFO'] + filename
	# for num in range(0, sys.maxsize):
	# 	suffix = str(num) if num is not 0 else ""
	# 	if not exists(fn + suffix):
	# 		fn += suffix
	# 		break
		
	# assert exists(fn) == True

	print("q!@#!@#!@#!@#!@#!@#!@#12")
	print(fn)
	f = open(fn, 'wb')


	qq = fd.read()
	f.write(fd.read())
	print(len(qq))

	# # 파일이 업로드되지 않은 경우
	# else:
			# message = 'No file was uploaded'
	# qn = {"CONTENT-TYPE": "image/png"}
	# f = open(0, 'rb')
	# dic = cgi.parse_multipart(f, qn)

	# print(dic)

except :
	print("qw")