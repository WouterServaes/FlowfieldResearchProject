#pragma once
#include "stdafx.h"
#include "projects/App_Flowfield/Grid.h"

namespace Algorithms
{

	std::vector<int> dijkstraGrid{};
	void Dijkstra(std::vector<Grid::GridSquare>* pGrid, const Elite::Vector2& gridResolution)
	{



		//toVisit struct containing and index to visit in distances and the distance from the startIdx to the goalIdx
		struct IdxToVisit {
			size_t idx{};
			int distance{};

		};

		//finding a goal
		auto it{ std::find_if(pGrid->begin(), pGrid->end(), [](const Grid::GridSquare& sqr) 
			{
				return sqr.squareType == Grid::SquareType::Goal;
			}) };


		//goal index and start index, searching the whole grid so starting at 0
		int goalIdx{ std::distance(pGrid->begin(), it) };
		int startIdx{ 0 };

		//to visit vector 
		std::vector<IdxToVisit>toVisit{};
		
		//max distance + 1, used for unexplored indexes
		int max{ int(pGrid->size()) + 1 };

		for (size_t idx{}; idx < pGrid->size(); ++idx)
			if (pGrid->at(idx).squareType != Grid::SquareType::Obstacle)
			{
				dijkstraGrid.push_back(max);
			}
			else
			{
				dijkstraGrid.push_back(INT_MAX); //int max used to mark obstacles
			}

		dijkstraGrid[goalIdx] = 0; //distance of the goal index is 0 as we start there
		
		std::vector<Elite::Vector2> neighbors{ {1,0}, {-1,0}, {0,1}, {0,-1} }; //used to calculate the neighbor of an index, no diagonals (x = column, y = row)

		//setting the initial index around the start node
		for (size_t idx{}; idx < 4; ++idx)
		{
			size_t column{ pGrid->at(goalIdx).column + size_t(neighbors[idx].x) };
			size_t row{ size_t(pGrid->at(goalIdx).row + neighbors[idx].y) };
			if (column >= gridResolution.x) continue;
			if (row>= gridResolution.y) continue;

			size_t neighborIdx{(
				column+ 
				size_t(row * gridResolution.x)) };		

			if (pGrid->at(neighborIdx).squareType != Grid::SquareType::Obstacle)
			{
				dijkstraGrid[neighborIdx] = 1;
				toVisit.push_back(IdxToVisit{ neighborIdx, 1 });
			}
		}
		
		//looping over all the to visit indices
		for (size_t toVisitLoopIdx{}; toVisitLoopIdx < toVisit.size(); ++toVisitLoopIdx) //for all indices to visit
		{
			auto toVisitColumn{ pGrid->at(toVisit[toVisitLoopIdx].idx).column }; //column of to-visit-idx
			auto toVisitRow{ pGrid->at(toVisit[toVisitLoopIdx].idx).row }; //row of to-visit-idx

			for (size_t neighborLoopIdx{}; neighborLoopIdx < 4; ++neighborLoopIdx) //for all neighbors of this index
			{
				const auto& neighbor{ neighbors[neighborLoopIdx] }; 
				size_t neighborColumn{ toVisitColumn + size_t(neighbor.x) };
				size_t neighborRow{ size_t(toVisitRow + neighbor.y) };

				if (neighborColumn >= gridResolution.x || neighborColumn < 0) continue; //checking if this "neighbor" is valid (if it's out the grid this will be a very large number) it's a nasty fix I know
				if (neighborRow >= gridResolution.y || neighborRow < 0) continue;	

				size_t neighborIdx{ (
					neighborColumn +
					size_t(neighborRow * gridResolution.x)) }; //index of this neighbor in the grid

				if (dijkstraGrid[neighborIdx] == max) //check if the this index has been explored
				{
					dijkstraGrid[neighborIdx] = toVisit[toVisitLoopIdx].distance +1; //change the distance to "visit index distance + 2"
					toVisit.push_back(IdxToVisit{ neighborIdx, toVisit[toVisitLoopIdx].distance + 1 }); //add this neighbor to the visit vector
				}
			}
		}


		//pointing the grid square direction to the neighbor with the lowest distance

		//4   3  2
		//5   0  1
		//6   7  8

		//also diagonal neighbors now
		neighbors = { 
			{ 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 1 }, { -1, 0 }, { -1, -1 },{ 0, -1 }, {1,-1} };

		//creating the flowfield, going over each grid index and setting the direction to the lowest neighbor distance
		for (size_t idx{}; idx < pGrid->size(); ++idx)
		{
			if (dijkstraGrid[idx] >= max) continue;
			auto column{ pGrid->at(idx).column }; //column 
			auto row{ pGrid->at(idx).row }; //row 

			int lowestDist{max};
			int lowestDistNeighborIdx{};

			for (size_t neighborLoopIdx{}; neighborLoopIdx < neighbors.size(); ++neighborLoopIdx)
			{
				size_t neighborColumn{ column + size_t(neighbors[neighborLoopIdx].x) };
				size_t neighborRow{ row + size_t( neighbors[neighborLoopIdx].y) };

				if (neighborColumn >= gridResolution.x || neighborColumn<0) continue; //checking if this "neighbor" is valid (if it's out the grid this will be a very large number) it's a nasty fix I know
				if (neighborRow >= gridResolution.y || neighborRow< 0) continue;

				size_t neighborIdx{ (
					neighborColumn +
					size_t(neighborRow * gridResolution.x)) }; //index of this neighbor in the grid

				if (dijkstraGrid[neighborIdx] <= lowestDist)
				{
					lowestDist = dijkstraGrid[neighborIdx];
					lowestDistNeighborIdx = neighborLoopIdx;
				}
			}

			pGrid->at(idx).flowDirections[0] = neighbors[lowestDistNeighborIdx];


		}


	}
	

	

}