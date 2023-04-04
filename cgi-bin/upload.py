#!/usr/bin/python3

import os
import sys

content_type = os.environ["CONTENT_TYPE"]
content_length = os.environ["CONTENT_LENGTH"]
request_method = os.environ["REQUEST_METHOD"]
query_string = os.environ["QUERY_STRING"]


