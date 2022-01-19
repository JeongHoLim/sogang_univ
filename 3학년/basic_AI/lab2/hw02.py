from util import manhattanDistance
from game import Directions
import random, util

from game import Agent

## Example Agent
class ReflexAgent(Agent):

  def Action(self, gameState):

    move_candidate = gameState.getLegalActions()
    scores = [self.reflex_agent_evaluationFunc(gameState, action) for action in move_candidate]
    bestScore = max(scores)
    Index = [index for index in range(len(scores)) if scores[index] == bestScore]
    get_index = random.choice(Index)

    return move_candidate[get_index]

  def reflex_agent_evaluationFunc(self, currentGameState, action):

    successorGameState = currentGameState.generatePacmanSuccessor(action)
    newPos = successorGameState.getPacmanPosition()
    oldFood = currentGameState.getFood()
    newGhostStates = successorGameState.getGhostStates()
    newScaredTimes = [ghostState.scaredTimer for ghostState in newGhostStates]

    return successorGameState.getScore()



def scoreEvalFunc(currentGameState):

  return currentGameState.getScore()

class AdversialSearchAgent(Agent):

  def __init__(self, getFunc ='scoreEvalFunc', depth ='2'):
    self.index = 0
    self.evaluationFunction = util.lookup(getFunc, globals())

    self.depth = int(depth)

######################################################################################

class MinimaxAgent(AdversialSearchAgent):
  """
    [문제 01] MiniMax의 Action을 구현하시오. (20점)
    (depth와 evaluation function은 위에서 정의한 self.depth and self.evaluationFunction을 사용할 것.)
  """
  def Action(self, gameState):
    ####################### Write Your Code Here ################################
    def max_value(state,depth):
      if depth == self.depth or state.isWin() or state.isLose():
        return self.evaluationFunction(state)
    
      value = float('-inf')
      actions = state.getLegalPacmanActions()

      for action in actions:
        successorGameState = state.generatePacmanSuccessor(action)
        value = max(value,min_value(successorGameState,depth,1))
      return value

    def min_value(state,depth,idx):

      if state.isWin() or state.isLose():       
        return self.evaluationFunction(state)
 
      value = float('inf')
      actions = state.getLegalActions(idx)
      
      for action in actions:
        successorGameState = state.generateSuccessor(idx,action)
        if idx == state.getNumAgents()-1:
          value = min(value,max_value(successorGameState,depth+1))
        else:
          value = min(value,min_value(successorGameState,depth,idx+1))

      return value

    best_move,best_score = None,float('-inf')
    
    for action in gameState.getLegalActions():
      successorGameState = gameState.generatePacmanSuccessor(action)
      check = min_value(successorGameState,0,1)
      if check > best_score:
        best_score,best_move = check,action
  
    return best_move
    ############################################################################




class AlphaBetaAgent(AdversialSearchAgent):
  """
    [문제 02] AlphaBeta의 Action을 구현하시오. (25점)
    (depth와 evaluation function은 위에서 정의한 self.depth and self.evaluationFunction을 사용할 것.)
  """
  def Action(self, gameState):
    ####################### Write Your Code Here ################################

    def max_value(state,depth,alpha,beta):
      
      if depth == self.depth or state.isWin() or state.isLose():
        return self.evaluationFunction(state)
    
      value = float('-inf')
      actions = state.getLegalPacmanActions()

      for action in actions:
        successorGameState = state.generatePacmanSuccessor(action)
        value = max(value,min_value(successorGameState,depth,1,alpha,beta))
        alpha = max(alpha,value)
        
        if value >= beta:
          return value

      return value

    def min_value(state,depth,idx,alpha,beta):
      
      if state.isWin() or state.isLose():       
        return self.evaluationFunction(state)
      
      value = float('inf')
      actions = state.getLegalActions(idx)
      
      for action in actions:
        successorGameState = state.generateSuccessor(idx,action)
        if idx == state.getNumAgents()-1:
          value = min(value,max_value(successorGameState,depth+1,alpha,beta))
        else:
          value = min(value,min_value(successorGameState,depth,idx+1,alpha,beta))  
        beta = min(value,beta)
        if value <= alpha:
          return value
      
      return value

    best_move,best_score = None,float('-inf')
    actions = gameState.getLegalActions()
    
    alpha,beta = float('-inf'),float('inf')
    for action in actions:
      successorGameState = gameState.generatePacmanSuccessor(action)
      check = min_value(successorGameState,0,1,alpha,beta)
      if check > best_score:
        best_score,best_move = check,action
      alpha = best_score
    return best_move
  
    ############################################################################



class ExpectimaxAgent(AdversialSearchAgent):
  """
    [문제 03] Expectimax의 Action을 구현하시오. (25점)
    (depth와 evaluation function은 위에서 정의한 self.depth and self.evaluationFunction을 사용할 것.)
  """
  def Action(self, gameState):
    ####################### Write Your Code Here ################################

    def chance_value(result):

      return sum(result)/len(result)

    def max_value(state,depth):
      if depth == self.depth or state.isWin() or state.isLose():
        return self.evaluationFunction(state)
    
      value = float('-inf')
      actions = state.getLegalPacmanActions()

      for action in actions:
        successorGameState = state.generatePacmanSuccessor(action)
        value = max(value,min_value(successorGameState,depth,1))
      return value

    def min_value(state,depth,idx):

      if state.isWin() or state.isLose():       
        return self.evaluationFunction(state)
 
      actions = state.getLegalActions(idx)
      values = []
      for action in actions:
        successorGameState = state.generateSuccessor(idx,action)
        if idx == state.getNumAgents()-1:
          value = max_value(successorGameState,depth+1)
        else:
          value = min_value(successorGameState,depth,idx+1)
        values.append(value)
              
      return chance_value(values)
    
    
    best_move,best_score = None,float('-inf')
    
    for action in gameState.getLegalActions():
      successorGameState = gameState.generatePacmanSuccessor(action)
      check = min_value(successorGameState,0,0)
      if check > best_score:
        best_score,best_move = check,action
  
    return best_move


    ############################################################################
