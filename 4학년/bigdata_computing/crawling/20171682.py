from urllib.request import *
from bs4 import *
import matplotlib.pyplot as plt


'''
    회사 번호와 기간을 입력 받아,
    회사 번호, 페이지 개수, 기간을 리턴합니다.
'''
def inputCompanyAndDays():
    print(f"1: {code[1][1]}, 2: {code[2][1]}, 3: {code[3][1]}")
    while True:
        try :
            n = int(input("회사를 고르세요(ex 1번) : "))
            if n<1 or n>3:
                print("1~3에서 골라주세요")
                continue
            DN = int(input("종가 추출 기간을 입력하세요(20의 배수가 되도록 상향 조정합니다) : "))
            if DN <= 0: 
                print("0보다 큰 수를 입력하세요")
                continue
        except ValueError:
            print("잘못된 입력 형식입니다. 다시 입력해주세요.")            
            continue
        break


    PN = DN // 20
    if DN > PN * 20 : # 추출 횟수를 20의 배수로
        PN += 1 
    DN = PN*20
    return n,PN,DN

'''
    입력받은 이동평균선 리스트를 화면에 보여줍니다.
'''
def drawGraph(ma_list,dn,n):

    plt.title('Price Chart')
    plt.xlabel('Day')
    plt.ylabel('Price')
    labels = [code[n][1],"5MA","20MA","60MA"]
    colors = ['tab:blue','tab:green','tab:red','tab:orange']
    for i in range(4):
        plt.plot(range(-dn,0),ma_list[i],color = colors[i],label=labels[i])
    
    
    plt.legend(loc='upper left')
    plt.grid(True,axis='y')
    plt.show()




'''
    입력받은 종가로 5일, 20일, 60일 이동평균선 리스트를 리턴합니다.
'''
def makeMA(pList) -> list[list]:
    mList = []
    mList.append(pList)

    for i,n in enumerate([5,20,60],1):
        mList.append([])
        queue = []
        mSum = pList[0]*n
        for _ in range(n): queue.append(pList[0])
        
        for m in pList:
            mSum -= queue.pop(0)
            mSum += m
            mList[i].append(mSum/n)
            queue.append(m)

    return mList



'''
    입력받은 회사의 종가를 추출하여 리턴합니다.
'''
def extractLastPrice(pn,codeNum) -> list:

    url="http://finance.naver.com/item/frgn.nhn?code=" + codeNum + "&page="
    pList = []
    for page in range(1,pn+1):
        req = Request(url+str(page))
        req.add_header('User-Agent','Mozilla/5.0')
        wPage = urlopen(req)
        soup = BeautifulSoup(wPage,'html.parser')
        trList = soup.find_all('tr', {'onmouseover':'mouseOver(this)'})
        for tr in trList:
            tdList = tr.find_all('td')
            pList.append(tdList[1].text)
    
    pList = list(map(lambda x : int(x.replace(',',"")),pList))
    pList.reverse()
    
    return pList

code = {
    1 : ["005930","samsung"],
    2 : ["035720","kakao"],
    3 : ["035420","naver"]
}

if __name__ == "__main__":
    n,pn,dn = inputCompanyAndDays()
    prices = extractLastPrice(pn,code[n][0])
    MA_list = makeMA(prices)   
    drawGraph(MA_list,dn,n)