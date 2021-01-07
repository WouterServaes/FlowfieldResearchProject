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
	//INPUT
	if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft))
	{
		auto const mouseData = INPUTMANAGER->GetMouseData(InputType::eMouseButton, InputMouseButton::eLeft);
		auto mousePos{ DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y) }) };
		if (!m_MadeObstacles)
			m_pGrid->AddObstacle(mousePos);
	}
	else if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eMiddle))
	{
		auto const mouseData = INPUTMANAGER->GetMouseData(InputType::eMouseButton, InputMouseButton::eMiddle);
		auto mousePos{ DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y) }) };
		if (!m_MadeGoals)
			m_pGrid->AddGoal(mousePos);
	}

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
		ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
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
		if (!m_MadeGoals)
		{
			if (ImGui::Button("Goals ready"))
				m_MadeGoals = true;
			
		}
		else
			ImGui::Text("Goals ready");
		//----

		//spawn agents
		if (!m_SpawnAgents && m_MadeGoals && m_MadeObstacles)
		{
			if (!m_pGrid->GoalVectorReady()) 
				m_pGrid->MakeGoalVector();

			if (ImGui::Button("SpawnAgents"))
			{
				m_SpawnAgents = true;
				SpawnAgents();
			}
		}
		else
			ImGui::Text("Agents Spawned");
		//---

		ImGui::Spacing();

		//toggle drawing grid things
		if (ImGui::Button("Draw grid on/off"))
			m_pGrid->ToggleDrawGrid();
		if (ImGui::Button("Draw obstacles on/off"))
			m_pGrid->ToggleDrawObstacles();
		if (ImGui::Button("Draw goals on/off"))
			m_pGrid->ToggleDrawGoals();
		if (ImGui::Button("Draw direction on/off"))
			m_pGrid->ToggleDrawDirections();

		ImGui::Spacing();

		ImGui::Spacing();
		ImGui::Separator();

		//End
		ImGui::PopAllowKeyboardFocus();
		ImGui::End();
	}
#pragma endregion
#endif

	m_pGrid->Update(deltaTime);

	for (auto& a : *m_pAgents)
	{
		a->UpdateAgent(deltaTime);
	}
}

void App_Flowfield::Render(float deltaTime) const
{
	m_pGrid->Render(deltaTime);

	for (auto& a : *m_pAgents)
	{
		a->RenderAgent(deltaTime);
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
		m_pAgents->push_back(new FlowfieldAgent(Elite::Vector2(25, 25)));
}