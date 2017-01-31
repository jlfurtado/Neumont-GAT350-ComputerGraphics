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
#include "BitmapLoader.h"

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
#include "FrameBuffer.h"

// Justin Furtado
// 6/21/2016
// EngineDemo.cpp
// The game

const int NUM_SCENES = 6;
const float SCENE_PLANE_SCALE = 10.0f;
const int NUM_USED_SHADERS = 10;
const int NUM_NON_TEAPOT_SHADERS = 4;
const int TEAPOTS_PER_SHADER = 3;
const int NUM_DEMO_OBJECTS = 50;
const float TEAPOT_DISTANCE = 35.0f;
const float ROTATE_DISTANCE = TEAPOT_DISTANCE / 2.5f;
Engine::GraphicalObject m_grid;
Engine::GraphicalObject m_demoObjects[NUM_DEMO_OBJECTS];
Engine::GraphicalObject m_lights[NUM_DEMO_OBJECTS];
Engine::GraphicalObject m_scenePlanes[NUM_SCENES];
Engine::GraphicalObject playerGraphicalObject;
//Engine::GraphicalObject cube;
const float EngineDemo::RENDER_DISTANCE = 2500.0f;
Engine::Entity player;
Engine::ChaseCameraComponent playerCamera(Engine::Vec3(0, 30, 50), Engine::Vec3(0, 5, 0), Engine::Vec3(0));
Engine::GraphicalObjectComponent playerGob;
Engine::SpatialComponent playerSpatial;
MouseComponent mouseComponent;
KeyboardComponent playerInput;
Engine::Vec3 spotlightDir(0.0f, -1.0f, 0.0f);
float spotlightAttenuation = 1.0f, spotlightCutoff = Engine::MathUtility::ToRadians(45.0f);
int numCelLevels = 4;
Engine::Vec3 backgroundColor(0.3f, 0.3f, 0.3f);
Engine::Vec3 planeColor(1.0f);
float fogMinDist = 70.0f, fogMaxDist = 250.0f;
//Engine::GraphicalObject m_crate;
int mossTexID;
int alphaTexID;

const int framebufferWidth = 512;
const int framebufferHeight = 512;
const float framebufferAspect = framebufferWidth * 1.0f / framebufferHeight;
Engine::Mat4 framebufferLookAt;
Engine::Mat4 framebufferPerspective;
Engine::FrameBuffer framebuffer;
int whiteSquareId = 0;
Engine::GraphicalObject framebufferCamera;
Engine::Camera fbCam;

void EngineDemo::MultipleTextureSetup()
{
	float offset = 200.0f;
	Engine::ShapeGenerator::MakeHorizontalPlane(&m_scenePlanes[1], Engine::Vec3(-0.5f, 0.0f, -0.5f), Engine::Vec3(0.5f, 0.0f, 0.5f), Engine::Vec3(0.5f, 0.5f, 0.5f));
	m_scenePlanes[1].SetTransMat(Engine::Mat4::Translation(Engine::Vec3(offset, 0.0f, 0.0f)));
	m_scenePlanes[1].SetScaleMat(Engine::Mat4::Scale(100.0f));
	m_scenePlanes[1].AddPhongUniforms(modelToWorldMatLoc, worldToViewMatLoc, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), perspectiveMatLoc, m_perspective.GetPerspectivePtr()->GetAddress(),
		tintColorLoc, diffuseColorLoc, ambientColorLoc, specularColorLoc, specularPowerLoc, diffuseIntensityLoc, ambientIntensityLoc, specularIntensityLoc,
		&m_lights[1].GetMatPtr()->m_materialColor, cameraPosLoc, playerCamera.GetPosPtr(), lightLoc, m_lights[1].GetLocPtr());
	m_scenePlanes[1].GetMatPtr()->m_materialColor = planeColor;
	m_scenePlanes[1].GetMatPtr()->m_diffuseReflectivity = Engine::Vec3(0.8f);
	m_scenePlanes[1].GetMatPtr()->m_ambientReflectivity = 0.1f * m_scenePlanes[0].GetMatPtr()->m_diffuseReflectivity;
	m_scenePlanes[1].GetMatPtr()->m_specularReflectivity = Engine::Vec3(0.1f);
	m_scenePlanes[1].GetMatPtr()->m_specularIntensity = 16.0f;
	Engine::RenderEngine::AddGraphicalObject(&m_scenePlanes[1]);

	for (int i = 2; i < 4; ++i)
	{
		Engine::ShapeGenerator::ReadSceneFile("..\\Data\\Scenes\\PTNCube.PTN.scene", &m_demoObjects[i], m_shaderPrograms[3].GetProgramId(), "..\\Data\\Textures\\cement.bmp", false);
		m_demoObjects[i].SetTransMat(Engine::Mat4::Translation(Engine::Vec3((i - 2.5f)*20.0f + offset, 5.0f, 0.0f)));
		m_demoObjects[i].SetScaleMat(Engine::Mat4::Scale(10.0f));
		m_demoObjects[i].AddPhongUniforms(modelToWorldMatLoc, worldToViewMatLoc, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), perspectiveMatLoc, m_perspective.GetPerspectivePtr()->GetAddress(),
			tintColorLoc, diffuseColorLoc, ambientColorLoc, specularColorLoc, specularPowerLoc, diffuseIntensityLoc, ambientIntensityLoc, specularIntensityLoc,
			&m_lights[1].GetMatPtr()->m_materialColor, cameraPosLoc, playerCamera.GetPosPtr(), lightLoc, m_lights[1].GetLocPtr());
		m_demoObjects[i].AddUniformData(Engine::UniformData(GL_TEXTURE0, m_demoObjects[i].GetMeshPointer()->GetTextureIDPtr(), brickLoc));
		m_demoObjects[i].AddUniformData(Engine::UniformData(GL_TEXTURE1, &mossTexID, mossLoc));

		m_demoObjects[i].GetMatPtr()->m_specularReflectivity = Engine::Vec3(0.1f);
		m_demoObjects[i].GetMatPtr()->m_diffuseReflectivity = Engine::Vec3(0.8f);
		m_demoObjects[i].GetMatPtr()->m_ambientReflectivity = Engine::Vec3(0.1f);
		m_demoObjects[i].GetMatPtr()->m_specularIntensity = 128.0f;
		Engine::RenderEngine::AddGraphicalObject(&m_demoObjects[i]);
	}

	Engine::ShapeGenerator::MakeLightingCube(&m_lights[1]);
	m_lights[1].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_lights[1].GetFullTransformPtr()->GetAddress(), modelToWorldMatLoc));
	m_lights[1].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), worldToViewMatLoc));
	m_lights[1].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_perspective.GetPerspectivePtr()->GetAddress(), perspectiveMatLoc));
	m_lights[1].AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &m_lights[1].GetMatPtr()->m_materialColor, tintColorLoc));
	m_lights[1].AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &m_lights[1].GetMatPtr()->m_materialColor, debugColorLoc));
	m_lights[1].AddUniformData(Engine::UniformData(GL_FLOAT, &m_lights[1].GetMatPtr()->m_specularIntensity, tintIntensityLoc));
	m_lights[1].SetTransMat(Engine::Mat4::Translation(Engine::Vec3(offset, 15.0f, 5.0f)));
	m_lights[1].GetMatPtr()->m_materialColor = Engine::Vec3(1.0f, 1.0f, 1.0f);
	m_lights[1].GetMatPtr()->m_diffuseReflectivity = Engine::Vec3(1.0f, 1.0f, 1.0f);
	Engine::RenderEngine::AddGraphicalObject(&m_lights[1]);
	Engine::CollisionTester::AddGraphicalObject(&m_lights[1]);

	Engine::CollisionTester::AddGraphicalObject(&m_scenePlanes[1]);

	mossTexID = Engine::BitmapLoader::LoadTexture("..\\Data\\Textures\\moss07.bmp");
}

