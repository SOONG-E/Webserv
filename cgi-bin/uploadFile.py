from os import environ
from os.path import exists
import sys
import cgi
from validateEnviron import validateEnviron

def validateRequest(form):
	ctype, pdict = cgi.parse_header(environ['CONTENT_TYPE'])
	if environ["REQUEST_METHOD"] != "POST":
		raise Exception("Request Method must be POST.")
	if ctype != "multipart/form-data":
		raise Exception("CONTENT-TYPE must be 'multipart/form-data.")
	if "upload_file" not in form:
		raise Exception("Request must be sent with a form including name field.")

def split_filename(filename):
	if filename.count(".") == 0:
		return filename, ""
	base_name = filename[:filename.rfind(".")]
	extension = filename[filename.rfind("."):]

	return base_name, extension

def recreate_filename(filename):
	if not exists(filename):
		return filename
	
	filename, extension = split_filename(filename)

	for num in range(1, sys.maxsize):
		suffix = str(num)
		if not exists(filename + suffix + extension):
			return filename + suffix + extension
	raise Exception("There are too many duplicated name.")

try:
	validateEnviron()

	form = cgi.FieldStorage()
	validateRequest(form)
	uploaded_file = form['upload_file']
	filename = environ['PATH_INFO'] + uploaded_file.filename
	filename = recreate_filename(filename)
	
	assert not exists(filename)

	save_file = open(filename, 'wb')

	save_file.write(uploaded_file.value)

	print("Status:", "200", "OK")

except Exception as e:
	print("Status:", "400", "Bad Request")