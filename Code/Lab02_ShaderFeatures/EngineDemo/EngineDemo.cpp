#include "EngineDemo.h"
#include "MousePicker.h"
#include "MyWindow.h"
#include <iostream>

#include "MouseManager.h"
#include "GameLogger.h"
#include "MyGL.h"
#include "ChaseCameraComponent.h"
#include "CollisionTester.h"
#include "MyFiles.h"
#include "ShaderProgram.h"

#pragma warning(push)
#pragma warning(disable : 4251)
#pragma warning(disable : 4127)
#include <QtGui\qmouseevent>
#pragma warning(pop)

#include "ShapeGenerator.h"

#include "RenderEngine.h"
#include "ConfigReader.h"
#include "MathUtility.h"
#include "GraphicalObjectComponent.h"
#include "SpatialComponent.h"
#include "KeyboardComponent.h"
#include "MouseComponent.h"
#include "UniformData.h"

// Justin Furtado
// 6/21/2016
// EngineDemo.cpp
// The game

const int NUM_TEAPOTS = 3;
const float TEAPOT_DISTANCE = 30.0f;
const float ROTATE_DISTANCE = TEAPOT_DISTANCE / 2.5f;
Engine::GraphicalObject m_teapots[NUM_TEAPOTS];
Engine::GraphicalObject m_lights[NUM_TEAPOTS];
Engine::GraphicalObject playerGraphicalObject;
//Engine::GraphicalObject cube;
const float EngineDemo::RENDER_DISTANCE = 2500.0f;
Engine::Entity player;
Engine::ChaseCameraComponent playerCamera(Engine::Vec3(0, 30, 50), Engine::Vec3(0, 5, 0), Engine::Vec3(0));
Engine::GraphicalObjectComponent playerGob;
Engine::SpatialComponent playerSpatial;
MouseComponent mouseComponent;
KeyboardComponent playerInput;
Engine::GraphicalObject goodCube;
Engine::GraphicalObject badCube;
Engine::Vec3 goodCubeBasePos(Engine::Vec3(10.0f, 0.0f, -50.0f));
Engine::Vec3 badCubeBasePos(Engine::Vec3(-10.0f, 0.0f, -50.0f));
float cubeTestDiff = 10.0f;