void EngineDemo::FramebufferSetup()
{
	float offset = 400.0f;
	Engine::ShapeGenerator::MakeHorizontalPlane(&m_scenePlanes[3], Engine::Vec3(-0.5f, 0.0f, -0.5f), Engine::Vec3(0.5f, 0.0f, 0.5f), Engine::Vec3(0.5f, 0.5f, 0.5f));
	m_scenePlanes[3].SetTransMat(Engine::Mat4::Translation(Engine::Vec3(offset, 0.0f, 0.0f)));
	m_scenePlanes[3].SetScaleMat(Engine::Mat4::Scale(100.0f));
	m_scenePlanes[3].AddPhongUniforms(modelToWorldMatLoc, worldToViewMatLoc, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), perspectiveMatLoc, m_perspective.GetPerspectivePtr()->GetAddress(),
		tintColorLoc, diffuseColorLoc, ambientColorLoc, specularColorLoc, specularPowerLoc, diffuseIntensityLoc, ambientIntensityLoc, specularIntensityLoc,
		&m_lights[3].GetMatPtr()->m_materialColor, cameraPosLoc, playerCamera.GetPosPtr(), lightLoc, m_lights[3].GetLocPtr());
	m_scenePlanes[3].GetMatPtr()->m_materialColor = planeColor;
	m_scenePlanes[3].GetMatPtr()->m_diffuseReflectivity = Engine::Vec3(0.8f);
	m_scenePlanes[3].GetMatPtr()->m_ambientReflectivity = 0.1f * m_scenePlanes[0].GetMatPtr()->m_diffuseReflectivity;
	m_scenePlanes[3].GetMatPtr()->m_specularReflectivity = Engine::Vec3(0.1f);
	m_scenePlanes[3].GetMatPtr()->m_specularIntensity = 16.0f;
	Engine::RenderEngine::AddGraphicalObject(&m_scenePlanes[3]);


	Engine::ShapeGenerator::ReadSceneFile("..\\Data\\Scenes\\PTNCube.PTN.scene", &m_demoObjects[6], m_shaderPrograms[1].GetProgramId(), nullptr, false);
	Engine::ShapeGenerator::MakeNormalCube(&m_demoObjects[7]);
	Engine::ShapeGenerator::ReadSceneFile("..\\Data\\Scenes\\Teapot.PN.scene", &m_demoObjects[8], m_shaderPrograms[5].GetProgramId(), nullptr, false);
	Engine::ShapeGenerator::ReadSceneFile("..\\Data\\Scenes\\BetterDargon.PN.scene", &m_demoObjects[9], m_shaderPrograms[5].GetProgramId(), nullptr, false);
	m_demoObjects[6].SetTransMat(Engine::Mat4::Translation(-30.0f + offset, 5.0f, -30.0f));
	m_demoObjects[7].SetTransMat(Engine::Mat4::Translation(+30.0f + offset, 10.0f, -30.0f));
	m_demoObjects[8].SetTransMat(Engine::Mat4::Translation(+30.0f + offset, 10.0f, +30.0f));
	m_demoObjects[9].SetTransMat(Engine::Mat4::Translation(-30.0f + offset, 10.0f, +30.0f));
	m_demoObjects[6].SetScaleMat(Engine::Mat4::Scale(10.0f));
	m_demoObjects[7].SetScaleMat(Engine::Mat4::Scale(7.0f));
	m_demoObjects[8].SetScaleMat(Engine::Mat4::Scale(5.0f));
	m_demoObjects[9].SetScaleMat(Engine::Mat4::Scale(1.0f));
	m_demoObjects[8].SetRotationRate(Engine::MathUtility::ToRadians(72.0f));
	m_demoObjects[8].SetRotationAxis(Engine::Vec3(0.0f, 1.0f, 0.0f));
	m_demoObjects[7].SetRotationRate(Engine::MathUtility::ToRadians(72.0f));
	m_demoObjects[7].SetRotationAxis(Engine::Vec3(1.0f, 0.0f, 0.0f));
	m_demoObjects[9].SetRotationRate(Engine::MathUtility::ToRadians(72.0f));
	m_demoObjects[9].SetRotationAxis(Engine::Vec3(0.0f, 0.0f, 1.0f));
	m_demoObjects[6].SetRotationRate(Engine::MathUtility::ToRadians(18.0f));
	m_demoObjects[6].SetRotationAxis(Engine::Vec3(0.0f, 1.0f, 0.0f));

	for (int i = 6; i < 10; ++i)
	{
		m_demoObjects[i].AddPhongUniforms(modelToWorldMatLoc, worldToViewMatLoc, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), perspectiveMatLoc, m_perspective.GetPerspectivePtr()->GetAddress(),
			tintColorLoc, diffuseColorLoc, ambientColorLoc, specularColorLoc, specularPowerLoc, diffuseIntensityLoc, ambientIntensityLoc, specularIntensityLoc,
			&m_lights[3].GetMatPtr()->m_materialColor, cameraPosLoc, playerCamera.GetPosPtr(), lightLoc, m_lights[3].GetLocPtr());

		m_demoObjects[i].GetMatPtr()->m_specularReflectivity = Engine::Vec3(0.1f);
		m_demoObjects[i].GetMatPtr()->m_diffuseReflectivity = Engine::Vec3(0.8f);
		m_demoObjects[i].GetMatPtr()->m_ambientReflectivity = Engine::Vec3(0.1f);
		m_demoObjects[i].GetMatPtr()->m_specularIntensity = 128.0f;
		Engine::RenderEngine::AddGraphicalObject(&m_demoObjects[i]);
	}
	m_demoObjects[6].AddUniformData(Engine::UniformData(GL_TEXTURE0, framebuffer.GetTexIdPtr(), brickLoc));

	Engine::ShapeGenerator::MakeLightingCube(&m_lights[3]);
	m_lights[3].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_lights[3].GetFullTransformPtr()->GetAddress(), modelToWorldMatLoc));
	m_lights[3].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), worldToViewMatLoc));
	m_lights[3].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_perspective.GetPerspectivePtr()->GetAddress(), perspectiveMatLoc));
	m_lights[3].AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &m_lights[3].GetMatPtr()->m_materialColor, tintColorLoc));
	m_lights[3].AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &m_lights[3].GetMatPtr()->m_materialColor, debugColorLoc));
	m_lights[3].AddUniformData(Engine::UniformData(GL_FLOAT, &m_lights[3].GetMatPtr()->m_specularIntensity, tintIntensityLoc));
	m_lights[3].SetTransMat(Engine::Mat4::Translation(Engine::Vec3(offset, 15.0f, 5.0f)));
	m_lights[3].GetMatPtr()->m_materialColor = Engine::Vec3(1.0f, 1.0f, 1.0f);
	m_lights[3].GetMatPtr()->m_diffuseReflectivity = Engine::Vec3(1.0f, 1.0f, 1.0f);
	Engine::RenderEngine::AddGraphicalObject(&m_lights[3]);
	Engine::CollisionTester::AddGraphicalObject(&m_lights[3]);

	Engine::CollisionTester::AddGraphicalObject(&m_scenePlanes[3]);
}


