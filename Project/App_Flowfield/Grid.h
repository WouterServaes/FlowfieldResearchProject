#pragma once
#include "framework/EliteMath/EMath.h"

class Obstacle;

class Grid
{
public:
	enum class SquareType //types of grid square
	{
		Default, //default type, no use other than being default 
		Obstacle, //obstacle, like a wall for the agents 
		Goal, //all paths will lead to these (atleast one required in the world)
		Spawner //optional type, will spawn agents from here instead of dropping them in all at once
	};

	struct GridSquare
	{
		size_t row{}, column{}; //row and column of this grid square
		Elite::Vector2 botLeft{}; //bottom left coordinates of this square
		std::vector<Elite::Vector2> flowDirections{}; //multiple flow directions [0] -> flowfield 0, [1] -> flowfield 1, etc
		SquareType squareType{ SquareType::Default }; //type of this square
	};

	Grid(const Elite::Vector2& worldSize, const Elite::Vector2& gridResolution);
	~Grid();

	void Render(float deltaTime) const; //render everything about the grid (grid, flowfields, obstacles, goals, spawners)
	void Update(float deltaTime); //general update function

	//agent movement
	void MoveSqr(const Elite::Vector2& currentPos, Elite::Vector2& targetPos, int goalNr); //takes currentPos and calculates a new targetPos, goalNr represents the flowfield nr, firstmove used for an initial move
	bool AgentReachedGoal(const Elite::Vector2& agentPos, int agentGoal); //checks whether the agent has reached their goal
	
	//flowfield
	void MakeFlowfield();  //this is where the magic happens (makes the flowfields)
	size_t GetAmountOfFlowfields()const { return m_Goals.size(); }; //returns the amount of flowfields (each goal has a flowfield)


	//adding to grid
	void AddObstacle(const Elite::Vector2& obstaclePos);
	void AddObstacleWhileRunningFf(const Elite::Vector2& obstaclePos); //add obstacle while running flowfield, recalculates all the flowfields
	void AddGoal(const Elite::Vector2& goalPos);
	void AddSpawner(const Elite::Vector2& spawnerPos);
	int AmountGoalsAdded()const; //returns the amount of goals added (to check if there's one)

	//goal vector
	void MakeGoalVector(); //makes the goals
	bool GoalVectorReady() { return m_MadeGoalVector; }; //made the goals?
	int GetNewGoal(int currentGoal) const; //returns a random next goal for the agent

	//spawners
	std::vector<Elite::Vector2> GetSpawnerPos() const;

	//grid drawing
	void ToggleDrawGrid() { m_DrawGrid = !m_DrawGrid; };
	void ToggleDrawGoals() { m_DrawGoals = !m_DrawGoals; };
	void ToggleDrawDirections() { m_DrawDirections = !m_DrawDirections; };
	void ToggleDrawSpawners() { m_DrawSpawners = !m_DrawSpawners; };
	void ToggleDrawObstacles() { m_DrawObstacles = !m_DrawObstacles; };
	bool SetFlowfieldToDraw(size_t flowfieldNr);
	bool FlowfieldDrawn()const { return m_DrawDirections; };

	//file save/read
	bool SaveToFile(const std::string& fileName) const;
	bool SetFromFile(const std::string& fileName);

	//no category
	Elite::Vector2 GetValidRandomPos(); //returns a random position on the grid (only on default squares)
	bool IsPointInGrid(const Elite::Vector2& point);//true if point is in the grid
private:
	//handling grid
	void InitGrid(); //makes the grid 
	void SetAllDefault(); //resets the grid

	//grid drawing
	void DrawGridSqr(size_t idx, const Elite::Color& color, bool fillSqr) const; //draws a square, used by other draw functions below
	void DrawGrid() const;
	void DrawFlowfield() const;
	void DrawObstacles() const;
	size_t m_FlowfieldToDraw{ 0 }; 
	void DrawGoals() const;
	void DrawSpawners() const;

	//obstacle 
	void MakeObstacleBodies(int sqrIdx = -1); //Rigid bodies get placed on squares of type obstacle, sqrIdx -1 => no specific sqrIdx, do all squares of type Obstacle
	void RemoveObstacleBody(int sqrIdx); //removes an obstacle

	Elite::Vector2 GetMidOfSquare(size_t idx) const { //get the mid position of a square
		return m_pGrid->at(idx).botLeft + (m_SquareSize / 2.f);
	};

	//no category
	size_t GetGridSqrIdxAtPos(const Elite::Vector2& pos) const; //returns the idx of the square at a position

	std::vector<GridSquare>* m_pGrid{ nullptr };
	
	Elite::Vector2 m_WorldDimensions, 
		m_GridResolution, 
		m_SquareSize;

	std::vector<Elite::Vector2> m_Goals{};

	std::vector <Obstacle*> m_pObstacles{};

	Elite::Color m_GridColor{ 180.f / 255.f, 180.f / 255.f, 180.f / 255.f },
		m_ObstacleColor{ 1.f, 0.f, 0.f },
		m_DirectionColor{ 0.f, 0.f, 1.f },
		m_GoalColor{ 1.f, 1.f, 0.f },
		m_HighlitedGoalColor{ m_DirectionColor },
		m_SpawnerColor{ 0.f, 1.f, 1.f };

	int m_AddedGoalsAmount{};
	
	bool m_DrawGrid{ true }, m_DrawGoals{ true }, m_DrawDirections{ false }, m_DrawSpawners{ true }, m_DrawObstacles{ true };
		
	bool m_MadeGoalVector{ false }, m_MadeFlowFields{ false };

	bool m_ObstaclesReady{ false }, m_AddedObsWhileRunning{ false };
};

class Obstacle
{
public:
	//rigid body of type static. doesn't move around and collides with agents.
	Obstacle(const Elite::Vector2& center, const Elite::Vector2& dimensions, int sqrIdx)
	{
		const Elite::RigidBodyDefine define = Elite::RigidBodyDefine(0.01f, 0.1f, Elite::eStatic, false);
		const Transform transform = Transform(center, Elite::ZeroVector2);
		m_pRigidBody = new RigidBody(define, transform);

		Elite::EPhysicsBoxShape shape;

		shape.height = dimensions.y;
		shape.width = dimensions.x;

		m_pRigidBody->AddShape(&shape);
		m_sqrIdx = sqrIdx;
	}
	~Obstacle() { SAFE_DELETE(m_pRigidBody); };
	int GetSqrIdx()const { return m_sqrIdx; };
private:
	RigidBody* m_pRigidBody{ nullptr };
	int m_sqrIdx{ 0 }; //idx of the square this obstalce is on
};