bool EngineDemo::Initialize(Engine::MyWindow *window)
{
	m_pWindow = window;

	if (!InitializeGL())
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Unable to initialize game! Failed to InitializeGL()!\n");
		return false;
	}

	if (!Engine::RenderEngine::Initialize(&m_shaderPrograms[0], NUM_SHADER_PROGRAMS))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Could not do anything because RenderEngine failed...\n");
		return false;
	}

	if (!Engine::ShapeGenerator::Initialize(m_shaderPrograms[2].GetProgramId(), m_shaderPrograms[0].GetProgramId(), 0))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to initialize game because shape generator failed to initialize!\n");
		return false;
	}

	player.SetName("Player");
	player.AddComponent(&playerSpatial, "PlayerSpatial");
	Engine::ShapeGenerator::ReadSceneFile("..\\Data\\Scenes\\BetterDargon.PN.scene", &playerGraphicalObject, m_shaderPrograms[2].GetProgramId());
	playerGraphicalObject.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, playerGraphicalObject.GetFullTransformPtr()->GetAddress(), modelToWorldMatLoc));
	playerGraphicalObject.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), worldToViewMatLoc));
	playerGraphicalObject.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_perspective.GetPerspectivePtr()->GetAddress(), perspectiveMatLoc));
	playerGraphicalObject.AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &playerGraphicalObject.GetMatPtr()->m_materialColor, tintColorLoc));
	playerGraphicalObject.AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &playerGraphicalObject.GetMatPtr()->m_materialColor, debugColorLoc));
	playerGraphicalObject.AddUniformData(Engine::UniformData(GL_FLOAT, &playerGraphicalObject.GetMatPtr()->m_specularIntensity, tintIntensityLoc));
	playerGraphicalObject.GetMatPtr()->m_specularIntensity = 0.0f;
	playerGraphicalObject.SetScaleMat(Engine::Mat4::Scale(0.25f));
	Engine::RenderEngine::AddGraphicalObject(&playerGraphicalObject);
	playerGob.SetGraphicalObject(&playerGraphicalObject);
	player.AddComponent(&playerGob, "PlayerGob");
	player.AddComponent(&playerCamera, "PlayerCamera");
	player.AddComponent(&playerInput, "PlayerInput");
	player.AddComponent(&mouseComponent, "MouseComponent");
	player.Initialize();

	/*Engine::ShapeGenerator::MakeCube(&cube);
	cube.SetCallback(PCCallback, this);
	Engine::RenderEngine::AddGraphicalObject(&cube);*/

	if (!Engine::TextObject::Initialize(matLoc, tintColorLoc))
	{ 
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Unable to initialize EngineDemo, failed to initialize text renderer!\n");
		return false;
	}

	m_fpsTextObject.SetupText(-0.9f, 0.9f, 0.0f, 1.0f, 0.0f, 1.0f, 0.5f, 1.0f, "FPS not calculated yet!\n");
	m_EngineDemoInfoObject.SetupText(0.3f, 0.9f, 0.0f, 1.0f, 0.0f, 1.0f, 0.5f, 1.0f, "New Game Started!\n");
	m_perspective.SetPerspective(m_pWindow->width() / static_cast<float>(m_pWindow->height()), Engine::MathUtility::ToRadians(60.0f), 1.0f, RENDER_DISTANCE);
	m_perspective.SetScreenDimmensions(static_cast<float>(m_pWindow->width()), static_cast<float>(m_pWindow->height()));
	Engine::MousePicker::SetPerspectiveInfo(m_perspective.GetFOVY(), m_perspective.GetNearDist(), m_perspective.GetWidth(), m_perspective.GetHeight());

	for (int i = 0; i < NUM_TEAPOTS; ++i)
	{
		Engine::ShapeGenerator::ReadSceneFile("..\\Data\\Scenes\\TeapotNoLid.PN.Scene", &m_teapots[i], m_shaderPrograms[3].GetProgramId(), nullptr, false);
		m_teapots[i].SetTransMat(Engine::Mat4::Translation(Engine::Vec3(i*TEAPOT_DISTANCE - TEAPOT_DISTANCE, 0.0f, 0.0f)));
		m_teapots[i].SetRotMat(Engine::Mat4::RotationAroundAxis(Engine::Vec3(i % 3 == 0 ? 1.0f : 0.0f,
																			 i % 3 == 1 ? 1.0f : 0.0f,
																			 i % 3 == 2 ? 1.0f : 0.0f), Engine::MathUtility::PI / 2.0f));
		m_teapots[i].SetScaleMat(Engine::Mat4::Scale(2.0f));
		m_teapots[i].AddPhongUniforms(modelToWorldMatLoc, worldToViewMatLoc, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), perspectiveMatLoc, m_perspective.GetPerspectivePtr()->GetAddress(),
									  tintColorLoc, diffuseColorLoc, ambientColorLoc, specularColorLoc, specularPowerLoc, diffuseIntensityLoc, ambientIntensityLoc, specularIntensityLoc,
									  &m_lights[i].GetMatPtr()->m_materialColor, cameraPosLoc, playerCamera.GetPosPtr(), lightLoc, m_lights[i].GetLocPtr());

		m_teapots[i].GetMatPtr()->m_materialColor = Engine::Vec3(1.0f, 1.0f, 1.0f);
		m_teapots[i].GetMatPtr()->m_diffuseReflectivity = i % 3 == 0 ? Engine::Vec3(0.0f, 0.0f, 0.9f)
															: i % 3 == 1 ? Engine::Vec3(0.9f, 0.9f, 0.0f) 
															: Engine::Vec3(0.75f, 0.75f, 0.75f);
		m_teapots[i].GetMatPtr()->m_ambientReflectivity = 0.1f * m_teapots[i].GetMatPtr()->m_diffuseReflectivity;
		m_teapots[i].GetMatPtr()->m_specularReflectivity = Engine::Vec3(1.0f, 1.0f, 1.0f);
		m_teapots[i].GetMatPtr()->m_specularIntensity = 16.0f;

		Engine::RenderEngine::AddGraphicalObject(&m_teapots[i]);

		Engine::ShapeGenerator::MakeLightingCube(&m_lights[i]);
		m_lights[i].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_lights[i].GetFullTransformPtr()->GetAddress(), modelToWorldMatLoc));
		m_lights[i].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), worldToViewMatLoc));
		m_lights[i].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_perspective.GetPerspectivePtr()->GetAddress(), perspectiveMatLoc));
		m_lights[i].AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &m_lights[i].GetMatPtr()->m_materialColor, tintColorLoc));
		m_lights[i].AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &m_lights[i].GetMatPtr()->m_materialColor, debugColorLoc));
		m_lights[i].AddUniformData(Engine::UniformData(GL_FLOAT, &m_lights[i].GetMatPtr()->m_specularIntensity, tintIntensityLoc));
		m_lights[i].GetMatPtr()->m_materialColor = i % 3 == 0 ? Engine::Vec3(1.0f, 1.0f, 1.0f)
													 : i % 3 == 1 ? Engine::Vec3(1.0f, 1.0f, 1.0f)
													 : Engine::Vec3(1.0f, 0.0f, 0.0f);
		m_lights[i].GetMatPtr()->m_diffuseReflectivity = i % 3 == 0 ? Engine::Vec3(1.0f, 1.0f, 1.0f)
														   : i % 3 == 1 ? Engine::Vec3(1.0f, 1.0f, 1.0f)
														   : Engine::Vec3(1.0f, 0.0f, 0.0f);

		Engine::Vec3 xyz = Engine::Vec3(i % 3 == 0 ? 1.0f : 0.0f,
										i % 3 == 1 ? 1.0f : 0.0f,
										i % 3 == 2 ? 1.0f : 0.0f);
		m_lights[i].SetRotationAxis(xyz);
		Engine::Vec3 zyx(xyz.GetZ(), xyz.GetY(), xyz.GetX());
		m_lights[i].SetTransMat(Engine::Mat4::Translation(Engine::Vec3(i*TEAPOT_DISTANCE - TEAPOT_DISTANCE, 0.0f, 0.0f) + (zyx.Normalize() * ROTATE_DISTANCE)));

		m_lights[i].SetRotationRate(Engine::MathUtility::PI / 10.0f);
		Engine::RenderEngine::AddGraphicalObject(&m_lights[i]);
	}

	//Engine::ShapeGenerator::MakeCube(&goodCube);
	//goodCube.SetBaseColor(Engine::Vec3(1.0f, 1.0f, 1.0f));
	//goodCube.SetTintColor(Engine::Vec3(1.0f, 1.0f, 1.0f));
	//goodCube.SetCallback(EngineDemo::PCCallback, this);
	//goodCube.SetTransMat(Engine::Mat4::Translation(goodCubeBasePos + Engine::Vec3(0.0f, 10.0f, 0.0f)));
	//goodCube.SetRotationAxis(Engine::Vec3(1.0f, 0.0f, 0.0f));
	//goodCube.SetRotationRate(Engine::MathUtility::PI / 1.0f);
	//Engine::RenderEngine::AddGraphicalObject(&goodCube);

	//Engine::ShapeGenerator::MakeCube(&badCube);
	//badCube.SetBaseColor(Engine::Vec3(1.0f, 1.0f, 1.0f));
	//badCube.SetTintColor(Engine::Vec3(1.0f, 1.0f, 1.0f));
	//badCube.SetCallback(EngineDemo::PCCallback, this);
	//badCube.SetTransMat(Engine::Mat4::Translation(badCubeBasePos + Engine::Vec3(0.0f, 10.0f, 0.0f)));
	//badCube.SetRotationAxis(Engine::Vec3(1.0f, 0.0f, 0.0f));
	//badCube.SetRotationRate(Engine::MathUtility::PI / 1.0f);
	//Engine::RenderEngine::AddGraphicalObject(&badCube);

	if (!ReadConfigValues())
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Unable to initialize EngineDemo, failed to read config values!\n");
		return false;
	}

	if (!Engine::ConfigReader::pReader->RegisterCallbackForConfigChanges(EngineDemo::OnConfigReload, this))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Unable to register callback for EngineDemo!\n");
		return false;
	}

	// ` for numpad 0
	if (!keyboardManager.AddKeys("XTWASDRFLGCM QE0`9iKN") || !keyboardManager.AddKey(VK_OEM_PERIOD))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to add keys!\n");
		return false;
	}

	if (!keyboardManager.AddToggle('P', &paused, true))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to add key toggle!\n");
		return false;
	}

	Engine::CollisionTester::CalculateGrid();
	Engine::GameLogger::Log(Engine::MessageType::cProcess, "Game Initialized Successfully!!!\n");
	return true;
}

