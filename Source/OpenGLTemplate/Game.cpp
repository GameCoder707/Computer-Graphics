/*
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
 - Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
 - Christy Quinn, Sam Kellett and others

 For educational use by Department of Computer Science, City University London UK.

 This template contains a skybox, simple terrain, camera, lighting, shaders, texturing

 Potential ways to modify the code:  Add new geometry types, shaders, change the terrain, load new meshes, change the lighting,
 different camera controls, different shaders, etc.

 Template version 5.0a 29/01/2017
 Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk)

 version 6.0a 29/01/2019
 Dr Eddie Edwards (Philip.Edwards@city.ac.uk)
*/

#define _CRT_SECURE_NO_WARNINGS

#include "game.h"
#include <math.h>

// Setup includes
#include "HighResolutionTimer.h"
#include "GameWindow.h"

// Game includes
#include "Camera.h"
#include "Skybox.h"
#include "Plane.h"
#include "Shaders.h"
#include "FreeTypeFont.h"
#include "Sphere.h"
#include "MatrixStack.h"
#include "OpenAssetImportMesh.h"
#include "Audio.h"
#include "CatmullRom.h"
#include "PowerUpAsset.h"
#include "Player.h"
#include "Collectible.h"
#include "Obstacle.h"
#include "Border.h"
#include "FrameBufferObject.h"

// Constructor
Game::Game()
{

	m_pCamera = NULL;
	m_pShaderPrograms = NULL;
	player = NULL;
	m_pHighResolutionTimer = NULL;
	m_pAudio = NULL;
	mainTrack = NULL;
	introTrack = NULL;
	m_pPlayerMesh = NULL;
	m_pCollectibleMesh = NULL;
	m_pObstacleMesh = NULL;
	m_pBorderMesh_L = NULL;
	m_pBorderMesh_R = NULL;
	introTextFile = NULL;

	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
	m_currentDistance = 0.0f;
	minimumCollisionDistance = 6.0f;

	countDown = 3;
	countDownTimer = 0.f;
	countDownDelay = 1.0f;

	currentTimeOfDay = 0;
	swapSkyDelay = 1.0f;
	swapSkyTimer = swapSkyDelay;

	countDownInitiated = false;

	fontDelay = 0.3f;
	fontTimer = fontDelay;

	srand(time(NULL));

}

// Destructor
Game::~Game()
{
	//game objects
	delete m_pCamera;
	for (size_t i = 0; i < Skyboxes.size(); i++)
		delete Skyboxes[i];
	for (size_t i = 0; i < m_pFonts.size(); i++)
		delete m_pFonts[i];
	for (size_t i = 0; i < ObstacleObjs.size(); i++)
		delete ObstacleObjs[i];
	delete m_pPlayerMesh;
	delete m_pCollectibleMesh;
	delete player;
	delete m_pAudio;
	delete mainTrack;
	delete introTrack;
	for (size_t i = 0; i < powerUps.size(); i++)
		delete powerUps[i];
	delete m_pBorderMesh_L;
	delete m_pBorderMesh_R;

	if (m_pShaderPrograms != NULL) {
		for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			delete (*m_pShaderPrograms)[i];
	}
	delete m_pShaderPrograms;

	//setup objects
	delete m_pHighResolutionTimer;
}

