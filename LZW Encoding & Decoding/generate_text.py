# pip install lorem-text

from lorem_text import lorem

paragraph_length = 1000

f = open("test_demo.txt",'w')
f.write(lorem.paragraphs(paragraph_length))