bool EngineDemo::Shutdown()
{
	// Display some info on shutdown
	Engine::RenderEngine::LogStats();

	if (!m_pWindow->Shutdown()) { return false; }

	for (int i = 0; i < NUM_SHADER_PROGRAMS; ++i)
	{
		if (!m_shaderPrograms[i].Shutdown()) { return false; }
	}

	if (!Engine::TextObject::Shutdown()) { return false; }
	if (!Engine::RenderEngine::Shutdown()) { return false; }
	if (!Engine::ShapeGenerator::Shutdown()) { return false; }
	
	player.Shutdown();

	Engine::GameLogger::Log(Engine::MessageType::cProcess, "Game Shutdown Successfully!!!\n");
	return true;
}

bool EngineDemo::InitializeCallback(void * game, Engine::MyWindow * window)
{
	if (!game) { return false; }
	return reinterpret_cast<EngineDemo *>(game)->Initialize(window);
}

void EngineDemo::UpdateCallback(void * game, float dt)
{
	if (!game) { return; }
	reinterpret_cast<EngineDemo *>(game)->Update(dt);
}

void EngineDemo::ResizeCallback(void * game)
{
	if (!game) { return; }
	reinterpret_cast<EngineDemo *>(game)->OnResizeWindow();
}

void EngineDemo::DrawCallback(void * game)
{
	if (!game) { return; }
	reinterpret_cast<EngineDemo *>(game)->Draw();
}

