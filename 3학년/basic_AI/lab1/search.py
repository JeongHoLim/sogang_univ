###### Write Your Library Here ###########

import collections
import heapq
from itertools import permutations


#########################################

def search(maze, func):
    return {
        "bfs": bfs,
        "astar": astar,
        "astar_four_circles": astar_four_circles,
        "astar_many_circles": astar_many_circles
    }.get(func)(maze)


# -------------------- Stage 01: One circle - BFS Algorithm ------------------------ #

def bfs(maze):
    """
    [문제 01] 제시된 stage1의 맵 세가지를 BFS Algorithm을 통해 최단 경로를 return하시오.(20점)
    """
    start_point=maze.startPoint()

    path = []

    ####################### Write Your Code Here ################################
    
    queue = collections.deque()
    visited = [[0 for _ in range(maze.cols)]for _ in range(maze.rows)] 
    # 미로의 방문 정보를 저장하기 위함
    history = [[[-1,-1] for _ in range(maze.cols)]for _ in range(maze.rows)]
    # bfs를 통해 찾은 경로를 추적하기 위함
    queue.append((start_point))

    while queue:
        posX,posY = queue.popleft()             #queue의 왼쪽부터 pop
        if maze.isObjective(posX,posY):         #objective이면 stage1에서는 중단
            break
        neighbors = maze.neighborPoints(posX,posY)      # 물리적으로 갈 수 있는지만 확인하여 list 리턴
        for x,y in neighbors:
            if visited[x][y] == 0:              #방문한 적이 없으면 방문
                queue.append((x,y))
                history[x][y] = (posX,posY)     #해당 미로에 어디서 왔는지 저장
                visited[x][y] = 1       
    
    while (posX,posY) != start_point:           #시작점 올 때까지 
        path.append((posX,posY))
        posX,posY = history[posX][posY]
    path.append(start_point)

    return path[::-1]

    ############################################################################

class Node:
    def __init__(self,parent,location):
        self.parent=parent
        self.location=location #현재 노드

        self.obj=[]

        # F = G+H
        self.f=0
        self.g=0
        self.h=0

    def __eq__(self, other):
        return self.location==other.location and str(self.obj)==str(other.obj)

    def __le__(self, other):
        return self.g+self.h<=other.g+other.h

    def __lt__(self, other):
        return self.g+self.h<other.g+other.h

    def __gt__(self, other):
        return self.g+self.h>other.g+other.h

    def __ge__(self, other):
        return self.g+self.h>=other.g+other.h


# -------------------- Stage 01: One circle - A* Algorithm ------------------------ #

def manhatten_dist(p1,p2):
    return abs(p1[0]-p2[0])+abs(p1[1]-p2[1])

def astar_modified(maze,start_point,end_point,func):
    visited = [[0 for _ in range(maze.cols)]for _ in range(maze.rows)] 
    # 미로의 방문 정보를 저장하기 위함
    new_node = Node(None,start_point)               #start_point 노드 생성
    heap = [new_node]       #heap 생성
    path = []
 
    visited[start_point[0]][start_point[1]] = 1         #start_point 노드 방문 표시
    while heap:
        cur_node = heapq.heappop(heap)
        if cur_node.location == end_point:              #goal test
            break
        neighbors = maze.neighborPoints(cur_node.location[0],cur_node.location[1])      # 물리적으로 갈 수 있는지만 확인하여 list 리턴
        for x,y in neighbors:
            if visited[x][y] == 0:              #방문한 적이 없으면 방문
                new_node = Node(cur_node,(x,y))
                new_node.g,new_node.h = cur_node.g+1,func((x,y),end_point)      #g는 그 전 노드 g 값에 1 더한 값, h는 휴리스틱으로 end_point까지의 거리 계산
                heapq.heappush(heap,new_node)           #heap에 push
                visited[x][y] = 1

    while cur_node:         #start_point의 parent를 None으로 초기화했기 때문
        path.append(cur_node.location)
        cur_node = cur_node.parent

    return path[::-1]


def astar(maze):

    """
    [문제 02] 제시된 stage1의 맵 세가지를 A* Algorithm을 통해 최단경로를 return하시오.(20점)
    (Heuristic Function은 위에서 정의한 manhatten_dist function을 사용할 것.)
    """

    start_point=maze.startPoint()

    end_point=maze.circlePoints()[0]

    path=[]

    ####################### Write Your Code Here ################################
    
    return astar_modified(maze,start_point,end_point,manhatten_dist) #휴리스틱으로 맨해튼거리 함수 넘겨줌

    ############################################################################


# -------------------- Stage 02: Four circles - A* Algorithm  ------------------------ #


def stage2_heuristic(p1,p2):                #휴리스틱으로 맨해튼 거리에 (맨해튼거리//유클리드 거리)를 더해줬다.
    
    dx,dy = abs(p1[0]-p2[0]),abs(p1[1]-p2[1])       
    if dx + dy == 0: return 0
    add = (dx+dy)//(dx**2+dy**2)**(1/2)
    return dx + dy + add


def astar_four_circles(maze):
    """
    [문제 03] 제시된 stage2의 맵 세가지를 A* Algorithm을 통해 최단 경로를 return하시오.(30점)
    (단 Heurstic Function은 위의 stage2_heuristic function을 직접 정의하여 사용해야 한다.)
    """

    end_points=maze.circlePoints()
    end_points.sort()

    path=[]

    ####################### Write Your Code Here ################################

    start_point=maze.startPoint()
    candidates = [start_point] + end_points
    
    dist = [[None for _ in range(5)] for _ in range(5)]
    
    for k,s in enumerate(candidates):
        for i,e in enumerate(end_points,1):
            if k == i or dist[i][k]: continue
            temp_p = astar_modified(maze,s,e,stage2_heuristic)
            dist[k][i],dist[i][k] = (len(temp_p),temp_p),(len(temp_p),temp_p[::-1])
    
    test = list(permutations([1,2,3,4], 4))
    path_combi,path_min = None, float('inf')
    
    for t in test:
        fr = temp_sum = 0
        for i in t:
            temp_sum += dist[fr][i][0]
            if temp_sum > path_min:
                break
            fr = i
        if path_min > temp_sum:
            path_combi,path_min = t,temp_sum
    
    path += dist[0][path_combi[0]][1]
    fr = path_combi[0]
    for i in path_combi[1:]:
        path += dist[fr][i][1][1:]
        fr = i    
    
    return path

    ############################################################################



# -------------------- Stage 03: Many circles - A* Algorithm -------------------- #

def stage3_heuristic(objectives,start,edges,tags):
    cost_sum=0

    ####################### Write Your Code Here ################################

    adjacent = collections.defaultdict(list)
    check_list = [0]*(len(edges)//2)
    for object in objectives:
        check_list[tags[object]]=1
    
    for w,f,t in edges:
        if check_list[t]: 
            if check_list[f]:     #f의 좌표가 objectives안에 있으면
                adjacent[t].append([w,t,f])
            elif f == start:             # f가 시작인덱스 이거나
                adjacent[f].append([w,f,t])
         
    adjacent_edges = adjacent[start]       
    heapq.heapify(adjacent_edges)       #weight기준으로 heap
    length = len(objectives)+1

    connected = [0]*(len(edges)//2)
    connected[start] = 1        #object가 연결된 지 확인하는 배열

    # prim alogorithm
    while adjacent_edges:
        w,f,t = heapq.heappop(adjacent_edges)
        if connected[t] == 0:        
            cost_sum += w
            connected[t] = 1
            length -= 1
            if length == 0: break       #다 연결 되었을 때 while문 빨리 탈출하기 위함  
            for edge in adjacent[t]:
                if connected[edge[2]]==0:        #인접 objective들 추가
                    heapq.heappush(adjacent_edges,edge)
    
    ############################################################################
    return cost_sum

def astar_many_circles(maze):
    """
    [문제 04] 제시된 stage3의 맵 세가지를 A* Algorithm을 통해 최단 경로를 return하시오.(30점)
    (단 Heurstic Function은 위의 stage3_heuristic function을 직접 정의하여 사용해야 하고, minimum spanning tree
    알고리즘을 활용한 heuristic function이어야 한다.)
    """

    end_points= maze.circlePoints()
    end_points.sort()

    path=[]

    ####################### Write Your Code Here ################################

    start_point=maze.startPoint()
    candidates = [start_point] + end_points
    edges = []
    # dist : candidates끼리 방문할 때 소요되는 cost 저장, [cost->int,path->list]
    dist = [[None for _ in range(len(candidates))] for _ in range(len(candidates))]     
    # checked : 방문한 경로 확인용
    checked = [[0 for _ in range(len(candidates))] for _ in range(len(candidates))]

    for k,s in enumerate(candidates):
        for i,e in enumerate(end_points,1):
            if checked[k][i] or s == e: continue   #같은 인덱스이거나 계산된 경로면 무시
            temp_p = astar_modified(maze,s,e,manhatten_dist)        #edge 비용 구할 때는 맨해튼 거리를 휴리스틱으로 사용
            checked[k][i]=checked[i][k] = 1
            dist[k][i],dist[i][k] = (len(temp_p),temp_p),(len(temp_p),temp_p[::-1])
            edges.append((len(temp_p),k,i))     #edge -> [cost,from,to]
            edges.append((len(temp_p),i,k))
            

    new_node = Node(None,start_point)               #start_point 노드 생성
    new_node.obj = end_points[:]                
                    
    heap = [new_node]       #heap 생성 
    tags = collections.defaultdict()

    for i,v in enumerate(candidates):           #편의상 tags로 objective에 인덱스 부여
        tags[v],tags[i] = i,v

    while heap:
        cur_node = heapq.heappop(heap)
        if not cur_node.obj:                #obj가 없다는 것은 이미 다 봤다는 얘기   
            break   
        for neighbor in cur_node.obj:
            new_node = Node(cur_node,neighbor)
            temp = cur_node.obj[:]          # 부모 노드의 obj 상속
            temp.remove(new_node.location)      #자식 노드의 목적지에서 본인 노드 제외
            new_node.obj = temp 
            f,t = tags[cur_node.location],tags[neighbor]        # f,t는 objective 인덱스, from과 to
            new_node.g,new_node.h = cur_node.g + dist[f][t][0],stage3_heuristic(temp,t,edges,tags) # 휴리스틱으로 mst(prim) 사용   
            heapq.heappush(heap,new_node)   #heap에 push
            
    #path가 겹치는 부분이 있지만, 처음은 겹치는 부분을 생략하면 안 되기 때문에 따로 빼놨음
    x,y = cur_node.location,cur_node.parent.location
    path += dist[tags[x]][tags[y]][1]   
    cur_node = cur_node.parent
    
    while cur_node.parent:         #start_point의 parent를 None으로 초기화했기 때문
        x,y = cur_node.location,cur_node.parent.location
        path += dist[tags[x]][tags[y]][1][1:]       #겹치는 path 생략
        cur_node = cur_node.parent

    return path[::-1]               #도착지부터 저장했기 때문에 역순으로 리턴
    
    ############################################################################
