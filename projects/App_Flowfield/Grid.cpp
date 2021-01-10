#include "stdafx.h"
#include "Grid.h"

#include "GeneratingFlowField.h"

Grid::Grid(const Elite::Vector2& worldSize, const Elite::Vector2& gridResolution)
	:m_WorldDimensions{ worldSize }, m_GridResolution(gridResolution), m_SquareSize{ m_WorldDimensions / m_GridResolution }
{
	InitGrid();
}

Grid::~Grid()
{
	m_pGrid->clear();
	SAFE_DELETE(m_pGrid);

	for (auto& obs : m_pObstacles)
		delete obs;
	m_pObstacles.clear();
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
	if (m_DrawGrid) DrawGrid();
	if (m_DrawObstacles) DrawObstacles();
	if (m_DrawGoals) DrawGoals();
	if (m_DrawDirections)DrawFlowfield();
}

void Grid::Update(float deltaTime)
{
	if (m_MadeGoalVector && !madeFlowFields)
	{
		madeFlowFields = true;
		MakeFlowfield();
	}

	if (m_ObstaclesReady)
	{
		MakeObstacleBodies();
		m_ObstaclesReady = false;
	}
}

void Grid::DrawGridSqr(size_t idx, const Elite::Color& color, bool fillSqr) const
{
	Elite::Vector2 sqrPoint[4]{};
	const auto& sqr = m_pGrid->at(idx);

	sqrPoint[0] = sqr.botLeft;
	sqrPoint[1] = Elite::Vector2{ sqr.botLeft.x + m_SquareSize.x, sqr.botLeft.y };
	sqrPoint[2] = Elite::Vector2{ sqr.botLeft.x + m_SquareSize.x, sqr.botLeft.y + m_SquareSize.y };
	sqrPoint[3] = Elite::Vector2{ sqr.botLeft.x, sqr.botLeft.y + m_SquareSize.y };

	if (!fillSqr)
		DEBUGRENDERER2D->DrawPolygon(&sqrPoint[0], 4, color, DEBUGRENDERER2D->NextDepthSlice());
	else
		DEBUGRENDERER2D->DrawSolidPolygon(&sqrPoint[0], 4, color, DEBUGRENDERER2D->NextDepthSlice());
}

void Grid::DrawGrid() const
{
	for (size_t idx{}; idx < m_pGrid->size(); ++idx)
	{
		if (m_pGrid->at(idx).squareType != SquareType::Default) continue;
		DrawGridSqr(idx, m_GridColor, false);
	}
}

void Grid::DrawFlowfield() const
{
	float arrowLength{ 2.f };

	if (m_pGrid->at(0).flowDirections.size() > 0)
		for (size_t idx{}; idx < m_pGrid->size(); ++idx)
		{
			if (m_pGrid->at(idx).squareType != SquareType::Default)
				continue;
			DEBUGRENDERER2D->DrawDirection(GetMidOfSquare(idx), m_pGrid->at(idx).flowDirections[m_FlowfieldToDraw].GetNormalized(), arrowLength, m_DirectionColor);
			DEBUGRENDERER2D->DrawPoint(GetMidOfSquare(idx), 2.f, { 0, 0, 0 });
		}
}