// Initialisation:  This method only runs once at startup
void Game::Initialise()
{
	// Set the clear colour and depth
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	// Create objects

	m_pCamera = new CCamera;
	// Pushing in 2 for day and night skyboxes
	for (size_t i = 0; i < 2; i++)
		Skyboxes.push_back(new CSkybox);

	m_pShaderPrograms = new vector <CShaderProgram *>;

	for (size_t i = 0; i < fontNames.size(); i++)
	{
		m_pFonts.push_back(new CFreeTypeFont);
	}

	player = new Player;
	m_pAudio = new CAudio;
	mainTrack = new CCatmullRom;
	introTrack = new CCatmullRom;

	m_pPlayerMesh = new COpenAssetImportMesh();
	m_pBorderMesh_L = new COpenAssetImportMesh();
	m_pBorderMesh_R = new COpenAssetImportMesh();
	m_pCollectibleMesh = new COpenAssetImportMesh();
	m_pObstacleMesh = new COpenAssetImportMesh();

	RECT dimensions = m_gameWindow.GetDimensions();

	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	// Set the orthographic and perspective projection matrices based on the image size
	m_pCamera->SetOrthographicProjectionMatrix(width, height);
	m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float)width / (float)height, 0.5f, 5000.0f);

	// Load shaders
	vector<CShader> shShaders;
	vector<string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");

	for (int i = 0; i < (int)sShaderFileNames.size(); i++) {
		string sExt = sShaderFileNames[i].substr((int)sShaderFileNames[i].size() - 4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\" + sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Create the main shader program
	CShaderProgram *pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	pMainProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMainProgram);

	// Create a shader program for fonts
	CShaderProgram *pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[2]);
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->LinkProgram();
	m_pShaderPrograms->push_back(pFontProgram);

	// Create the skybox
	// Day Skybox downloaded from http://www.humus.name/index.php?page=Cubemap&item=Langholmen
	// Night Skybox downloaded from http://www.humus.name/index.php?page=Cubemap&item=HornstullsStrand
	for (size_t i = 0; i < Skyboxes.size(); i++)
		Skyboxes[i]->Create(2500.0f, i);

	for (size_t i = 0; i < m_pFonts.size(); i++)
	{
		m_pFonts[i]->LoadFont("resources\\fonts\\" + fontNames[i], 32);
		m_pFonts[i]->SetShaderProgram(pFontProgram);
	}

	glEnable(GL_CULL_FACE);

	// Create the track
	// 0 to indicate that it's the mainTrack
	mainTrack->CreateCentreline(0);
	mainTrack->CreateOffsetCurves();

	// Track texture downloaded from https://www.shutterstock.com/video/clip-25857725-road-highway-seamless-texture-slow-motion-4k
	mainTrack->CreateTrack("resources\\textures\\road_3_lane_straight.jpg");

	// 1 to indicate that it's the intro track
	introTrack->CreateCentreline(1);

	// Opening the intro text file
	fopen_s(&introTextFile, "resources\\scripts\\introTexts.txt", "r");

	// i to loop through each text
	// j to place the texts at regular intervals
	for (size_t i = 0, j = 80; i < 4; i++, j += 80)
	{

		introTexts.push_back(new Text2Point5);

		// Read every character until it reaches \t character
		fscanf(introTextFile, "%[^\t]", &introTexts[i]->text);

		// Reads the size and each RGBA value
		fscanf(introTextFile, "%d %f %f %f %f", &introTexts[i]->size, &introTexts[i]->colour.r,
			&introTexts[i]->colour.g, &introTexts[i]->colour.b, &introTexts[i]->colour.a);

		// Setting the position at the interval
		introTexts[i]->position = introTrack->GetCentreLinePoints()[j];

		// We're obtaining the local Z axis to
		// push the texts towards either sides
		// of the intro track
		T = glm::normalize(introTrack->GetCentreLinePoints()[j] - introTrack->GetCentreLinePoints()[j + 1]);
		N = glm::normalize(glm::cross(T, glm::vec3(0.0f, 1.0f, 0.0f)));

		// Texts at even places get pushed to the right
		if (i % 2 == 0)
			introTexts[i]->position += (7.0f * N);
		// Texts at odd places get pushed to the left
		else
			introTexts[i]->position -= (7.0f * N);
	}

	fclose(introTextFile);

	// Setting the initial position of the player
	// and countdown to the first point of the track
	player->SetPos(mainTrack->GetCentreLinePoints()[0]);

	countdownPos = mainTrack->GetCentreLinePoints()[0];

	// To make it appear above the player object
	countdownPos.y += 5.0f;

	// Both left and right border objects use the same
	// download from https://www.turbosquid.com/3d-models/fence-wood-model-1199737
	m_pBorderMesh_L->Load("resources\\models\\Fence\\Fence_L.obj");
	m_pBorderMesh_R->Load("resources\\models\\Fence\\Fence_R.obj");

	// LeftEdgePoints and RightEdgePoints have the same
	// size use so we can use any one
	// We're placing the object at intervals of 10 indices
	for (int i = 0, j = 0;
		i < mainTrack->GetLeftEdgePoints().size();
		i += 10, j += 2)
		// Increasing j by 2 to place and left and right at
		// the same index of the track
	{
		// Set the position and rotate it
		// along the track
		BorderObjs.push_back(new Border);
		BorderObjs[j]->SetPos(mainTrack->GetLeftEdgePoints()[i]);
		BorderObjs[j]->SetRotation(mainTrack->GetRotationMatrix(BorderObjs[j]->GetPos(), 0)); // 0 for LeftEdge enum


		BorderObjs.push_back(new Border);
		BorderObjs[j + 1]->SetPos((mainTrack->GetRightEdgePoints()[i]));
		BorderObjs[j + 1]->SetRotation(mainTrack->GetRotationMatrix(BorderObjs[j + 1]->GetPos(), 4)); // 4 for RightEdge enum
	}

	// START OF SCRIPT READING

	// Number of obstacles, collectibles and power ups
	int count;

	// Downloaded from https://www.turbosquid.com/3d-models/free-road-barrier-3d-model/621829
	m_pObstacleMesh->Load("resources\\models\\Barrier\\Barrier.obj");

	fopen_s(&obstacleInfoFile, "resources\\scripts\\obstaclesInfo.txt", "r");

	// Obtain the number
	fscanf(obstacleInfoFile, "%d", &count);

	for (size_t i = 0; i < count; i++)
	{
		// We get the lane it should be placed at
		int lane = 0;

		// Position in the track
		int index = 0;

		// Whether it should be placed in the air or not
		int inAir = 0;

		// The type of obstacle
		int type = 0;

		type = rand() % 3;

		fscanf(obstacleInfoFile, "%d %d %d", &lane, &index, &inAir);

		ObstacleObjs.push_back(new Obstacle(type));

		// Set the position and rotate it along the track
		ObstacleObjs[i]->SetPos(mainTrack->GetPoint(lane, index, inAir));
		ObstacleObjs[i]->SetRotation(mainTrack->GetRotationMatrix(ObstacleObjs[i]->GetPos(), lane));
	}

	fclose(obstacleInfoFile);


	// Downloaded from https://www.turbosquid.com/3d-models/114-wrench-model-1259302
	m_pCollectibleMesh->Load("resources\\models\\Wrench\\PB114_wrench_Low.obj");

	fopen_s(&collectibleInfoFile, "resources\\scripts\\collectiblesInfo.txt", "r");

	// Scanning the script the same way as obstacles

	fscanf(collectibleInfoFile, "%d", &count);

	for (size_t i = 0; i < count; i++)
	{

		int lane = 0;
		int index = 0;
		int inAir = 0;
		int type = 0;

		// Get a random type
		type = rand() % 3;

		fscanf(collectibleInfoFile, "%d %d %d", &lane, &index, &inAir);

		CollectibleObjs.push_back(new Collectible(type));

		CollectibleObjs[i]->SetPos(mainTrack->GetPoint(lane, index, inAir));
		CollectibleObjs[i]->SetAxis(mainTrack->GetNormal(CollectibleObjs[i]->GetPos(), lane));

	}

	fclose(collectibleInfoFile);

	fopen_s(&powerUpInfoFile, "resources\\scripts\\powerUpsInfo.txt", "r");

	fscanf(powerUpInfoFile, "%d", &count);

	for (int i = 0; i < count; i++)
	{
		int lane = 0;
		int index = 0;
		int inAir = 0;
		int type = 0;

		fscanf(powerUpInfoFile, "%d %d %d %d", &lane, &index, &inAir, &type);

		powerUps.push_back(new PowerUpAsset(type));

		powerUps[i]->SetPos(mainTrack->GetPoint(lane, index, inAir));
		powerUps[i]->SetAxis(mainTrack->GetNormal(powerUps[i]->GetPos(), lane));

	}

	fclose(powerUpInfoFile);

	// END OF SCRIPT READING

	// Downloaded from https://www.turbosquid.com/FullPreview/Index.cfm/ID/1055728 on 1 Mar 2019
	m_pPlayerMesh->Load("resources\\models\\Car\\rio.obj");

	// Initialise audio and play background music
	m_pAudio->Initialise();
	m_pAudio->LoadEventSound("resources\\Audio\\Boing.wav"); // Royalty free sound from freesound.org
	m_pAudio->LoadMusicStream("resources\\Audio\\SummerTown.mp3");	// Free Urban Music from https://opengameart.org/content/summertown
	m_pAudio->PlayMusicStream();

	// Orange Colour
	spotLightColours.push_back(glm::vec3(1.0f, 0.5f, 0.0f));

	// Azure Colour
	spotLightColours.push_back(glm::vec3(0.0f, 0.5f, 1.0f));

	blinkTimer = 0.0f;
	blinkDelay = 0.05f;

}

