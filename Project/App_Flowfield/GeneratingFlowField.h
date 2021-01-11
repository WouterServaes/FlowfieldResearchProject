#pragma once
#include "stdafx.h"
#include "Project/App_Flowfield/Grid.h"

namespace Algorithms
{
	class Algorithm  
	{
	public:
		Algorithm(const Elite::Vector2* gridResolution) :m_pGridResolution(gridResolution) {};
		virtual ~Algorithm() = default;
		virtual void RunAlgorithm(int goalNr, size_t goalGridIdx, std::vector<Grid::GridSquare>* pGrid) = 0;
		void MakeFlowfield(int goalNr,std::vector<Grid::GridSquare>* pGrid, const std::vector<Elite::Vector2>& flowfieldFlowDirections)
		{
			int max{ int(pGrid->size()) + 1 };

			for (size_t idx{}; idx < pGrid->size(); ++idx)
			{
				if (m_DistancesGrid[idx] >= max) continue;
				auto column{ pGrid->at(idx).column }; //column 
				auto row{ pGrid->at(idx).row }; //row 

				int lowestDist{ max };
				int lowestDistNeighborIdx{};

				for (size_t neighborLoopIdx{}; neighborLoopIdx < flowfieldFlowDirections.size(); ++neighborLoopIdx)
				{
					size_t neighborColumn{ column + size_t(flowfieldFlowDirections[neighborLoopIdx].x) };
					size_t neighborRow{ row + size_t(flowfieldFlowDirections[neighborLoopIdx].y) };

					if (neighborColumn >= m_pGridResolution->x || neighborColumn < 0) continue; //checking if this "neighbor" is valid 
					if (neighborRow >= m_pGridResolution->y || neighborRow < 0) continue;

					size_t neighborIdx{ (
						neighborColumn +
						size_t(neighborRow * m_pGridResolution->x)) }; //index of this neighbor in the grid

					if (m_DistancesGrid[neighborIdx] <= lowestDist)
					{
						if(flowfieldFlowDirections[neighborLoopIdx].y != 0 && flowfieldFlowDirections[neighborLoopIdx].x != 0)
							if (CheckDiagonalThroughWall(pGrid, flowfieldFlowDirections, neighborLoopIdx, neighborColumn, neighborRow)) continue;

						lowestDistNeighborIdx = neighborLoopIdx;
						lowestDist = m_DistancesGrid[neighborIdx];
					}
				}
				pGrid->at(idx).flowDirections[goalNr] = flowfieldFlowDirections[lowestDistNeighborIdx];
			}
		}
		
	protected:
		struct IdxToVisit {
			size_t idx{};
			int distance{};

		};
		std::vector<int> m_DistancesGrid{};
		const Elite::Vector2* m_pGridResolution{};

	private:
		bool CheckDiagonalThroughWall(const std::vector<Grid::GridSquare>* pGrid, const std::vector<Elite::Vector2>& flowfieldFlowDirections
			, const size_t& neighborLoopIdx, const size_t& neighborC, const size_t& neighborR) const
		{

			//check if the flowfield doesn's go through a wall corner
				//         
				//	xxxxx/---------      => where x are walls and - / is a flow field path.
				//	----/xxxxxxxxxx				=> instead of doing, the path should go around the walls

			size_t neighborYneighbor{}, //neighbor of neighbor either below or above original neighbor
				neighborXneighbor{};	//neighbor of neighbor either left or right of original neighbor
			if (flowfieldFlowDirections[neighborLoopIdx].y > 0)
			{
				neighborYneighbor = neighborC + size_t((neighborR - 1) * m_pGridResolution->x);

				if (flowfieldFlowDirections[neighborLoopIdx].x > 0)
				{
					neighborXneighbor = (neighborC - 1) + size_t(neighborR * m_pGridResolution->x);
				}
				else if (flowfieldFlowDirections[neighborLoopIdx].x < 0)
				{
					neighborXneighbor = (neighborC + 1) + size_t(neighborR * m_pGridResolution->x);
				}
			}
			else if(flowfieldFlowDirections[neighborLoopIdx].y < 0)
			{
				neighborYneighbor = neighborC + size_t((neighborR + 1) * m_pGridResolution->x);

				if (flowfieldFlowDirections[neighborLoopIdx].x > 0)
				{
					neighborXneighbor = (neighborC - 1) + size_t(neighborR * m_pGridResolution->x);
				}
				else if (flowfieldFlowDirections[neighborLoopIdx].x < 0)
				{
					neighborXneighbor = (neighborC + 1) + size_t(neighborR * m_pGridResolution->x);
				}
			}

			return  (pGrid->at(neighborYneighbor).squareType == Grid::SquareType::Obstacle &&
				pGrid->at(neighborXneighbor).squareType == Grid::SquareType::Obstacle);
		}
	};
		