void EngineDemo::CustomSetup()
{
	float offset = -400.0f;
	Engine::ShapeGenerator::MakeHorizontalPlane(&m_scenePlanes[4], Engine::Vec3(-0.5f, 0.0f, -0.5f), Engine::Vec3(0.5f, 0.0f, 0.5f), Engine::Vec3(0.5f, 0.5f, 0.5f));
	m_scenePlanes[4].SetTransMat(Engine::Mat4::Translation(Engine::Vec3(offset, 0.0f, 0.0f)));
	m_scenePlanes[4].SetScaleMat(Engine::Mat4::Scale(100.0f));
	m_scenePlanes[4].AddPhongUniforms(modelToWorldMatLoc, worldToViewMatLoc, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), perspectiveMatLoc, m_perspective.GetPerspectivePtr()->GetAddress(),
		tintColorLoc, diffuseColorLoc, ambientColorLoc, specularColorLoc, specularPowerLoc, diffuseIntensityLoc, ambientIntensityLoc, specularIntensityLoc,
		&m_lights[4].GetMatPtr()->m_materialColor, cameraPosLoc, playerCamera.GetPosPtr(), lightLoc, m_lights[4].GetLocPtr());
	m_scenePlanes[4].GetMatPtr()->m_materialColor = planeColor;
	m_scenePlanes[4].GetMatPtr()->m_diffuseReflectivity = Engine::Vec3(0.8f);
	m_scenePlanes[4].GetMatPtr()->m_ambientReflectivity = 0.1f * m_scenePlanes[4].GetMatPtr()->m_diffuseReflectivity;
	m_scenePlanes[4].GetMatPtr()->m_specularReflectivity = Engine::Vec3(0.1f);
	m_scenePlanes[4].GetMatPtr()->m_specularIntensity = 16.0f;
	Engine::RenderEngine::AddGraphicalObject(&m_scenePlanes[4]);
	Engine::CollisionTester::AddGraphicalObject(&m_scenePlanes[4]);

	for (int i = 10; i < 12; ++i)
	{
		Engine::ShapeGenerator::ReadSceneFile("..\\Data\\Scenes\\PTNCube.PTN.scene", &m_demoObjects[i], m_shaderPrograms[6].GetProgramId(), "..\\Data\\Textures\\NormalMap.bmp", false);
		m_demoObjects[i].SetTransMat(Engine::Mat4::Translation(Engine::Vec3((i - 10.5f)*20.0f + offset, 5.0f, 0.0f)));
		m_demoObjects[i].SetScaleMat(Engine::Mat4::Scale(10.0f));
		m_demoObjects[i].AddPhongUniforms(modelToWorldMatLoc, worldToViewMatLoc, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), perspectiveMatLoc, m_perspective.GetPerspectivePtr()->GetAddress(),
			tintColorLoc, diffuseColorLoc, ambientColorLoc, specularColorLoc, specularPowerLoc, diffuseIntensityLoc, ambientIntensityLoc, specularIntensityLoc,
			&m_lights[4].GetMatPtr()->m_materialColor, cameraPosLoc, playerCamera.GetPosPtr(), lightLoc, m_lights[4].GetLocPtr());
		m_demoObjects[i].AddUniformData(Engine::UniformData(GL_TEXTURE0, m_demoObjects[i].GetMeshPointer()->GetTextureIDPtr(), texLoc));
		m_demoObjects[i].GetMatPtr()->m_specularReflectivity = Engine::Vec3(0.1f);
		m_demoObjects[i].GetMatPtr()->m_diffuseReflectivity = Engine::Vec3(0.8f);
		m_demoObjects[i].GetMatPtr()->m_ambientReflectivity = Engine::Vec3(0.1f);
		m_demoObjects[i].GetMatPtr()->m_specularIntensity = 128.0f;
		Engine::RenderEngine::AddGraphicalObject(&m_demoObjects[i]);
	}

	Engine::ShapeGenerator::MakeLightingCube(&m_lights[4]);
	m_lights[4].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_lights[4].GetFullTransformPtr()->GetAddress(), modelToWorldMatLoc));
	m_lights[4].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), worldToViewMatLoc));
	m_lights[4].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_perspective.GetPerspectivePtr()->GetAddress(), perspectiveMatLoc));
	m_lights[4].AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &m_lights[4].GetMatPtr()->m_materialColor, tintColorLoc));
	m_lights[4].AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &m_lights[4].GetMatPtr()->m_materialColor, debugColorLoc));
	m_lights[4].AddUniformData(Engine::UniformData(GL_FLOAT, &m_lights[4].GetMatPtr()->m_specularIntensity, tintIntensityLoc));
	m_lights[4].SetTransMat(Engine::Mat4::Translation(Engine::Vec3(offset, 15.0f, 0.0f)));
	m_lights[4].GetMatPtr()->m_materialColor = Engine::Vec3(1.0f, 1.0f, 1.0f);
	m_lights[4].GetMatPtr()->m_diffuseReflectivity = Engine::Vec3(1.0f, 1.0f, 1.0f);
	Engine::RenderEngine::AddGraphicalObject(&m_lights[4]);
	Engine::CollisionTester::AddGraphicalObject(&m_lights[4]);
}

