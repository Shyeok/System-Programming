#!/usr/bin/python

from sys import stdin, stdout

while True:
    line = stdin.readline().strip()                                  # input the data
    if line == "":
        break
    words = line.split()                                             # split data
    for i in range(len(words) - 1):                                  # print the result
        stdout.write(words[i] + " " + words[i + 1] + "\t1\n")