void EngineDemo::MouseScrollCallback(void * game, int degrees)
{
	if (!game) { return; }
	reinterpret_cast<EngineDemo *>(game)->OnMouseScroll(degrees);
}

void EngineDemo::MouseMoveCallback(void * game, int dx, int dy)
{
	if (!game) { return; }
	reinterpret_cast<EngineDemo *>(game)->OnMouseMove(dx, dy);
}

void EngineDemo::Update(float dt)
{
	const float maxBorder = 0.1f;
	const float minBorder = -0.1f;

	keyboardManager.Update(dt); // needs to come before to un-pause

	if (!won) { ShowFrameRate(dt); }
	if (!ProcessInput(dt)) { return; }

	if (paused) { return; }

	for (int i = 0; i < NUM_TEAPOTS; ++i)
	{
		Engine::Vec3 xyz = Engine::Vec3(i % 3 == 2 ? 1.0f : 0.0f,
										i % 3 == 0 ? 1.0f : 0.0f,
										i % 3 == 1 ? 1.0f : 0.0f);
		Engine::Vec3 teapotPos = m_teapots[i].GetPos();
		m_lights[i].SetRotation(m_lights[i].GetRotation() + m_lights[i].GetRotationRate() * dt);
		Engine::Mat4 rotationAroundAxis = Engine::Mat4::RotationAroundAxis(m_lights[i].GetRotationAxis(), m_lights[i].GetRotation());
		m_lights[i].SetTransMat(Engine::Mat4::Translation(teapotPos + (ROTATE_DISTANCE * (rotationAroundAxis * xyz))));
		m_lights[i].CalcFullTransform();
		m_teapots[i].CalcFullTransform();
	}

	playerGraphicalObject.CalcFullTransform();

	//goodCube.SetRotation(goodCube.GetRotation() + goodCube.GetRotationRate() * dt);
	//Engine::Mat4 rotationMat = Engine::Mat4::RotationAroundAxis(goodCube.GetRotationAxis(), goodCube.GetRotation());
	//goodCube.SetTransMat(Engine::Mat4::Translation(goodCubeBasePos + ((rotationMat * Engine::Vec3(0.0f, 10.0f, 0.0f)))));

	//static Engine::Vec3 offsetVec(0.0f, 10.0f, 0.0f);
	//Engine::Mat4 badRotationMat = Engine::Mat4::RotationAroundAxis(badCube.GetRotationAxis(), badCube.GetRotationRate() * dt);
	//offsetVec = badRotationMat * offsetVec;
	//badCube.SetTransMat(Engine::Mat4::Translation(badCubeBasePos + offsetVec));

	//float goodAngle = Engine::MathUtility::ToDegrees(acosf((goodCube.GetPos() - goodCubeBasePos).Normalize().Dot(Engine::Vec3(0.0f, 10.0f, 0.0f).Normalize())));
	//float badAngle = Engine::MathUtility::ToDegrees(acosf((badCube.GetPos() - badCubeBasePos).Normalize().Dot(Engine::Vec3(0.0f, 10.0f, 0.0f).Normalize())));
	//
	//static float timer = 0.0f;
	//timer += dt;

	//char buffer[50];
	//sprintf_s(&buffer[0], 50, "%3.3f : %3.3f : %3.3f", goodAngle, badAngle, (goodAngle - badAngle));
	//m_EngineDemoInfoObject.SetupText(0.3f, 0.9f, 0.0f, 1.0f, 0.0f, 1.0f, 0.5f, 1.0f, buffer);

	player.Update(dt);
}

