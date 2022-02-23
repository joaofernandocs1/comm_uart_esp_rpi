Python 3.8.2 (tags/v3.8.2:7b3ab59, Feb 25 2020, 22:45:29) [MSC v.1916 32 bit (Intel)] on win32
Type "help", "copyright", "credits" or "license()" for more information.
>>> data = 'agk'
>>> bytes(data)
Traceback (most recent call last):
  File "<pyshell#1>", line 1, in <module>
    bytes(data)
TypeError: string argument without an encoding
>>> bytes(data, 'ascii')
b'agk'
>>> bytes(data, 'utf-8')
b'agk'
>>> bytes(data, 'latin-1')
b'agk'
>>> data
'agk'
>>> data = 'joãofernandô'
>>> data
'joãofernandô'
>>> data_enc1 = data.encode('utf-8')
>>> data_enc1
b'jo\xc3\xa3ofernand\xc3\xb4'
>>> 
>>> data_enc2 = data.encode('ascii')
Traceback (most recent call last):
  File "<pyshell#11>", line 1, in <module>
    data_enc2 = data.encode('ascii')
UnicodeEncodeError: 'ascii' codec can't encode character '\xe3' in position 2: ordinal not in range(128)
>>> data
'joãofernandô'
>>> bytes(data_enc1, 'utf-8')
Traceback (most recent call last):
  File "<pyshell#13>", line 1, in <module>
    bytes(data_enc1, 'utf-8')
TypeError: encoding without a string argument
>>> type(data)
<class 'str'>
>>> type(data_enc1)
<class 'bytes'>
>>> data_enc1
b'jo\xc3\xa3ofernand\xc3\xb4'
>>> data_2 = 'joaofernando'
>>> data_2enc1 = data_2.encode('utf-8')
>>> data_2enc1
b'joaofernando'
>>> type(data_2enc1)
<class 'bytes'>
>>> print(data_2enc1)
b'joaofernando'
>>> str(data_2)
'joaofernando'
>>> type(data_2)
<class 'str'>
>>> num = 2
>>> num
2
>>> type(num)
<class 'int'>
>>> num = str(num)
>>> num
'2'
>>> type(num)
<class 'str'>
>>> write(data_2)
Traceback (most recent call last):
  File "<pyshell#30>", line 1, in <module>
    write(data_2)
NameError: name 'write' is not defined
>>> data
'joãofernandô'
>>> data_enc1
b'jo\xc3\xa3ofernand\xc3\xb4'
>>> type(data_enc1)
<class 'bytes'>
>>> data_dec1 = data_enc1.decode('utf-8)
			     
SyntaxError: EOL while scanning string literal
>>> data_dec1 = data_enc1.decode('utf-8')
>>> data_dec1
'joãofernandô'
>>> type(data_dec1)
<class 'str'>
>>> 