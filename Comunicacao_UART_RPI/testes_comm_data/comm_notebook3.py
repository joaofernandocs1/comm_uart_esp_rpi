Python 3.9.6 (tags/v3.9.6:db3ff76, Jun 28 2021, 15:26:21) [MSC v.1929 64 bit (AMD64)] on win32
Type "help", "copyright", "credits" or "license()" for more information.
>>> import chardet
>>> chardet.detect(b'\xcd')
{'encoding': 'ISO-8859-1', 'confidence': 0.73, 'language': ''}
>>> 