// Render method runs repeatedly in a loop
void Game::Render()
{

	// Clear the buffers and enable depth testing (z-buffering)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

	CShaderProgram *pMainProgram;

	// Use the main shader program 
	pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("bUseMultiTexture", false);
	pMainProgram->SetUniform("sampler0", 0);
	pMainProgram->SetUniform("sampler1", 1);


	// Note: cubemap and non-cubemap textures should not be mixed in the same texture unit.  Setting unit 10 to be a cubemap texture.
	int cubeMapTextureUnit = 10;
	pMainProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);


	// Set the projection matrix
	pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());

	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = m_pCamera->ComputeNormalMatrix(viewMatrix);

	// Setting light and materials in main shader program
	pMainProgram->SetUniform("isNight", currentTimeOfDay);

	// 1 for Night Time aka SpotLights
	if (currentTimeOfDay == 1)
	{

		for (size_t i = 0; i < 2; i++)
		{
			// Set the light to the player's position
			glm::vec3 lightPos = player->GetPos();

			// Right headlight
			if (i % 2 == 0)
				lightPos += (3.0f * N);
			// Left Headlight
			else
				lightPos -= (3.0f * N);

			// A point 30 units from the current point
			glm::vec3 targetPoint = lightPos + (30.0f * T);

			// Pushing the light slightly upwards
			lightPos += (2.0f * B);

			lightPos += (5.0f * T);

			// Get the vector between the target point and the position
			glm::vec3 dir = glm::normalize(targetPoint - lightPos);


			glm::vec4 lightPosition1 = glm::vec4(lightPos, 1.0f);	// Position of light source *in world coordinates*
			pMainProgram->SetUniform("light[" + std::to_string(i) + "].position", viewMatrix*lightPosition1); // Position of light source *in eye coordinates*
			pMainProgram->SetUniform("light[" + std::to_string(i) + "].La", glm::vec3(0.2f));		// Ambient colour of light
			pMainProgram->SetUniform("light[" + std::to_string(i) + "].Ld", spotLightColours[i]);		// Diffuse colour of light
			pMainProgram->SetUniform("light[" + std::to_string(i) + "].Ls", spotLightColours[i]);		// Specular colour of light
			pMainProgram->SetUniform("light[" + std::to_string(i) + "].direction", glm::normalize(viewNormalMatrix*dir)); // Direction vector of light
			pMainProgram->SetUniform("light[" + std::to_string(i) + "].exponent", 1.0f); // Strength of light (Small Value -> High Strength)
			pMainProgram->SetUniform("light[" + std::to_string(i) + "].cutoff", 15.0f); // Half angle of the cone
			pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));	// Ambient material reflectance
			pMainProgram->SetUniform("material1.Md", glm::vec3(0.0f));	// Diffuse material reflectance
			pMainProgram->SetUniform("material1.Ms", glm::vec3(0.0f));	// Specular material reflectance
			pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property
		}

	}
	// Day Time aka Directional Light representing Sunlight
	else
	{
		glm::vec4 lightPosition1 = glm::vec4(-1000, 1000, 1000, 1); // Position of light source *in world coordinates*
		pMainProgram->SetUniform("light1.position", viewMatrix*lightPosition1); // Position of light source *in eye coordinates*
		pMainProgram->SetUniform("light1.La", glm::vec3(1.0f));		// Ambient colour of light
		pMainProgram->SetUniform("light1.Ld", glm::vec3(1.0f, 1.0f, 1.0f));		// Diffuse colour of light
		pMainProgram->SetUniform("light1.Ls", glm::vec3(1.0f, 1.0f, 1.0f));		// Specular colour of light
		pMainProgram->SetUniform("light1.direction", glm::normalize(viewNormalMatrix*glm::vec3(1, -1, -1))); // Direction of sunlight
		pMainProgram->SetUniform("material1.Ma", glm::vec3(1.0f));	// Ambient material reflectance
		pMainProgram->SetUniform("material1.Md", glm::vec3(0.0f));	// Diffuse material reflectance
		pMainProgram->SetUniform("material1.Ms", glm::vec3(0.0f));	// Specular material reflectance
		pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property
	}

	// Render the skybox and terrain with full ambient reflectance 
	modelViewMatrixStack.Push();
	{
		pMainProgram->SetUniform("renderSkybox", true);
		// Translate the modelview matrix to the camera eye point so skybox stays centred around camera
		glm::vec3 vEye = m_pCamera->GetPosition();
		modelViewMatrixStack.Translate(vEye);
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		// Rendering the appropriate skybox
		Skyboxes[currentTimeOfDay]->Render(cubeMapTextureUnit);
		pMainProgram->SetUniform("renderSkybox", false);
	}
	modelViewMatrixStack.Pop();


	// Turn on diffuse + specular materials
	pMainProgram->SetUniform("material1.Ma", glm::vec3(0.5f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(1.0f));	// Specular material reflectance	

	// Render the player car
	modelViewMatrixStack.Push();
	{
		// If the player has collided with an obstacle
		// then switch the render state after an interval
		if (player->GetCollidableState() == false)
		{
			blinkTimer += 0.01f;

			if (blinkTimer >= blinkDelay)
			{
				canRender = !canRender;
				blinkTimer = 0.0f;
			}

		}

		// Translate, Rotate and then Scale
		modelViewMatrixStack.Translate(player->GetPos());
		modelViewMatrixStack *= player->GetRotation();
		modelViewMatrixStack.Scale(3.f);

		// We use the toon shader when the player is invulnerbale
		pMainProgram->SetUniform("useToon", player->GetInvulnerabilityState());
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));

		if (canRender)
			m_pPlayerMesh->Render();

		// Reverting values
		pMainProgram->SetUniform("useToon", false);

	}
	modelViewMatrixStack.Pop();

	// To reset values for next collision event
	if (canRender == false && player->GetCollidableState() == true)
	{
		canRender = true;
		blinkTimer = blinkDelay;
	}

	// Render the track
	modelViewMatrixStack.Push();
	{
		pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
		pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
		mainTrack->RenderTrack();
	}
	modelViewMatrixStack.Pop();


	// Render the powerUps
	for (size_t i = 0; i < powerUps.size(); i++)
	{
		// Render them if they haven't been picked up
		if (powerUps[i]->GetRenderState() == true)
		{
			modelViewMatrixStack.Push();
			{
				// Translate, Rotate and Scale
				modelViewMatrixStack.Translate(powerUps[i]->GetPos());

				// Rotate the collectible along its own axis
				modelViewMatrixStack.Rotate(powerUps[i]->GetAxis(), powerUps[i]->GetRotationDegrees());
				modelViewMatrixStack.Scale(2.f);


				pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
				pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
				powerUps[i]->Render();
			}
			modelViewMatrixStack.Pop();
		}

	}

	// Render the obstacles
	for (size_t i = 0; i < ObstacleObjs.size(); i++)
	{

		modelViewMatrixStack.Push();
		{
			modelViewMatrixStack.Translate(ObstacleObjs[i]->GetPos());
			modelViewMatrixStack *= ObstacleObjs[i]->GetRotation();
			modelViewMatrixStack.Scale(0.75);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));

			// Using multi texture to visualise the type of obstacle
			pMainProgram->SetUniform("bUseMultiTexture", true);
			ObstacleObjs[i]->GetTexture().Bind(1);
			m_pObstacleMesh->Render();

			// Reverting values
			pMainProgram->SetUniform("bUseMultiTexture", false);
		}
		modelViewMatrixStack.Pop();

	}

	// Render the collectibles
	for (size_t i = 0; i < CollectibleObjs.size(); i++)
	{
		if (CollectibleObjs[i]->GetRenderState() == true)
		{
			modelViewMatrixStack.Push();
			{
				modelViewMatrixStack.Translate(CollectibleObjs[i]->GetPos());
				modelViewMatrixStack.Rotate(CollectibleObjs[i]->GetAxis(), CollectibleObjs[i]->GetRotationDegrees());
				modelViewMatrixStack.Scale(0.5f);

				pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
				pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));

				// Using multi texture for this as well 
				// to determine the type of collectible
				pMainProgram->SetUniform("bUseMultiTexture", true);
				CollectibleObjs[i]->GetTexture().Bind(1);
				m_pCollectibleMesh->Render();

				// Reverting values
				pMainProgram->SetUniform("bUseMultiTexture", false);
			}
			modelViewMatrixStack.Pop();

		}

	}

	// Render Border Fences
	for (int i = 0; i < BorderObjs.size(); i++)
	{
		modelViewMatrixStack.Push();
		{
			modelViewMatrixStack.Translate(BorderObjs[i]->GetPos());
			modelViewMatrixStack *= BorderObjs[i]->GetRotation();
			modelViewMatrixStack.Scale(0.5);
			pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));

			// The left border meshes are located 
			// at even places in the vector
			if (i % 2 == 0)
				m_pBorderMesh_L->Render();
			else
				m_pBorderMesh_R->Render();

		}
		modelViewMatrixStack.Pop();
	}

	// Draw the 2D graphics after the 3D graphics
	DisplayHUD();

	// Display the countdown text only when it's
	// initiated and not zero
	if (countDown != 0 && countDownInitiated == true)
		DisplayCountdown(modelViewMatrixStack.Top());

	// Display the intro texts before the countdown is initiated
	if (countDownInitiated == false)
		DisplayIntro(modelViewMatrixStack.Top());

	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());

}