void EngineDemo::CrateSetup()
{
	Engine::ShapeGenerator::MakeHorizontalPlane(&m_scenePlanes[0], Engine::Vec3(-0.5f, 0.0f, -0.5f), Engine::Vec3(0.5f, 0.0f, 0.5f), Engine::Vec3(0.5f, 0.5f, 0.5f));
	m_scenePlanes[0].SetTransMat(Engine::Mat4::Translation(Engine::Vec3(0.0f, 0.0f, 0.0f)));
	m_scenePlanes[0].SetScaleMat(Engine::Mat4::Scale(100.0f));
	m_scenePlanes[0].AddPhongUniforms(modelToWorldMatLoc, worldToViewMatLoc, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), perspectiveMatLoc, m_perspective.GetPerspectivePtr()->GetAddress(),
		tintColorLoc, diffuseColorLoc, ambientColorLoc, specularColorLoc, specularPowerLoc, diffuseIntensityLoc, ambientIntensityLoc, specularIntensityLoc,
		&m_lights[0].GetMatPtr()->m_materialColor, cameraPosLoc, playerCamera.GetPosPtr(), lightLoc, m_lights[0].GetLocPtr());
	m_scenePlanes[0].GetMatPtr()->m_materialColor = planeColor;
	m_scenePlanes[0].GetMatPtr()->m_diffuseReflectivity = Engine::Vec3(0.8f);
	m_scenePlanes[0].GetMatPtr()->m_ambientReflectivity = 0.1f * m_scenePlanes[0].GetMatPtr()->m_diffuseReflectivity;
	m_scenePlanes[0].GetMatPtr()->m_specularReflectivity = Engine::Vec3(0.1f);
	m_scenePlanes[0].GetMatPtr()->m_specularIntensity = 16.0f;
	Engine::RenderEngine::AddGraphicalObject(&m_scenePlanes[0]);
	Engine::CollisionTester::AddGraphicalObject(&m_scenePlanes[0]);

	for (int i = 0; i < 2; ++i)
	{
		Engine::ShapeGenerator::ReadSceneFile("..\\Data\\Scenes\\PTNCube.PTN.scene", &m_demoObjects[i], m_shaderPrograms[1].GetProgramId(), "..\\Data\\Textures\\crate.bmp", false);
		m_demoObjects[i].SetTransMat(Engine::Mat4::Translation(Engine::Vec3((i - 0.5f)*20.0f, 5.0f, 0.0f)));
		m_demoObjects[i].SetScaleMat(Engine::Mat4::Scale(10.0f));
		m_demoObjects[i].AddPhongUniforms(modelToWorldMatLoc, worldToViewMatLoc, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), perspectiveMatLoc, m_perspective.GetPerspectivePtr()->GetAddress(),
			tintColorLoc, diffuseColorLoc, ambientColorLoc, specularColorLoc, specularPowerLoc, diffuseIntensityLoc, ambientIntensityLoc, specularIntensityLoc,
			&m_lights[0].GetMatPtr()->m_materialColor, cameraPosLoc, playerCamera.GetPosPtr(), lightLoc, m_lights[0].GetLocPtr());
		m_demoObjects[i].AddUniformData(Engine::UniformData(GL_TEXTURE0, m_demoObjects[i].GetMeshPointer()->GetTextureIDPtr(), texLoc));
		m_demoObjects[i].GetMatPtr()->m_specularReflectivity = Engine::Vec3(0.1f);
		m_demoObjects[i].GetMatPtr()->m_diffuseReflectivity = Engine::Vec3(0.8f);
		m_demoObjects[i].GetMatPtr()->m_ambientReflectivity = Engine::Vec3(0.1f);
		m_demoObjects[i].GetMatPtr()->m_specularIntensity = 128.0f;
		Engine::RenderEngine::AddGraphicalObject(&m_demoObjects[i]);
	}

	Engine::ShapeGenerator::MakeLightingCube(&m_lights[0]);
	m_lights[0].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_lights[0].GetFullTransformPtr()->GetAddress(), modelToWorldMatLoc));
	m_lights[0].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), worldToViewMatLoc));
	m_lights[0].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_perspective.GetPerspectivePtr()->GetAddress(), perspectiveMatLoc));
	m_lights[0].AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &m_lights[0].GetMatPtr()->m_materialColor, tintColorLoc));
	m_lights[0].AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &m_lights[0].GetMatPtr()->m_materialColor, debugColorLoc));
	m_lights[0].AddUniformData(Engine::UniformData(GL_FLOAT, &m_lights[0].GetMatPtr()->m_specularIntensity, tintIntensityLoc));
	m_lights[0].SetTransMat(Engine::Mat4::Translation(Engine::Vec3(0.0f, 15.0f, 0.0f)));
	m_lights[0].GetMatPtr()->m_materialColor = Engine::Vec3(1.0f, 1.0f, 1.0f);
	m_lights[0].GetMatPtr()->m_diffuseReflectivity = Engine::Vec3(1.0f, 1.0f, 1.0f);
	Engine::RenderEngine::AddGraphicalObject(&m_lights[0]);
	Engine::CollisionTester::AddGraphicalObject(&m_lights[0]);
}

