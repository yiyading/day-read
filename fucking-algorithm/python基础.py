# lstrip()方法：用于截掉字符串左边的空格或指定字符
# 语法：str.lstrip([chars])
# 	chars：指定截取的字符，默认空格
str = "   this is example"
str = str.lstrip()
print(str)
str = "888 this is example"
str = str.lstrip('8')
print(str)


# findall()方法：re.findall("匹配规则","要匹配的字符串")
# 正则表达式要引入re模块
import re
str = "aaafff"
str_re = re.compile('^a+')
str1 = str_re.findall(str)
str2 = re.findall('^f', str)
print('str1:', str1, '\nstr2:', str2)

f = [[True] * 3 for _ in range(3)]
print(f)
