from os import environ

def validateEnviron():
	if "CONTENT_LENGTH" not in environ:
		raise Exception("Header 'CONTENT_LENGTH' is required.")
	if "CONTENT_TYPE" not in environ:
		raise Exception("Header 'CONTENT_TYPE' is required.")
	if "PATH_INFO" not in environ:
		raise Exception("Header 'PATH_INFO' is required.")
	if "REQUEST_METHOD" not in environ:
		raise Exception("Header 'REQUEST_METHOD' is required.")

