//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "App_Flowfield.h"


//Destructor
App_Flowfield::~App_Flowfield()
{
	
	m_pAgents->clear();
	SAFE_DELETE(m_pAgents);
}



//Functions
void App_Flowfield::Start()
{
	m_pAgents = new std::vector< FlowfieldAgent>();

}

void App_Flowfield::Update(float deltaTime)
{
	//INPUT
	if(INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft) && m_VisualizeTarget)
	{
		auto const mouseData = INPUTMANAGER->GetMouseData(InputType::eMouseButton, InputMouseButton::eLeft);

		//m_Target.Position = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y) });
	//for (auto& a : m_AgentVec)
		//{
		//	UpdateTarget(a);
		//}
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

		ImGui::Checkbox("Trim World", &m_TrimWorld);
		if (m_TrimWorld)
		{
			ImGui::SliderFloat("Trim Size", &m_TrimWorldSize, 0.f, 200.f, "%.1");
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

	for (auto& a : *m_pAgents)
	{
		a.UpdateAgent(deltaTime);
	}
}

void App_Flowfield::Render(float deltaTime) const
{
	for (auto& a : *m_pAgents)
	{
		a.RenderAgent(deltaTime);
	}

	if (m_TrimWorld)
	{
		vector<Elite::Vector2> points =
		{
			{ -m_TrimWorldSize,m_TrimWorldSize },
			{ m_TrimWorldSize,m_TrimWorldSize },
			{ m_TrimWorldSize,-m_TrimWorldSize },
			{-m_TrimWorldSize,-m_TrimWorldSize }
		};
		DEBUGRENDERER2D->DrawPolygon(&points[0], 4, { 1,0,0,1 }, 0.4f);
	}

	
}