void EngineDemo::Draw()
{
	// Clear window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Engine::RenderEngine::Draw();

	// draw fps text
	m_fpsTextObject.RenderText(&m_shaderPrograms[0], debugColorLoc);
	m_EngineDemoInfoObject.RenderText(&m_shaderPrograms[0], debugColorLoc);
}

void EngineDemo::OnResizeWindow()
{
	if (m_pWindow == nullptr) { return; }
	float aspect = static_cast<float>(m_pWindow->width()) / m_pWindow->height();
	m_perspective.SetAspectRatio(aspect);
	m_perspective.SetScreenDimmensions(static_cast<float>(m_pWindow->width()), static_cast<float>(m_pWindow->height()));
	Engine::MousePicker::SetPerspectiveInfo(m_perspective.GetFOVY(), m_perspective.GetNearDist(), m_perspective.GetWidth(), m_perspective.GetHeight());
	glViewport(0, 0, m_pWindow->width(), m_pWindow->height());

}

void EngineDemo::OnMouseScroll(int degrees)
{
	mouseComponent.MouseScroll(degrees);
}

void EngineDemo::OnMouseMove(int deltaX, int deltaY)
{
	if (Engine::MouseManager::IsRightMouseDown()) { mouseComponent.MouseMove(deltaX, deltaY); }
	mouseComponent.SetMousePos(Engine::MouseManager::GetMouseX(), Engine::MouseManager::GetMouseY());
}

