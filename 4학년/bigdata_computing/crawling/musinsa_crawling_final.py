#!/usr/bin/env python
# coding: utf-8

# In[1]:

if __name__ == '__main__' : 

    from selenium import webdriver
    from bs4 import BeautifulSoup as bs
    import requests
    import pandas as pd
    import numpy as np
    import tqdm as tqdm
    import matplotlib.pyplot as plt
    import warnings
    import re
    warnings.filterwarnings('ignore')


    # # 크롤링 하는 부분

    # In[2]:


    # 전체 코드


    urlList = [
        'https://www.musinsa.com/ranking/best?period=now&age=ALL&mainCategory=&subCategory=&leafCategory=&price=&golf=false&kids=false&newProduct=false&exclusive=false&discount=false&soldOut=false&page=1&viewType=small&priceMin=&priceMax=',
        'https://www.musinsa.com/ranking/best?period=now&age=ALL&mainCategory=001&subCategory=&leafCategory=&price=&golf=false&kids=false&newProduct=false&exclusive=false&discount=false&soldOut=false&page=1&viewType=small&priceMin=&priceMax=',
        'https://www.musinsa.com/ranking/best?period=now&age=ALL&mainCategory=002&subCategory=&leafCategory=&price=&golf=false&kids=false&newProduct=false&exclusive=false&discount=false&soldOut=false&page=1&viewType=small&priceMin=&priceMax=',
        'https://www.musinsa.com/ranking/best?period=now&age=ALL&mainCategory=003&subCategory=&leafCategory=&price=&golf=false&kids=false&newProduct=false&exclusive=false&discount=false&soldOut=false&page=1&viewType=small&priceMin=&priceMax=',
        'https://www.musinsa.com/ranking/best?period=now&age=ALL&mainCategory=020&subCategory=&leafCategory=&price=&golf=false&kids=false&newProduct=false&exclusive=false&discount=false&soldOut=false&page=1&viewType=small&priceMin=&priceMax=',
        'https://www.musinsa.com/ranking/best?period=now&age=ALL&mainCategory=022&subCategory=&leafCategory=&price=&golf=false&kids=false&newProduct=false&exclusive=false&discount=false&soldOut=false&page=1&viewType=small&priceMin=&priceMax=',
        'https://www.musinsa.com/ranking/best?period=now&age=ALL&mainCategory=018&subCategory=&leafCategory=&price=&golf=false&kids=false&newProduct=false&exclusive=false&discount=false&soldOut=false&page=1&viewType=small&priceMin=&priceMax=',
        'https://www.musinsa.com/ranking/best?period=now&age=ALL&mainCategory=005&subCategory=&leafCategory=&price=&golf=false&kids=false&newProduct=false&exclusive=false&discount=false&soldOut=false&page=1&viewType=small&priceMin=&priceMax='
    ]

    def p2d(url,driver) :
        driver.get(url)
        html = driver.page_source
        soup = bs(html, 'html.parser')
        contents = soup.select('#goodsRankList > li')
        df = pd.DataFrame()
        like = re.compile(r'like_[0-9]*') #like_tag 추출시 사용

        for content in contents : 
            dic = {}
            #브랜드명
            brand = content.find_all('p', class_ = 'item_title')[-1].find('a').text.strip()
            #상품명
            goods_name = content.find('p', class_ = 'list_info').find('a')['title']
            #가격(세일 전 가격 제외)
            if content.find('p', class_ = 'price').find('del') != None : 
                price = content.find('p', class_ = 'price').text.strip().split('\n')[1].strip()
            else : 
                price = content.find('p', class_ = 'price').text.strip()
            #평가 수
            if content.find('p', class_ = 'point') != None : 
                rating_count = content.find('p', class_ = 'point').find('span', class_ = 'count').text
            else : 
                rating_count = '0'
            #좋아요 수
            p_tags = content.find_all('p')
            for tag in p_tags : 
                try : 
                    if re.match(like, tag['id']) != None : 
                        like_tag = tag
                    else : 
                        pass
                except : 
                    continue
            like_count = like_tag.find('span').text

            #딕셔너리 저장
            dic['브랜드명'] = brand
            dic['상품명'] = goods_name
            dic['가격'] = price
            dic['평가 수'] = rating_count
            dic['좋아요 수'] = like_count
            df = df.append(dic, ignore_index = True)
        return df


    # # 기능 2 - 1
    # ## 의류 카테고리별 어떤 상품이 있는지

    # In[3]:


    driver_path = "/Users/bokkimin/selenium/chromedriver" # chromedriver 경로 입력하기


    # In[4]:


    driver = webdriver.Chrome(driver_path) # chromedriver 다운로드 후 경로 수정


    # In[5]:


    all_df = p2d(urlList[0],driver)


    # In[6]:


    top_df = p2d(urlList[1],driver)


    # In[7]:


    outer_df = p2d(urlList[2],driver)


    # In[8]:


    bottom_df = p2d(urlList[3],driver)


    # In[9]:


    onepiece_df = p2d(urlList[4],driver)


    # In[10]:


    skirt_df = p2d(urlList[5],driver)


    # In[11]:


    sneakers_df = p2d(urlList[6],driver)


    # In[12]:


    shoes_df = p2d(urlList[7],driver)


    # In[13]:


    driver.close()


    # In[17]:


    all_df


    # In[18]:


    top_df


    # In[19]:


    outer_df


    # In[20]:


    bottom_df


    # In[21]:


    onepiece_df


    # In[22]:


    skirt_df


    # In[23]:


    sneakers_df


    # In[24]:


    shoes_df


    # # 기능 2 - 2
    # ## 의류 카테고리별 어떤 브랜드 포진

    # In[21]:


    from matplotlib import font_manager, rc

    cmap = plt.get_cmap("tab20c")
    font_path = "C:/Windows/Fonts/NGULIM.TTF"
    font = font_manager.FontProperties(fname=font_path).get_name()
    rc('font', family=font)


    # In[31]:


    def show_brand_ratio(df):
        brands = set(df["브랜드명"])
        tot = len(df)
        labels,ratios = [],[]

        plt.figure(figsize=(8,10))
        extra = ["기타",0]
        extra_label = []
        for brand in brands:
            ratio = len(df[df["브랜드명"]== brand])/tot
            if ratio < 0.012:  # 1개 있는 브랜드는 기타로 처리, 1/90 
                extra[1] += ratio
                extra_label.append(brand)
                continue
            ratios.append(ratio)
            labels.append(brand)

        ratios.append(extra[1])
        labels.append(extra[0])


        explode = [0.05]*len(ratios)
        wedgeprops = {'width': 0.6, 'linewidth': 1, 'edgecolor': 'black'}

        colors = cmap([i for i in range(len(ratios))])
        plt.title("주요 브랜드")
        plt.pie(ratios,labels=labels, autopct='%.1f%%', colors = colors,explode = explode,  wedgeprops = wedgeprops, pctdistance=0.7,counterclock=False, shadow=True)
        plt.legend(bbox_to_anchor=(1.05, 1.0), loc='upper left')

        plt.figure
        plt.show()

        print("\033[1m \033[43m"+ "기타 브랜드"+'\033[0m')
        for i,v in enumerate(sorted(extra_label)):
            if i % 9 == 0: print()
            print(v,end= ", ")


    # ### 모든 아이템

    # In[23]:


    show_brand_ratio(all_df)


    # ### Top

    # In[24]:


    show_brand_ratio(top_df)


    # ### Outer

    # In[25]:


    show_brand_ratio(outer_df)


    # ### Bottom

    # In[26]:


    show_brand_ratio(bottom_df)


    # ### Onepiece

    # In[27]:


    show_brand_ratio(onepiece_df)


    # ### Skirt

    # In[28]:


    show_brand_ratio(skirt_df)


    # ### Sneakers

    # In[29]:


    show_brand_ratio(sneakers_df)


    # ### Shoes

    # In[30]:


    show_brand_ratio(shoes_df)


    # # 좋아요 수의 범위 분류 ->  범위에 따른 의류의 가격대 분석/시각화
    # ## 의미 : 의류의 가격대와 사람들의 구매 선호도간의 상관관계를 유추해 볼 수 있을 것으로 생각된다.
    # ### 좋아요 수의 범위 설정 : quantile을 사용해서 q1, q2, q3, q4로 분류
    # ### median 가격 사용 : Outlier로 인해 mean값 대신 median값을 사용해서 시각화 진행

    # ## 전처리

    # In[19]:


    df = pd.read_csv('musinsa_df.csv')


    # In[20]:


    all_df['분류'] = 'all'
    top_df['분류'] = 'top'
    outer_df['분류'] = 'outer'
    bottom_df['분류'] = 'bottom'
    onepiece_df['분류'] = 'onepiece'
    skirt_df['분류'] = 'skirt'
    sneakers_df['분류'] = 'sneakers'
    shoes_df['분류'] = 'shoes'


    # In[59]:


    df = pd.concat([all_df, top_df, outer_df, bottom_df, onepiece_df, skirt_df, sneakers_df, shoes_df], axis = 0)


    # In[60]:


    df['가격'] = df['가격'].apply(lambda x: re.sub(r'[원]|[,]','',x)).map(lambda x : int(x))
    df['평가 수'] = df['평가 수'].apply(lambda x: re.sub(r'[,]','',x)).map(lambda x : int(x))
    df['좋아요 수'] = df['좋아요 수'].apply(lambda x: re.sub(r'[,]','',x)).map(lambda x : int(x))


    # In[61]:


    df[df['분류'] == 'bottom']


    # In[63]:


    def categorize(df) : 
        category = ['all','top','outer','bottom','onepiece','skirt','sneakers','shoes']

        for cat in category : 
            globals()[f'{cat}_'] = df[df['분류'] == cat]

    categorize(df)


    # In[65]:


    q1 = df[df['분류'] == 'all']['좋아요 수'].describe()['25%']
    q2 = df[df['분류'] == 'all']['좋아요 수'].describe()['50%']
    q3 = df[df['분류'] == 'all']['좋아요 수'].describe()['75%']
    q4 = df[df['분류'] == 'all']['좋아요 수'].describe()['max']


    # In[66]:


    category = ['all','top','outer','bottom','onepiece','skirt','sneakers','shoes']
    def get_q(df, category) : 
        q1 = df[df['분류'] == category]['좋아요 수'].describe()['25%']
        q2 = df[df['분류'] == category]['좋아요 수'].describe()['50%']
        q3 = df[df['분류'] == category]['좋아요 수'].describe()['75%']
        q4 = df[df['분류'] == category]['좋아요 수'].describe()['max']

        return [q1, q2, q3, q4]


    for cat in category : 
        globals()[f'{cat}_q_lst'] = get_q(df, cat)


    # In[81]:


    print("좋아요 수 quantile")
    for cat in category : 
        print(f'{cat} : ', globals()[f'{cat}_q_lst'])


    # In[93]:


    # quantile에 따른 df 만들기 [변수 = (cateogry_q1, q2, q3, q4)]

    def like_quantile_range(cat) : 
        tmp_lst = globals()[f'{cat}_q_lst']
        tmp_df = globals()[f'{cat}_']

        for i in range(len(tmp_lst)) : 
            if i == 0 : 
                globals()[f'{cat}_q{str(i+1)}'] = tmp_df[tmp_df['좋아요 수'] <= tmp_lst[i]]
            else : 
                globals()[f'{cat}_q{str(i+1)}'] = tmp_df[(tmp_df['좋아요 수'] <= tmp_lst[i]) & (tmp_df['좋아요 수'] > tmp_lst[i-1])]


    for cat in category : 
        like_quantile_range(cat)


    # ## 좋아요 수를 기준으로 어떤 가격대의 제품이 선호되는가

    # In[121]:


    for cat in category : 
        print(f"{cat} 종류의 가격대 중앙값")
        for i in range(4) : 
            print(f'q{i+1} : ', globals()[f'{cat}_q{i+1}']['가격'].median())


    # ## quantile 별로 시각화(median)

    # In[161]:


    f, axe = plt.subplots(2,2, figsize = (20,15))
    # set title
    axe[0][0].set_title('Q1', fontsize = 15)
    axe[0][1].set_title('Q2', fontsize = 15)
    axe[1][0].set_title('Q3', fontsize = 15)
    axe[1][1].set_title('Q4', fontsize = 15)
    # set x_label
    axe[0][0].set_xlabel('category', fontsize = 15)
    axe[0][1].set_xlabel('category', fontsize = 15)
    axe[1][0].set_xlabel('category', fontsize = 15)
    axe[1][1].set_xlabel('category', fontsize = 15)
    # set y_label
    axe[0][0].set_ylabel('median_price', fontsize = 20)
    axe[0][1].set_ylabel('median_price', fontsize = 20)
    axe[1][0].set_ylabel('median_price', fontsize = 20)
    axe[1][1].set_ylabel('median_price', fontsize = 20)
    # plotting
    axe[0][0].bar(category,
                  [all_q1['가격'].median(),top_q1['가격'].median()
                  ,outer_q1['가격'].median(),bottom_q1['가격'].median()
                  ,onepiece_q1['가격'].median(),skirt_q1['가격'].median()
                  ,sneakers_q1['가격'].median(),shoes_q1['가격'].median()])
    axe[0][1].bar(category,
                  [all_q2['가격'].median(),top_q2['가격'].median()
                  ,outer_q2['가격'].median(),bottom_q2['가격'].median()
                  ,onepiece_q2['가격'].median(),skirt_q2['가격'].median()
                  ,sneakers_q2['가격'].median(),shoes_q2['가격'].median()])
    axe[1][0].bar(category,
                  [all_q3['가격'].median(),top_q3['가격'].median()
                  ,outer_q3['가격'].median(),bottom_q3['가격'].median()
                  ,onepiece_q3['가격'].median(),skirt_q3['가격'].median()
                  ,sneakers_q3['가격'].median(),shoes_q3['가격'].median()])
    axe[1][1].bar(category,
                  [all_q4['가격'].median(),top_q4['가격'].median()
                  ,outer_q4['가격'].median(),bottom_q4['가격'].median()
                  ,onepiece_q4['가격'].median(),skirt_q4['가격'].median()
                  ,sneakers_q4['가격'].median(),shoes_q4['가격'].median()])
    plt.show()


    # 전반적으로 Q1 > Q2 > Q3 > Q4의 순서대로 가격대가 형성되는 것으로 보다 전체적으로 가격대와 선호도의 음의 상관관계가 있을 것으로 보인다.

    # ## category 별로 시각화(median)

    # In[153]:


    category_nda = np.array(category)
    category_nda = category_nda.reshape(4,2)


    # In[160]:


    f, axe = plt.subplots(4,2, figsize = (20,30))

    for i in range(4) : 
        for j in range(2) : 
            # set title
            axe[i][j].set_title(category_nda[i][j], fontsize = 15)
            # set x_label
            axe[i][j].set_xlabel('quantile', fontsize = 15)
            # set y label
            axe[i][j].set_ylabel('median_price', fontsize = 20)
            # plotting
            axe[i][j].bar(['q1','q2','q3','q4'],[globals()[f'{category_nda[i][j]}_q1']['가격'].median(),
                                                 globals()[f'{category_nda[i][j]}_q2']['가격'].median(),
                                                 globals()[f'{category_nda[i][j]}_q3']['가격'].median(),
                                                 globals()[f'{category_nda[i][j]}_q4']['가격'].median()])

    plt.show()


    # 1. 가격의 중앙값 = Q1 > Q2 > Q3 > Q4의 순서인 카테고리 : all, top, outer, bottom
    # 2. else : onepiece, skirt, sneakers, shoes

    # - 1번에 해당되는 의류의 종류는 2번에 해당되는 의류들 보다 평소에 더 자주 많이 사는 종류의 의류들이므로 가격대가 낮을 수록 선호도가 높은 것으로 예상된다.
    # - 2번에 해당되는 의류의 종류는 1번 보다 자주 사지 않기 때문에 한번 살 때 괜찮은 퀄리티, 디자인, 가격대 등을 갖춘 제품을 선호하는 것으로 예상된다.

    # In[ ]:




