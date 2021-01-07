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
			m_pGrid->push_back(sqr);
		}
	}
}

void Grid::Render(float deltaTime) const
{

}

void Grid::Update(float deltaTime)
{

}