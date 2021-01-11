//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "App_Flowfield.h"

//Destructor
App_Flowfield::~App_Flowfield()
{
	for (auto& a : *m_pAgents)
		SAFE_DELETE(a);

	m_pAgents->clear();
	SAFE_DELETE(m_pAgents);

	SAFE_DELETE(m_pGrid);
}

//Functions
void App_Flowfield::Start()
{
	m_TrimWorldSize = 500.f;

	m_pGrid = new Grid(Elite::Vector2(m_TrimWorldSize, m_TrimWorldSize), Elite::Vector2(50.f, 50.f));

	m_pAgents = new std::vector< FlowfieldAgent*>();
}

void App_Flowfield::Update(float deltaTime)
{
	if (m_AddAgentsWithMouse)
		m_AgentSpawnWithMouseElapsedSec += deltaTime;

	if (m_SpawnAgents && m_UseSpawners)
		UseSpawners(deltaTime);

	HandleKeyboardInput();
	HandleMouseInput();
	if (!m_MadeGoals)
		m_AmountOfGoals = m_pGrid->AmountGoalsAdded();

	m_pGrid->Update(deltaTime);
	HandleAgentUpdate(deltaTime);

	HandleImGui();
}

void App_Flowfield::Render(float deltaTime) const
{
	m_pGrid->SetFlowfieldToDraw(m_FlowfieldToDraw);
	m_pGrid->Render(deltaTime);

	for (auto& a : *m_pAgents)
	{
		a->RenderAgent(deltaTime, m_FlowfieldToDraw);
	}

	if (m_TrimWorld)
	{
		vector<Elite::Vector2> points =
		{
			{ -m_TrimWorldSize, m_TrimWorldSize },
			{ m_TrimWorldSize, m_TrimWorldSize },
			{ m_TrimWorldSize, -m_TrimWorldSize },
			{ -m_TrimWorldSize, -m_TrimWorldSize }
		};
		DEBUGRENDERER2D->DrawPolygon(&points[0], 4, { 1, 0, 0, 1 }, 0.4f);
	}
}

void App_Flowfield::SpawnAgents()
{
	if (m_UseSpawners) return;;
	for (size_t idx{}; idx < m_StartAmountAgents; ++idx)
		SpawnAgent(m_pGrid->GetValidRandomPos());
}

void App_Flowfield::SpawnAgent(const Elite::Vector2& pos)
{
	m_pAgents->push_back(new FlowfieldAgent(pos, m_AmountOfGoals));
}

