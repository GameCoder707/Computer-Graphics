#pragma once

#include "Common.h"
#include "GameWindow.h"

// Classes used in game.  For a new class, declare it here and provide a pointer to an object of this class below.  Then, in Game.cpp, 
// include the header.  In the Game constructor, set the pointer to NULL and in Game::Initialise, create a new object.  Don't forget to 
// delete the object in the destructor.   
class CCamera;
class CSkybox;
class CShader;
class CShaderProgram;
class CPlane;
class CFreeTypeFont;
class CHighResolutionTimer;
class CSphere;
class COpenAssetImportMesh;
class CAudio;
class CCatmullRom;
class PowerUpAsset;
class Player;
class Collectible;
class Obstacle;
class Border;
class CFrameBufferObject;

class Game 
{
	// A structure for 2.5D texts
	struct Text2Point5
	{
		// Position of the text in world space
		glm::vec3 position;

		// A c string of 256 characters
		char text[256];

		// Size of the text;
		int size;

		// Colour of the text
		glm::vec4 colour;

	};

private:

	// Three main methods used in the game.  Initialise runs once, while Update and Render run repeatedly in the game loop.
	void Initialise();
	void Update();
	void Render();

	// Used to render twice using FBO
	void RenderScene(int pass);

	// Pointers to game objects.  They will get allocated in Game::Initialise()
	vector<CSkybox*> Skyboxes;

	// GAME UPDATE ESSENTIALS

	// The current point of the track
	glm::vec3 p;

	// The next point in the track
	glm::vec3 pNext;

	// Tangent Vector between 
	// pNext and p (local X axis)
	glm::vec3 T;

	// The normal vector (local Z axis)
	glm::vec3 N;

	// The binormal vector (local Y axis)
	glm::vec3 B;

	// To indicate day or night game mode
	int currentTimeOfDay;

	// SHADER RELATED VARIABLES
	// Used specifically for the player object
	// when collided with an obstacle
	bool canRender;
	float blinkTimer;
	float blinkDelay;

	// END GAME
	// Whether to display the
	// total points earned at the end
	// of the level
	bool displayResults;

	// FONT SWITCH VARIABLES
	// A timer and delay to prevent from
	// continuously switching
	float fontTimer;
	float fontDelay;

	// The font that's currently rendering
	int currentFont;

	// COUNTDOWN VARIABLES
	// World position and to
	// check if the countdown has
	// initiated
	glm::vec3 countdownPos;
	bool countDownInitiated;

	// INTRO TRACK RELATED VARIABLES
	// The current text that's rendering
	// and a variable to switch to the next one
	int currentIntroText;
	bool switchIntroText;

	// A timer to prevent
	// from continuously swapping
	// the skybox
	float swapSkyTimer;
	float swapSkyDelay;


	CCamera *m_pCamera;
	vector <CShaderProgram *> *m_pShaderPrograms;

	/******************************/

	// FOR INTRO TEXTS
	
	// Font Objects to render 2.5D texts
	vector<CFreeTypeFont*> m_pFonts;

	// Various font styles
	// Salsa font downloaded from https://www.1001freefonts.com/salsa.font
	// Lilly font downloaded from https://www.1001freefonts.com/lilly.font
	// Comic Neue Sans font downloaded from https://www.dafont.com/comicneuesansid.font
	vector<string> fontNames = { "Salsa.ttf", "Lilly.ttf", "ComicSans.ttf" };

	// Text values loaded from scripts
	vector<Text2Point5*> introTexts;

	/******************************/

	// Different colours for the headlights
	vector<glm::vec3> spotLightColours;

	// Text Files
	FILE* introTextFile;
	FILE* collectibleInfoFile;
	FILE* obstacleInfoFile;
	FILE* powerUpInfoFile;

	// Mesh for the Obstacles and a vector
	// of Obstacles objs with various damage values
	COpenAssetImportMesh* m_pObstacleMesh;
	vector <Obstacle*> ObstacleObjs;

	// Mesh for the Collectibles and a vector
	// of Collectibles with various points values
	COpenAssetImportMesh* m_pCollectibleMesh;
	vector <Collectible*> CollectibleObjs;

	// Vector of powerUps
	// Rendered using their own primitive assets
	vector<PowerUpAsset*> powerUps;
	
	// Using 2 different Border Meshes for
	// both edges to look good
	// And a vector of Border Objs
	COpenAssetImportMesh* m_pBorderMesh_L;
	COpenAssetImportMesh* m_pBorderMesh_R;
	vector<Border*> BorderObjs;

	// Mesh for the Player car and
	// a player object
	COpenAssetImportMesh *m_pPlayerMesh;
	Player* player;

	// Timer to calculate Frames Per Second (FPS)
	CHighResolutionTimer *m_pHighResolutionTimer;

	// Audio Object
	CAudio *m_pAudio;

	// Two tracks are created for the intro
	// and the main road track for the main gameplay
	CCatmullRom *mainTrack;
	CCatmullRom *introTrack;

	// Some other member variables
	double m_dt;
	int m_framesPerSecond;
	bool m_appActive;
	float m_currentDistance;

	float minimumCollisionDistance;

	// An initial countdown to start the gameplay
	int countDown;

	// A delay and timer to decrease the countdown
	float countDownTimer;
	float countDownDelay;


public:
	Game();
	~Game();
	static Game& GetInstance();
	LRESULT ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param);
	void SetHinstance(HINSTANCE hinstance);
	WPARAM Execute();

private:
	static const int FPS = 60;
	void Display2Point5Text(glm::mat4 topMatrix, glm::vec3 position, CFreeTypeFont* font, char text[256], int size, glm::vec4 colour);
	void DisplayHUD();
	void DisplayCountdown(glm::mat4 topMatrix);
	void DisplayIntro(glm::mat4 topMatrix);

	// A function to move the countdown
	// in various directions
	void AnimateCountdown();

	void GameLoop();
	GameWindow m_gameWindow;
	HINSTANCE m_hInstance;
	int m_frameCount;
	double m_elapsedTime;


};
