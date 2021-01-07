#include "stdafx.h"
#include "Grid.h"



Grid::Grid(const Elite::Vector2& worldSize, const Elite::Vector2& gridResolution)
	:m_WorldDimensions{worldSize}, m_GridResolution(gridResolution), m_SquareSize{m_WorldDimensions/m_GridResolution}
{
	InitGrid();

}

Grid::~Grid()
{
	m_pGrid->clear();
	SAFE_DELETE(m_pGrid);
}

void Grid::InitGrid()
{
	m_pGrid = new std::vector<GridSquare>{};
	auto worldBot{ -m_WorldDimensions.y / 2 }
		, worldTop{ m_WorldDimensions.y / 2 }
		, worldLeft{ -m_WorldDimensions.x / 2 }
	, worldRight{ m_WorldDimensions.x / 2 };

	for (size_t idxRow{}; idxRow < m_GridResolution.y; ++idxRow)
	{
		Elite::Vector2 gridPoint{};
		gridPoint.y = worldBot + (m_SquareSize.y * idxRow) * (worldTop - worldBot) / m_WorldDimensions.y;
		for (size_t idxColumn{}; idxColumn < m_GridResolution.x; ++idxColumn)
		{
			gridPoint.x = worldLeft + (m_SquareSize.x * idxColumn) * (worldRight - worldLeft) / m_WorldDimensions.x;
			GridSquare sqr{};
			sqr.column = idxColumn;
			sqr.row = idxRow;
			sqr.botLeft = gridPoint;
			m_pGrid->push_back(sqr);
		}
	}
}

void Grid::Render(float deltaTime) const
{
	if(m_DrawGrid) DrawGrid();
	if (m_DrawObstacles) DrawObstacles();
	if (m_DrawGoals) DrawGoals();
	if (m_DrawDirections)DrawDirections();
}

void Grid::Update(float deltaTime)
{
	
}

void Grid::DrawGridSqr(size_t idx, const Elite::Color& color, bool fillSqr) const
{
	Elite::Vector2 sqrPoint[4]{};
	const auto& sqr = m_pGrid->at(idx);

	sqrPoint[0] = sqr.botLeft;
	sqrPoint[1] = Elite::Vector2{ sqr.botLeft.x + m_SquareSize.x, sqr.botLeft.y };
	sqrPoint[2] = Elite::Vector2{ sqr.botLeft.x + m_SquareSize.x, sqr.botLeft.y + m_SquareSize.y };
	sqrPoint[3] = Elite::Vector2{ sqr.botLeft.x, sqr.botLeft.y + m_SquareSize.y };

	if(fillSqr)
		DEBUGRENDERER2D->DrawSolidPolygon(&sqrPoint[0], 4, color, DEBUGRENDERER2D->NextDepthSlice());
	else
		DEBUGRENDERER2D->DrawPolygon(&sqrPoint[0], 4, color, DEBUGRENDERER2D->NextDepthSlice());
}

void Grid::DrawGrid() const
{
	for (size_t idx{}; idx < m_pGrid->size(); ++idx)
	{
		if (m_pGrid->at(idx).squareType != SquareType::Default) continue;
		DrawGridSqr(idx, m_GridColor, false);
	}
}

void Grid::DrawDirections() const 
{
	size_t directionNr{ 0 };
	float arrowLength{ 2.f };

	for (size_t idx{}; idx < m_pGrid->size(); ++idx)
	{
		if (m_pGrid->at(idx).squareType != SquareType::Default) 
			continue;
		DEBUGRENDERER2D->DrawDirection(GetMidOfSquare(idx), m_pGrid->at(idx).flowDirections[directionNr], arrowLength, m_DirectionColor);
		DEBUGRENDERER2D->DrawPoint(GetMidOfSquare(idx), 2.f, { 0, 0, 0 });

	}
}

void Grid::DrawObstacles() const 
{
	for (size_t idx{}; idx < m_pGrid->size(); ++idx)
	{
		if (m_pGrid->at(idx).squareType != SquareType::Obstacle) continue;
		DrawGridSqr(idx, m_ObstacleColor, true);
	}
}

void Grid::DrawGoals() const
{
	for (size_t idx{}; idx < m_pGrid->size(); ++idx)
	{
		if (m_pGrid->at(idx).squareType != SquareType::Goal) continue;
		DrawGridSqr(idx, m_GoalColor, true);
	}
}

void Grid::AddObstacle(const Elite::Vector2& obstaclePos)
{
	size_t sqrIdx{ GetGridSqrIdxAtPos(obstaclePos) };
	auto& sqrType{ m_pGrid->at(sqrIdx).squareType };

	if (sqrType != SquareType::Obstacle)
		sqrType = SquareType::Obstacle;
	else
		sqrType = SquareType::Default;
	

}

void Grid::AddGoal(const Elite::Vector2& goalPos)
{
	size_t sqrIdx{ GetGridSqrIdxAtPos(goalPos) };
	auto& sqrType{ m_pGrid->at(sqrIdx).squareType };

	if (sqrType != SquareType::Goal)
		sqrType = SquareType::Goal;
	else
		sqrType = SquareType::Default;
	
}

size_t Grid::GetGridSqrIdxAtPos(const Elite::Vector2& pos) const
{
	for (size_t idxRow{}; idxRow < m_GridResolution.y; ++idxRow)
		for (size_t idxColumn{}; idxColumn < m_GridResolution.x; ++idxColumn)
		{
			auto idx{ idxColumn + (idxRow * size_t(m_GridResolution.x)) };
			auto gridSquareBotLeft{ m_pGrid->at(idx).botLeft };

			if (pos.x >= gridSquareBotLeft.x
				&& pos.x <= gridSquareBotLeft.x + m_SquareSize.x
				&& pos.y >= gridSquareBotLeft.y
				&& pos.y <= gridSquareBotLeft.y + m_SquareSize.y)
			{
				return idx;
			}
		}

	return -1;
}

void Grid::MakeGoalVector()
{
	m_MadeGoalVector = true;
	for (size_t idx{}; idx < m_pGrid->size(); ++idx)
		if (m_pGrid->at(idx).squareType == SquareType::Goal)
			m_Goals.push_back(GetMidOfSquare(idx));
}

bool Grid::MoveSqr(const Elite::Vector2& currentPos, const Elite::Vector2& endGoalTarget,Elite::Vector2& posTarget, int goalNr)
{
	float minDistFromTarget{ 2.f };
	if (Elite::Distance(currentPos, posTarget)<= minDistFromTarget)
	{
		const auto sqrIdx{ GetGridSqrIdxAtPos(currentPos) }; //sqr index of square the agent is currently in 
		const auto nextSqrPosFromDirection{currentPos + (m_pGrid->at(sqrIdx).flowDirections[goalNr].GetNormalized() * (m_SquareSize.x + (m_SquareSize.x/2)))}; //a position from the current position of the agent along the direction the agent should be following over a length (1.5 a square's length)
		
		const auto newSqrIdx{ GetGridSqrIdxAtPos(nextSqrPosFromDirection) }; //the square idx of the square at this new position
		posTarget =  GetMidOfSquare(newSqrIdx); //next target for the agent = the middle of this new square
		return true;
	}

	return false;

}