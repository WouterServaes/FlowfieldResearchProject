#pragma once
#include "stdafx.h"
#include "projects/App_Flowfield/Grid.h"

namespace Algorithms
{
	
	void Dijkstra(std::vector<Grid::GridSquare>* pGrid, const Elite::Vector2& gridResolution)
	{
		struct dijkstraSqr{
			float distanceCost{FLT_MAX};
			int prevGridIdx{};
		};

		//finding a goal
		auto it{ std::find_if(pGrid->begin(), pGrid->end(), [](const Grid::GridSquare& sqr) 
			{
				return sqr.squareType == Grid::SquareType::Goal;
			}) };

		int goalIdx{ std::distance(pGrid->begin(), it) };


		//making a dijkstra grid
		std::vector<float> dijkstraGrid{};

		//1 generate an empty grid - everything 0
		//2 all places with obstacles to max
		float max{ pGrid->size() + 1 };

		for (size_t idx{}; idx < pGrid->size(); ++idx)
			if (pGrid->at(idx).squareType != Grid::SquareType::Obstacle)
				dijkstraGrid.push_back(max);
			else
				dijkstraGrid.push_back(FLT_MAX);

		//set end as 0

		dijkstraGrid[goalIdx] = 0;

		



	}


}