void EngineDemo::OnConfigReload(void * classInstance)
{
	// error checking
	if (!classInstance) { return; }

	// get pointer to instance
	EngineDemo *pGame = reinterpret_cast<EngineDemo*>(classInstance);

	if (Engine::ConfigReader::pReader->GetFloatForKey("EngineDemo.FpsInterval", pGame->m_fpsInterval))
	{
		if (pGame->m_fpsInterval < 0.5f) { pGame->m_fpsInterval = 0.5f; }
	}
}

//void EngineDemo::PTCallback(Engine::GraphicalObject * instance, void * classInstance)
//{
//	EngineDemo *pGame = reinterpret_cast<EngineDemo*>(classInstance);
//
//	PassMatrix(instance, classInstance);
//	PassTint(instance, classInstance);
//
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, instance->GetMeshPointer()->GetTextureID());
//	glUniform1i(pGame->texLoc, 0);
//}

bool EngineDemo::InitializeGL()
{
	glViewport(0, 0, m_pWindow->width(), m_pWindow->height());

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);

	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	if (m_shaderPrograms[0].Initialize())
	{
		m_shaderPrograms[0].AddVertexShader("..\\Data\\Shaders\\Debug.Vert.shader");
		m_shaderPrograms[0].AddFragmentShader("..\\Data\\Shaders\\Debug.Frag.shader");
		m_shaderPrograms[0].LinkProgram();
		m_shaderPrograms[0].UseProgram();
	}

	if (m_shaderPrograms[1].Initialize())
	{
		m_shaderPrograms[1].AddVertexShader("..\\Data\\Shaders\\PT.vert.shader");
		m_shaderPrograms[1].AddFragmentShader("..\\Data\\Shaders\\PT.frag.shader");
		m_shaderPrograms[1].LinkProgram();
		m_shaderPrograms[1].UseProgram();
	}

	if (m_shaderPrograms[2].Initialize())
	{
		m_shaderPrograms[2].AddVertexShader("..\\Data\\Shaders\\PC.vert.shader");
		m_shaderPrograms[2].AddFragmentShader("..\\Data\\Shaders\\PC.frag.shader");
		m_shaderPrograms[2].LinkProgram();
		m_shaderPrograms[2].UseProgram();
	}

	if (m_shaderPrograms[3].Initialize())
	{					 
		m_shaderPrograms[3].AddVertexShader("..\\Data\\Shaders\\TwoSidedPhong.vert.shader");
		m_shaderPrograms[3].AddFragmentShader("..\\Data\\Shaders\\TwoSidedPhong.frag.shader");
		m_shaderPrograms[3].LinkProgram();
		m_shaderPrograms[3].UseProgram();
	}

	debugColorLoc = m_shaderPrograms[0].GetUniformLocation("tint");
	tintColorLoc = m_shaderPrograms[1].GetUniformLocation("tintColor");
	tintIntensityLoc = m_shaderPrograms[1].GetUniformLocation("tintIntensity");
	texLoc = m_shaderPrograms[1].GetUniformLocation("textureSampler");
	ambientColorLoc = m_shaderPrograms[3].GetUniformLocation("ambientLightColor");
	ambientIntensityLoc = m_shaderPrograms[3].GetUniformLocation("ambientLightIntensity");
	diffuseColorLoc = m_shaderPrograms[3].GetUniformLocation("diffuseLightColor");
	diffuseIntensityLoc = m_shaderPrograms[3].GetUniformLocation("diffuseLightIntensity");
	specularColorLoc = m_shaderPrograms[3].GetUniformLocation("specularLightColor");
	specularIntensityLoc = m_shaderPrograms[3].GetUniformLocation("specularLightIntensity");
	specularPowerLoc = m_shaderPrograms[3].GetUniformLocation("specularPower");
	modelToWorldMatLoc = m_shaderPrograms[3].GetUniformLocation("modelToWorld");
	worldToViewMatLoc = m_shaderPrograms[3].GetUniformLocation("worldToView");
	perspectiveMatLoc = m_shaderPrograms[3].GetUniformLocation("projection");
	lightLoc = m_shaderPrograms[3].GetUniformLocation("lightPos_WorldSpace");
	cameraPosLoc = m_shaderPrograms[3].GetUniformLocation("cameraPosition_WorldSpace");

	if (Engine::MyGL::TestForError(Engine::MessageType::cFatal_Error, "InitializeGL errors!"))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to InitializeGL()! TestForErrors found gl errors!\n");
		return false;
	}
	
	Engine::GameLogger::Log(Engine::MessageType::Process, "EngineDemo::InitializeGL() succeeded!\n");
	return true;
}