bool Grid::SetFlowfieldToDraw(size_t flowfieldNr)
{
	if (flowfieldNr <= m_Goals.size())
	{
		m_FlowfieldToDraw = flowfieldNr;
		return true;
	}
	return false;
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
	size_t flowfieldNr{};
	for (size_t idx{}; idx < m_pGrid->size(); ++idx)
	{
		if (m_pGrid->at(idx).squareType != SquareType::Goal) continue;

		auto color = m_GoalColor;
		if (flowfieldNr == m_FlowfieldToDraw)
			color = m_HighlitedGoalColor;

		flowfieldNr += 1;
		DrawGridSqr(idx, color, true);
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
	{
		m_AddedGoalsAmount++;
		sqrType = SquareType::Goal;
	}
	else
	{
		m_AddedGoalsAmount--;
		sqrType = SquareType::Default;
	}

}

int Grid::AmountGoalsAdded()const
{
	return m_AddedGoalsAmount;
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
		{
			for (size_t idx2{}; idx2 < m_pGrid->size(); ++idx2)
			{
				m_pGrid->at(idx2).flowDirections.push_back({ 0, 0 });
			}
			m_Goals.push_back(GetMidOfSquare(idx));
		}
}

bool Grid::MoveSqr(const Elite::Vector2& currentPos, Elite::Vector2& targetPos, int goalNr, bool firstMove)
{
	if (firstMove || Elite::Distance(currentPos, targetPos) <= m_MindDistanceFromTarget)
	{
		const auto sqrIdx{ GetGridSqrIdxAtPos(currentPos) }; //sqr index of square the agent is currently in
		const auto nextSqrPosFromDirection{ currentPos + (m_pGrid->at(sqrIdx).flowDirections[goalNr].GetNormalized() * (m_SquareSize.x + (m_SquareSize.x / 2))) }; //a position from the current position of the agent along the direction the agent should be following over a length (1.5 a square's length)
		const auto newSqrIdx{ GetGridSqrIdxAtPos(nextSqrPosFromDirection) }; //the square idx of the square at this new position
		targetPos = GetMidOfSquare(newSqrIdx); //next target for the agent = the middle of this new square
		return true;
	}

	return false;
}

int Grid::GetNewGoal(int currentGoal) const
{
	int newGoalIdx{};
	do
	{
		newGoalIdx = Elite::randomInt(m_Goals.size() - 1);
	} while (newGoalIdx == currentGoal);
	return newGoalIdx;
}

bool Grid::AgentReachedGoal(const Elite::Vector2& agentPos, int agentGoal)
{
	return Elite::Distance(agentPos, m_Goals[agentGoal]) <= m_SquareSize.x / 2.f;
}

Elite::Vector2 Grid::GetValidRandomPos()
{
	int randomIdx{};

	do
	{
		randomIdx = Elite::randomInt(m_pGrid->size() - 1);
	} while (m_pGrid->at(randomIdx).squareType != SquareType::Default);

	return GetMidOfSquare(randomIdx);
}

bool Grid::IsPointInGrid(const Elite::Vector2& point)
{
	return (point.x > -(m_WorldDimensions.x / 2.f) &&
		point.x < m_WorldDimensions.x / 2.f &&
		point.y > -(m_WorldDimensions.y / 2.f) &&
		point.y < m_WorldDimensions.y / 2.f);
}

void Grid::MakeFlowfield()
{
	const std::vector<Elite::Vector2> flowfieldFlowDirections = {
		{ 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 1 }, { -1, 0 }, { -1, -1 }, { 0, -1 }, { 1, -1 } }; //every square around a square

		//finding a goals
	std::vector<size_t> goalIndxs{};

	for (const auto& gridSqr : *m_pGrid)
		if (gridSqr.squareType == Grid::SquareType::Goal)
			goalIndxs.push_back(gridSqr.column + (gridSqr.row * m_GridResolution.x));
	

	Algorithms::Dijkstra* dijkstraAlgorithm = new Algorithms::Dijkstra(&m_GridResolution);

	//for every goal: run algorithm and make flowfield;

	for (size_t idx{}; idx < goalIndxs.size(); ++idx)
	{
		dijkstraAlgorithm->RunAlgorithm(idx, goalIndxs[idx], m_pGrid);
		dijkstraAlgorithm->MakeFlowfield(idx, m_pGrid, flowfieldFlowDirections);
	}
	delete dijkstraAlgorithm;

	m_ObstaclesReady = true;
}

bool Grid::SaveToFile(const std::string& fileName) const
{
	std::ofstream output{};
	output.open(fileName);
	if (output.is_open())
	{
		for (size_t idx{}; idx< m_pGrid->size(); ++idx)
		{
			if (m_pGrid->at(idx).squareType == SquareType::Default) continue;

			char type{};
			if (m_pGrid->at(idx).squareType == SquareType::Goal) type = 'g';
			else if (m_pGrid->at(idx).squareType == SquareType::Obstacle) type = 'o';	
			output << type;
			if (idx < 10)
			{
				output << "000";
			}
			else if (idx < 100)
			{
				output << "00";
			}
			else if (idx < 1000)
			{
				output << "0";
			}

			output << idx << std::endl;
		}
		output.close();
		return true;
	}
		return false;
}

bool Grid::SetFromFile(const std::string& fileName)
{
	std::ifstream input{};
	input.open(fileName);
	if (input.is_open())
	{
		SetAllDefault();
		std::string line{};

		for (line; getline(input, line);)
		{
			switch (line.at(0))
			{
			case 'g':
				m_pGrid->at(std::stoi(line.substr(1, 4))).squareType = SquareType::Goal;
				break;

			case 'o':
				m_pGrid->at(std::stoi(line.substr(1, 4))).squareType = SquareType::Obstacle;
				break;
			}
		}
		return true;
	}
	return false;
	
}

void Grid::SetAllDefault()
{
	for (auto& sqr: *m_pGrid)
	{
		if (sqr.squareType == SquareType::Default) continue;
		sqr.squareType = SquareType::Default;
	}
}

void Grid::MakeObstacleBodies()
{
	for (auto& sqr : *m_pGrid)
	{
		if (sqr.squareType != SquareType::Obstacle) continue;

		m_pObstacles.push_back(new Obstacle(sqr.botLeft + Elite::Vector2(2.5f,2.5f), m_SquareSize/2.f));
		
	}

}