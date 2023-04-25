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
	if "username" not in form:
		raise Exception("Request must be sent with a form including 'username' field.")
	if "saveId" not in form:
		raise Exception("Request must be sent with a form including 'saveId' field.")

try:
	validateEnviron()

	form = cgi.FieldStorage()
	validateRequest(form)

except Exception as e:
	print("Status:", "400", "Bad Request")

try:
	if form["saveID"] == true:
		

	print("Status:", "200", "OK")

except Exception as e:
	print("Status:", "500", "Internal Server Error")

