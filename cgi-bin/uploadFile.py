from os import environ
from os.path import exists
from os.path import isdir
from os import makedirs
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
		raise Exception("Request must be sent with a form including 'upload_file' field.")
	if "HTTP_X_SERVER_KEY" not in environ:
		raise Exception("Server key is needed for upoloading file.")
	if "HTTP_X_SESSION_ID" not in environ:
		raise Exception("Session ID is needed for upoloading file.")

def split_filename(filename):
	if filename.count(".") == 0:
		return filename, ""

	boundary = filename.rfind(".")
	if boundary == 0:
		return filename, ""

	base_name = filename[:boundary]
	extension = filename[boundary:]

	return base_name, extension

def recreate_filename(path, filename):
	if not exists(path + filename):
		return filename

	filename, extension = split_filename(filename)

	for num in range(1, sys.maxsize):
		suffix = str(num)
		if not exists(path + filename + suffix + extension):
			return filename + suffix + extension
	raise Exception("There are too many duplicated name.")

try:
	validateEnviron()

	form = cgi.FieldStorage()
	validateRequest(form)
	uploaded_file = form['upload_file']

except Exception as e:
	print("Status:", "400", "Bad Request")

try:
	path = "./upload_file/" + environ["HTTP_X_SERVER_KEY"] + "/" + environ["HTTP_X_SESSION_ID"] + "/"
	makedirs(path, exist_ok=True)

	assert isdir(path)

	filename = recreate_filename(path, uploaded_file.filename)
	filename = path + filename

	assert not exists(filename)

	save_file = open(filename, 'wb')

	save_file.write(uploaded_file.value)

	print("Status:", "303", "See Other")
	print("Location : /mypage.html")

except Exception as e:
	print("Status:", "500", "Internal Server Error")