void App_Flowfield::HandleImGui()
{
#ifdef PLATFORM_WINDOWS
#pragma region UI
	//UI
	{
		//Setup
		int const menuWidth = 235;
		int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
		int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
		bool windowActive = true;

		//tile types
		ImGui::SetNextWindowPos(ImVec2(10, 10));
		ImGui::SetNextWindowSize(ImVec2(100, 100));
		ImGui::Begin("Tile types", &windowActive, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
		ImGui::Text("Goal [1]");
		ImGui::Text("Obstacle [2]");
		ImGui::Text("Spawner [3]");
		ImGui::End();
		//----

		//main
		ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
		ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 20));
		ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NoMove);
		ImGui::PushAllowKeyboardFocus(false);
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Text("LMB: fill tile");
		ImGui::Text("RMB: move cam.");
		ImGui::Text("MMB: spawn agent");
		ImGui::Text("Scrollwheel: zoom cam.");
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		if (m_pAgents->size())
			ImGui::Text("%1i Agents", int(m_pAgents->size()));

		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("Flow field");
		ImGui::Spacing();
		ImGui::Spacing();

		if (!m_SpawnAgents)
			if (ImGui::Button("Use from file"))
				ReadFromFile();
		// obstacles
		if (!m_MadeObstacles)
		{
			if (ImGui::Button("Obstacles ready"))
				m_MadeObstacles = true;
		}
		else
			ImGui::Text("Obstacles ready");
		//----

		//goals

		if (m_AmountOfGoals)
		{
			if (!m_MadeGoals)
			{
				if (ImGui::Button("Goals ready"))
					m_MadeGoals = true;
			}
			else
				ImGui::Text("Goals ready");
		}
		//----

		//make flow field
		if (m_MadeGoals && m_MadeObstacles)
		{
			if (!m_MadeFlowfield)
			{
				if (ImGui::Button("make flowfields"))
				{
					m_pGrid->MakeGoalVector();
					m_MadeFlowfield = true;
					m_pGrid->ToggleDrawObstacles();
				}
			}
		}
		//----

		//save/load file

		if (m_MadeFlowfield)
		{
			if (ImGui::Button("save to file"))
			{
				SaveToFile();
			}
		}
		//----

		if (m_AmountOfGoals > 1)
			ImGui::Checkbox("remove at goal", &m_KillAtGoal);

		//spawn agents
		if (!m_SpawnAgents && m_MadeFlowfield)
		{
			ImGui::InputInt("Amount of agents", &m_StartAmountAgents);

			if (ImGui::Button("SpawnAgents"))
			{
				AddSpawners();
				m_SpawnAgents = true;
				SpawnAgents();
			}
		}

		if (m_SpawnAgents)
			ImGui::Text("Agents Spawned");
		//---

		//reset agents
		if (m_SpawnAgents && m_MadeFlowfield)
		{
			if (ImGui::Button("reset agents"))
				ResetAgents();
		}
		//---
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		//toggle drawing grid things
		if (ImGui::Button("Draw grid on/off"))
			m_pGrid->ToggleDrawGrid();
		if (ImGui::Button("Draw goals on/off"))
			m_pGrid->ToggleDrawGoals();
		if (ImGui::Button("Draw spawners on/off"))
			m_pGrid->ToggleDrawSpawners();

		if (m_MadeGoals && m_MadeObstacles)
		{
			if (ImGui::Button("Draw flowfield on/off"))
				m_pGrid->ToggleDrawDirections();

			ImGui::SliderInt("flowfield to draw", &m_FlowfieldToDraw, 0, m_pGrid->GetAmountOfFlowfields() - 1);
		}

		ImGui::Spacing();

		ImGui::Spacing();
		ImGui::Separator();

		//End
		ImGui::PopAllowKeyboardFocus();
		ImGui::End();
	}
#pragma endregion
#endif
}

void App_Flowfield::HandleMouseInput()
{
	if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft))
	{
		auto const mouseData = INPUTMANAGER->GetMouseData(InputType::eMouseButton, InputMouseButton::eLeft);
		auto mousePos{ DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y) }) };

		if (!m_pGrid->IsPointInGrid(mousePos)) return;

		switch (m_TypeToPlace)
		{
		case Grid::SquareType::Default:
			break;
		case Grid::SquareType::Obstacle:
			if (!m_MadeObstacles)
				m_pGrid->AddObstacle(mousePos);
			else if (m_SpawnAgents)
				m_pGrid->AddObstacleWhileRunningFf(mousePos);
			break;
		case Grid::SquareType::Goal:
			if (!m_MadeGoals)
				m_pGrid->AddGoal(mousePos);
			break;
		case Grid::SquareType::Spawner:
			if (!m_MadeFlowfield)
				m_pGrid->AddSpawner(mousePos);
			break;
		}
	}

	if (INPUTMANAGER->IsMouseButtonDown(InputMouseButton::eMiddle))
	{
		if (!m_SpawnAgents) return;

		auto const mouseData = INPUTMANAGER->GetMouseData(InputType::eMouseButton, InputMouseButton::eMiddle);
		auto mousePos{ DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y) }) };

		if (!m_pGrid->IsPointInGrid(mousePos)) return;
		m_AddAgentsWithMouse = true;

		ContinueAgentSpawning(mousePos);
	}

	if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eMiddle))
	{
		m_AddAgentsWithMouse = false;
	}
}

void App_Flowfield::HandleKeyboardInput()
{
	if (INPUTMANAGER->IsKeyboardKeyUp(InputScancode::eScancode_1))
		m_TypeToPlace = Grid::SquareType::Goal;
	if (INPUTMANAGER->IsKeyboardKeyUp(InputScancode::eScancode_2))
		m_TypeToPlace = Grid::SquareType::Obstacle;
	if (INPUTMANAGER->IsKeyboardKeyUp(InputScancode::eScancode_3))
		m_TypeToPlace = Grid::SquareType::Spawner;
	if (INPUTMANAGER->IsKeyboardKeyUp(InputScancode::eScancode_4))
		m_TypeToPlace = Grid::SquareType::Default;
}

