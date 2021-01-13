#ifndef STEERINGBEHAVIORS_APPLICATION_H
#define STEERINGBEHAVIORS_APPLICATION_H
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteInterfaces/EIApp.h"
#include "FlowfieldAgent.h"
#include "Grid.h"

class FlowfieldAgent;

//-----------------------------------------------------------------
// Application
//-----------------------------------------------------------------
class App_Flowfield final : public IApp
{
public:
	//Constructor & Destructor
	App_Flowfield() = default;
	virtual ~App_Flowfield();

	//App Functions
	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;

private:
	struct Spawner
	{
		float elapsedTime{};
		Elite::Vector2 location{};
		int agentsSpawned{};
	};
	//agents
	void SpawnAgents(); //spawn all agents at random positions
	void SpawnAgent(const Elite::Vector2& pos); //spawns one agent at pos
	void ResetAgents(); //removes every agent

	//general handle function
	void HandleImGui(); //UI
	void HandleMouseInput(); // mouse input
	void HandleKeyboardInput(); //keyboard input
	void HandleAgentUpdate(float deltaTime); //agent update

	//spawners
	void AddSpawners();  
	void UseSpawners(float deltaTime); //goes over spawners and spawns an agent when it's time

	//files
	void SaveToFile();
	void ReadFromFile();

	std::vector<FlowfieldAgent*>* m_pAgents{}; //agents
	Grid* m_pGrid; //the grid

	bool m_SpawnAgents{ false },
		m_UseSpawners{ false },
		m_KillAtGoal{ true }, //remove an agent when it reaches its goal
		m_MadeObstacles{ false }, //finished placing obstacles (for initial ff)
		m_MadeGoals{ false }, //finished placing goals
		m_MadeFlowfield{ false }; //made the flowfield (initial ff)

	
	int m_StartAmountAgents{ 5 }, //amount of agents 
		m_FlowfieldToDraw{ 0 }, //nr of ff to draw 
		m_AmountOfGoals{}; //amount of goals

	Grid::SquareType m_TypeToPlace{ Grid::SquareType::Default };
	std::vector< Spawner*> m_pSpawners{};
	const float m_TimePerSpawn{ .2f }; //time per spawn (for all spawners)
};
#endif