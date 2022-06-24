//include GLEW
#define GLEW_STATIC
#include <GL/glew.h>

# ifdef _MSC_VER
#  ifdef _DEBUG
#      pragma comment (lib, "glew32sd.lib")
#  else
#      pragma comment (lib, "glew32s.lib")
#  endif
# endif

#include <stdio.h> //for c usage debugging
#include <cstdlib> //for random
#include <iostream> //cout
#include <vector> //for storing obj vertex, normal and uv
#include <map> //for mtl file data
#include <string> //for general string usage
#include <windows.h> //for sleep
//#include <cmath>

//for imgui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

//include GLFW
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "LoadShaders.h"
#include "objloader.hpp"
#include "imgui/imgui.h"

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//include vgzmo
//#include "imgui/imGuIZMOquat.h"

//irrKlang, for music
#include "irrKlang.h"
#pragma comment(lib, "irrKlang.lib") //link with dll

using namespace std;
using namespace irrklang;

//rendering fps
#define TARGET_FPS 60


//degree and radiant conversion if needed
#define deg2rad(x) ((x)*((3.1415926f)/(180.0f)))
#define rad2deg(x) ((180.0f) / ((x)*(3.1415926f)))

//body parts number
#define EXTRAPARTS 2
#define PARTSNUM 19
#define PARTSTOTAL 19+2

//body parts indexing
#define HEAD 0
#define NECK 1
#define UPPER_BODY 2
#define ABS 3
#define CROTCH 4
#define LEFT_UPPER_THIGH 5
#define LEFT_LOWER_THIGH 6
#define LEFT_FOOT 7
#define LEFT_FOOT_TOES 8
#define RIGHT_UPPER_THIGH 9
#define RIGHT_LOWER_THIGH 10
#define RIGHT_FOOT 11
#define RIGHT_FOOT_TOES 12
#define RIGHT_UPPER_ARM 13
#define RIGHT_LOWER_ARM 14
#define RIGHT_PALM 15
#define LEFT_UPPER_ARM 16
#define LEFT_LOWER_ARM 17
#define LEFT_PALM 18

//for walking purposes
#define WALKSPEED 0.3
#define TRANSLATEXLIMIT 25.0 //preventing out of bound
#define TRANSLATEZLIMIT 25.0 //preventing out of bound
#define TRANSLATEYLOWERLIMIT 25.0 //for lighting
#define TRANSLATEYUPPERLIMIT 50.0 //for lighting

//for debugging
bool renderBodyTop = true;
bool renderArm = true;
bool renderHead = true;
bool renderLeg = true;

//background usage
bool useBackground = true;
glm::mat4 ModelBackground;
float backGroundShiftUp = 30.02f;
float robotShiftUp = 9.653 + 1.064; //height of from crotch to ankle + height of foot

//for imgui usage
int SCREENWIDTH; //obtained later during init
int SCREENHEIGHT; //obtained later during init
const char* glsl_version = "#version 130";
bool show_demo_window = true;
ImFont* font1 = NULL;
ImFont* titleFont = NULL;
ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
void makeCenter(std::string text) {
	float font_size = ImGui::GetFontSize() * text.size() / 2;
	ImGui::SameLine(
		ImGui::GetWindowSize().x / 2 -
		font_size + (font_size / 2)
	);

	ImGui::Text(text.c_str());
}

bool changePalm = false;
enum PALMMODE {
	OPEN, 
	CLENCH
};

bool modeChangeExist = false;
void resetGlobalVariables();

enum ANIMATEMODE {
	IDLE,
	WALK, 
	SQUAT, 
	JUMPINGJACK,
	GANGNAMSTYLE,
	MOONWALK,
	PUSHUP,
	SITUP
};


//for material
GLuint M_KaID;
GLuint M_KdID;
GLuint M_KsID;

void myUpdateModel();


