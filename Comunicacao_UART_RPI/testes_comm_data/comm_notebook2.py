Python 3.8.2 (tags/v3.8.2:7b3ab59, Feb 25 2020, 22:45:29) [MSC v.1916 32 bit (Intel)] on win32
Type "help", "copyright", "credits" or "license()" for more information.
>>> data = 'joaofernando'
>>> data_u = data.encode('utf-8')
>>> data_u
b'joaofernando'
>>> data_a = data.encode('ascii')
>>> datA_a
Traceback (most recent call last):
  File "<pyshell#4>", line 1, in <module>
    datA_a
NameError: name 'datA_a' is not defined
>>> data_a
b'joaofernando'
>>> datax = 'joãofernandô'
>>> datax_u = datax.encode('utf-8')
>>> datax_a = datax.encode('ascii')
Traceback (most recent call last):
  File "<pyshell#8>", line 1, in <module>
    datax_a = datax.encode('ascii')
UnicodeEncodeError: 'ascii' codec can't encode character '\xe3' in position 2: ordinal not in range(128)
>>> datax_u
b'jo\xc3\xa3ofernand\xc3\xb4'
>>> datax_u.decode(utf-8')
	       
SyntaxError: EOL while scanning string literal
>>> datax_u.decode('utf-8')
'joãofernandô'
>>> teste = b'\x9e\x10C\x9e\x10C\x9e\x10'
>>> type(teste)
<class 'bytes'>
>>> teste.decode('utf-8')
Traceback (most recent call last):
  File "<pyshell#14>", line 1, in <module>
    teste.decode('utf-8')
UnicodeDecodeError: 'utf-8' codec can't decode byte 0x9e in position 0: invalid start byte
>>> teste.decode('utf-16')
'႞鹃䌐႞'
>>> teste.decode('utf-32')
Traceback (most recent call last):
  File "<pyshell#16>", line 1, in <module>
    teste.decode('utf-32')
UnicodeDecodeError: 'utf-32-le' codec can't decode bytes in position 0-3: code point not in range(0x110000)
>>> 