bool EngineDemo::ReadConfigValues()
{
	if (!Engine::ConfigReader::pReader->GetFloatForKey("EngineDemo.FpsInterval", m_fpsInterval)) { Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to get float for key FpsInterval!\n"); return false; }
	if (m_fpsInterval < 0.5f) { m_fpsInterval = 0.5f; }

	float cameraSpeed = 1.0f;
	if (!Engine::ConfigReader::pReader->GetFloatForKey("EngineDemo.CameraSpeed", cameraSpeed)) { Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to get float for key CameraSpeed!\n"); return false; }
	if (cameraSpeed < 0.0f) { cameraSpeed = 1.0f; }
	player.GetComponentByType<Engine::ChaseCameraComponent>()->SetSpeed(cameraSpeed);

	float cameraRotationSpeed = 1.0f;
	if (!Engine::ConfigReader::pReader->GetFloatForKey("EngineDemo.CameraRotationSpeed", cameraRotationSpeed)) { Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to get float for key CameraRotationSpeed!\n"); return false; }
	if (cameraRotationSpeed < 0.0f) { cameraRotationSpeed = 1.0f; }
	player.GetComponentByType<Engine::ChaseCameraComponent>()->SetRotateSpeed(cameraRotationSpeed);

	Engine::GameLogger::Log(Engine::MessageType::Process, "Successfully read in config values!\n");
	return true;
}

bool EngineDemo::ProcessInput(float dt)
{
	if (keyboardManager.KeyWasPressed('T')) { Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "%f\n", dt); }
	if (keyboardManager.KeyWasReleased('C')) { Engine::CollisionTester::ConsoleLogOutput(); }
	if (keyboardManager.KeyWasPressed('0') || keyboardManager.KeyWasPressed('`')) { Engine::ConfigReader::pReader->ProcessConfigFile(); }
	if (keyboardManager.KeyWasPressed('M')) { Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "(%.3f, %.3f, %.3f)\n", playerGraphicalObject.GetPos().GetX(), playerGraphicalObject.GetPos().GetY(), playerGraphicalObject.GetPos().GetZ()); }
	if (keyboardManager.KeyWasPressed('L')) { Engine::RenderEngine::LogStats(); }

	if (keyboardManager.KeyWasPressed(('X'))) { Shutdown(); return false; }
	return true;
}

void EngineDemo::ShowFrameRate(float dt)
{
	static unsigned int numFrames = 0;
	static float    timeAccumulator = 0.0f;

	++numFrames;
	timeAccumulator += dt;	

	if (timeAccumulator >= m_fpsInterval)
	{
		//Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "FPS: %6.1f over %3.1f seconds.\n", numFrames / timeAccumulator, m_fpsInterval);
		char fpsText[50];
		sprintf_s(fpsText, 50, "FPS: %6.1f over %3.1f seconds.\n", numFrames / timeAccumulator, m_fpsInterval);
		m_fpsTextObject.SetupText(-0.9f, 0.9f, 0.0f, 1.0f, 0.0f, 1.0f, 0.5f, 1.0f, fpsText);
		timeAccumulator = 0;
		numFrames = 0;
	}
}