//for using with GLFW
GLFWwindow* initProgramGLFW();
void resetModel();
void displayOnly(bool isShadow); //render scene only, no GUI
void displayGLFW(GLFWwindow* window); //displayGLFW()
void displayInitial(int currentIndices, GLFWwindow* window); //only for initial, displaying loading progress
bool loadingProgress[PARTSTOTAL+1]; //parts total is not including background, + 1 to include background
void KeyboardGLFW(GLFWwindow*, int key, int scancode, int action, int mods);
//for GLFW
void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}

void Obj2Buffer(GLFWwindow* window);
void load2Buffer( string obj,int);

//animation purposes
void walk();
void updateWalkTranslate();
void squat();
void jumpingJack();
void gangnamStyle();
void moonWalk();
void pushUp();
void sitUp();
float clip(float &var, float min, float max);

bool isFrame;
PALMMODE palmMode;
ANIMATEMODE animateMode;

GLuint VAO; //vertex attribute
GLuint VBO; //buffer for vertices
GLuint uVBO; //buffer for uv
GLuint nVBO; //buffer for normal
GLuint mVBO; //
GLuint UBO;
GLuint VBOs[PARTSTOTAL+1];
GLuint uVBOs[PARTSTOTAL+1];
GLuint nVBOs[PARTSTOTAL+1];
GLuint program;



//for viewing
float viewPos[3]; //eye Position
GLuint viewPosID;
bool viewChange = false;
float eyeAngley = 0.0;
float eyedistance = 20.0;
float FoV = 80; //in degree, between 30-90
float nearClip = 0.1;
float farClip = 200;


GLint MatricesIdx;
GLuint ModelID;
int vertices_size[PARTSTOTAL+1];
int uvs_size[PARTSTOTAL+1];
int normals_size[PARTSTOTAL+1];
int materialCount[PARTSTOTAL+1];

//for shadow and lighting
void displayLightSource(glm::mat4,glm::mat4);
GLuint lightVAO; //create a different VAO for lighting, so it won't get mix up with object VAO, for easier debugging
bool renderLightBox = true;
bool lightingFollowRobocop = false;
bool discoticLighting = false;
const float lightScale = 1.f;
GLuint lightBoxBuffer;
GLuint lightBoxIndices;
GLuint ModelMatrixID;
GLuint ViewMatrixID;
GLuint ProjectionMatrixID;
const float lightModel[] = {
	//x-y-z element
	//0 = right up back
	//1 = left up back
	//2 = right up front
	//3 = left up front
	//4 = right bottom back
	//5 = left bottom back
	//6 = left bottom front
	//7 = right bottom front
	1.f,1.f,-1.f,
	-1.f,1.f,-1.f,
	1.f,1.f,1.f,
	-1.f,1.f,1.f,
	1.f,-1.f,-1.f,
	-1.f,-1.f,-1.f,
	-1.f,-1.f,1.f,
	1.f,-1.f,1.f
};
static int recTriangleStripOrder[] = {
	//https://stackoverflow.com/questions/28375338/cube-using-single-gl-triangle-strip
	//obtained from here
	3,2,6,7,4,2,0,3,1,6,5,4,1,0
};
float lightPosition[] = { 10, 25 ,10 };
GLuint renderLightProgram;
float ambientColor[4] = { 0.1,0.1,0.1, 1 };
float diffuseColor[4] = { 0.8,0.8,0.8, 1 };
float specularColor[4] = { 1,1,1,1 };
float Shininess = 32.0;
GLuint ambientID;
GLuint diffuseID;
GLuint  specularID;
GLuint ShininessID; 
float kConstant = 1.0f;
float kLinear = 0.09f;
float kQuadratic = 0.032f;
GLuint KcID;
GLuint KlID;
GLuint KqID;
float lightColor[3] = { 1,1,1 }; //for light box color
GLuint depthMapFBO; //for frame buffer
GLuint lightPosID;
const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
unsigned int depthMap; //for textures
float lightProjectX = 79; //set to variable for debugging
float lightProjectY = 75; //set to variable for debugging
float lightProjectZ = 0.4; //set to variable for debugging
float lightProjectZFar = 106.7; //set to variable for debugging
glm::mat4 lightSpace; //projection x view of light source, for vertex shader
glm::mat4 lightProjection = glm::ortho(-lightProjectX, lightProjectX, -lightProjectY, lightProjectY, lightProjectZ, lightProjectZFar);
GLuint depthProgram;
GLuint lightSpaceID; //projection x view of light source to depth shader
GLuint lightModelID; //model matrix for light box
GLuint lightSpaceOrigID; //for passing in phong vertex shader, same with lightSpaceID, just different program
bool firstTimeDiscoticLighting = true; //when false, discoticLighting already repeat
double lastDiscoticLighting; //to count discotic periodic
bool fromGangnamMoonwalk = false; //discotic disable

