import cgi
import cgitb

cgitb.enable()

form = cgi.FieldStorage()

# a = form["NAME"]


# print(a.file)
# fileitem = form['file']


t = open("./a", "rb")
# 파일이 업로드된 경우
if True:
    # 파일 저장 경로와 파일 이름 설정
    fn = './temp_file/' + "aa"
    with open(fn, 'wb') as f:
        # 파일 데이터를 한 번에 읽어서 저장
        f.write(t.read())

# 파일이 업로드되지 않은 경우
else:
    message = 'No file was uploaded'

print('Content-Type: text/html')
print()
print('<html><body>')
print('</body></html>')