// Update method runs repeatedly with the Render method
void Game::Update()
{
	// Increase the timer only when
	// the font or skybox is switched

	if (fontTimer < fontDelay)
	{
		fontTimer += 0.01f;
	}

	if (swapSkyTimer < swapSkyDelay)
	{
		swapSkyTimer += 0.01f;
	}

	if (displayResults == false)
	{

		// A zero vector to pass it to track sample method
		// because we don't use up vectors
		glm::vec3 zero = glm::vec3(0.0f);

		// Don't update the game when we're in countDown
		// and the camera is in free view
		if (m_pCamera->IsCameraInFreeMode() == false &&
			countDown == 0
			)
		{
			// Increasing the distance to travel
			// along the track by a value
			m_currentDistance += (m_dt * 0.06f);

			if (mainTrack->CurrentLap(m_currentDistance) == 1)
				displayResults = true;

			// Sample the current point and the next point
			mainTrack->Sample(m_currentDistance, p, zero);
			mainTrack->Sample(m_currentDistance + 1.0f, pNext, zero);

			glm::vec3 y = glm::vec3(0, 1, 0);

			// Obtain the TNB vectors for rotating
			// the camera and the player
			T = glm::normalize(pNext - p);
			N = glm::normalize(glm::cross(T, y));
			B = glm::normalize(glm::cross(N, T));

			// Updating the player
			player->Update(p, T, N, B, mainTrack->GetTrackHalfWidth(), mainTrack->GetOffset(), m_dt);

			// Updating the player's power up 
			// when it's active
			player->PowerUpUpdate();

			// Update the camera
			m_pCamera->TrackUpdate(T, N, B, p, player->GetPos(), float(m_dt));

			// Collectible Update
			for (size_t i = 0; i < CollectibleObjs.size(); i++)
			{
				// Don't bother updating if it's already picked up by the player
				if (CollectibleObjs[i]->GetRenderState() == true)
				{
					float deg = 0.1f;

					CollectibleObjs[i]->SetRotation(CollectibleObjs[i]->GetRotationDegrees() + deg);

					// If the distance between the player and
					// the collectible is less than the minimum distance,
					// then increase the points and don't let it appear on screen
					if (glm::distance(CollectibleObjs[i]->GetPos(), player->GetPos()) <= minimumCollisionDistance)
					{
						// Increase the points by multiplying the collectible's
						// points value with the multiplier
						player->SetPoints(player->GetPoints() +
							(CollectibleObjs[i]->GetPoints() * player->GetPointsMultiplier()));
						CollectibleObjs[i]->SetRenderState(false);
					}

				}

			}

			// Barrier Update
			if (player->GetCollidableState() == true && player->GetInvulnerabilityState() == false)
			{
				for (size_t i = 0; i < ObstacleObjs.size(); i++)
				{
					// Same logic as the collectibles but 
					// decrease the points on collision
					if (glm::distance(ObstacleObjs[i]->GetPos(), player->GetPos()) <= minimumCollisionDistance)
					{
						player->SetPoints(player->GetPoints() - ObstacleObjs[i]->GetDamage());
						player->SetCollidableState(false);
						break;
					}

				}
			}


			// Power Up Update
			for (size_t i = 0; i < powerUps.size(); i++)
			{
				if (powerUps[i]->GetRenderState() == true)
				{
					// Update the power ups to rotate it
					powerUps[i]->Update();

					if (player->GetPowerActiveState() == false)
					{
						// Same distance check to activate the power up
						if (glm::distance(powerUps[i]->GetPos(), player->GetPos()) <= minimumCollisionDistance)
						{
							player->SetPowerUpInfo(powerUps[i]->GetPowerUp());
							powerUps[i]->SetRenderState(false);
							break;
						}

					}
				}

			}




		}
		else if (countDownInitiated == false)
		{
			// We're gonna move along the intro track the
			// same way as the main track

			m_currentDistance += (m_dt * 0.075f);

			introTrack->Sample(m_currentDistance, p, zero);
			introTrack->Sample(m_currentDistance + 1.0f, pNext, zero);

			glm::vec3 y = glm::vec3(0, 1, 0);

			T = glm::normalize(pNext - p);
			N = glm::normalize(glm::cross(T, y));
			B = glm::normalize(glm::cross(N, T));

			m_pCamera->Set(p, p + 10.0f * T, y);

			// If we reach the end of the control line path, initiate the countdown
			if (glm::distance(p, introTrack->FinalControlLinePoint) <= 50.0f)
			{
				countDownInitiated = true;
				m_currentDistance = 0.0f;
			}
			// If we reach close to the rendered 2.5D text, then render the next one
			else if (glm::distance(p, introTrack->GetCentreLinePoints()[introTrack->GetCentreLinePoints().size() - 2]) <= 10.0f)
			{
				currentIntroText++;
				currentIntroText = currentIntroText % introTexts.size();
			}

		}
		// Get the camera to initial position of the main track
		else if (countDown != 0 && countDownInitiated == true)
		{
			mainTrack->Sample(m_currentDistance, p, zero);
			mainTrack->Sample(m_currentDistance + 1.0f, pNext, zero);

			glm::vec3 y = glm::vec3(0, 1, 0);

			// Used by the countdown to move along these vectors
			T = glm::normalize(pNext - p);
			N = glm::normalize(glm::cross(T, y));
			B = glm::normalize(glm::cross(N, T));

			// Update the camera using the TNB frame
			// and the player position
			m_pCamera->TrackUpdate(T, N, B, p, player->GetPos(), float(m_dt));

			// We're gonna move the countdown number
			// along the above mentioned vectors
			AnimateCountdown();

			countDownTimer += 0.02f;

			// If the timer is up,
			// then reduce the countdown number
			if (countDownTimer >= countDownDelay)
			{
				countDown--;

				// and place it above the player object
				countdownPos = p + (5.0f * B);
				countDownTimer = 0.0f;
			}
		}
		else
		{
			m_pCamera->TrackUpdate(T, N, B, p, player->GetPos(), m_dt);
		}

	}

	m_pAudio->Update();

}