void App_Flowfield::HandleAgentUpdate(float deltaTime)
{
	Elite::Vector2 agentCurrentPos{};
	int agentCurrentEndGoal{};

	int agentsToRemove{};
	for (auto& a : *m_pAgents)
	{
		agentCurrentPos = a->GetCurrentPos();
		agentCurrentEndGoal = a->GetEndGoal();

		a->SetReachedGoal(m_pGrid->AgentReachedGoal(agentCurrentPos, agentCurrentEndGoal));
		bool reachedGoal{ a->GetReachedGoal() };

		if (!m_KillAtGoal && reachedGoal)
		{
			a->NewEndGoalTarget(Elite::randomInt(m_AmountOfGoals));
			reachedGoal = false;
		}

		if (!reachedGoal);
		{
			m_pGrid->MoveSqr(agentCurrentPos, a->CurrentTargetPos(), agentCurrentEndGoal, a->GetNeedsInitialMove());
			a->UpdateAgent(deltaTime);
		}

		if (!m_KillAtGoal) continue;
		if (a->IsMarkedForRemove())
			agentsToRemove++;
	}

	for (int idx{}; idx < agentsToRemove; ++idx)
	{
		m_pAgents->erase(std::remove_if(m_pAgents->begin(), m_pAgents->end(), [](FlowfieldAgent* agent)
			{
				if (agent->IsMarkedForRemove())
				{
					delete agent;
					return true;
				}
				return false;
			}), m_pAgents->end());
	}
}

void App_Flowfield::SaveToFile()
{
	std::cout << "\n======\n";
	std::cout << "file name (only file name and extension, no path, file is saved in Environments folder): ";
	std::string fileName;
	std::cin >> fileName;
	fileName = "Environments/" + fileName;
	if (m_pGrid->SaveToFile(fileName))
		std::cout << "successfully written to file " << fileName << "\n";
	else
	{
		std::cout << "\n===========ERROR===========================\n";
		std::cout << "can't write to file " << fileName << "\n";
	}
}

void App_Flowfield::ReadFromFile()
{
	std::cout << "\n======\n";
	std::cout << "file name (only file name and extension, no path, file is saved in Environments folder): ";
	std::string fileName;
	std::cin >> fileName;
	fileName = "Environments/" + fileName;
	if (m_pGrid->SetFromFile(fileName))
	{
		std::cout << "successfully read file " << fileName << "\n";
		m_MadeGoals = true;
		m_MadeObstacles = true;
		m_AmountOfGoals = m_pGrid->AmountGoalsAdded();
	}
	else
	{
		std::cout << "\n===========ERROR===========================\n";
		std::cout << "can't read file " << fileName << "\n";
	}
}

void App_Flowfield::AddSpawners()
{
	auto spawners{ m_pGrid->GetSpawnerPos() };

	if (spawners.size() <= 0) return;

	for (const auto& s : spawners)
		m_pSpawners.push_back(new Spawner(Spawner{ 0.f, s, 0 }));

	m_UseSpawners = true;
}

void App_Flowfield::UseSpawners(float deltaTime)
{
	int amountAgentsPerSpawner{ m_StartAmountAgents / int(m_pSpawners.size()) };

	for (auto& s : m_pSpawners)
	{
		if (s->agentsSpawned <= amountAgentsPerSpawner - 1)
		{
			s->elapsedTime += deltaTime;
			if (s->elapsedTime >= m_TimePerSpawn)
			{
				s->elapsedTime = 0.f;
				s->agentsSpawned += 1;
				m_pAgents->push_back(new FlowfieldAgent(s->location, m_AmountOfGoals));
			}
		}
	}
}

void App_Flowfield::ContinueAgentSpawning(const Elite::Vector2& pos)
{
	//if (m_AgentSpawnWithMouseElapsedSec < .2f) return;
	m_AgentSpawnWithMouseElapsedSec = 0.f;
	SpawnAgent(pos);
}

void App_Flowfield::ResetAgents()
{
	for (auto& a : *m_pAgents)
		SAFE_DELETE(a);

	for (auto& s : m_pSpawners)
		s->agentsSpawned = 0;

	m_pAgents->clear();
}