void EngineDemo::AlphaDiscardSetup()
{
	float offset = -200.0f;
	Engine::ShapeGenerator::MakeHorizontalPlane(&m_scenePlanes[2], Engine::Vec3(-0.5f, 0.0f, -0.5f), Engine::Vec3(0.5f, 0.0f, 0.5f), Engine::Vec3(0.5f, 0.5f, 0.5f));
	m_scenePlanes[2].SetTransMat(Engine::Mat4::Translation(Engine::Vec3(offset, 0.0f, 0.0f)));
	m_scenePlanes[2].SetScaleMat(Engine::Mat4::Scale(100.0f));
	m_scenePlanes[2].AddPhongUniforms(modelToWorldMatLoc, worldToViewMatLoc, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), perspectiveMatLoc, m_perspective.GetPerspectivePtr()->GetAddress(),
		tintColorLoc, diffuseColorLoc, ambientColorLoc, specularColorLoc, specularPowerLoc, diffuseIntensityLoc, ambientIntensityLoc, specularIntensityLoc,
		&m_lights[2].GetMatPtr()->m_materialColor, cameraPosLoc, playerCamera.GetPosPtr(), lightLoc, m_lights[2].GetLocPtr());
	m_scenePlanes[2].GetMatPtr()->m_materialColor = planeColor;
	m_scenePlanes[2].GetMatPtr()->m_diffuseReflectivity = Engine::Vec3(0.8f);
	m_scenePlanes[2].GetMatPtr()->m_ambientReflectivity = 0.1f * m_scenePlanes[0].GetMatPtr()->m_diffuseReflectivity;
	m_scenePlanes[2].GetMatPtr()->m_specularReflectivity = Engine::Vec3(0.1f);
	m_scenePlanes[2].GetMatPtr()->m_specularIntensity = 16.0f;
	Engine::RenderEngine::AddGraphicalObject(&m_scenePlanes[2]);

	for (int i = 4; i < 6; ++i)
	{
		Engine::ShapeGenerator::ReadSceneFile("..\\Data\\Scenes\\PTNCube.PTN.scene", &m_demoObjects[i], m_shaderPrograms[4].GetProgramId(), "..\\Data\\Textures\\leaf03.bmp", false);
		m_demoObjects[i].SetTransMat(Engine::Mat4::Translation(Engine::Vec3((i - 4.5f)*20.0f + offset, 10.0f, 0.0f)));
		m_demoObjects[i].SetScaleMat(Engine::Mat4::Scale(10.0f));
		m_demoObjects[i].AddPhongUniforms(modelToWorldMatLoc, worldToViewMatLoc, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), perspectiveMatLoc, m_perspective.GetPerspectivePtr()->GetAddress(),
			tintColorLoc, diffuseColorLoc, ambientColorLoc, specularColorLoc, specularPowerLoc, diffuseIntensityLoc, ambientIntensityLoc, specularIntensityLoc,
			&m_lights[2].GetMatPtr()->m_materialColor, cameraPosLoc, playerCamera.GetPosPtr(), lightLoc, m_lights[2].GetLocPtr());
		m_demoObjects[i].AddUniformData(Engine::UniformData(GL_TEXTURE0, m_demoObjects[i].GetMeshPointer()->GetTextureIDPtr(), brickLoc));
		m_demoObjects[i].AddUniformData(Engine::UniformData(GL_TEXTURE1, &alphaTexID, mossLoc));

		m_demoObjects[i].GetMatPtr()->m_specularReflectivity = Engine::Vec3(0.1f);
		m_demoObjects[i].GetMatPtr()->m_diffuseReflectivity = Engine::Vec3(0.8f);
		m_demoObjects[i].GetMatPtr()->m_ambientReflectivity = Engine::Vec3(0.1f);
		m_demoObjects[i].GetMatPtr()->m_specularIntensity = 128.0f;
		m_demoObjects[i].SetRotationAxis(Engine::Vec3(0.0f, 1.0f, 0.0f));
		m_demoObjects[i].SetRotationRate(Engine::MathUtility::ToRadians(36.0f));
		Engine::RenderEngine::AddGraphicalObject(&m_demoObjects[i]);
	}

	Engine::ShapeGenerator::MakeLightingCube(&m_lights[2]);
	m_lights[2].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_lights[2].GetFullTransformPtr()->GetAddress(), modelToWorldMatLoc));
	m_lights[2].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), worldToViewMatLoc));
	m_lights[2].AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_perspective.GetPerspectivePtr()->GetAddress(), perspectiveMatLoc));
	m_lights[2].AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &m_lights[2].GetMatPtr()->m_materialColor, tintColorLoc));
	m_lights[2].AddUniformData(Engine::UniformData(GL_FLOAT_VEC3, &m_lights[2].GetMatPtr()->m_materialColor, debugColorLoc));
	m_lights[2].AddUniformData(Engine::UniformData(GL_FLOAT, &m_lights[2].GetMatPtr()->m_specularIntensity, tintIntensityLoc));
	m_lights[2].SetTransMat(Engine::Mat4::Translation(Engine::Vec3(offset, 15.0f, 5.0f)));
	m_lights[2].GetMatPtr()->m_materialColor = Engine::Vec3(1.0f, 1.0f, 1.0f);
	m_lights[2].GetMatPtr()->m_diffuseReflectivity = Engine::Vec3(1.0f, 1.0f, 1.0f);
	Engine::RenderEngine::AddGraphicalObject(&m_lights[2]);
	Engine::CollisionTester::AddGraphicalObject(&m_lights[2]);

	Engine::CollisionTester::AddGraphicalObject(&m_scenePlanes[2]);

	alphaTexID = Engine::BitmapLoader::LoadTexture("..\\Data\\Textures\\leaf03Alpha.bmp");
}

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

	if (!Engine::ShapeGenerator::Initialize(m_shaderPrograms[2].GetProgramId(), m_shaderPrograms[0].GetProgramId(), m_shaderPrograms[5].GetProgramId()))
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
	playerGraphicalObject.SetScaleMat(Engine::Mat4::Scale(1.0f));
	playerGraphicalObject.SetTransMat(Engine::Mat4::Translation(Engine::Vec3(375.0f, 5.0f, 5.0f)));
	Engine::RenderEngine::AddGraphicalObject(&playerGraphicalObject);
	playerGob.SetGraphicalObject(&playerGraphicalObject);
	player.AddComponent(&playerGob, "PlayerGob");
	player.AddComponent(&playerCamera, "PlayerCamera");
	player.AddComponent(&playerInput, "PlayerInput");
	player.AddComponent(&mouseComponent, "MouseComponent");
	player.Initialize();
	player.GetComponentByType<Engine::SpatialComponent>()->SetPosition(Engine::Vec3(375.0f, 5.0f, 5.0f));
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

	CrateSetup();
	MultipleTextureSetup();
	AlphaDiscardSetup();
	FramebufferSetup();
	CustomSetup();

	if (!framebuffer.Initialize(framebufferWidth, framebufferHeight)) { return false; }
	Engine::Perspective p;
	p.SetPerspective(framebufferAspect, Engine::MathUtility::ToRadians(60.0f), 1.0f, RENDER_DISTANCE);
	framebufferPerspective = p.GetPerspective();
	fbCam.SetPosition(Engine::Vec3(400.0f, 15.0f, 0.0f));

	Engine::ShapeGenerator::MakeSphere(&framebufferCamera, Engine::Vec3(0.0f, 1.0f, 0.0f));
	framebufferCamera.SetTransMat(Engine::Mat4::Translation(fbCam.GetPosition()));
	framebufferCamera.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, framebufferCamera.GetFullTransformPtr(), modelToWorldMatLoc));
	framebufferCamera.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), worldToViewMatLoc));
	framebufferCamera.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_perspective.GetPerspectivePtr()->GetAddress(), perspectiveMatLoc));
	Engine::RenderEngine::AddGraphicalObject(&framebufferCamera);

	framebufferLookAt = fbCam.GetWorldToViewMatrix();
	whiteSquareId = Engine::BitmapLoader::SetupWhitePixel();

	Engine::ShapeGenerator::MakeGrid(&m_grid, Engine::CollisionTester::GetGridWidthSections(), Engine::CollisionTester::GetGridHeightSections(), Engine::Vec3(0.5f));
	Engine::RenderEngine::AddGraphicalObject(&m_grid);
	m_grid.SetScaleMat(Engine::Mat4::Scale(Engine::CollisionTester::GetGridScale()));
	m_grid.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_grid.GetFullTransformPtr(), modelToWorldMatLoc));
	m_grid.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, playerCamera.GetWorldToViewMatrixPtr()->GetAddress(), worldToViewMatLoc));
	m_grid.AddUniformData(Engine::UniformData(GL_FLOAT_MAT4, m_perspective.GetPerspectivePtr()->GetAddress(), perspectiveMatLoc));

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
	if (!keyboardManager.AddKeys("XTWASDRFLGCM QE01234567`9iKNJB")
		|| !keyboardManager.AddKey(VK_OEM_4) || !keyboardManager.AddKey(VK_OEM_6) || !keyboardManager.AddKey(VK_OEM_5)
		|| !keyboardManager.AddKey(VK_PRIOR) || !keyboardManager.AddKey(VK_NEXT)
		|| !keyboardManager.AddKey(VK_OEM_PERIOD) || !keyboardManager.AddKey(VK_SHIFT))
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

	playerGraphicalObject.CalcFullTransform();
	player.Update(dt);

	if (paused) { return; }

	for (int i = 0; i < NUM_DEMO_OBJECTS; ++i)
	{
		m_demoObjects[i].SetRotation(m_demoObjects[i].GetRotation() + dt * m_demoObjects[i].GetRotationRate());
		m_demoObjects[i].SetRotMat(Engine::Mat4::RotationAroundAxis(m_demoObjects[i].GetRotationAxis(), m_demoObjects[i].GetRotation()));

		/*if (i >= 4 && i <= 6)
		{
			Engine::Vec3 xyz = Engine::Vec3(i % 3 == 2 ? 1.0f : 0.0f,
				i % 3 == 0 ? 1.0f : 0.0f,
				i % 3 == 1 ? 1.0f : 0.0f);
			Engine::Vec3 teapotPos = m_demoObjects[5].GetPos();
				m_lights[i].SetRotation(m_lights[i].GetRotation() + m_lights[i].GetRotationRate() * dt);
			Engine::Mat4 rotationAroundAxis = Engine::Mat4::RotationAroundAxis(m_lights[i].GetRotationAxis(), m_lights[i].GetRotation());
			m_lights[i].SetTransMat(Engine::Mat4::Translation(teapotPos + (ROTATE_DISTANCE * (rotationAroundAxis * xyz))));
		}*/

		m_lights[i].CalcFullTransform();
		m_demoObjects[i].CalcFullTransform();
	}

	m_grid.CalcFullTransform();

	static Engine::GraphicalObject *s_pSelected = nullptr;
	bool thisFrame = false;
	Engine::RayCastingOutput rco = Engine::CollisionTester::FindFromMousePos(Engine::MouseManager::GetMouseX(), Engine::MouseManager::GetMouseY(), 1000.0f);
	for (int i = 0; i < NUM_DEMO_OBJECTS; ++i)
	{
		if (rco.m_belongsTo == &m_lights[i] && Engine::MouseManager::IsLeftMouseClicked())
		{
			s_pSelected = &m_lights[i];
			thisFrame = true;
		}
	}

	if (s_pSelected && !thisFrame && Engine::MouseManager::IsLeftMouseClicked())
	{
		s_pSelected = nullptr;
		Engine::CollisionTester::CalculateGrid(); // handle de-selection of light
	}

	bool isPlane = false;
	for (int i = 0; i < NUM_SCENES; ++i) { if (rco.m_belongsTo == &m_scenePlanes[i]) { isPlane = true; } }
	if (rco.m_didIntersect && (isPlane) && s_pSelected)
	{ 
		s_pSelected->SetTransMat(Engine::Mat4::Translation(rco.m_intersectionPoint + Engine::Vec3(0.0f, 15.0f, 0.0f))); 
	}

	for (int i = 0; i < NUM_SCENES; ++i){ m_scenePlanes[i].CalcFullTransform(); }
	framebufferCamera.CalcFullTransform();

	static float error = 0.0f;
	error += 25.0f * dt;
	if (error > 1.0f) { error -= 1.0f; 	fbCam.MouseRotate(1, 0);}
	framebufferLookAt = fbCam.GetWorldToViewMatrix();
}

