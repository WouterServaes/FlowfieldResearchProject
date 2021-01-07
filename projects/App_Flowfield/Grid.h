#pragma once
#include "framework/EliteMath/EMath.h"
class Grid
{
public:
	Grid(const Elite::Vector2& worldSize, const Elite::Vector2& gridResolution);
	~Grid();

	void Render(float deltaTime) const;
	void Update(float deltaTime);
private:
	void InitGrid();
	struct GridSquare
	{
		size_t row{}, column{};
		std::vector<Elite::Vector2> flowDirections{};
		bool isObstacle{false};
	};

	Elite::Vector2 m_WorldDimensions, m_GridResolution, m_SquareSize;
	std::vector<GridSquare>* m_pGrid{ nullptr };

};

