#ifndef STEERINGBEHAVIORS_APPLICATION_H
#define STEERINGBEHAVIORS_APPLICATION_H
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteInterfaces/EIApp.h"
#include "FlowfieldAgent.h"

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

	std::vector<FlowfieldAgent>* m_pAgents{};

	
	bool m_VisualizeTarget = true;
	bool m_TrimWorld = true;
	float m_TrimWorldSize = 50.f;


};
#endif