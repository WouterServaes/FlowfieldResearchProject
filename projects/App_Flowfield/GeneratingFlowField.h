#pragma once
#include "stdafx.h"
#include "projects/App_Flowfield/Grid.h"

namespace Algorithms
{

	void Dijkstra(std::vector<Grid::GridSquare>* pGrid, const Elite::Vector2& gridResolution)
	{

		std::vector<int> dijkstraGrid{};

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
		
		const std::vector<Elite::Vector2> neighbors{ {1,0}, {-1,0}, {0,1}, {0,-1} }; //used to calculate the neighbor of an index, no diagonals (x = column, y = row)

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
		
		//looping over all the indices
		for (size_t toVisitLoopIdx{}; toVisitLoopIdx < toVisit.size(); ++toVisitLoopIdx) //for all indices to visit
		{
			auto toVisitColumn{ pGrid->at(toVisit[toVisitLoopIdx].idx).column }; //column of to-visit-idx
			auto toVisitRow{ pGrid->at(toVisit[toVisitLoopIdx].idx).row }; //row of to-visit-idx

			for (size_t neighborLoopIdx{}; neighborLoopIdx < 4; ++neighborLoopIdx) //for all neighbors of this index
			{
				const auto& neighbor{ neighbors[neighborLoopIdx] }; 
				size_t neighborColumn{ toVisitColumn + size_t(neighbor.x) };
				size_t neighborRow{ size_t(toVisitRow + neighbor.y) };

				if (neighborColumn >= gridResolution.x) continue; //checking if this "neighbor" is valid (if it's out the grid this will be a very large number) it's a nasty fix I know
				if (neighborRow >= gridResolution.y) continue;	

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
	}
	

	

}