// Display 2D UI Text
void Game::DisplayHUD()
{
	// Using the text shader program
	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	// Getting the height of the game window
	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;

	// Increase the elapsed time and frame counter
	m_elapsedTime += m_dt;
	m_frameCount++;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
	{
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// Reset the frames per second
		m_frameCount = 0;
	}

	if (m_framesPerSecond > 0) {
		// Use the font shader program and render the text
		fontProgram->UseProgram();
		glDisable(GL_DEPTH_TEST);

		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());

		// Rendering the FPS in white colour
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		m_pFonts[currentFont]->Render(20, height - 20, 20, "FPS: %d", m_framesPerSecond);

	}



	if (displayResults == false)
	{

		// Rendering the points value at Violet colour
		fontProgram->SetUniform("vColour", glm::vec4(0.5f, 0.0f, 1.0f, 1.0f));

		if (player->GetPointsMultiplier() != 1)
			m_pFonts[currentFont]->Render(((int)m_gameWindow.SCREEN_WIDTH) / 2 - 60, height - 50, 30, "Points (2x): %d", player->GetPoints());
		else
			m_pFonts[currentFont]->Render(((int)m_gameWindow.SCREEN_WIDTH) / 2 - 60, height - 50, 30, "Points: %d", player->GetPoints());
	}
	else
	{
		// Rendering the points value at White colour
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

		m_pFonts[currentFont]->Render(((int)m_gameWindow.SCREEN_WIDTH) / 2 - 150, ((int)m_gameWindow.SCREEN_HEIGHT) / 2, 50, "You earned %d Points!", player->GetPoints());
		m_pFonts[currentFont]->Render(((int)m_gameWindow.SCREEN_WIDTH) / 2 - 150, ((int)m_gameWindow.SCREEN_HEIGHT) / 2 - 50, 50, "Thanks for Playing!");
	}


	// The player turns cartoony when it's invulnerable 
	// but there is text as well rendering at Rose colour
	if (player->GetInvulnerabilityState() == true)
	{
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 0.0f, 0.5f, 1.0f));
		m_pFonts[currentFont]->Render(((int)m_gameWindow.SCREEN_WIDTH) / 2 - 60, height - 80, 30, "INVULNERABLE!", player->GetPoints());
	}

}

