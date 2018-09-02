import requests                                                      # import python library
from bs4 import BeautifulSoup

visited = []                                                         # declare list and variable
n = 0

def visiting(url, visited):
    site = ""
    global n
    if url in visited:                                               # check url in visited
        return 0
    else:
        flag = 0
        visited.append(url)
        r = requests.get(url)
        c = r.content
        soup = BeautifulSoup(c)
        n = n + 1
        outcontent = open('Output_' + str(n).zfill(4) + '.txt','w')  # open file
        outcontent.writelines(soup.text)
        outcontent.close()                                           # close file
        for link in soup.find_all('a'):                              # get url data
            nurl = link.get('href')
            nurl = nurl.strip()
            if nurl != "":
                nurl = (nurl.split("#"))[0]
            if nurl != "":
                nurl = (nurl.split("?"))[0]
            if nurl != "":
                if nurl.startswith("http://") == False:              # check url
                    nurl = site + '/' + nurl
                try:
                    r = requests.get(nurl)
                except requests.exception.ConnectError:
                    flag = 1
                except requests.exception.RequestException:
                    flag = 1
                if flag == 0 and r.ok == True:                       # url is correct
                    visiting(nurl,visited)

rootpage = ""
visiting(rootpage, visited)
outurl = open('URL.txt','w')
n = len(visited)
for i in range(n - 1):                                               # print visited url
    outurl.write(visited[i] + '\n')
outurl.write(visited[n - 1])
outurl.close()