void EngineDemo::Draw()
{
	// Clear window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// bind the framebuffer
	framebuffer.Bind();

	// draw the crate
	Engine::RenderEngine::DrawSingleObjectDifferently(&m_scenePlanes[3], framebufferPerspective.GetAddress(), framebufferLookAt.GetAddress(), nullptr, worldToViewMatLoc, perspectiveMatLoc, texLoc);
	Engine::RenderEngine::DrawSingleObjectDifferently(&m_demoObjects[7], framebufferPerspective.GetAddress(), framebufferLookAt.GetAddress(), nullptr, worldToViewMatLoc, perspectiveMatLoc, texLoc);
	Engine::RenderEngine::DrawSingleObjectDifferently(&m_demoObjects[8], framebufferPerspective.GetAddress(), framebufferLookAt.GetAddress(), nullptr, worldToViewMatLoc, perspectiveMatLoc, texLoc);
	Engine::RenderEngine::DrawSingleObjectDifferently(&m_demoObjects[9], framebufferPerspective.GetAddress(), framebufferLookAt.GetAddress(), nullptr, worldToViewMatLoc, perspectiveMatLoc, texLoc);
	//Engine::RenderEngine::DrawSingleObjectDifferently(&playerGraphicalObject, framebufferPerspective.GetAddress(), framebufferLookAt.GetAddress(), nullptr, worldToViewMatLoc, perspectiveMatLoc, texLoc);

	// restore stuff
	framebuffer.UnBind(0, 0, m_pWindow->width(), m_pWindow->height());
	
	// draw regular stuff
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

	Engine::ConfigReader::pReader->GetFloatsForKey("EngineDemo.Spotlight.AttenuationExponent", 4, pGame->spotlightAttenuations.GetAddress());
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

	glClearColor(backgroundColor.GetX(), backgroundColor.GetY(), backgroundColor.GetZ(), 1.0f);

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
		m_shaderPrograms[1].AddVertexShader("..\\Data\\Shaders\\SimpleTexture.vert.shader");
		m_shaderPrograms[1].AddFragmentShader("..\\Data\\Shaders\\SimpleTexture.frag.shader");
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
		m_shaderPrograms[3].AddVertexShader("..\\Data\\Shaders\\MultipleTextures.vert.shader");
		m_shaderPrograms[3].AddFragmentShader("..\\Data\\Shaders\\MultipleTextures.frag.shader");
		m_shaderPrograms[3].LinkProgram();
		m_shaderPrograms[3].UseProgram();
	}

	if (m_shaderPrograms[4].Initialize())
	{					 
		m_shaderPrograms[4].AddVertexShader("..\\Data\\Shaders\\AlphaDiscard.vert.shader");
		m_shaderPrograms[4].AddFragmentShader("..\\Data\\Shaders\\AlphaDiscard.frag.shader");
		m_shaderPrograms[4].LinkProgram();
		m_shaderPrograms[4].UseProgram();
	}


	if (m_shaderPrograms[5].Initialize())
	{					 
		m_shaderPrograms[5].AddVertexShader("..\\Data\\Shaders\\PhongPhong.vert.shader");
		m_shaderPrograms[5].AddFragmentShader("..\\Data\\Shaders\\PhongPhong.frag.shader");
		m_shaderPrograms[5].LinkProgram();
		m_shaderPrograms[5].UseProgram();
	}


	if (m_shaderPrograms[6].Initialize())
	{					 
		m_shaderPrograms[6].AddVertexShader("..\\Data\\Shaders\\NormalMapPhong.vert.shader");
		m_shaderPrograms[6].AddFragmentShader("..\\Data\\Shaders\\NormalMapPhong.frag.shader");
		m_shaderPrograms[6].LinkProgram();
		m_shaderPrograms[6].UseProgram();
	}

	if (m_shaderPrograms[7].Initialize())
	{					 
		m_shaderPrograms[7].AddVertexShader("..\\Data\\Shaders\\Spotlight.vert.shader");
		m_shaderPrograms[7].AddFragmentShader("..\\Data\\Shaders\\Spotlight.frag.shader");
		m_shaderPrograms[7].LinkProgram();
		m_shaderPrograms[7].UseProgram();
	}

	if (m_shaderPrograms[8].Initialize())
	{					 
		m_shaderPrograms[8].AddVertexShader("..\\Data\\Shaders\\CelPhong.vert.shader");
		m_shaderPrograms[8].AddFragmentShader("..\\Data\\Shaders\\CelPhong.frag.shader");
		m_shaderPrograms[8].LinkProgram();
		m_shaderPrograms[8].UseProgram();
	}

	if (m_shaderPrograms[9].Initialize())
	{					 
		m_shaderPrograms[9].AddVertexShader("..\\Data\\Shaders\\FogPhong.vert.shader");
		m_shaderPrograms[9].AddFragmentShader("..\\Data\\Shaders\\FogPhong.frag.shader");
		m_shaderPrograms[9].LinkProgram();
		m_shaderPrograms[9].UseProgram();
	}

	debugColorLoc = m_shaderPrograms[0].GetUniformLocation("tint");
	tintColorLoc = m_shaderPrograms[1].GetUniformLocation("tint");
	tintIntensityLoc = m_shaderPrograms[0].GetUniformLocation("tintIntensity");
	texLoc = m_shaderPrograms[1].GetUniformLocation("textureSampler");
	ambientColorLoc = m_shaderPrograms[4].GetUniformLocation("ambientLightColor");
	ambientIntensityLoc = m_shaderPrograms[4].GetUniformLocation("ambientLightIntensity");
	diffuseColorLoc = m_shaderPrograms[4].GetUniformLocation("diffuseLightColor");
	diffuseIntensityLoc = m_shaderPrograms[4].GetUniformLocation("diffuseLightIntensity");
	specularColorLoc = m_shaderPrograms[4].GetUniformLocation("specularLightColor");
	specularIntensityLoc = m_shaderPrograms[4].GetUniformLocation("specularLightIntensity");
	specularPowerLoc = m_shaderPrograms[4].GetUniformLocation("specularPower");
	modelToWorldMatLoc = m_shaderPrograms[4].GetUniformLocation("modelToWorld");
	worldToViewMatLoc = m_shaderPrograms[4].GetUniformLocation("worldToView");
	perspectiveMatLoc = m_shaderPrograms[4].GetUniformLocation("projection");
	lightLoc = m_shaderPrograms[4].GetUniformLocation("lightPos_WorldSpace");
	cameraPosLoc = m_shaderPrograms[4].GetUniformLocation("cameraPosition_WorldSpace");
	subOneIndex = m_shaderPrograms[8].GetSubroutineIndex(GL_FRAGMENT_SHADER, "CalculateCelPhongLight");
	subTwoIndex = m_shaderPrograms[8].GetSubroutineIndex(GL_FRAGMENT_SHADER, "CalculatePhongLight");
	//directionalPositionLoc = m_shaderPrograms[3].GetUniformLocation("objectCenterPosition_WorldSpace");
	//lightsMin = m_shaderPrograms[6].GetUniformLocation("lights[0].positionEye");
	spotMin = m_shaderPrograms[7].GetUniformLocation("spotLight.positionEye");
	levelsLoc = m_shaderPrograms[8].GetUniformLocation("numLevels");
	fogMinLoc = m_shaderPrograms[9].GetUniformLocation("fog.minDist");
	fogMaxLoc = m_shaderPrograms[9].GetUniformLocation("fog.maxDist");
	fogColorLoc = m_shaderPrograms[9].GetUniformLocation("fog.color");
	mossLoc = m_shaderPrograms[3].GetUniformLocation("mossSampler");
	brickLoc = m_shaderPrograms[3].GetUniformLocation("brickSampler");

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

	if (!Engine::ConfigReader::pReader->GetFloatsForKey("EngineDemo.Spotlight.AttenuationExponent", 4, spotlightAttenuations.GetAddress()))
	{
		Engine::GameLogger::Log(Engine::MessageType::cFatal_Error, "Failed to get floats for key AttenuationExponent!\n"); return false;
	}


	Engine::GameLogger::Log(Engine::MessageType::Process, "Successfully read in config values!\n");
	return true;
}