	class Dijkstra final:public Algorithm
	{
	public:
		Dijkstra(const Elite::Vector2* gridResolution) :Algorithm(gridResolution) {};
		~Dijkstra() = default;
		void RunAlgorithm(int goalNr, size_t goalGridIdx, std::vector<Grid::GridSquare>* pGrid)
		{
			m_DistancesGrid.clear();

			//to visit vector 
			std::vector<IdxToVisit>toVisit{};

			//max distance + 1, used for unexplored indexes
			int max{ int(pGrid->size()) + 1 };

			//setting up dijkstra grid
			for (size_t idx{}; idx < pGrid->size(); ++idx)
				if (pGrid->at(idx).squareType != Grid::SquareType::Obstacle)
					m_DistancesGrid.push_back(max);
				else
					m_DistancesGrid.push_back(INT_MAX); //int max used to mark obstacles

			m_DistancesGrid[goalGridIdx] = 0; //distance of the goal index is 0 as we start there

			std::vector<Elite::Vector2> neighbors{ { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } }; //used to calculate the neighbor of an index, no diagonals (x = column, y = row)

			//===========================================================================
			//================== Initial start point ====================================
			//===========================================================================
			//setting the initial index around the start node
			for (size_t idx{}; idx < 4; ++idx)
			{
				size_t column{ pGrid->at(goalGridIdx).column + size_t(neighbors[idx].x) };
				size_t row{ size_t(pGrid->at(goalGridIdx).row + neighbors[idx].y) };
				if (column >= m_pGridResolution->x) continue;
				if (row >= m_pGridResolution->y) continue;

				size_t neighborIdx{ (
					column +
					size_t(row * m_pGridResolution->x)) };

				if (pGrid->at(neighborIdx).squareType != Grid::SquareType::Obstacle)
				{
					m_DistancesGrid[neighborIdx] = 1;
					toVisit.push_back(IdxToVisit{ neighborIdx, 1 });
				}
			}


			//===========================================================================
			//================= Dijkstra algorithm ======================================
			//===========================================================================
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

					if (neighborColumn >= m_pGridResolution->x || neighborColumn < 0) continue; //checking if this "neighbor" is valid (if it's out the grid this will be a very large number) it's a nasty fix I know
					if (neighborRow >= m_pGridResolution->y || neighborRow < 0) continue;

					size_t neighborIdx{ (
						neighborColumn +
						size_t(neighborRow * m_pGridResolution->x)) }; //index of this neighbor in the grid

					if (m_DistancesGrid[neighborIdx] == max) //check if the this index has been explored
					{
						m_DistancesGrid[neighborIdx] = toVisit[toVisitLoopIdx].distance + 1; //change the distance to "visit index distance + 2"
						toVisit.push_back(IdxToVisit{ neighborIdx, toVisit[toVisitLoopIdx].distance + 1 }); //add this neighbor to the visit vector
					}
				}
			}

		}
	};
}