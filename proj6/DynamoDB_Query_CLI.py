execfile('')
from sys import stdin, stdout
from boto.dynamodb2.table import Table
from boto.dynamodb2.exceptions import ItemNotFound
myTable = Table('prjsixresult')

def dorequest(bigram):                                               # find bigram
    try:
        result = myTable.get_item(words = bigram)
    except ItemNotFound:                                             # exception handling
        result = None
    if not result:
	stdout.write('0\n')                                          # print zero
    else:
        stdout.write(str(result['counts']) + '\n')                   # print count

while True:
    line = stdin.readline().strip()
    if line == "":                                                   # stop input
        break;
    dorequest(line)
