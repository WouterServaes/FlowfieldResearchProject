# Flowfield Research Project
Research project about Flowfields. Part of first semester Exam Assignment for Gameplay Programming, a second year course at Howest - [Digital Arts and Entertainment](https://digitalartsandentertainment.be/) 

## Table of contents
- [Program overview](#overview)  
     - [Making the world](#makingWorld)
- [Flow field implementation](#implementation)
     - [Explanation](#flowfieldExplanation)
     - [Code](#flowfieldCode)
       - [Starting in Grid.cpp](#startingInGrid)
       - [Dijkstra](#dijkstraAlgo)
          - [Dijkstra in short](#dijkstraInShort)
       - [Making flow field](#makingFlowField)
       - [Handling the agents](#agentHandling)
          -[Moving agent in directio](#movingAgent)
- [Extras](#extras)

## Program overview <a name ="overview"></a>
Program starts with a blank 500x500 world. Spawnpoints, obstacles and goals can be places onto the world. The agents will either spawn at a random location in the world when no spawn points are present or spawn at spawnpoints over time. Agents will collide with obstalces and with other agents. Every agent will go to a random goal when spawned into the world. There has to be atleast one goal present in the world. If only one goal is present in the world and an agent reaches this goal, they despawn. If more than one goals are present in the world the agents can go to a next, random, goal unless the "remove at goal" checkbox is checked, they'll despawn if this is checked. A world can either be made by manually clicking on tiles or by loading in one of the saved file in the "Environments" folder, created worlds can be saved to a new file in this folder.
### Making the world <a name ="makingWorld"></a>
You can load in a pre made world by clicking "use from file" and writing a correct file name in the console. (all pre made worlds are listed below).  
To make your own world, use LMB to "place" the environment. Clicking a placed tile will remove it. Tile types can be switched by pressing 1 to 4 on the keyboard.
- 1: Goal* 
- 2: Obstacle
- 3: Spawner**
- 4: empty tile  
\*One goal is required.  
\*\*No spawner are  required, agents will spawn at a random location if there are no spawners.

premade worlds  
- Env_01.txt
- Env_02.txt
- Env_03.txt
- Env_04.txt
- Env_05.txt

## Flow field implementation <a name ="implementation"></a>
### Explanation <a name ="flowfieldExplanation"></a>
For every goal in the world, an algorithm ([Dijkstra](#dijkstraAlgo)) goes over the grid, sets the distance costs per tile for this goal. When every tile has a distance cost for this goal, go over the tiles again. Set the direction of this tile to point to the neighbor with the lowest distance cost, diagonals allowed (watch out with diagonals through walls).   
Every agent as a goal, the agent will follow the direction of the square it's currently on to it's goal. The agent does this using a basic seek movement behavior. I set the target of this behavior to the middle of the grid the direction points to, when the agent is close to this target, a new target is made.  
Obstacles can be placed while it's running, when an obstacle is placed, the flow fields get remade.
### Code <a name ="flowfieldCode"></a>
_[Generating Flowfield File](Project/App_Flowfield/GeneratingFlowField.h)_
#### Starting in Grid.cpp <a name ="startingInGrid"></a>

I make the flow fields in the [Grid.cpp](Project/App_Flowfield/Grid.cpp). First, I define the directions the flow field can go to (every direction, including diagonals). Then, I search for all the goals in the grid and go over each goal, as each goal has to have flowfield. For every goal, I run the algorithm, [Dijkstra](#dijkstraAlgo), and make the flowfield. Every grid square has a [GridSquare struct](#gridSquareStruct), which has a std::vector of Elite::Vector2 type attribute: flowDirections ([to code](https://github.com/WouterServaes/FlowfieldResearchProject/blob/aa795e2cebb96aec2524997e73cfefde78cc3de9/Project/App_Flowfield/Grid.cpp#L274)). 
```cpp
void Grid::MakeFlowfield()
{
	const std::vector<Elite::Vector2> flowfieldFlowDirections = {
		{ 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 1 }, { -1, 0 }, { -1, -1 }, { 0, -1 }, { 1, -1 } }; 

	std::vector<size_t> goalIndxs{};

	for (const auto& gridSqr : *m_pGrid)
		if (gridSqr.squareType == Grid::SquareType::Goal)
			goalIndxs.push_back(gridSqr.column + (gridSqr.row * m_GridResolution.x));

	Algorithms::Dijkstra* dijkstraAlgorithm = new Algorithms::Dijkstra(&m_GridResolution);

	for (size_t idx{}; idx < goalIndxs.size(); ++idx)
	{
		dijkstraAlgorithm->RunAlgorithm(idx, goalIndxs[idx], m_pGrid);
		dijkstraAlgorithm->MakeFlowfield(idx, m_pGrid, flowfieldFlowDirections);
	}
	delete dijkstraAlgorithm;

	m_ObstaclesReady = true;
}
```
GridSquare struct<a name="gridSquareStruct"></a>
```cpp
struct GridSquare
{
	size_t row{}, column{}; 
	Elite::Vector2 botLeft{}; 
	std::vector<Elite::Vector2> flowDirections{}; 
	SquareType squareType{ SquareType::Default }; 
};
```
#### Dijkstra <a name ="dijkstraAlgo"></a>
I first clear the m_DistancesGrid vector, this holds the distance costs of every grid square. I define the max cost (size of the grid + 1) and set every distance cost to this max, except the squares that contain an obstacle, these get INT_MAX to make sure they won't be visited. The goal gets a distance cost of 0, the distance from the goal to the goal is 0. Dijkstra will only be allowed to visit the direct neighbors, no diagonals (I allow diagonals when making the flowfield itself) ([to code](https://github.com/WouterServaes/FlowfieldResearchProject/blob/aa795e2cebb96aec2524997e73cfefde78cc3de9/Project/App_Flowfield/GeneratingFlowField.h#L109)).
```cpp
m_DistancesGrid.clear();

std::vector<IdxToVisit>toVisit{};

int max{ int(pGrid->size()) + 1 };

for (size_t idx{}; idx < pGrid->size(); ++idx)
	if (pGrid->at(idx).squareType != Grid::SquareType::Obstacle)
		m_DistancesGrid.push_back(max);
	else
		m_DistancesGrid.push_back(INT_MAX); 

m_DistancesGrid[goalGridIdx] = 0;

std::vector<Elite::Vector2> neighbors{ { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } }; 
```
To have a begin, initial point, I go over the neighbors of the goal square. If this neighbor is a valid square (it's not outside the grid) and it's not an obstacle, I set the distance cost of this neighbor to 1 (it's next to the goal) and I add it to the toVisit vector. The toVisit vector holds the idxs of the squares Dijkstra has to visit ([to code](https://github.com/WouterServaes/FlowfieldResearchProject/blob/aa795e2cebb96aec2524997e73cfefde78cc3de9/Project/App_Flowfield/GeneratingFlowField.h#L131)). After this, I go over all the toVisit idxs. For every toVisit idx, I go over its neighbors and check if it's a valid neighbor. I then check if this square has been explored or not. If it has not been explored, set the distance cost of this neighbor to the distance cost of the toVisit sqr + 1 and add this neighbor to the toVisit vector ([to code](https://github.com/WouterServaes/FlowfieldResearchProject/blob/aa795e2cebb96aec2524997e73cfefde78cc3de9/Project/App_Flowfield/GeneratingFlowField.h#L153)).
```cpp
for (size_t toVisitLoopIdx{}; toVisitLoopIdx < toVisit.size(); ++toVisitLoopIdx) 
{
     auto toVisitColumn{ pGrid->at(toVisit[toVisitLoopIdx].idx).column }; 
	auto toVisitRow{ pGrid->at(toVisit[toVisitLoopIdx].idx).row }; 

	for (size_t neighborLoopIdx{}; neighborLoopIdx < 4; ++neighborLoopIdx) 
	{
          const auto& neighbor{ neighbors[neighborLoopIdx] };
          size_t neighborColumn{ toVisitColumn + size_t(neighbor.x) };
          size_t neighborRow{ size_t(toVisitRow + neighbor.y) };
                              
          if (neighborColumn >= m_pGridResolution->x || neighborColumn < 0) continue; 
          if (neighborRow >= m_pGridResolution->y || neighborRow < 0) continue;

          size_t neighborIdx{ (
               neighborColumn +
               size_t(neighborRow * m_pGridResolution->x)) }; 

          if (m_DistancesGrid[neighborIdx] == max) 
          {
               m_DistancesGrid[neighborIdx] = toVisit[toVisitLoopIdx].distance + 1; 
               toVisit.push_back(IdxToVisit{ neighborIdx, toVisit[toVisitLoopIdx].distance + 1 }); 
          }
	}
}
``` 
In short <a name="dijkstraInShort"></a>
1. Set distance of cost of all squares to max cost
     1. Cost of goal idx = 1
     2. Cost of squares with obstacle = max int
2. Goal grid square
     1. For every neighbor of this goal idx
          1. Set cost of this neighbor idx to 1
          2. Add this neighbor to toVisit
3. For every grid square idx in toVisit
     1. For every neighbor of this idx
          1. if the neighbor has not been visited
               1. Set cost of this neighbor to cost of toVisit idx cost + 1
               2. Add this neighbor to toVisit
     
#### Making flow field <a name="makingFlowField"></a>
I go over every grid square. If the distance cost of this square is >= max (obstacle), I go to the next one. I check all the (valid) neighbors of this square, diagonals included, and set the direction of the square to point to the neighbor with the lowest distance cost. To avoid the direction going diagonally through a wall, I check this first and skip this neighbors if this is the case ([to code](https://github.com/WouterServaes/FlowfieldResearchProject/blob/aa795e2cebb96aec2524997e73cfefde78cc3de9/Project/App_Flowfield/GeneratingFlowField.h#L60)).
```cpp
for (size_t idx{}; idx < pGrid->size(); ++idx)
{
	if (m_DistancesGrid[idx] >= max) continue;
	auto column{ pGrid->at(idx).column }; 
	auto row{ pGrid->at(idx).row }; 

	int lowestDist{ max };
	int lowestDistNeighborIdx{};

	for (size_t neighborLoopIdx{}; neighborLoopIdx < flowfieldFlowDirections.size(); ++neighborLoopIdx)
	{
		size_t neighborColumn{ column + size_t(flowfieldFlowDirections[neighborLoopIdx].x) };
		size_t neighborRow{ row + size_t(flowfieldFlowDirections[neighborLoopIdx].y) };

		if (neighborColumn >= m_pGridResolution->x || neighborColumn < 0) continue; 
		if (neighborRow >= m_pGridResolution->y || neighborRow < 0) continue;

		size_t neighborIdx{ (
			neighborColumn +
			size_t(neighborRow * m_pGridResolution->x)) }; //index of this neighbor in the grid

		if (m_DistancesGrid[neighborIdx] <= lowestDist)
		{
			if (flowfieldFlowDirections[neighborLoopIdx].y != 0 && flowfieldFlowDirection[neighborLoopIdx].x != 0)
				if (CheckDiagonalThroughWall(pGrid, flowfieldFlowDirections, neighborLoopIdx, neighborColumn,neighborRow)) continue;

			lowestDistNeighborIdx = neighborLoopIdx;
			lowestDist = m_DistancesGrid[neighborIdx];
		}
	}
	pGrid->at(idx).flowDirections[goalNr] = flowfieldFlowDirections[lowestDistNeighborIdx];
}
```
#### Handling the agents <a name = "agentHandling"></a>
I update all the agents in  [App_Flowfield.cpp](Project/App_Flowfield/App_Flowfield.cpp):: [HandleAgentUpdate](https://github.com/WouterServaes/FlowfieldResearchProject/blob/aa795e2cebb96aec2524997e73cfefde78cc3de9/Project/App_Flowfield/App_Flowfield.cpp#L295). I go over every agent and check if this agent has reached its goal. If it has, I mark it for removal if you decide to remove the agent at the goal. If it has reached its goal but the agent shouldn't be removed, it gets a new, random, goal. If the agent has not reached its goal, I call MoveSqr from the grid (more info below) and update the agent itself ([to agent Update function](https://github.com/WouterServaes/FlowfieldResearchProject/blob/aa795e2cebb96aec2524997e73cfefde78cc3de9/Project/App_Flowfield/FlowfieldAgent.cpp#L31)). After going over all the agents, I go over them again and remove the ones marked for remove ([to code](https://github.com/WouterServaes/FlowfieldResearchProject/blob/aa795e2cebb96aec2524997e73cfefde78cc3de9/Project/App_Flowfield/App_Flowfield.cpp#L326)).

##### Moving agent <a name="movingAgent"></a>
To set a new target for the agent, I get the square idx of the position the agent is at [to code GetGridSqrIdxAtPos()](https://github.com/WouterServaes/FlowfieldResearchProject/blob/aa795e2cebb96aec2524997e73cfefde78cc3de9/Project/App_Flowfield/Grid.cpp#L197). I go in the direction of the goalNr (flow field nr) of this square and get a position in the next square. I then get the idx of this new square and set the new target for this agent to the middle of this new squares
```cpp
void Grid::MoveSqr(const Elite::Vector2& currentPos, Elite::Vector2& targetPos, int goalNr)
{
	const auto sqrIdx{ GetGridSqrIdxAtPos(currentPos) }; 
	const auto nextSqrPosFromDirection{ currentPos + (m_pGrid->at(sqrIdx).flowDirections[goalNr].GetNormalized() * (m_SquareSize.x + (m_SquareSize.x / 2))) }; 
	const auto newSqrIdx{ GetGridSqrIdxAtPos(nextSqrPosFromDirection) }; 
	targetPos = GetMidOfSquare(newSqrIdx); 
}
```
## Extras <a name="Extras"></a>
  For this project I used the Elite Engine framework, authors of this framework are Matthieu Delaere and Thomas Goussaert. 