bool EngineDemo::ProcessInput(float dt)
{
	static int spotLightIndex = 0;
	static bool specToggle = false;

	int multiKeyTest[]{ 'J', 'K', VK_OEM_PERIOD };
	if (keyboardManager.KeysArePressed(&multiKeyTest[0], 3)) { Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "3 keys pressed!\n"); }
	if (keyboardManager.KeyWasPressed('T')) { Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "%f\n", dt); }
	if (keyboardManager.KeyWasReleased('C')) { Engine::CollisionTester::ConsoleLogOutput(); }
	if (keyboardManager.KeyWasPressed('`')) { Engine::ConfigReader::pReader->ProcessConfigFile(); }
	if (keyboardManager.KeyWasPressed('M')) { Engine::GameLogger::Log(Engine::MessageType::ConsoleOnly, "(%.3f, %.3f, %.3f)\n", playerGraphicalObject.GetPos().GetX(), playerGraphicalObject.GetPos().GetY(), playerGraphicalObject.GetPos().GetZ()); }
	if (keyboardManager.KeyWasPressed('L')) { Engine::RenderEngine::LogStats(); }
	//if (keyboardManager.KeyWasPressed('0')) { HandleBitKeys(0); }
	//if (keyboardManager.KeyWasPressed('1')) { HandleBitKeys(1); }
	//if (keyboardManager.KeyWasPressed('2')) { HandleBitKeys(2); }
	//if (keyboardManager.KeyWasPressed('3')) { HandleBitKeys(3); }
	//if (keyboardManager.KeyWasPressed('4')) { HandleBitKeys(4); }
	//if (keyboardManager.KeyWasPressed('5')) { HandleBitKeys(5); }
	//if (keyboardManager.KeyWasPressed('6')) { HandleBitKeys(6); }
	//if (keyboardManager.KeyWasPressed('7')) { HandleBitKeys(7); }

	/*if (keyboardManager.KeyWasPressed('B'))
	{
		specToggle = !specToggle;
		Engine::Vec3 spec(specToggle ? 1.0f : 0.0f);
		m_scenePlanes[5].GetMatPtr()->m_specularReflectivity = spec;
		m_demoObjects[8].GetMatPtr()->m_specularReflectivity = spec;
		m_demoObjects[9].GetMatPtr()->m_specularReflectivity = spec;
	}

	int levelChange = 1;
	float amount = 1.0f;
	if (keyboardManager.KeyIsDown(VK_SHIFT)) { amount *= 10.0f; levelChange *= 10; }
	if (keyboardManager.KeyWasPressed(VK_PRIOR)) { numCelLevels = Engine::MathUtility::Clamp(numCelLevels + levelChange, 1, 100); }
	if (keyboardManager.KeyWasPressed(VK_NEXT)) { numCelLevels = Engine::MathUtility::Clamp(numCelLevels - levelChange, 1, 100); }
	if (keyboardManager.KeyWasPressed(VK_OEM_4)) { spotlightCutoff -= Engine::MathUtility::ToRadians(amount); }
	if (keyboardManager.KeyWasPressed(VK_OEM_6)) { spotlightCutoff += Engine::MathUtility::ToRadians(amount); }
	if (keyboardManager.KeyWasPressed(VK_OEM_5)) { spotLightIndex++; spotLightIndex %= 4; spotlightAttenuation = spotlightAttenuations.GetAddress()[spotLightIndex]; }
*/
	if (keyboardManager.KeyWasPressed('X')) { Shutdown(); return false; }

	return true;
}

const int xBit = 4, xIndex = 4;
const int yBit = 2, yIndex = 5;
const int zBit = 1, zIndex = 6;
void EngineDemo::HandleBitKeys(int keyBits)
{
	m_lights[xIndex].SetRotationRate((keyBits & xBit) != 0 ? Engine::MathUtility::PI / 10.0f : 0.0f);
	m_lights[yIndex].SetRotationRate((keyBits & yBit) != 0 ? Engine::MathUtility::PI / 10.0f : 0.0f);
	m_lights[zIndex].SetRotationRate((keyBits & zBit) != 0 ? Engine::MathUtility::PI / 10.0f : 0.0f);
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

