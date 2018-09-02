#!/usr/bin/python
from sys import stdin, stdout
library = {}
while True:
    line = stdin.readline().strip()                                  # input the data
    if line == "":
        break
    word, cnt = line.split('\t')                                     # split the words and counts
    if word in library:
        library[word] += int(cnt)
    else :
        library[word] = int(cnt)
for word in library:                                                 # print the result
    stdout.write(word + "\t" + str(library[word]) + '\n')

