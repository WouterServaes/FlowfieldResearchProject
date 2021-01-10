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

	HandleMouseInput();
	if (!m_MadeGoals)
		m_HasGoals = m_pGrid->AmountGoalsAdded() > 0;

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
	for (size_t idx{}; idx < m_AmountOfAgent; ++idx)
		m_pAgents->push_back(new FlowfieldAgent(m_pGrid->GetValidRandomPos()));
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
		ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
		ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 20));
		ImGui::Begin("Gameplay Programming", &windowActive,  ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NoMove);
		ImGui::PushAllowKeyboardFocus(false);

		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Text("LMB: place target");
		ImGui::Text("RMB: move cam.");
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
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("Flow field");
		ImGui::Spacing();
		ImGui::Spacing();

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

		if (m_HasGoals)
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


		//spawn agents
		if (!m_SpawnAgents && m_MadeFlowfield)
		{
			ImGui::InputInt("Amount of agents", &m_AmountOfAgent);

			if (ImGui::Button("SpawnAgents"))
			{
				m_SpawnAgents = true;
				SpawnAgents();
			}
		}

		if (m_SpawnAgents)
			ImGui::Text("Agents Spawned");
		//---

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		//toggle drawing grid things
		if (ImGui::Button("Draw grid on/off"))
			m_pGrid->ToggleDrawGrid();
		if (ImGui::Button("Draw obstacles on/off"))
			m_pGrid->ToggleDrawObstacles();
		if (ImGui::Button("Draw goals on/off"))
			m_pGrid->ToggleDrawGoals();

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
		if (m_pGrid->IsPointInGrid(mousePos))
			if (!m_MadeObstacles)
				m_pGrid->AddObstacle(mousePos);
	}
	else if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eMiddle))
	{
		auto const mouseData = INPUTMANAGER->GetMouseData(InputType::eMouseButton, InputMouseButton::eMiddle);
		auto mousePos{ DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y) }) };
		if (m_pGrid->IsPointInGrid(mousePos))
			if (!m_MadeGoals)
				m_pGrid->AddGoal(mousePos);
	}
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

		if (a->GetReachedGoal() == false);
		{
			m_pGrid->MoveSqr(agentCurrentPos, a->CurrentTargetPos(), agentCurrentEndGoal, a->GetNeedsInitialMove());
			a->UpdateAgent(deltaTime);
		}

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
	std::cout << "file name: ";
	std::cin >> m_FileName;

	if (m_pGrid->SaveToFile(m_FileName))
		std::cout << "successfully written to file " << m_FileName << "\n";
	else
	{
		std::cout << "\n===========ERROR===========================\n";
		std::cout << "can't write to file " << m_FileName << "\n";
	}
}