//for loading object
std::vector<std::string> mtls[PARTSTOTAL+1];//use material
std::vector<unsigned int> faces[PARTSTOTAL+1];//face count
map<string, glm::vec3> KAs;
map<string,glm::vec3> KDs;
map<string,glm::vec3> KSs;

//for rendering in every swap buffers
glm::mat4 Projection ;
glm::mat4 View;
glm::mat4 Models[PARTSNUM];

//for monitor, shift due to GUI window
int widthStart = 400;
int heightStart = 0;

float clampValMin(float x, float clampToMin);
float clampValMax(float x, float clampToMax);
float getTime();

////////////////////////tools for animation/////////////////////////
float startTime;
///////////
// For walk
bool r_isFr = false;

bool r_isUp = true; // upper leg
bool r_isUp2 = false; // lower leg
bool r_isUp3 = false; // foot
bool r_isUp4 = true; // foot toes
bool r_is_return = false;

bool l_isUp = false; // upper leg
bool l_isUp2 = false; // lower leg
bool l_isUp3 = false; // foot
bool l_isUp4 = true; // foot toes
bool l_is_return = false;

bool wasdPressed = false;
int direction = 0; // 0 front, 1 left, 2 back , 3 right, changes when wasd key change
int lastDirection = direction; //only change when walk safestop is true
bool walkSafeStop = false;

///////////
// For Jumping Jack
bool isOpen = false;
bool squat1 = false;
bool squat2 = false;
bool pause = true;
bool jjSaveStop = false;
int jjCount = 0;

///////////
// For Gangnam Style
float dGangnamFunction(float followSpeed, float followTarget, float imiTarget);
bool setReadyGS = false;
bool jumpLeft = true;
bool isJump = true;
int counter = 2;
bool gsSaveStop = false;

///////////
// For Moon Walk
bool setReady = false;
bool l_slide = false;
bool moon_reset = false;
bool mwSaveStop = false;
float speedDirection = -0.2f;
float spinSpeed = 8.0f;
int mwcount = 0;
float mwStartZ = 0;// to count for the possibility of out of bound

///////////
// For Squat
bool isSquatReady = false;
bool isGoDown = true;
bool squatSaveStop = false;
int squatCount = 0;

///////////
// For Sit Up
bool isSitUpready = false;
bool isSitUpready2 = false;
bool isUP = true;
bool sleepOnce = true;
int times = 0;
bool isDone = false;
bool isDone2 = false;
bool suSaveStop = false;

///////////
// For Push up
bool isPushUpready = false;
bool isPushUpready2 = false;
bool pushDown = true;
bool isPDone = false;
bool isPDone2 = false;
int pTimes = 0;
bool puSaveStop = false;


//for all animation that is timed, for example gangnam style 16 second, moonwalk 12 second, and walk in place 5 second
double startTimeMove;

//background music
ISoundEngine* SoundEngine;
ISound* gangnamSong = NULL;
ISound* moonwalkSong = NULL;