// Rendering the 2.5D text
// at the requested position with
// font style, the text, size and its colour
void Game::Display2Point5Text(glm::mat4 topMatrix, glm::vec3 position,
	CFreeTypeFont* font, char text[256], int size, glm::vec4 colour)
{
	GLint viewport[4];
	glm::vec3 q = position;

	// Getting the viewport
	glGetIntegerv(GL_VIEWPORT, viewport);
	glm::vec4 vp(viewport[0], viewport[1], viewport[2], viewport[3]);

	// Projecting the text towards the camera 
	// i.e the text rotates if the camera goes around the text
	glm::vec3 p = glm::project(q, topMatrix, *(m_pCamera->GetPerspectiveProjectionMatrix()), vp);

	GLfloat depth;
	glReadPixels((int)p.x, (int)p.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
	float offset = 0.001f;

	if (p.z - offset < depth)
	{
		CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];
		fontProgram->UseProgram();
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());

		// Rendering the text at black colour at 3 units
		// to the left and 3 units to the top to provide
		// a shadow effect
		fontProgram->SetUniform("vColour", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		font->Render(p.x - 3, p.y + 3, size, text);

		fontProgram->SetUniform("vColour", colour);
		font->Render(p.x, p.y, size, text);

	}
}

void Game::DisplayCountdown(glm::mat4 topMatrix)
{
	// Converting the integer to a string
	char countDownText[256];
	string str = std::to_string(countDown);

	// Converting the string to a c_string of 256 characters
	// We're adding 1 to include the null character as well
	for (int i = 0; i < str.size() + 1; i++)
	{
		countDownText[i] = str[i];
	}

	Display2Point5Text(topMatrix, countdownPos, m_pFonts[countDown - 1], countDownText, 100, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

}

void Game::DisplayIntro(glm::mat4 topMatrix)
{
	for (size_t i = 0; i < introTexts.size(); i++)
	{
		// Render the intro texts one at a time
		if (i == currentIntroText)
		{
			Display2Point5Text(topMatrix, introTexts[i]->position, m_pFonts[i % m_pFonts.size()],
				introTexts[i]->text, introTexts[i]->size, introTexts[i]->colour);
		}
	}

}

void Game::AnimateCountdown()
{
	switch (countDown)
	{
	case 3:
		// Move along the local +Y and -X axes
		countdownPos += (0.1f * B);
		countdownPos -= (0.2f * T);
		break;
	case 2:
		// Move along the local -Z and -X axes
		countdownPos -= (0.2f * N);
		countdownPos -= (0.2f * T);
		break;
	case 1:
		// Move along the local +Z and -X axes
		countdownPos += (0.2f * N);
		countdownPos -= (0.2f * T);
		break;
	}

}


// The game loop runs repeatedly until game over
void Game::GameLoop()
{

	// Fixed timer
	m_dt = m_pHighResolutionTimer->Elapsed();
	if (m_dt > 1000.0 / (double) Game::FPS) {
		m_pHighResolutionTimer->Start();
		Update();
		Render();
	}



	// Variable timer
	/*m_pHighResolutionTimer->Start();
	Update();
	Render();
	m_dt = m_pHighResolutionTimer->Elapsed();*/


}


WPARAM Game::Execute()
{
	m_pHighResolutionTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if (!m_gameWindow.Hdc()) {
		return 1;
	}

	Initialise();

	m_pHighResolutionTimer->Start();


	MSG msg;

	while (1) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if (m_appActive) {
			GameLoop();
		}
		else Sleep(200); // Do not consume processor power if application isn't active
	}

	m_gameWindow.Deinit();

	return(msg.wParam);
}

LRESULT Game::ProcessEvents(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	LRESULT result = 0;

	switch (message) {


	case WM_ACTIVATE:
	{
		switch (LOWORD(w_param))
		{
		case WA_ACTIVE:
		case WA_CLICKACTIVE:
			m_appActive = true;
			m_pHighResolutionTimer->Start();
			break;
		case WA_INACTIVE:
			m_appActive = false;
			break;
		}
		break;
	}

	case WM_SIZE:
		RECT dimensions;
		GetClientRect(window, &dimensions);
		m_gameWindow.SetDimensions(dimensions);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;

	case WM_KEYDOWN:
		switch (w_param)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case '1':
			m_pAudio->PlayEventSound();
			break;
		case VK_F1:
			m_pAudio->PlayEventSound();
			break;
			// Press Q to change the font style
		case 'Q':
			if (fontTimer >= fontDelay)
			{
				currentFont++;
				// And making sure we don't beyond the range
				currentFont = currentFont % m_pFonts.size();
				fontTimer = 0.0f;
			}
			break;
			// Press N to change between day and night
		case 'T':
			if (swapSkyTimer >= swapSkyDelay)
			{
				currentTimeOfDay++;
				// And making sure we don't beyond the range
				currentTimeOfDay = currentTimeOfDay % Skyboxes.size();
				swapSkyTimer = 0.0f;
			}

		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}

	return result;
}

Game& Game::GetInstance()
{
	static Game instance;

	return instance;
}

void Game::SetHinstance(HINSTANCE hinstance)
{
	m_hInstance = hinstance;
}

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int)
{
	Game &game = Game::GetInstance();
	game.SetHinstance(hinstance);

	return game.Execute();
}
