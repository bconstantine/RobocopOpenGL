#include "main.h"

string partsList[PARTSNUM] = { "head",
"neck",
"upper_body",
"abs",
"crotch",
"left_upper_thigh",
"left_lower_thigh",
"left_foot",
"left_foot_toes",
"right_upper_thigh",
"right_lower_thigh",
"right_foot",
"right_foot_toes",
"right_upper_arm",
"right_lower_arm",
"right_palm",
"left_upper_arm",
"left_lower_arm",
"left_palm"
};

string extraBodyPart[EXTRAPARTS] = {
	"right_palm_clench",
	"left_palm_clench"
};

float initialOffset[][3] = {
	{0, 6.094, 0.173}, //head
	{0, 4.581, 0.673}, //neck
	{0,2.3932, 0}, //upper_body
	{0,1.245, 0.085}, //abs
	{0,0,0}, //crotch
	{-1.288, -0.735, 0.172}, //left_upper_thigh 
	{-1.358, -5.544, -0.313}, //left_lower_thigh 
	{-1.432, -9.653, -0.182}, //left_foot 
	{-1.543, -10.489, -1.762}, //left_foot_toes 
	{1.288, -0.735, 0.172}, //right_upper_thigh
	{1.358, -5.544, -0.313}, //right_lower_thigh //10 index
	{1.432, -9.653, -0.182}, //right_foot
	{1.543, -10.489, -1.762}, //right_foot_toes
	{2.963,3.59,0.491 }, //right_upper_arm 
	{3.023,0.991,0.4}, //right_lower_arm
	{3.249,-2.01,0.342}, //right_palm // 15
	{-2.963,3.59,0.491}, //left_upper_arm
	{-3.023,0.991,0.4}, //left_lower_arm
	{-3.249,-2.01,0.342}, //left_parm // 18
};

float translatePart[PARTSNUM][3];
float rotatePart[PARTSNUM][3];

int main(int argc, char** argv) {
	//init glfw, parameters, shaders, gui
	GLFWwindow* window = initProgramGLFW();
	
	//init fonts (need to be same scope with main)
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	font1 = io.Fonts->AddFontFromFileTTF("fonts/Roboto-Medium.ttf", 16.0f);
	if (font1 == NULL) {
		cout << "font load unsuccessful\n";
	}
	else {
		cout << "font load successful\n";
	}
	IM_ASSERT(font1 != NULL);
	io.Fonts->Build();
	titleFont = io.Fonts->AddFontFromFileTTF("fonts/Roboto-Medium.ttf", 30.0f);
	if (titleFont == NULL) {
		cout << "title font load unsuccessful\n";
	}
	else {
		cout << "title font load successful\n";
	}
	IM_ASSERT(titleFont != NULL);
	io.Fonts->Build();

	//loadObj and show the progress to the window
	for (int i = 0; i < PARTSTOTAL+1; i++) {
		loadingProgress[i] = false;
	}
	Obj2Buffer(window);

	double lastTime = glfwGetTime();
	double delta = 1.0 / TARGET_FPS;
	double sz = 400;
	/*quat qt2(1.0, 0, 0, 0);
	vec3 light(1.0, 0, 0);*/
	while (!glfwWindowShouldClose(window)) {
		//Keep running, put the code here
		//correct eyeAngleY
		if (eyeAngley < 0) {
			eyeAngley = 360 + eyeAngley;
		}
		else if (eyeAngley > 360) {
			eyeAngley = eyeAngley - 360;
		}


		 // Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//push font
		ImGui::PushFont(font1);
		
		//ImGui::ShowDemoWindow(&show_demo_window);

		{
			ImGui::SetNextWindowSizeConstraints(ImVec2(400,SCREENHEIGHT), ImVec2(400,SCREENHEIGHT)); //width x height fixed
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::Begin("Miscellaneous Toolbox");

			//title 
			ImGui::PushFont(titleFont);
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
			ImGui::Text("Robocop Demo Project");
			ImGui::PopStyleColor();
			ImGui::PopFont();
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
			ImGui::Text("Warning! If you are using BoPoMoFo Keyboard");
			ImGui::Text("Press Shift once, change the keyboard to English...");
			ImGui::PopStyleColor();
			ImGui::Text("Press ESC to quit...");

			//for debugging only
			//ImGui::Checkbox("Demo Window", &show_demo_window);
			if (lightingFollowRobocop) {
				lightPosition[0] = translatePart[CROTCH][0];
				lightPosition[1] = 25;
				lightPosition[2] = translatePart[CROTCH][2];
				discoticLighting = false;
			}
			if (discoticLighting) {
				if (firstTimeDiscoticLighting) {
					lastDiscoticLighting = glfwGetTime();
					firstTimeDiscoticLighting = false;
				}
				lightingFollowRobocop = false;
			}
			else {
				firstTimeDiscoticLighting = true;
			}
			if (ImGui::CollapsingHeader("Lighting related")) {
				ImGui::Checkbox("Show Lighting box", &renderLightBox);
				ImGui::SliderFloat("Light position X", &lightPosition[0], -TRANSLATEXLIMIT, TRANSLATEXLIMIT);
				ImGui::SliderFloat("Light position Y", &lightPosition[1], 25, 50);
				ImGui::SliderFloat("Light position Z", &lightPosition[2], -TRANSLATEZLIMIT, TRANSLATEZLIMIT);
				static int tempRadioVar; 
				if (lightingFollowRobocop) {
					tempRadioVar = 0;
				}
				else if(discoticLighting){
					tempRadioVar = 1;
				}
				else {
					tempRadioVar = 2;
				}
				ImGui::RadioButton("Custom", &tempRadioVar, 2);
				ImGui::RadioButton("Make Lighting follow Robocop", &tempRadioVar, 0);
				ImGui::RadioButton("Discotic Lighting", &tempRadioVar, 1);
				if (tempRadioVar == 0) {
					lightingFollowRobocop = true;
					discoticLighting = false;
				}
				else if (tempRadioVar == 1) {
					lightingFollowRobocop = false;
					discoticLighting = true;
				}
				else {
					lightingFollowRobocop = false;
					discoticLighting = false;
				}
				if (ImGui::Button("Reset Lighting")) {
					lightingFollowRobocop = false;
					discoticLighting = false;
					lightPosition[0] = 10;
					lightPosition[1] = 25;
					lightPosition[2] = 10;
					ambientColor[0] = 0.1;
					ambientColor[1] = 0.1;
					ambientColor[2] = 0.1;
					diffuseColor[0] = 0.8;
					diffuseColor[1] = 0.8;
					diffuseColor[2] = 0.8;
					specularColor[0] = 1;
					specularColor[1] = 1;
					specularColor[2] = 1;
					Shininess = 32;
					kConstant = 1.0f;
					kLinear = 0.09f;
					kQuadratic = 0.032f;
				}
				if (ImGui::TreeNode("Ambient Color")) {
					ImGui::SetColorEditOptions(ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_PickerHueWheel);
					ImGui::ColorPicker4("", (float*)&ambientColor); // Edit 3 floats representing a color
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Diffuse Color")) {
					ImGui::SetColorEditOptions(ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_PickerHueWheel);
					ImGui::ColorPicker4("", (float*)&diffuseColor); // Edit 3 floats representing a color
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Specular Color")) {
					ImGui::SetColorEditOptions(ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_PickerHueWheel);
					ImGui::ColorPicker4("", (float*)&specularColor); // Edit 3 floats representing a color
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Shininess")) {
					ImGui::SliderFloat("Shininess", &Shininess, 2.0, 200);
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Point Lighting Variables")) {
					ImGui::SliderFloat("Kconstant", &kConstant, 0.1, 5);
					ImGui::SliderFloat("Klinear", &kLinear, 0.005, 0.8);
					ImGui::SliderFloat("Kquadratic", &kQuadratic, 0.001, 0.05);
					ImGui::TreePop();
				}
			}
			if (ImGui::CollapsingHeader("Projection Related")) {
				ImGui::SliderFloat("Change FoV: ", &FoV, 30.0f, 90.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::SliderFloat("Change near clip: ", &nearClip, 0.1f, 10.f);
				ImGui::SliderFloat("Change far clip: ", &farClip, 50.f, 200.f);
			}
			if (ImGui::CollapsingHeader("View Related ")) {
				viewChange = true;
				ImGui::Text("Control for keyboard: ");
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
				ImGui::BulletText("ARROW UP "); ImGui::SameLine();
				ImGui::PopStyleColor();
				ImGui::Text("for zoom in");
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
				ImGui::BulletText("ARROW DOWN "); ImGui::SameLine();
				ImGui::PopStyleColor();
				ImGui::Text("for zoomout");
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
				ImGui::BulletText("ARROW LEFT "); ImGui::SameLine();
				ImGui::PopStyleColor();
				ImGui::Text("for turning left");
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
				ImGui::BulletText("ARROW RIGHT "); ImGui::SameLine();
				ImGui::PopStyleColor();
				ImGui::Text("for turning right");
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
				ImGui::BulletText("ESC "); ImGui::SameLine();
				ImGui::PopStyleColor();
				ImGui::Text("for quit");
				ImGui::Separator();
				ImGui::Text("Or use the slider below...");
				ImGui::SliderFloat("Change zoom", &eyedistance, 10.0f, 40.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::SliderFloat("Change eye rotation", &eyeAngley, 0.f, 360.f);
			}
			//comment, this is only for debugging
			//if (ImGui::CollapsingHeader("lightproject: ")) {
			//	ImGui::SliderFloat("Change x: ", &lightProjectX, 0, 400.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			//	ImGui::SliderFloat("Change y: ", &lightProjectY, 0.f, 400.f);
			//	ImGui::SliderFloat("Change z: ", &lightProjectZ, 0.f, 10.f);
			//	ImGui::SliderFloat("Change z far: ", &lightProjectZFar, 10.f, 400.f);
			//	lightProjection = glm::ortho(-lightProjectX, lightProjectX, -lightProjectY, lightProjectY, lightProjectZ, lightProjectZFar);
			//}
			if (ImGui::CollapsingHeader("Current Modes")) {
				ImGui::Text("Control for keyboard: ");
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
				ImGui::BulletText("WASD "); ImGui::SameLine();
				ImGui::PopStyleColor();
				ImGui::Text("for walk");
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
				ImGui::BulletText("F "); ImGui::SameLine();
				ImGui::PopStyleColor();
				ImGui::Text("for squat");
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
				ImGui::BulletText("G "); ImGui::SameLine();
				ImGui::PopStyleColor();
				ImGui::Text("for jumping jack");
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
				ImGui::BulletText("H "); ImGui::SameLine();
				ImGui::PopStyleColor();
				ImGui::Text("for gangnam style");
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
				ImGui::BulletText("J "); ImGui::SameLine();
				ImGui::PopStyleColor();
				ImGui::Text("for moonwalk");
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
				ImGui::BulletText("K "); ImGui::SameLine();
				ImGui::PopStyleColor();
				ImGui::Text("for push up");
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
				ImGui::BulletText("L "); ImGui::SameLine();
				ImGui::PopStyleColor();
				ImGui::Text("for situp");
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
				ImGui::BulletText("ESC "); ImGui::SameLine();
				ImGui::PopStyleColor();
				ImGui::Text("for quit");
				ImGui::Separator();
				ImGui::Text("Or press the circle buttons below...");
				static int currentMode;
				static string animateInfo;
				if (animateMode == IDLE) {
					currentMode = 0;
				}
				else if (animateMode == WALK) {
					currentMode = 1;
				}
				else if (animateMode == SQUAT) {
					currentMode = 2;
				}
				else if (animateMode == JUMPINGJACK) {
					currentMode = 3;
				}
				else if (animateMode == GANGNAMSTYLE) {
					currentMode = 4;
				}
				else if (animateMode == MOONWALK) {
					currentMode = 5;
				}
				else if (animateMode == PUSHUP) {
					currentMode = 6;
				}
				else if (animateMode == SITUP) {
					currentMode = 7;
				}
				ImGui::RadioButton("Idle", &currentMode, 0); ImGui::SameLine();
				ImGui::RadioButton("Walk", &currentMode, 1); ImGui::SameLine();
				ImGui::RadioButton("Squat", &currentMode, 2); ImGui::SameLine();
				ImGui::RadioButton("Jumping Jack", &currentMode, 3); 
				ImGui::RadioButton("Gangnam Style", &currentMode, 4); ImGui::SameLine();
				ImGui::RadioButton("MoonWalk", &currentMode, 5); ImGui::SameLine();
				ImGui::RadioButton("Pushup", &currentMode, 6); ImGui::SameLine();
				ImGui::RadioButton("Situp", &currentMode, 7);
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
				ImGui::Separator();
				//ImGui::Text("Wait until each move is done before starting the next one!");
				ImGui::PopStyleColor();
				if (currentMode == 0) {
					if (animateMode == GANGNAMSTYLE || animateMode == MOONWALK) {
						fromGangnamMoonwalk = true;
					}
					modeChangeExist = true;
					animateMode = IDLE;
					animateInfo = "IDLE";
				}
				else if (currentMode == 1) {
					if (animateMode != WALK) {
						if (animateMode == GANGNAMSTYLE || animateMode == MOONWALK) {
							fromGangnamMoonwalk = true;
						}
						modeChangeExist = true;
						lastDirection = 4;
					}
					animateMode = WALK;
					animateInfo = "WALK";
				}
				else if (currentMode == 2) {
					if (animateMode != SQUAT) {
						if (animateMode == GANGNAMSTYLE || animateMode == MOONWALK) {
							fromGangnamMoonwalk = true;
						}
						modeChangeExist = true;
					}
					animateMode = SQUAT;
					animateInfo = "SQUAT";
				}
				else if (currentMode == 3) {
					if (animateMode != JUMPINGJACK) {
						if (animateMode == GANGNAMSTYLE || animateMode == MOONWALK) {
							fromGangnamMoonwalk = true;
						}
						modeChangeExist = true;
					}
					animateMode = JUMPINGJACK;
					animateInfo = "JUMPING JACK";
				}
				else if (currentMode == 4) {
					if (animateMode != GANGNAMSTYLE) {
						modeChangeExist = true;
					}
					animateMode = GANGNAMSTYLE;
					animateInfo = "GANGNAM STYLE";
				}
				else if (currentMode == 5) {
					if (animateMode != MOONWALK) {
						modeChangeExist = true;
					}
					animateMode = MOONWALK;
					animateInfo = "MOON WALK";
				}
				else if (currentMode == 6) {
					if (animateMode != PUSHUP) {
						if (animateMode == GANGNAMSTYLE || animateMode == MOONWALK) {
							fromGangnamMoonwalk = true;
						}
						modeChangeExist = true;
					}
					animateMode = PUSHUP;
					animateInfo = "PUSH UP";
				}
				else if (currentMode == 7) {
					if (animateMode != SITUP) {
						if (animateMode == GANGNAMSTYLE || animateMode == MOONWALK) {
							fromGangnamMoonwalk = true;
						}
						modeChangeExist = true;
					}
					animateMode = SITUP;
					animateInfo = "SIT UP";
				}
				ImGui::Text("Current Mode = "); ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,0,0,255));
				ImGui::Text("%s", animateInfo.c_str());
				ImGui::PopStyleColor();
			}
			ImGui::Separator();
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("Current X Position: %.3f", translatePart[CROTCH][0]);
			ImGui::Text("Current Y Position: %.3f", translatePart[CROTCH][1]);
			ImGui::Text("Current Z Position: %.3f", translatePart[CROTCH][2]);
			ImGui::End();
		}
		//popping font
		ImGui::PopFont();

		//check based on animation
		if (animateMode == IDLE || modeChangeExist) {
			resetGlobalVariables(); 
			resetModel();
			viewChange = true;
			if (animateMode == WALK) {
				if (!wasdPressed) {
					//for walking in place
					startTimeMove = glfwGetTime();
				}
				else {
					//set initial direction
					lastDirection = direction;
				}
			}
			if (animateMode == GANGNAMSTYLE) {
				gangnamSong = SoundEngine->play2D("../Assets/music/gangnam.mp3", false,false,true);
				startTimeMove = glfwGetTime();
				discoticLighting = true;
				lastDiscoticLighting = glfwGetTime();
			}
			else {
				if (gangnamSong != NULL) {
					gangnamSong->stop();
					gangnamSong->drop(); //free memory
					gangnamSong = NULL;
				}
			}
			if (animateMode == MOONWALK) {
				moonwalkSong = SoundEngine->play2D("../Assets/music/moonwalk.mp3", false, false, true);
				rotatePart[CROTCH][0] = 0;
				rotatePart[CROTCH][1] = 0;
				rotatePart[CROTCH][2] = 0;
				startTimeMove = glfwGetTime();
				discoticLighting = true;
				lastDiscoticLighting = glfwGetTime();
			}
			else {
				if (moonwalkSong != NULL) {
					moonwalkSong->stop();
					moonwalkSong->drop(); //free memory
					moonwalkSong = NULL;
				}
			}
			if (fromGangnamMoonwalk) {
				//reset discotic just in case
				lightingFollowRobocop = false;
				discoticLighting = false;
				lightPosition[0] = 10;
				lightPosition[1] = 25;
				lightPosition[2] = 10;
				ambientColor[0] = 0.1;
				ambientColor[1] = 0.1;
				ambientColor[2] = 0.1;
				diffuseColor[0] = 0.8;
				diffuseColor[1] = 0.8;
				diffuseColor[2] = 0.8;
				specularColor[0] = 1;
				specularColor[1] = 1;
				specularColor[2] = 1;
				Shininess = 32;
				kConstant = 1.0f;
				kLinear = 0.09f;
				kQuadratic = 0.032f;
				fromGangnamMoonwalk = false;
			}
			modeChangeExist = false;
		}
		else  if (animateMode == WALK) {
			for (int i = 0; i < 3; i++) {
				walk();
			}
			if (walkSafeStop) {
				if (lastDirection == 4) {
					//on button click
					if (glfwGetTime() - startTimeMove > 5) {
						animateMode = IDLE;
						modeChangeExist = true;
					}
				}
				else {
					lastDirection = direction;
					if (wasdPressed) {
						updateWalkTranslate();
					}
					else {
						modeChangeExist = true;
						animateMode = IDLE;
					}
				}
			}
			else {
				updateWalkTranslate();
			}
		}
		else if (animateMode == JUMPINGJACK) {
			palmMode = OPEN;
			jumpingJack();
			if (jjSaveStop) {
				modeChangeExist = true;
				animateMode = IDLE;
			}
			//viewChange = true;
		}
		else if (animateMode == MOONWALK)
		{
			palmMode = OPEN;
			moonWalk();
			viewChange = true;
			if (mwSaveStop && ((glfwGetTime() - startTimeMove) > 12.0)) {
				//after more than 12 second, stop
				animateMode = IDLE;
				modeChangeExist = true;

				//resetlighting
				lightingFollowRobocop = false;
				discoticLighting = false;
				lightPosition[0] = 10;
				lightPosition[1] = 25;
				lightPosition[2] = 10;
				ambientColor[0] = 0.1;
				ambientColor[1] = 0.1;
				ambientColor[2] = 0.1;
				diffuseColor[0] = 0.8;
				diffuseColor[1] = 0.8;
				diffuseColor[2] = 0.8;
				specularColor[0] = 1;
				specularColor[1] = 1;
				specularColor[2] = 1;
				Shininess = 32;
				kConstant = 1.0f;
				kLinear = 0.09f;
				kQuadratic = 0.032f;
			}
			//viewChange = true;
		}
		else if (animateMode == GANGNAMSTYLE)
		{
			palmMode = CLENCH;
			gangnamStyle();
			if (gsSaveStop && (glfwGetTime() - startTimeMove > 16.0)) {
				animateMode = IDLE;
				modeChangeExist = true;
				
				//resetlighting
				lightingFollowRobocop = false;
				discoticLighting = false;
				lightPosition[0] = 10;
				lightPosition[1] = 25;
				lightPosition[2] = 10;
				ambientColor[0] = 0.1;
				ambientColor[1] = 0.1;
				ambientColor[2] = 0.1;
				diffuseColor[0] = 0.8;
				diffuseColor[1] = 0.8;
				diffuseColor[2] = 0.8;
				specularColor[0] = 1;
				specularColor[1] = 1;
				specularColor[2] = 1;
				Shininess = 32;
				kConstant = 1.0f;
				kLinear = 0.09f;
				kQuadratic = 0.032f;
			}
			//viewChange = true;
		}
		else if (animateMode == SQUAT)
		{
			palmMode = OPEN;
			squat();
			viewChange = true;
			if (squatSaveStop) {
				modeChangeExist = true;
				animateMode = IDLE;
			}
		}
		else if (animateMode == SITUP)
		{
			sitUp();
			viewChange = true;
			if (suSaveStop) {
				modeChangeExist = true;
				animateMode = IDLE;
			}
		}
		else if (animateMode == PUSHUP)
		{
			pushUp();
			viewChange = true;
			if (puSaveStop) {
				modeChangeExist = true;
				animateMode = IDLE;
			}
		}
		displayGLFW(window);

		//frame rate limit
		while (glfwGetTime() < lastTime + delta) {

		}
		lastTime = glfwGetTime();
		
	}


	//stop imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	//stop glfw
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void resetGlobalVariables() {
	///////////
	// For walk
	r_isFr = false;

	r_isUp = true; // upper leg
	r_isUp2 = false; // lower leg
	r_isUp3 = false; // foot
	r_isUp4 = true; // foot toes
	r_is_return = false;


	l_isUp = false; // upper leg
	l_isUp2 = false; // lower leg
	l_isUp3 = false; // foot
	l_isUp4 = true; // foot toes
	l_is_return = false;

	///////////
	// For Jumping Jack
	isOpen = false;
	squat1 = false;
	squat2 = false;
	pause = true;
	jjSaveStop = false;
	jjCount = 0;

	///////////
	// For Gangnam Style
	setReadyGS = false;
	jumpLeft = true;
	isJump = true;
	counter = 2;
	counter = 2;
	gsSaveStop = false;

	///////////
	// For Sit Up
	isSitUpready = false;
	isSitUpready2 = false;
	isUP = true;
	sleepOnce = true;
	times = 0;
	isDone = false;
	isDone2 = false;
	suSaveStop = false;

	///////////
	// For Moon Walk
	setReady = false;
	l_slide = false;
	moon_reset = false;
	mwSaveStop = false;
	mwcount = 0;
	mwStartZ = translatePart[CROTCH][2];

	///////////
	// For Walk
	isSquatReady = false;
	isGoDown = true;
	squatSaveStop = false;

	///////////
	// For Push up
	isPushUpready = false;
	isPushUpready2 = false;
	pushDown = true;
	isPDone = false;
	isPDone2 = false;
	pTimes = 0;
	puSaveStop = false;

	///////////
	//for squat
	isSquatReady = false;
	isGoDown = true;
	squatSaveStop = false;
	squatCount = 0;
}

void resetModel() {
	for (int i = 0; i < PARTSNUM; i++) {
		if (i != CROTCH) {
			translatePart[i][0] = 0.f;
			translatePart[i][1] = 0.f;
			translatePart[i][2] = 0.f;
			rotatePart[i][0] = 0.f;
			rotatePart[i][1] = 0.f;
			rotatePart[i][2] = 0.f;
		}
		else {
			//for crotch, only reset the y of translate, but skip the y axis of rotate
			translatePart[i][1] = 0.f;
			rotatePart[i][0] = 0.f;
			rotatePart[i][2] = 0.f;
		}
	}
}

GLFWwindow* initProgramGLFW() {
	resetModel();
	ModelBackground = glm::mat4(1.0);
	ModelBackground = glm::translate(ModelBackground, glm::vec3(0, backGroundShiftUp, 0));

	//work backwards to correct the offset
	initialOffset[LEFT_FOOT_TOES][0] -= initialOffset[LEFT_FOOT][0];
	initialOffset[LEFT_FOOT_TOES][1] -= initialOffset[LEFT_FOOT][1];
	initialOffset[LEFT_FOOT_TOES][2] -= initialOffset[LEFT_FOOT][2];

	initialOffset[LEFT_FOOT][0] -= initialOffset[LEFT_LOWER_THIGH][0];
	initialOffset[LEFT_FOOT][1] -= initialOffset[LEFT_LOWER_THIGH][1];
	initialOffset[LEFT_FOOT][2] -= initialOffset[LEFT_LOWER_THIGH][2];

	initialOffset[LEFT_LOWER_THIGH][0] -= initialOffset[LEFT_UPPER_THIGH][0];
	initialOffset[LEFT_LOWER_THIGH][1] -= initialOffset[LEFT_UPPER_THIGH][1];
	initialOffset[LEFT_LOWER_THIGH][2] -= initialOffset[LEFT_UPPER_THIGH][2];

	initialOffset[RIGHT_FOOT_TOES][0] -= initialOffset[RIGHT_FOOT][0];
	initialOffset[RIGHT_FOOT_TOES][1] -= initialOffset[RIGHT_FOOT][1];
	initialOffset[RIGHT_FOOT_TOES][2] -= initialOffset[RIGHT_FOOT][2];

	initialOffset[RIGHT_FOOT][0] -= initialOffset[RIGHT_LOWER_THIGH][0];
	initialOffset[RIGHT_FOOT][1] -= initialOffset[RIGHT_LOWER_THIGH][1];
	initialOffset[RIGHT_FOOT][2] -= initialOffset[RIGHT_LOWER_THIGH][2];

	initialOffset[RIGHT_LOWER_THIGH][0] -= initialOffset[RIGHT_UPPER_THIGH][0];
	initialOffset[RIGHT_LOWER_THIGH][1] -= initialOffset[RIGHT_UPPER_THIGH][1];
	initialOffset[RIGHT_LOWER_THIGH][2] -= initialOffset[RIGHT_UPPER_THIGH][2];

	initialOffset[LEFT_PALM][0] -= initialOffset[LEFT_LOWER_ARM][0];
	initialOffset[LEFT_PALM][1] -= initialOffset[LEFT_LOWER_ARM][1];
	initialOffset[LEFT_PALM][2] -= initialOffset[LEFT_LOWER_ARM][2];

	initialOffset[LEFT_LOWER_ARM][0] -= initialOffset[LEFT_UPPER_ARM][0];
	initialOffset[LEFT_LOWER_ARM][1] -= initialOffset[LEFT_UPPER_ARM][1];
	initialOffset[LEFT_LOWER_ARM][2] -= initialOffset[LEFT_UPPER_ARM][2];

	initialOffset[LEFT_UPPER_ARM][0] -= initialOffset[UPPER_BODY][0];
	initialOffset[LEFT_UPPER_ARM][1] -= initialOffset[UPPER_BODY][1];
	initialOffset[LEFT_UPPER_ARM][2] -= initialOffset[UPPER_BODY][2];

	initialOffset[RIGHT_PALM][0] -= initialOffset[RIGHT_LOWER_ARM][0];
	initialOffset[RIGHT_PALM][1] -= initialOffset[RIGHT_LOWER_ARM][1];
	initialOffset[RIGHT_PALM][2] -= initialOffset[RIGHT_LOWER_ARM][2];

	initialOffset[RIGHT_LOWER_ARM][0] -= initialOffset[RIGHT_UPPER_ARM][0];
	initialOffset[RIGHT_LOWER_ARM][1] -= initialOffset[RIGHT_UPPER_ARM][1];
	initialOffset[RIGHT_LOWER_ARM][2] -= initialOffset[RIGHT_UPPER_ARM][2];

	initialOffset[RIGHT_UPPER_ARM][0] -= initialOffset[UPPER_BODY][0];
	initialOffset[RIGHT_UPPER_ARM][1] -= initialOffset[UPPER_BODY][1];
	initialOffset[RIGHT_UPPER_ARM][2] -= initialOffset[UPPER_BODY][2];

	initialOffset[HEAD][0] -= initialOffset[NECK][0];
	initialOffset[HEAD][1] -= initialOffset[NECK][1];
	initialOffset[HEAD][2] -= initialOffset[NECK][2];

	initialOffset[NECK][0] -= initialOffset[UPPER_BODY][0];
	initialOffset[NECK][1] -= initialOffset[UPPER_BODY][1];
	initialOffset[NECK][2] -= initialOffset[UPPER_BODY][2];

	initialOffset[UPPER_BODY][0] -= initialOffset[ABS][0];
	initialOffset[UPPER_BODY][1] -= initialOffset[ABS][1];
	initialOffset[UPPER_BODY][2] -= initialOffset[ABS][2];

	//our robot is flipped in xz plane, so translate should also be rotated
	for (int i = 0; i < PARTSNUM; i++) {
		initialOffset[i][0] *= -1;
		initialOffset[i][2] *= -1;
	}

	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}
	glfwSetErrorCallback(error_callback);

	//create window and context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_REFRESH_RATE, TARGET_FPS);

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	SCREENWIDTH = mode->width;
	SCREENHEIGHT = mode->height;

	//fullscreen
	GLFWwindow* window = glfwCreateWindow(SCREENWIDTH, SCREENHEIGHT, "Project1 - Robocop", glfwGetPrimaryMonitor(), NULL);
	
	//windowed full screen
	/*glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	GLFWwindow* window = glfwCreateWindow(800, 800, "Project1 - Robocop", NULL, NULL);*/

	if (!window) {
		//window creation failed, check whether version apply with the machine
		cout << "window creation failed\n";
		exit(EXIT_FAILURE);
	}

	//reset global variables for animation
	resetGlobalVariables();

	//callback functions
	glfwSetKeyCallback(window, KeyboardGLFW);
	glfwMakeContextCurrent(window);

	//init glew
	glewExperimental = GLFW_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		cout << "glew init failed: " << glewGetErrorString(err) << endl;
		exit(EXIT_FAILURE);
	}



	//multisample for polygons smooth
	//glfw by default already use double buffering, depth buffer
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glfwSwapInterval(0); // how many frame update before updating the screen? set 0 for no limit

	animateMode = IDLE;
	palmMode = OPEN;

	//init imgui
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform / Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	//VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//for white light boxes
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glGenBuffers(1, &lightBoxBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, lightBoxBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lightModel), lightModel, GL_STATIC_DRAW);
	glGenBuffers(1, &lightBoxIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightBoxIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(recTriangleStripOrder), recTriangleStripOrder, GL_STATIC_DRAW);
	
	//for shadow mapping
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	//depth map we only care to its depth, so set format ot GL_DEPTH_COMPONENT. Set the width and height as desired
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//parameterized the texture
	//minification filter. It is applied when an image is zoomed out so far that multiple pixels
	//Nearest neigbor filtering, does not attempt to scale the image
	//Result is sharp / pixelated
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//fix oversampling the outer region that is not reached by the light, make them area unreachable with the border to CLAMP to border
	//and also set the border color to white, which means that the unreachable light basically clamped to it, no shadow
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	//bind the frame buffer to the generated depth frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

	//we only need depth information when rendering shadow, color will not affect, so no need to put color
	//However, framebuffer object however needs color buffer, so we tell opengl we won't need color data by telling
	//DrawBuffer and ReadBuffer to GL_NONE
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	//check the frame buffer, if no problem, unbind
	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
		exit(EXIT_FAILURE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //unbind frame buffer

	//upload shaders
	//main shader
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "blinn_phong_obj.vp.glsl" },//vertex shader
		{ GL_FRAGMENT_SHADER, "blinn_phong_obj.fp.glsl" },//fragment shader
		{ GL_NONE, NULL } };
	program = LoadShaders(shaders);

	//white box shader, separate so not affected by lighting
	ShaderInfo lightshaders[] = {
		{ GL_VERTEX_SHADER, "light_box.vp.glsl" },//vertex shader
		{ GL_FRAGMENT_SHADER, "light_box.fp.glsl" },//fragment shader
		{ GL_NONE, NULL }
	};
	renderLightProgram = LoadShaders(lightshaders);

	//for shadow mapping shader
	ShaderInfo depthshaders[] = {
		{ GL_VERTEX_SHADER, "shadow.vp.glsl" },//vertex shader
		{ GL_FRAGMENT_SHADER, "shadow.fp.glsl" },//fragment shader
		{ GL_NONE, NULL }
	};
	depthProgram = LoadShaders(depthshaders);

	//get uniform locations
	//for white light box
	glUseProgram(renderLightProgram);
	ProjectionMatrixID = glGetUniformLocation(renderLightProgram, "Projection");
	ViewMatrixID = glGetUniformLocation(renderLightProgram, "View");
	ModelMatrixID = glGetUniformLocation(renderLightProgram, "ModelMatrix");

	//for shadow mapping shader
	glUseProgram(depthProgram);
	lightSpaceID = glGetUniformLocation(depthProgram, "lightSpaceMatrix");
	lightModelID = glGetUniformLocation(depthProgram, "model");

	//for main shader
	glUseProgram(program);
	MatricesIdx = glGetUniformBlockIndex(program, "MatVP"); //view+projection
	ModelID = glGetUniformLocation(program, "Model"); // model
	lightPosID = glGetUniformLocation(program, "vLightPosition"); //vec3 of light position
	//Ka, Kd, Ks of each used mtl
	M_KaID = glGetUniformLocation(program, "Material.Ka");
	M_KdID = glGetUniformLocation(program, "Material.Kd");
	M_KsID = glGetUniformLocation(program, "Material.Ks");
	//vec4 of chosen ambient, diffuse and specular color
	ambientID = glGetUniformLocation(program, "ambientColor");
	diffuseID = glGetUniformLocation(program, "diffuseColor");
	specularID = glGetUniformLocation(program, "specularColor");
	//float to indicate shininess index for specularity
	ShininessID = glGetUniformLocation(program, "Shininess");
	//vec3 position of where my camera location is now, for blinn-phong technique
	viewPosID = glGetUniformLocation(program, "vViewPos");
	//the same mat4 lightspacematrix for depth map, but used again for this shader
	lightSpaceOrigID = glGetUniformLocation(program, "lightSpaceMatrix");
	//for attenuation of point lighting, kconstant, klinear, and kquadratic
	KcID = glGetUniformLocation(program, "Kc");
	KlID = glGetUniformLocation(program, "Kl");
	KqID = glGetUniformLocation(program, "Kq");
	
	//projection matrix, FOR USER NOT FOR LIGHT!
	Projection = glm::perspective(80.0f, (float)(SCREENWIDTH) / (SCREENHEIGHT), 0.1f, 100.f);
	
	//view matrix for user, not light
	viewPos[0] = translatePart[CROTCH][0];
	viewPos[1] = translatePart[CROTCH][1]+robotShiftUp+2;
	viewPos[2] = translatePart[CROTCH][2]+eyedistance;
	View = glm::lookAt(
		glm::vec3(viewPos[0],viewPos[1], viewPos[2]), // Camera is at (0,0,20), in World Space), // Camera is at (0,10,25), in World Space
		glm::vec3(translatePart[CROTCH][0], translatePart[CROTCH][1] + robotShiftUp, translatePart[CROTCH][2]), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,1,0 to look upside-down)
	);

	//UBO
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, NULL, GL_DYNAMIC_DRAW);
	//get uniform struct size
	int UBOsize = 0;
	glGetActiveUniformBlockiv(program, MatricesIdx, GL_UNIFORM_BLOCK_DATA_SIZE, &UBOsize);
	//bind UBO to its idx
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, UBOsize);
	glUniformBlockBinding(program, MatricesIdx, 0);
	
	//init bgm
	SoundEngine = createIrrKlangDevice();
	SoundEngine->play2D("../Assets/music/labAmbient.mp3", true); //loop
	return window;
}

void displayLightSource(glm::mat4 view, glm::mat4 proj) {
	glBindBuffer(GL_ARRAY_BUFFER, lightBoxBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightBoxIndices);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(lightPosition[0], lightPosition[1], lightPosition[2]));
	model = glm::scale(model, glm::vec3(lightScale, lightScale, lightScale));
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &model[0][0]);
	glm::mat4 viewMatrix = view;
	glm::mat4 projMatrix = proj;
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(ProjectionMatrixID, 1, GL_FALSE, &projMatrix[0][0]);
	glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, sizeof(recTriangleStripOrder) / sizeof(int), GL_UNSIGNED_INT, NULL,0);
}

void displayInitial(int currentIndices, GLFWwindow* window) {
	//create new imgui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

		int windowWidth = 700;
		int windowHeight = 800; 

		const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered); //color for buttom
		const ImU32 bg = ImGui::GetColorU32(ImGuiCol_Button); //color for background color
		ImGui::SetNextWindowSizeConstraints(ImVec2(windowWidth, windowHeight), ImVec2(windowWidth, windowHeight)); //width x height fixed
		ImGui::SetNextWindowPos(ImVec2(SCREENWIDTH/2-windowWidth/2.0, SCREENHEIGHT / 2 - windowHeight / 2.0 - 50));
		ImGui::Begin("Loading Obj...");
		ImGui::PushFont(titleFont);
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
		ImGui::Text("Welcome! Obj files are loading..."); ImGui::SameLine();
		ImGui::Spinner("##spinner", 15, 6, col);
		ImGui::PopStyleColor();
		ImGui::PopFont();
		ImGui::PushFont(font1);
		ImGui::BufferingBar("##buffer_bar", ((float)currentIndices)/(PARTSTOTAL+1), ImVec2(400, 6), bg, col);
		ImGui::Text("Please wait... Progress: ");
		bool tempProgress[PARTSTOTAL + 1];
		//create separate boolean array so user pressing the checkbox will not change the global variable
		for (int j = 0; j < PARTSTOTAL + 1; j++) {
			tempProgress[j] = loadingProgress[j];
		}
		//print the checkbox and the name in every line
		for (int j = 0; j < PARTSTOTAL + 1; j++) {
			if (currentIndices == j) 
			{
				//set different color for the parts that is used right now for uploading
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(154, 230, 230, 255));
			}
			if (j < PARTSNUM) {
				ImGui::Checkbox(partsList[j].c_str(), &tempProgress[j]);
			}
			else if (j < PARTSTOTAL) {
				ImGui::Checkbox(extraBodyPart[j-PARTSNUM].c_str(), &tempProgress[j]);
			}
			else {
				ImGui::Checkbox("background", &tempProgress[j]);
			}
			if (currentIndices == j)
			{
				ImGui::PopStyleColor();
			}
		}

		
		ImGui::PopFont();
		ImGui::End();


	//for the input inside current indices, it will be indicated with color red
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void displayOnly(bool depthMode) {
	// function purely for drawing, choose depthMode as true if we want to render with
	//respect to depth shader
	if (renderLightBox && !depthMode) {
		glBindVertexArray(lightVAO);
		glUseProgram(renderLightProgram);
		displayLightSource(View, Projection);
	}
	if (depthMode) {
		//REMEMBER FOR DEPTH SHADER, WE DONT NEED TO RENDER THE BACKGROUND. WE ONLY NEED TO RENDER THE ROBOT ITSELF
		//ALSO, ONLY NEED VERTICES, DISCARD UV AND NORMAL  IN THIS RENDER
		glCullFace(GL_FRONT); //to fix peter panning
		glUseProgram(depthProgram);
		glBindVertexArray(VAO); //bind regular VAO, just in case the previous one is lightVAO
		myUpdateModel();

		//update data to UBO for MVP
		GLuint offset[3] = { 0,0,0 };//offset for vertices , uvs , normals
		glUniformMatrix4fv(lightSpaceID, 1, GL_FALSE, &lightSpace[0][0]);
		for (int i = 0; i < PARTSNUM; i++) {
			glUniformMatrix4fv(lightModelID, 1, GL_FALSE, &Models[i][0][0]);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glEnableVertexAttribArray(0);

			//handle extra part substitution
			int currentIndex = i; //for later mtls accessing
			if (palmMode == CLENCH && (i == 15 || i == 18)) {
				GLuint tempOffset[3] = { offset[0], offset[1], offset[2] }; //tempOffset are used to shift the offset to the end of the non-extra parts

				if (i == 15) {
					for (int j = 15; j < PARTSNUM; j++) {
						tempOffset[0] += vertices_size[j] * sizeof(glm::vec3);
						tempOffset[1] += uvs_size[j] * sizeof(glm::vec2);
						tempOffset[2] += normals_size[j] * sizeof(glm::vec3);
					}
					currentIndex = PARTSNUM;
				}
				else {
					for (int j = 18; j < PARTSNUM + 1; j++) {
						tempOffset[0] += vertices_size[j] * sizeof(glm::vec3);
						tempOffset[1] += uvs_size[j] * sizeof(glm::vec2);
						tempOffset[2] += normals_size[j] * sizeof(glm::vec3);
					}
					currentIndex = PARTSNUM + 1;
				}


				glVertexAttribPointer(0, 3,	GL_FLOAT, GL_FALSE,	0, (void*)tempOffset[0]);
				offset[0] += vertices_size[i] * sizeof(glm::vec3);
			}
			else {
				glVertexAttribPointer(0, 3,	GL_FLOAT,GL_FALSE,0, (void*)offset[0]);//buffer offset
				offset[0] += vertices_size[i] * sizeof(glm::vec3);
			}
			int vertexIDoffset = 0;
			for (int j = 0; j < mtls[currentIndex].size(); j++) {//
				glDrawArrays(GL_TRIANGLES, vertexIDoffset, faces[currentIndex][j + 1] * 3);
				vertexIDoffset += faces[currentIndex][j + 1] * 3;
			}	
		}

		//offset added by extra parts offset
		for (int j = PARTSNUM; j < PARTSTOTAL; j++) {
			offset[0] += vertices_size[j] * sizeof(glm::vec3);
			offset[1] += uvs_size[j] * sizeof(glm::vec2);
			offset[2] += normals_size[j] * sizeof(glm::vec3);
		}
		glCullFace(GL_BACK); //return to original for usual render
	}
	else {
		//usual render
		glBindVertexArray(VAO);
		glUseProgram(program);
		myUpdateModel(); //based on the contents of the translatePart and rotatePart, update the Models mat4 array

		//update data to UBO for MVP
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &View);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &Projection);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glUniform4fv(ambientID, 1, &ambientColor[0]);
		glUniform4fv(diffuseID, 1, &diffuseColor[0]);
		glUniform4fv(specularID, 1, &specularColor[0]);
		glUniform1fv(ShininessID, 1, &Shininess);
		glUniform3fv(viewPosID, 1, &viewPos[0]);
		glUniformMatrix4fv(lightSpaceOrigID, 1, GL_FALSE, &lightSpace[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glUniform1fv(KcID, 1, &kConstant);
		glUniform1fv(KlID, 1, &kLinear);
		glUniform1fv(KqID, 1, &kQuadratic);
		glUniform3fv(lightPosID, 1, &lightPosition[0]);
		//use the depth map we have generated previously
		glBindTexture(GL_TEXTURE_2D, depthMap);

		GLuint offset[3] = { 0,0,0 };//offset for vertices , uvs , normals
		for (int i = 0; i < PARTSNUM; i++) {
			//bind model exclusively with each model matrix
			glUniformMatrix4fv(ModelID, 1, GL_FALSE, &Models[i][0][0]);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glEnableVertexAttribArray(0);


			//handle extra part substitution
			int currentIndex = i; //for later mtls accessing, because we might access different index for extra parts
			if (palmMode == CLENCH && (i == 15 || i == 18)) {
				GLuint tempOffset[3] = { offset[0], offset[1], offset[2] };

				if (i == 15) {
					for (int j = 15; j < PARTSNUM; j++) {
						tempOffset[0] += vertices_size[j] * sizeof(glm::vec3);
						tempOffset[1] += uvs_size[j] * sizeof(glm::vec2);
						tempOffset[2] += normals_size[j] * sizeof(glm::vec3);
					}
					currentIndex = PARTSNUM;
				}
				else {
					for (int j = 18; j < PARTSNUM + 1; j++) {
						tempOffset[0] += vertices_size[j] * sizeof(glm::vec3);
						tempOffset[1] += uvs_size[j] * sizeof(glm::vec2);
						tempOffset[2] += normals_size[j] * sizeof(glm::vec3);
					}
					currentIndex = PARTSNUM + 1;
				}

				//first, vertices
				glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)tempOffset[0]);//buffer offset
				offset[0] += vertices_size[i] * sizeof(glm::vec3); //for next vertices starting point

				//next, uv
				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ARRAY_BUFFER, uVBO);
				glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,(void*)tempOffset[1]);
				offset[1] += uvs_size[i] * sizeof(glm::vec2);

				//finally, normal
				glEnableVertexAttribArray(2);
				glBindBuffer(GL_ARRAY_BUFFER, nVBO);
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)tempOffset[2]);
				offset[2] += normals_size[i] * sizeof(glm::vec3);
			}
			else {
				//the same, but this is for default part
				//first, vertices
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset[0]);//buffer offset
				offset[0] += vertices_size[i] * sizeof(glm::vec3); //for next vertices starting point

				//next, uv
				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ARRAY_BUFFER, uVBO);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)offset[1]);
				offset[1] += uvs_size[i] * sizeof(glm::vec2);

				//finally, normal
				glEnableVertexAttribArray(2);
				glBindBuffer(GL_ARRAY_BUFFER, nVBO);
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)offset[2]);
				offset[2] += normals_size[i] * sizeof(glm::vec3);
			}
			int vertexIDoffset = 0;//glVertexID's offset 
			string mtlname;//material name
			for (int j = 0; j < mtls[currentIndex].size(); j++) {//
				mtlname = mtls[currentIndex][j];
				//bind Ka, Kd and Ks of current material
				glUniform3fv(M_KaID, 1, &KAs[mtlname][0]);
				glUniform3fv(M_KdID, 1, &KDs[mtlname][0]);
				glUniform3fv(M_KsID, 1, &KSs[mtlname][0]);
				//faces[currentIndex][j + 1] * 3
				glDrawArrays(GL_TRIANGLES, vertexIDoffset, faces[currentIndex][j + 1] * 3);
				vertexIDoffset += faces[currentIndex][j + 1] * 3; 
			}	
		}

		//offset added by extra parts offset
		for (int j = PARTSNUM; j < PARTSTOTAL; j++) {
			offset[0] += vertices_size[j] * sizeof(glm::vec3);
			offset[1] += uvs_size[j] * sizeof(glm::vec2);
			offset[2] += normals_size[j] * sizeof(glm::vec3);
		}

		//render background
		if (useBackground) {
			//bind model matrix for background
			glUniformMatrix4fv(ModelID, 1, GL_FALSE, &ModelBackground[0][0]);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);

			//vertices
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)offset[0]);
			offset[0] += vertices_size[PARTSTOTAL] * sizeof(glm::vec3);

			//uv
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, uVBO);
			glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,(void*)offset[1]);
			offset[1] += uvs_size[PARTSTOTAL] * sizeof(glm::vec2);

			// normal
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, nVBO);
			glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,(void*)offset[2]);
			offset[2] += normals_size[PARTSTOTAL] * sizeof(glm::vec3);

			int vertexIDoffset = 0;
			string mtlname;
			for (int j = 0; j < mtls[PARTSTOTAL].size(); j++) {//
				mtlname = mtls[PARTSTOTAL][j];
				glUniform3fv(M_KaID, 1, &KAs[mtlname][0]);
				glUniform3fv(M_KdID, 1, &KDs[mtlname][0]);
				glUniform3fv(M_KsID, 1, &KSs[mtlname][0]);
				glDrawArrays(GL_TRIANGLES, vertexIDoffset, faces[PARTSTOTAL][j + 1] * 3);
				vertexIDoffset += faces[PARTSTOTAL][j + 1] * 3;
			}
		}
	}
}

void displayGLFW(GLFWwindow* window) {
	//render pipeline

	//accumulate data for gui to be rendered
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//only update View when angle change, to save computation
	if (viewChange) {
		float eyeAngleRad = deg2rad(eyeAngley);
		viewPos[0] = translatePart[CROTCH][0] + eyedistance * sin(eyeAngleRad);
		viewPos[1] = translatePart[CROTCH][1] + robotShiftUp + 2;
		viewPos[2] = translatePart[CROTCH][2] + eyedistance * cos(eyeAngleRad);
		View = glm::lookAt(
			glm::vec3(viewPos[0],viewPos[1],viewPos[2] ), //camera position
			glm::vec3(translatePart[CROTCH][0], translatePart[CROTCH][1] + robotShiftUp, translatePart[CROTCH][2]), //point to crotch
			glm::vec3(0, 1, 0) //up vector parallel to y axis
		);
		viewChange = false;
	}
	//projection matrix update, just in case it is changed in GuI input
	Projection = glm::perspective(FoV, (float)(SCREENWIDTH - widthStart) / (SCREENHEIGHT - heightStart), nearClip, farClip);
	//handles discoticLighting
	if (discoticLighting) {
		glm::mat4 lightPosMV(1.0);
		float currentTime = getTime();
		lightPosMV = glm::translate(lightPosMV, glm::vec3(sinf(5.1f * currentTime) * 2.f, cosf(5.7f * currentTime) * 2.f, sinf(5.3f * currentTime) * cosf(5.5f * currentTime) * 2.f));
		lightPosMV = glm::rotate(lightPosMV, currentTime * 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		lightPosMV = glm::rotate(lightPosMV, currentTime * 81.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		lightPosMV = glm::translate(lightPosMV, glm::vec3(0, 40, 0));
		glm::vec4 robocopPos(translatePart[CROTCH][0], translatePart[CROTCH][1], translatePart[CROTCH][2], 1.0f);
		robocopPos = lightPosMV * robocopPos;
		//offset based on robocop Position
		
		//update global variable
		lightPosition[0] = robocopPos[0];
		lightPosition[1] = robocopPos[1];
		lightPosition[2] = robocopPos[2];

		//clamp light position
		if (lightPosition[0] < -TRANSLATEXLIMIT) {
			lightPosition[0] = -TRANSLATEXLIMIT;
		}
		else if (lightPosition[0] > TRANSLATEXLIMIT) {
			lightPosition[0] = TRANSLATEXLIMIT;
		}
		if (lightPosition[2] < -TRANSLATEZLIMIT) {
			lightPosition[2] = -TRANSLATEZLIMIT;
		}
		else if (lightPosition[2] > TRANSLATEZLIMIT) {
			lightPosition[2] = TRANSLATEZLIMIT;
		}
		if (lightPosition[1] < TRANSLATEYLOWERLIMIT) {
			lightPosition[1] = TRANSLATEYLOWERLIMIT;
		}
		else if (lightPosition[1] > TRANSLATEYUPPERLIMIT) {
			lightPosition[1] = TRANSLATEYUPPERLIMIT;
		}

		//handles change color, different period of color change for moonwalk and gangnam style (and other discotic lighting)
		if (animateMode == MOONWALK) {
			if ((glfwGetTime() - lastDiscoticLighting) > 0.5)
			{
				//change discotic color
				ambientColor[0] = 0.1;
				ambientColor[1] = 0.1;
				ambientColor[2] = 0.1;
				diffuseColor[0] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				diffuseColor[1] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				diffuseColor[2] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				lastDiscoticLighting = glfwGetTime();
			}
		}
		else {
			if ((glfwGetTime() - lastDiscoticLighting) > 1.7)
			{
				//change discotic color
				ambientColor[0] = 0.1;
				ambientColor[1] = 0.1;
				ambientColor[2] = 0.1;
				diffuseColor[0] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				diffuseColor[1] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				diffuseColor[2] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				lastDiscoticLighting = glfwGetTime();
			}
		}
	}

	//update light space matrix
	glm::vec3 curLightPos(lightPosition[0], lightPosition[1], lightPosition[2]);
	glm::mat4 lightView = glm::lookAt(curLightPos,
		glm::vec3(translatePart[CROTCH][0], translatePart[CROTCH][1]+robotShiftUp, translatePart[CROTCH][2]),
		glm::vec3(0, 1, 0));
	lightSpace = lightProjection * lightView; //light projection will never change, so just set to global variable.
	//-------------- for the record, this is lightProjection content  -----------------
	//lightProjection = glm::ortho(-79, 79, -75, 75, 0.4, 106.7);
	//----------------------------------------------------------------------------------
	
	//render to depth map, for shadow mapping
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT); 
		displayOnly(true);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//usual rendering
	glViewport(widthStart, heightStart, SCREENWIDTH, SCREENHEIGHT);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, depthMap);
		displayOnly(false);

	//render gui
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Obj2Buffer(GLFWwindow* window) {
	std::vector<glm::vec3> Kds;
	std::vector<glm::vec3> Kas;
	std::vector<glm::vec3> Kss;
	std::vector<std::string> Materials;//mtl-name
	std::string texture;

	//load MTLs, for every parts, extra parts, and backgrounds. 
	for (int i = 0; i < PARTSNUM; i++) {
		loadMTL(("../Assets/Obj/" + partsList[i] + ".mtl").c_str(), Kds, Kas, Kss, Materials, texture);
	}
	for (int i = 0; i < EXTRAPARTS; i++) {
		loadMTL(("../Assets/Obj/" + partsList[i] + ".mtl").c_str(), Kds, Kas, Kss, Materials, texture);
	}
	if (useBackground) {
		loadMTL("../Assets/Obj/Sci_Fi_Corridor.mtl", Kds, Kas, Kss, Materials, texture);
	}
	//identify make a mapping of MTLs KD,KA and KS based on each name, just in case there is duplicity
	for (int i = 0; i < Materials.size(); i++) {
		string mtlname = Materials[i];
		KDs[mtlname] = Kds[i];
		KAs[mtlname] = Kas[i];
		KSs[mtlname] = Kss[i];
	}

	displayInitial(-1, window); //show progress of loading on GUI
	for (int i = 0; i < PARTSNUM; i++) {
		//for debug, skip certain indices
		if (!renderBodyTop && (i >= 0 && i <= 3 || i >= 13)) {
			continue;
		}
		if (!renderHead && (i >= 0 && i <= 1)) {
			continue;
		}
		if (!renderArm && (i >= 13)) {
			continue;
		}
		if (!renderLeg && (i >= 5 && i <= 12)) {
			continue;
		}


		displayInitial(i,window); //for displaying loading progress;
		load2Buffer("../Assets/Obj/"+partsList[i]+".obj", i);
		loadingProgress[i] = true; //updateProgress
	}
	for (int i = 0; i < EXTRAPARTS; i++) {
		//for debug, skip certain indices
		if (!renderBodyTop || !renderArm) {
			continue;
		}
		displayInitial(i+PARTSNUM,window); //for displaying loading progress;
		load2Buffer("../Assets/Obj/" + extraBodyPart[i] + ".obj", i + PARTSNUM);
		loadingProgress[i+PARTSNUM] = true; //updateProgress
	}
	//for debugging purposes, skip loading if background is disabled
	if (useBackground) {
		displayInitial(PARTSTOTAL,window); //for displaying loading progress;
		load2Buffer("../Assets/Obj/Sci_Fi_Corridor.obj", PARTSTOTAL);
		loadingProgress[PARTSTOTAL] = true; //updateProgress
	}
	displayInitial(-1,window); //for displaying loading progress;
	
	//count total data for the vertices, uvs and also the normal size
	GLuint totalSize[3] = { 0,0,0 };
	GLuint offset[3] = { 0,0,0 };
	//Parts total is all body parts+extra parts
	for (int i = 0; i < PARTSTOTAL; i++) {
		totalSize[0] += vertices_size[i] * sizeof(glm::vec3);
		totalSize[1] += uvs_size[i] * sizeof(glm::vec2);
		totalSize[2] += normals_size[i] * sizeof(glm::vec3);
	}
	//add also the background
	if (useBackground) {
		totalSize[0] += vertices_size[PARTSTOTAL] * sizeof(glm::vec3);
		totalSize[1] += uvs_size[PARTSTOTAL] * sizeof(glm::vec2);
		totalSize[2] += normals_size[PARTSTOTAL] * sizeof(glm::vec3);
	}
	//generate 3 vbo for the vertices, uv and  also the normal
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &uVBO);
	glGenBuffers(1, &nVBO);
	
	//allocate space for the three VBOs. Since the data aren't changed often, use GL_STATIC_DRAW
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, totalSize[0], NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, uVBO);
	glBufferData(GL_ARRAY_BUFFER, totalSize[1], NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, nVBO);
	glBufferData(GL_ARRAY_BUFFER, totalSize[2], NULL, GL_STATIC_DRAW);
	
	for (int i = 0; i < PARTSTOTAL; i++) {
		//copy individual VBO vertices into the universal VBO
		glBindBuffer(GL_COPY_WRITE_BUFFER, VBO);
		glBindBuffer(GL_COPY_READ_BUFFER, VBOs[i]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, offset[0], vertices_size[i] * sizeof(glm::vec3));
		offset[0] += vertices_size[i] * sizeof(glm::vec3);
		glInvalidateBufferData(VBOs[i]);
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
		//copy individual VBO uv into the universal VBO
		glBindBuffer(GL_COPY_WRITE_BUFFER, uVBO);
		glBindBuffer(GL_COPY_READ_BUFFER, uVBOs[i]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, offset[1], uvs_size[i] * sizeof(glm::vec2));
		offset[1] += uvs_size[i] * sizeof(glm::vec2);
		glInvalidateBufferData(uVBOs[i]);
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
		//copy individual VBO normals into the universal VBO
		glBindBuffer(GL_COPY_WRITE_BUFFER, nVBO);
		glBindBuffer(GL_COPY_READ_BUFFER, nVBOs[i]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, offset[2], normals_size[i] * sizeof(glm::vec3));
		offset[2] += normals_size[i] * sizeof(glm::vec3);
		glInvalidateBufferData(nVBOs[i]);
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	}
	if (useBackground) {
		//do the same for background, but indices must be  PARTSTOTAL, == number of the whole parts+extra parts
		glBindBuffer(GL_COPY_WRITE_BUFFER, VBO);
		glBindBuffer(GL_COPY_READ_BUFFER, VBOs[PARTSTOTAL]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, offset[0], vertices_size[PARTSTOTAL] * sizeof(glm::vec3));
		offset[0] += vertices_size[PARTSTOTAL] * sizeof(glm::vec3);
		glInvalidateBufferData(VBOs[PARTSTOTAL]);//free vbo
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

		glBindBuffer(GL_COPY_WRITE_BUFFER, uVBO);
		glBindBuffer(GL_COPY_READ_BUFFER, uVBOs[PARTSTOTAL]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, offset[1], uvs_size[PARTSTOTAL] * sizeof(glm::vec2));
		offset[1] += uvs_size[PARTSTOTAL] * sizeof(glm::vec2);
		glInvalidateBufferData(uVBOs[PARTSTOTAL]);//free vbo
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

		glBindBuffer(GL_COPY_WRITE_BUFFER, nVBO);
		glBindBuffer(GL_COPY_READ_BUFFER, nVBOs[PARTSTOTAL]);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, offset[2], normals_size[PARTSTOTAL] * sizeof(glm::vec3));
		offset[2] += normals_size[PARTSTOTAL] * sizeof(glm::vec3);
		glInvalidateBufferData(nVBOs[PARTSTOTAL]);//free vbo
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	}

	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}


void myUpdateModel() {
	//reset state, glm::translate to initial place
	for (int i = 0; i < PARTSNUM; i++) {
		Models[i] = glm::mat4(1.0f);
	}
	//Hierarchy transform
	//based on the world coordinate, the rotation axis is X-Y-Z axis, or Z-Y-X axis by the object coordinate
	//Body
	Models[CROTCH] = glm::translate(Models[CROTCH], glm::vec3(0, robotShiftUp, 0));
	Models[CROTCH] = glm::translate(Models[CROTCH], glm::vec3(translatePart[CROTCH][0], translatePart[CROTCH][1], translatePart[CROTCH][2]));
	Models[CROTCH] = glm::translate(Models[CROTCH], glm::vec3(initialOffset[CROTCH][0], initialOffset[CROTCH][1], initialOffset[CROTCH][2]));
	Models[CROTCH] = glm::rotate(Models[CROTCH], rotatePart[CROTCH][2], glm::vec3(0, 0, 1));
	Models[CROTCH] = glm::rotate(Models[CROTCH], rotatePart[CROTCH][1], glm::vec3(0, 1, 0));
	Models[CROTCH] = glm::rotate(Models[CROTCH], rotatePart[CROTCH][0], glm::vec3(1, 0, 0));
		//Abs
		Models[ABS] = glm::translate(Models[CROTCH], glm::vec3(translatePart[ABS][0], translatePart[ABS][1], translatePart[ABS][2]));
		Models[ABS] = glm::translate(Models[ABS], glm::vec3(initialOffset[ABS][0], initialOffset[ABS][1], initialOffset[ABS][2]));
		Models[ABS] = glm::rotate(Models[ABS], rotatePart[ABS][2], glm::vec3(0, 0, 1));
		Models[ABS] = glm::rotate(Models[ABS], rotatePart[ABS][1], glm::vec3(0, 1, 0));
		Models[ABS] = glm::rotate(Models[ABS], rotatePart[ABS][0], glm::vec3(1, 0, 0));
			//Upper Body
			Models[UPPER_BODY] = glm::translate(Models[ABS], glm::vec3(translatePart[UPPER_BODY][0], translatePart[UPPER_BODY][1], translatePart[UPPER_BODY][2]));
			Models[UPPER_BODY] = glm::translate(Models[UPPER_BODY], glm::vec3(initialOffset[UPPER_BODY][0], initialOffset[UPPER_BODY][1], initialOffset[UPPER_BODY][2]));
			Models[UPPER_BODY] = glm::rotate(Models[UPPER_BODY], rotatePart[UPPER_BODY][2], glm::vec3(0, 0, 1));
			Models[UPPER_BODY] = glm::rotate(Models[UPPER_BODY], rotatePart[UPPER_BODY][1], glm::vec3(0, 1, 0));
			Models[UPPER_BODY] = glm::rotate(Models[UPPER_BODY], rotatePart[UPPER_BODY][0], glm::vec3(1, 0, 0));
				//Neck
				Models[NECK] = glm::translate(Models[UPPER_BODY], glm::vec3(translatePart[NECK][0], translatePart[NECK][1], translatePart[NECK][2]));
				Models[NECK] = glm::translate(Models[NECK], glm::vec3(initialOffset[NECK][0], initialOffset[NECK][1], initialOffset[NECK][2]));
				Models[NECK] = glm::rotate(Models[NECK], rotatePart[NECK][2], glm::vec3(0, 0, 1));
				Models[NECK] = glm::rotate(Models[NECK], rotatePart[NECK][1], glm::vec3(0, 1, 0));
				Models[NECK] = glm::rotate(Models[NECK], rotatePart[NECK][0], glm::vec3(1, 0, 0));
					//Head
					Models[HEAD] = glm::translate(Models[NECK], glm::vec3(translatePart[HEAD][0], translatePart[HEAD][1], translatePart[HEAD][2]));
					Models[HEAD] = glm::translate(Models[HEAD], glm::vec3(initialOffset[HEAD][0], initialOffset[HEAD][1], initialOffset[HEAD][2]));
					Models[HEAD] = glm::rotate(Models[HEAD], rotatePart[HEAD][2], glm::vec3(0, 0, 1));
					Models[HEAD] = glm::rotate(Models[HEAD], rotatePart[HEAD][1], glm::vec3(0, 1, 0));
					Models[HEAD] = glm::rotate(Models[HEAD], rotatePart[HEAD][0], glm::vec3(1, 0, 0));

				//Right_Upper_arm
				Models[RIGHT_UPPER_ARM] = glm::translate(Models[UPPER_BODY], glm::vec3(translatePart[RIGHT_UPPER_ARM][0], translatePart[RIGHT_UPPER_ARM][1], translatePart[RIGHT_UPPER_ARM][2]));
				Models[RIGHT_UPPER_ARM] = glm::translate(Models[RIGHT_UPPER_ARM], glm::vec3(initialOffset[RIGHT_UPPER_ARM][0], initialOffset[RIGHT_UPPER_ARM][1], initialOffset[RIGHT_UPPER_ARM][2]));
				Models[RIGHT_UPPER_ARM] = glm::rotate(Models[RIGHT_UPPER_ARM], rotatePart[RIGHT_UPPER_ARM][2], glm::vec3(0, 0, 1));
				Models[RIGHT_UPPER_ARM] = glm::rotate(Models[RIGHT_UPPER_ARM], rotatePart[RIGHT_UPPER_ARM][1], glm::vec3(0, 1, 0));
				Models[RIGHT_UPPER_ARM] = glm::rotate(Models[RIGHT_UPPER_ARM], rotatePart[RIGHT_UPPER_ARM][0], glm::vec3(1, 0, 0));
					//Right_Lower_arm
					Models[RIGHT_LOWER_ARM] = glm::translate(Models[RIGHT_UPPER_ARM], glm::vec3(translatePart[RIGHT_LOWER_ARM][0], translatePart[RIGHT_LOWER_ARM][1], translatePart[RIGHT_LOWER_ARM][2]));
					Models[RIGHT_LOWER_ARM] = glm::translate(Models[RIGHT_LOWER_ARM], glm::vec3(initialOffset[RIGHT_LOWER_ARM][0], initialOffset[RIGHT_LOWER_ARM][1], initialOffset[RIGHT_LOWER_ARM][2]));
					Models[RIGHT_LOWER_ARM] = glm::rotate(Models[RIGHT_LOWER_ARM], rotatePart[RIGHT_LOWER_ARM][2], glm::vec3(0, 0, 1));
					Models[RIGHT_LOWER_ARM] = glm::rotate(Models[RIGHT_LOWER_ARM], rotatePart[RIGHT_LOWER_ARM][1], glm::vec3(0, 1, 0));
					Models[RIGHT_LOWER_ARM] = glm::rotate(Models[RIGHT_LOWER_ARM], rotatePart[RIGHT_LOWER_ARM][0], glm::vec3(1, 0, 0));
						//Right_Palm
						Models[RIGHT_PALM] = glm::translate(Models[RIGHT_LOWER_ARM], glm::vec3(translatePart[RIGHT_PALM][0], translatePart[RIGHT_PALM][1], translatePart[RIGHT_PALM][2]));
						Models[RIGHT_PALM] = glm::translate(Models[RIGHT_PALM], glm::vec3(initialOffset[RIGHT_PALM][0], initialOffset[RIGHT_PALM][1], initialOffset[RIGHT_PALM][2]));
						Models[RIGHT_PALM] = glm::rotate(Models[RIGHT_PALM], rotatePart[RIGHT_PALM][2], glm::vec3(0, 0, 1));
						Models[RIGHT_PALM] = glm::rotate(Models[RIGHT_PALM], rotatePart[RIGHT_PALM][1], glm::vec3(0, 1, 0));
						Models[RIGHT_PALM] = glm::rotate(Models[RIGHT_PALM], rotatePart[RIGHT_PALM][0], glm::vec3(1, 0, 0));
				//LEFT_Upper_arm
				Models[LEFT_UPPER_ARM] = glm::translate(Models[UPPER_BODY], glm::vec3(translatePart[LEFT_UPPER_ARM][0], translatePart[LEFT_UPPER_ARM][1], translatePart[LEFT_UPPER_ARM][2]));
				Models[LEFT_UPPER_ARM] = glm::translate(Models[LEFT_UPPER_ARM], glm::vec3(initialOffset[LEFT_UPPER_ARM][0], initialOffset[LEFT_UPPER_ARM][1], initialOffset[LEFT_UPPER_ARM][2]));
				Models[LEFT_UPPER_ARM] = glm::rotate(Models[LEFT_UPPER_ARM], rotatePart[LEFT_UPPER_ARM][2], glm::vec3(0, 0, 1));
				Models[LEFT_UPPER_ARM] = glm::rotate(Models[LEFT_UPPER_ARM], rotatePart[LEFT_UPPER_ARM][1], glm::vec3(0, 1, 0));
				Models[LEFT_UPPER_ARM] = glm::rotate(Models[LEFT_UPPER_ARM], rotatePart[LEFT_UPPER_ARM][0], glm::vec3(1, 0, 0));
					//LEFT_Lower_arm
					Models[LEFT_LOWER_ARM] = glm::translate(Models[LEFT_UPPER_ARM], glm::vec3(translatePart[LEFT_LOWER_ARM][0], translatePart[LEFT_LOWER_ARM][1], translatePart[LEFT_LOWER_ARM][2]));
					Models[LEFT_LOWER_ARM] = glm::translate(Models[LEFT_LOWER_ARM], glm::vec3(initialOffset[LEFT_LOWER_ARM][0], initialOffset[LEFT_LOWER_ARM][1], initialOffset[LEFT_LOWER_ARM][2]));
					Models[LEFT_LOWER_ARM] = glm::rotate(Models[LEFT_LOWER_ARM], rotatePart[LEFT_LOWER_ARM][2], glm::vec3(0, 0, 1));
					Models[LEFT_LOWER_ARM] = glm::rotate(Models[LEFT_LOWER_ARM], rotatePart[LEFT_LOWER_ARM][1], glm::vec3(0, 1, 0));
					Models[LEFT_LOWER_ARM] = glm::rotate(Models[LEFT_LOWER_ARM], rotatePart[LEFT_LOWER_ARM][0], glm::vec3(1, 0, 0));
						//LEFT_Palm
						Models[LEFT_PALM] = glm::translate(Models[LEFT_LOWER_ARM], glm::vec3(translatePart[LEFT_PALM][0], translatePart[LEFT_PALM][1], translatePart[LEFT_PALM][2]));
						Models[LEFT_PALM] = glm::translate(Models[LEFT_PALM], glm::vec3(initialOffset[LEFT_PALM][0], initialOffset[LEFT_PALM][1], initialOffset[LEFT_PALM][2]));
						Models[LEFT_PALM] = glm::rotate(Models[LEFT_PALM], rotatePart[LEFT_PALM][2], glm::vec3(0, 0, 1));
						Models[LEFT_PALM] = glm::rotate(Models[LEFT_PALM], rotatePart[LEFT_PALM][1], glm::vec3(0, 1, 0));
						Models[LEFT_PALM] = glm::rotate(Models[LEFT_PALM], rotatePart[LEFT_PALM][0], glm::vec3(1, 0, 0));

		//Left_Upper_thigh
		Models[LEFT_UPPER_THIGH] = glm::translate(Models[CROTCH], glm::vec3(translatePart[LEFT_UPPER_THIGH][0], translatePart[LEFT_UPPER_THIGH][1], translatePart[LEFT_UPPER_THIGH][2]));
		Models[LEFT_UPPER_THIGH] = glm::translate(Models[LEFT_UPPER_THIGH], glm::vec3(initialOffset[LEFT_UPPER_THIGH][0], initialOffset[LEFT_UPPER_THIGH][1], initialOffset[LEFT_UPPER_THIGH][2]));
		Models[LEFT_UPPER_THIGH] = glm::rotate(Models[LEFT_UPPER_THIGH], rotatePart[LEFT_UPPER_THIGH][2], glm::vec3(0, 0, 1));
		Models[LEFT_UPPER_THIGH] = glm::rotate(Models[LEFT_UPPER_THIGH], rotatePart[LEFT_UPPER_THIGH][1], glm::vec3(0, 1, 0));
		Models[LEFT_UPPER_THIGH] = glm::rotate(Models[LEFT_UPPER_THIGH], rotatePart[LEFT_UPPER_THIGH][0], glm::vec3(1, 0, 0));
			//LEFT_lower_thigh
			Models[LEFT_LOWER_THIGH] = glm::translate(Models[LEFT_UPPER_THIGH], glm::vec3(translatePart[LEFT_LOWER_THIGH][0], translatePart[LEFT_LOWER_THIGH][1], translatePart[LEFT_LOWER_THIGH][2]));
			Models[LEFT_LOWER_THIGH] = glm::translate(Models[LEFT_LOWER_THIGH], glm::vec3(initialOffset[LEFT_LOWER_THIGH][0], initialOffset[LEFT_LOWER_THIGH][1], initialOffset[LEFT_LOWER_THIGH][2]));
			Models[LEFT_LOWER_THIGH] = glm::rotate(Models[LEFT_LOWER_THIGH], rotatePart[LEFT_LOWER_THIGH][2], glm::vec3(0, 0, 1));
			Models[LEFT_LOWER_THIGH] = glm::rotate(Models[LEFT_LOWER_THIGH], rotatePart[LEFT_LOWER_THIGH][1], glm::vec3(0, 1, 0));
			Models[LEFT_LOWER_THIGH] = glm::rotate(Models[LEFT_LOWER_THIGH], rotatePart[LEFT_LOWER_THIGH][0], glm::vec3(1, 0, 0));
				//LEFT_foot
				Models[LEFT_FOOT] = glm::translate(Models[LEFT_LOWER_THIGH], glm::vec3(translatePart[LEFT_FOOT][0], translatePart[LEFT_FOOT][1], translatePart[LEFT_FOOT][2]));
				Models[LEFT_FOOT] = glm::translate(Models[LEFT_FOOT], glm::vec3(initialOffset[LEFT_FOOT][0], initialOffset[LEFT_FOOT][1], initialOffset[LEFT_FOOT][2]));
				Models[LEFT_FOOT] = glm::rotate(Models[LEFT_FOOT], rotatePart[LEFT_FOOT][2], glm::vec3(0, 0, 1));
				Models[LEFT_FOOT] = glm::rotate(Models[LEFT_FOOT], rotatePart[LEFT_FOOT][1], glm::vec3(0, 1, 0));
				Models[LEFT_FOOT] = glm::rotate(Models[LEFT_FOOT], rotatePart[LEFT_FOOT][0], glm::vec3(1, 0, 0));
					//LEFT_foot_toes
					Models[LEFT_FOOT_TOES] = glm::translate(Models[LEFT_FOOT], glm::vec3(translatePart[LEFT_FOOT_TOES][0], translatePart[LEFT_FOOT_TOES][1], translatePart[LEFT_FOOT_TOES][2]));
					Models[LEFT_FOOT_TOES] = glm::translate(Models[LEFT_FOOT_TOES], glm::vec3(initialOffset[LEFT_FOOT_TOES][0], initialOffset[LEFT_FOOT_TOES][1], initialOffset[LEFT_FOOT_TOES][2]));
					Models[LEFT_FOOT_TOES] = glm::rotate(Models[LEFT_FOOT_TOES], rotatePart[LEFT_FOOT_TOES][2], glm::vec3(0, 0, 1));
					Models[LEFT_FOOT_TOES] = glm::rotate(Models[LEFT_FOOT_TOES], rotatePart[LEFT_FOOT_TOES][1], glm::vec3(0, 1, 0));
					Models[LEFT_FOOT_TOES] = glm::rotate(Models[LEFT_FOOT_TOES], rotatePart[LEFT_FOOT_TOES][0], glm::vec3(1, 0, 0));
		//right_upper_thigh
		Models[RIGHT_UPPER_THIGH] = glm::translate(Models[CROTCH], glm::vec3(translatePart[RIGHT_UPPER_THIGH][0], translatePart[RIGHT_UPPER_THIGH][1], translatePart[RIGHT_UPPER_THIGH][2]));
		Models[RIGHT_UPPER_THIGH] = glm::translate(Models[RIGHT_UPPER_THIGH], glm::vec3(initialOffset[RIGHT_UPPER_THIGH][0], initialOffset[RIGHT_UPPER_THIGH][1], initialOffset[RIGHT_UPPER_THIGH][2]));
		Models[RIGHT_UPPER_THIGH] = glm::rotate(Models[RIGHT_UPPER_THIGH], rotatePart[RIGHT_UPPER_THIGH][2], glm::vec3(0, 0, 1));
		Models[RIGHT_UPPER_THIGH] = glm::rotate(Models[RIGHT_UPPER_THIGH], rotatePart[RIGHT_UPPER_THIGH][1], glm::vec3(0, 1, 0));
		Models[RIGHT_UPPER_THIGH] = glm::rotate(Models[RIGHT_UPPER_THIGH], rotatePart[RIGHT_UPPER_THIGH][0], glm::vec3(1, 0, 0));
			//Right_lower_thigh
			Models[RIGHT_LOWER_THIGH] = glm::translate(Models[RIGHT_UPPER_THIGH], glm::vec3(translatePart[RIGHT_LOWER_THIGH][0], translatePart[RIGHT_LOWER_THIGH][1], translatePart[RIGHT_LOWER_THIGH][2]));
			Models[RIGHT_LOWER_THIGH] = glm::translate(Models[RIGHT_LOWER_THIGH], glm::vec3(initialOffset[RIGHT_LOWER_THIGH][0], initialOffset[RIGHT_LOWER_THIGH][1], initialOffset[RIGHT_LOWER_THIGH][2]));
			Models[RIGHT_LOWER_THIGH] = glm::rotate(Models[RIGHT_LOWER_THIGH], rotatePart[RIGHT_LOWER_THIGH][2], glm::vec3(0, 0, 1));
			Models[RIGHT_LOWER_THIGH] = glm::rotate(Models[RIGHT_LOWER_THIGH], rotatePart[RIGHT_LOWER_THIGH][1], glm::vec3(0, 1, 0));
			Models[RIGHT_LOWER_THIGH] = glm::rotate(Models[RIGHT_LOWER_THIGH], rotatePart[RIGHT_LOWER_THIGH][0], glm::vec3(1, 0, 0));
				//Right_foot
				Models[RIGHT_FOOT] = glm::translate(Models[RIGHT_LOWER_THIGH], glm::vec3(translatePart[RIGHT_FOOT][0], translatePart[RIGHT_FOOT][1], translatePart[RIGHT_FOOT][2]));
				Models[RIGHT_FOOT] = glm::translate(Models[RIGHT_FOOT], glm::vec3(initialOffset[RIGHT_FOOT][0], initialOffset[RIGHT_FOOT][1], initialOffset[RIGHT_FOOT][2]));
				Models[RIGHT_FOOT] = glm::rotate(Models[RIGHT_FOOT], rotatePart[RIGHT_FOOT][2], glm::vec3(0, 0, 1));
				Models[RIGHT_FOOT] = glm::rotate(Models[RIGHT_FOOT], rotatePart[RIGHT_FOOT][1], glm::vec3(0, 1, 0));
				Models[RIGHT_FOOT] = glm::rotate(Models[RIGHT_FOOT], rotatePart[RIGHT_FOOT][0], glm::vec3(1, 0, 0));
					//Right_foot_toes
					Models[RIGHT_FOOT_TOES] = glm::translate(Models[RIGHT_FOOT], glm::vec3(translatePart[RIGHT_FOOT_TOES][0], translatePart[RIGHT_FOOT_TOES][1], translatePart[RIGHT_FOOT_TOES][2]));
					Models[RIGHT_FOOT_TOES] = glm::translate(Models[RIGHT_FOOT_TOES], glm::vec3(initialOffset[RIGHT_FOOT_TOES][0], initialOffset[RIGHT_FOOT_TOES][1], initialOffset[RIGHT_FOOT_TOES][2]));
					Models[RIGHT_FOOT_TOES] = glm::rotate(Models[RIGHT_FOOT_TOES], rotatePart[RIGHT_FOOT_TOES][2], glm::vec3(0, 0, 1));
					Models[RIGHT_FOOT_TOES] = glm::rotate(Models[RIGHT_FOOT_TOES], rotatePart[RIGHT_FOOT_TOES][1], glm::vec3(0, 1, 0));
					Models[RIGHT_FOOT_TOES] = glm::rotate(Models[RIGHT_FOOT_TOES], rotatePart[RIGHT_FOOT_TOES][0], glm::vec3(1, 0, 0));

	//Our model is reversed in y axis, so we need to turn it once again for all coordinates to make it correct
	for (int i = 0; i < PARTSNUM; i++) {
		Models[i] = glm::rotate(Models[i], 180.f, glm::vec3(0, 1, 0));
	}
}

void load2Buffer(string obj, int i) {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> materialIndices;

	const char* c = obj.c_str();
	bool res = loadOBJ(c, vertices, uvs, normals, faces[i], mtls[i]);
	cout << "load finished!\n";
	if (!res) printf("load failed\n");

	glGenBuffers(1, &VBOs[i]);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	vertices_size[i] = vertices.size();

	glGenBuffers(1, &uVBOs[i]);
	glBindBuffer(GL_ARRAY_BUFFER, uVBOs[i]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	uvs_size[i] = uvs.size();

	glGenBuffers(1, &nVBOs[i]);
	glBindBuffer(GL_ARRAY_BUFFER, nVBOs[i]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	normals_size[i] = normals.size();
}
//Keyboard for GLFW
void KeyboardGLFW(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//related to animation
	if (key == GLFW_KEY_W) {
		if (action == GLFW_REPEAT) {
			wasdPressed = true;
			direction = 0;
			animateMode = WALK;
		}
		else if (action == GLFW_PRESS) {
			wasdPressed = true;
			direction = 0;
			if (animateMode != WALK) {
				if (animateMode == GANGNAMSTYLE || animateMode == MOONWALK) {
					fromGangnamMoonwalk = true; //disable bgm
				}
				modeChangeExist = true;
				animateMode = WALK;
			}
		}
		else if (action == GLFW_RELEASE) {
			wasdPressed = false;
		}
		
	}
	else if (key == GLFW_KEY_A) {
		if (action == GLFW_REPEAT) {
			animateMode = WALK;
			wasdPressed = true;
			direction = 1;
		}
		else if (action == GLFW_PRESS) {
			wasdPressed = true;
			direction = 1;
			if (animateMode != WALK) {
				if (animateMode == GANGNAMSTYLE || animateMode == MOONWALK) {
					fromGangnamMoonwalk = true; //disable bgm
				}
				modeChangeExist = true;
				animateMode = WALK;
			}
		}
		else if (action == GLFW_RELEASE) {
			wasdPressed = false;
		}
	}
	else if (key == GLFW_KEY_S) {
		if (action == GLFW_REPEAT) {
			animateMode = WALK;
			direction = 2;
			wasdPressed = true;
		}
		else if (action == GLFW_PRESS) {
			wasdPressed = true;
			direction = 2;
			if (animateMode != WALK) {
				if (animateMode == GANGNAMSTYLE || animateMode == MOONWALK) {
					fromGangnamMoonwalk = true;//disable bgm
				}
				modeChangeExist = true;
				animateMode = WALK;
			}
		}
		else if (action == GLFW_RELEASE) {
			wasdPressed = false;
		}
	}
	else if (key == GLFW_KEY_D) {
		if (action == GLFW_REPEAT) {
			animateMode = WALK;
			direction = 3;
			wasdPressed = true;
		}
		else if (action == GLFW_PRESS) {
			wasdPressed = true;
			direction = 3;
			if (animateMode != WALK) {
				if (animateMode == GANGNAMSTYLE || animateMode == MOONWALK) {
					fromGangnamMoonwalk = true;//disable bgm
				}
				modeChangeExist = true;
				animateMode = WALK;
			}
		}
		else if (action == GLFW_RELEASE) {
			wasdPressed = false;
		}
	}
	else if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		if (animateMode != SQUAT) {
			if (animateMode == GANGNAMSTYLE || animateMode == MOONWALK) {
				fromGangnamMoonwalk = true;//disable bgm
			}
			animateMode = SQUAT;
			modeChangeExist = true;
		}
	}
	else if (key == GLFW_KEY_G && action == GLFW_PRESS) {
		if (animateMode != JUMPINGJACK) {
			if (animateMode == GANGNAMSTYLE || animateMode == MOONWALK) {
				fromGangnamMoonwalk = true;//disable bgm
			}
			animateMode = JUMPINGJACK;
			modeChangeExist = true;
		}
	}
	else if (key == GLFW_KEY_H && action == GLFW_PRESS) {
		if (animateMode != GANGNAMSTYLE) {
			animateMode = GANGNAMSTYLE;
			modeChangeExist = true;
		}
	}
	else if (key == GLFW_KEY_J && action == GLFW_PRESS) {
		if (animateMode != MOONWALK) {
			animateMode = MOONWALK;
			modeChangeExist = true;
		}
	}
	else if (key == GLFW_KEY_K && action == GLFW_PRESS) {
		if (animateMode != PUSHUP) {
			if (animateMode == GANGNAMSTYLE || animateMode == MOONWALK) {
				fromGangnamMoonwalk = true; //disable bgm
			}
			animateMode = PUSHUP;
			modeChangeExist = true;
		}
	}
	else if (key == GLFW_KEY_L && action == GLFW_PRESS) {
		if (animateMode != SITUP) {
			if (animateMode == GANGNAMSTYLE || animateMode == MOONWALK) {
				fromGangnamMoonwalk = true;//disable bgm
			}
			animateMode = SITUP;
			modeChangeExist = true;
		}
	}

	//related to viewing and exit
	switch (key) {
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
	case GLFW_KEY_UP:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			viewChange = true; //update view matrix
			eyedistance -= 0.2;
			clip(eyedistance, 5, 40);
		}
		break;
	case GLFW_KEY_DOWN:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			viewChange = true; //update view matrix
			eyedistance += 0.2;
			clip(eyedistance, 5, 40);
		}
		break;
	case GLFW_KEY_LEFT:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			viewChange = true; //update view matrix
			eyeAngley += 2;
		}
		break;
	case GLFW_KEY_RIGHT:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			eyeAngley -= 2;
			viewChange = true;//update view matrix
		}
		break;
	default:
		break;
	}
}

float clip(float &var, float min, float max) {
	if (var < min) {
		var = min;
	}
	else if (var > max) {
		var = max;
	}
	return var;
}

float getTime()
{
	return glfwGetTime();
}

float clampValMin(float x, float clampToMin)
{
	if (x <= clampToMin)
	{
		return clampToMin;
	}
	else
	{
		return x;
	}
}

float clampValMaxMin(float x, float clampToMax, float clampToMin)
{
	if (x >= clampToMax)
	{
		return clampToMax;
	}
	else if (x <= clampToMin)
	{
		return clampToMin;
	}
	else
	{
		return x;
	}
}

float clampValMax(float x, float clampToMax)
{
	if (x >= clampToMax)
	{
		return clampToMax;
	}
	else
	{
		return x;
	}
}
void updateWalkTranslate() {
	//translate based on the angle
	viewChange = true;
	float nowAngle = eyeAngley;
	float radAngle = deg2rad(-nowAngle);
	float xChange;
	float zChange;
	if (lastDirection == 0) {
		//w
		rotatePart[CROTCH][0] = 0;
		rotatePart[CROTCH][1] = 180+nowAngle;
		rotatePart[CROTCH][2] = 0;
		
		zChange = -WALKSPEED * cosf(radAngle);
		xChange = WALKSPEED * sinf(radAngle);
	}
	else if (lastDirection == 1) {
		//a
		rotatePart[CROTCH][0] = 0;
		rotatePart[CROTCH][1] = -90+nowAngle;
		rotatePart[CROTCH][2] = 0;

		xChange = -WALKSPEED * cosf(radAngle);
		zChange = -WALKSPEED * sinf(radAngle);
	}
	else if (lastDirection == 2) {
		//s
		rotatePart[CROTCH][0] = 0;
		rotatePart[CROTCH][1] = nowAngle;
		rotatePart[CROTCH][2] = 0;

		zChange = WALKSPEED * cosf(radAngle);
		xChange = -WALKSPEED * sinf(radAngle);
	}
	else if(lastDirection == 3){
		//d
		rotatePart[CROTCH][0] = 0;
		rotatePart[CROTCH][1] = 90+nowAngle;
		rotatePart[CROTCH][2] = 0;

		zChange = WALKSPEED * sinf(radAngle);
		xChange = WALKSPEED * cosf(radAngle);
	}
	else {
		//nochange anything, walk in place
		xChange = 0;
		zChange = 0;
	}
	//check for map boundary
	if (xChange < 0) {
		if (translatePart[CROTCH][0] >= -TRANSLATEXLIMIT) {
			translatePart[CROTCH][0] += xChange;
		}
	}
	else {
		if (translatePart[CROTCH][0] <= TRANSLATEXLIMIT) {
			translatePart[CROTCH][0] += xChange;
		}
	}
	
	//check for map boundary
	if (zChange < 0) {
		if (translatePart[CROTCH][2] >= -TRANSLATEZLIMIT) {
			translatePart[CROTCH][2] += zChange;
		}
	}
	else {
		if (translatePart[CROTCH][2] <= TRANSLATEZLIMIT) {
			translatePart[CROTCH][2] += zChange;
		}
	}
}


void walk()
{
	float currentTime = glfwGetTime();

	// RIGHTLEG
	if (r_isUp)
	{
		// leg go up
		if (rotatePart[RIGHT_UPPER_THIGH][0] > -30.0f)
		{
			rotatePart[RIGHT_UPPER_THIGH][0] -= 0.7f;
		}

		if (!r_isUp2 && !r_is_return)
		{
			rotatePart[RIGHT_LOWER_THIGH][0] += 1.6f;
			if (rotatePart[RIGHT_LOWER_THIGH][0] >= 35.0f)
			{
				r_isUp2 = true;
			}
		}
		else if (!r_isUp2 && r_is_return)
		{
			if (rotatePart[RIGHT_UPPER_THIGH][0] <= 999.0f)
			{
				rotatePart[RIGHT_LOWER_THIGH][0] += 1.0f;
				if (rotatePart[RIGHT_LOWER_THIGH][0] >= 60.0f)
				{
					r_isUp2 = true;
				}
			}
		}
		else
		{
			rotatePart[RIGHT_LOWER_THIGH][0] -= 1.0f;
			if (rotatePart[RIGHT_LOWER_THIGH][0] <= 0.0f)
			{
				rotatePart[RIGHT_LOWER_THIGH][0] = 0.0f;
				// do nothing
			}
		}

		if (!r_isUp4 && r_is_return)
		{
			rotatePart[RIGHT_FOOT_TOES][0] += 0.5f;
			if (rotatePart[RIGHT_FOOT_TOES][0] >= 0.0f)
			{
				rotatePart[RIGHT_FOOT_TOES][0] = 0.0f;
				r_isUp4 = false;
			}
		}


		if (rotatePart[RIGHT_UPPER_THIGH][0] <= -30.0f && rotatePart[RIGHT_LOWER_THIGH][0] == 0.0f)
		{
			r_isUp = false;
			r_isUp2 = false;
			r_isUp3 = false;
			r_isUp4 = true;
			// system("pause");
		}
	}
	else
	{
		// leg go down
		rotatePart[RIGHT_UPPER_THIGH][0] += 0.7f;
		if (!r_isUp2 && rotatePart[RIGHT_UPPER_THIGH][0] >= 0.0f)
		{
			rotatePart[RIGHT_LOWER_THIGH][0] += 0.6f;

			if (rotatePart[RIGHT_LOWER_THIGH][0] >= 30.0f)
			{
				rotatePart[RIGHT_LOWER_THIGH][0] = 30.0f;
				r_isUp2 = true;
			}
		}
		else if (!r_isUp2 && rotatePart[RIGHT_UPPER_THIGH][0] >= -10.0f)
		{
			rotatePart[RIGHT_LOWER_THIGH][0] += 0.3f;
			// cout << rotatePart[RIGHT_LOWER_THIGH][0] << endl;
			// system("pause");
		} 

		if (!r_isUp3)
		{
			rotatePart[RIGHT_FOOT][0] += 0.8f;

			if (rotatePart[RIGHT_FOOT][0] >= 10.0f)
			{
				rotatePart[RIGHT_FOOT][0] = 10.0f;
				r_isUp3 = true;
			}
		}
		else
		{
			rotatePart[RIGHT_FOOT][0] -= 0.8f;
			if (rotatePart[RIGHT_FOOT][0] <= 0.0f)
			{
				rotatePart[RIGHT_FOOT][0] = 0.0f;
				r_isUp3 = true;
			}
		}

		if (r_isUp4 && rotatePart[RIGHT_FOOT][0] <= 0.0f)
		{
			rotatePart[RIGHT_FOOT_TOES][0] -= 0.5f;
			if (rotatePart[RIGHT_FOOT_TOES][0] <= -20.0f)
			{
				rotatePart[RIGHT_FOOT_TOES][0] = -20.0f;
				r_isUp4 = false;
			}
		}
		


		if (rotatePart[RIGHT_UPPER_THIGH][0] >= 30.0f)
		{
			// system("pause");
			r_isUp = true;
			r_isUp2 = false;
			r_isUp3 = false;
			r_isUp4 = false;
			r_is_return = true;
		}
	}

	// LEFT LEG
	if (l_isUp)
	{
		// leg go up
		if (rotatePart[LEFT_UPPER_THIGH][0] > -30.0f)
		{
			rotatePart[LEFT_UPPER_THIGH][0] -= 0.7f;
		}

		if (!l_isUp2 && !l_is_return)
		{
			rotatePart[LEFT_LOWER_THIGH][0] += 1.6f;
			if (rotatePart[LEFT_LOWER_THIGH][0] >= 35.0f)
			{
				l_isUp2 = true;
			}
		}
		else if (!l_isUp2 && l_is_return)
		{
			if (rotatePart[LEFT_UPPER_THIGH][0] <= 999.0f)
			{
				rotatePart[LEFT_LOWER_THIGH][0] += 1.0f;
				if (rotatePart[LEFT_LOWER_THIGH][0] >= 60.0f)
				{
					l_isUp2 = true;
				}
			}
		}
		else
		{
			rotatePart[LEFT_LOWER_THIGH][0] -= 1.0f;
			if (rotatePart[LEFT_LOWER_THIGH][0] <= 0.0f)
			{
				rotatePart[LEFT_LOWER_THIGH][0] = 0.0f;
				// do nothing
			}
		}

		if (!l_isUp4 && l_is_return)
		{
			rotatePart[LEFT_FOOT_TOES][0] += 0.5f;
			if (rotatePart[LEFT_FOOT_TOES][0] >= 0.0f)
			{
				rotatePart[LEFT_FOOT_TOES][0] = 0.0f;
				l_isUp4 = false;
			}
		}


		if (rotatePart[LEFT_UPPER_THIGH][0] <= -30.0f && rotatePart[LEFT_LOWER_THIGH][0] == 0.0f)
		{
			l_isUp = false;
			l_isUp2 = false;
			l_isUp3 = false;
			l_isUp4 = true;
			SoundEngine->play2D("../Assets/music/robotstep.mp3", false, false, true);
			// system("pause");
		}
	}
	else
	{
		// leg go down
		rotatePart[LEFT_UPPER_THIGH][0] = -rotatePart[RIGHT_UPPER_THIGH][0];
		if (!l_isUp2 && rotatePart[LEFT_UPPER_THIGH][0] >= 0.0f)
		{
			rotatePart[LEFT_LOWER_THIGH][0] += 0.6f;

			if (rotatePart[LEFT_LOWER_THIGH][0] >= 30.0f)
			{
				rotatePart[LEFT_LOWER_THIGH][0] = 30.0f;
				l_isUp2 = true;
			}
		}
		else if (!l_isUp2 && rotatePart[LEFT_UPPER_THIGH][0] >= -10.0f)
		{
			rotatePart[LEFT_LOWER_THIGH][0] += 0.3f;
		}

		if (!l_isUp3)
		{
			rotatePart[LEFT_FOOT][0] += 0.8f;

			if (rotatePart[LEFT_FOOT][0] >= 10.0f)
			{
				rotatePart[LEFT_FOOT][0] = 10.0f;
				l_isUp3 = true;
			}
		}
		else
		{
			rotatePart[LEFT_FOOT][0] -= 0.8f;
			if (rotatePart[LEFT_FOOT][0] <= 0.0f)
			{
				rotatePart[LEFT_FOOT][0] = 0.0f;
				l_isUp3 = true;
			}
		}

		if (l_isUp4 && rotatePart[LEFT_FOOT][0] <= 0.0f)
		{
			rotatePart[LEFT_FOOT_TOES][0] -= 0.5f;
			if (rotatePart[LEFT_FOOT_TOES][0] <= -20.0f)
			{
				rotatePart[LEFT_FOOT_TOES][0] = -20.0f;
				l_isUp4 = false;
			}
		}



		if (rotatePart[LEFT_UPPER_THIGH][0] >= 30.0f)
		{
			// system("pause");
			l_isUp = true;
			l_isUp2 = false;
			l_isUp3 = false;
			l_isUp4 = false;
			l_is_return = true;
			SoundEngine->play2D("../Assets/music/robotstep.mp3", false, false, true);
		}
	}

	// RIGHT HAND
	rotatePart[RIGHT_UPPER_ARM][0] = -rotatePart[RIGHT_UPPER_THIGH][0];
	if (rotatePart[RIGHT_UPPER_ARM][0] > 0) // when move back
	{
		rotatePart[RIGHT_LOWER_ARM][0] = clampValMax(rotatePart[RIGHT_LOWER_ARM][0] + 0.8f, 0.0f);
	}
	else
	{
		rotatePart[RIGHT_LOWER_ARM][0] -= 0.5f;
	}

	rotatePart[LEFT_UPPER_ARM][0] = rotatePart[RIGHT_UPPER_THIGH][0];
	if (rotatePart[LEFT_UPPER_ARM][0] > 0) // when move back
	{
		rotatePart[LEFT_LOWER_ARM][0] = clampValMax(rotatePart[LEFT_LOWER_ARM][0] + 0.8f, 0.0f);
		rotatePart[ABS][1] = clampValMax(rotatePart[ABS][1] + 0.1f, 3.0f);
	}
	else
	{
		rotatePart[LEFT_LOWER_ARM][0] -= 0.5f;
		rotatePart[ABS][1] = clampValMin(rotatePart[ABS][1] - 0.1f, -3.0f);
	}

	//translatePart[CROTCH][1] = clampValMin(sin((currentTime - startTime)  * 2 * 3.14 / 1.5) * 0.1f, 0.0f);
	//rotatePart[ABS][1] = -sin((currentTime - startTime) * 2 * 3.14 / 3.0f) * 1.5f;

	if (abs(rotatePart[RIGHT_UPPER_THIGH][0]) < 2.0) {
		walkSafeStop = true;
	}
	else {
		walkSafeStop = false;
	}

	/*if (abs(rotatePart[RIGHT_UPPER_THIGH][0]) < 3.0 && abs(rotatePart[RIGHT_UPPER_THIGH][0]) > 0.0) {
		SoundEngine->play2D("../Assets/music/robotstep.mp3", false,false, true);
	}*/

	//return rotatePart[RIGHT_UPPER_THIGH][0];
	
}

void jumpingJack()
{
	if (jjCount != 5)
	{
		if (!isOpen)
		{
			if (pause)
			{
				Sleep(400);
			}
			pause = false;


			if (!squat1)
			{
				rotatePart[LEFT_UPPER_ARM][2] += 10.0f;
				rotatePart[LEFT_LOWER_ARM][1] += 10.0f;

				rotatePart[LEFT_UPPER_THIGH][0] -= 1.0f;
				rotatePart[LEFT_LOWER_THIGH][0] += 3.0f;
				rotatePart[LEFT_FOOT][0] -= 3.0f;

				if (rotatePart[LEFT_LOWER_ARM][1] >= 80.0f)
				{
					rotatePart[LEFT_LOWER_ARM][1] = 80.0f;
				}
				translatePart[CROTCH][1] = clampValMax(translatePart[CROTCH][1] + 0.3, 1.0f);

				if (rotatePart[LEFT_FOOT][0] <= -20.0f)
				{
					rotatePart[LEFT_FOOT][0] = -20.0f;

					if (rotatePart[LEFT_LOWER_THIGH][0] >= 25.0f)
					{
						rotatePart[LEFT_LOWER_THIGH][0] = 25.0f;
						if (rotatePart[LEFT_UPPER_THIGH][0] <= -10.0f)
						{
							rotatePart[LEFT_UPPER_THIGH][0] = -10.0f;
							squat1 = true;
						}
					}
				}

				rotatePart[RIGHT_UPPER_ARM][2] = -rotatePart[LEFT_UPPER_ARM][2];
				rotatePart[RIGHT_LOWER_ARM][1] = -rotatePart[LEFT_LOWER_ARM][1];
				rotatePart[RIGHT_UPPER_THIGH][2] = -rotatePart[LEFT_UPPER_THIGH][2];
				rotatePart[RIGHT_UPPER_THIGH][0] = rotatePart[LEFT_UPPER_THIGH][0];
				rotatePart[RIGHT_LOWER_THIGH][0] = rotatePart[LEFT_LOWER_THIGH][0];
				rotatePart[RIGHT_FOOT][0] = rotatePart[LEFT_FOOT][0];
			}
			else if (!squat2)
			{
				rotatePart[LEFT_UPPER_ARM][2] += 10.0f;
				rotatePart[LEFT_LOWER_ARM][0] -= 10.0f;
				rotatePart[LEFT_UPPER_THIGH][0] += 1.0f;
				rotatePart[LEFT_LOWER_THIGH][0] -= 3.0f;
				rotatePart[LEFT_FOOT][0] += 10.0f;
				rotatePart[LEFT_FOOT][2] -= 3.0f;
				// rotatePart[LEFT_FOOT_TOES][0] -= 5.f;

				if (rotatePart[LEFT_UPPER_ARM][2] >= 140.0f)
				{
					rotatePart[LEFT_UPPER_ARM][2] = 140.0f;
				}

				if (rotatePart[LEFT_LOWER_ARM][0] <= -80.0f)
				{
					rotatePart[LEFT_LOWER_ARM][0] = -80.0f;
				}

				if (rotatePart[LEFT_FOOT][0] >= 0)
				{
					rotatePart[LEFT_FOOT][0] = 0.0f;
				}

				/*if (rotatePart[LEFT_FOOT_TOES][0] <= -35.0f)
				{
					rotatePart[LEFT_FOOT_TOES][0] = -35.0f;
				}*/

				if (rotatePart[LEFT_FOOT][2] <= -10.0f)
				{
					rotatePart[LEFT_FOOT][2] = -10.0f;
				}

				if (rotatePart[LEFT_UPPER_ARM][2] >= 120.0f)
				{
					rotatePart[LEFT_UPPER_ARM][2] = 120.0f;
				}

				rotatePart[LEFT_UPPER_THIGH][2] += 1.0f;

				translatePart[CROTCH][1] = clampValMin(translatePart[CROTCH][1] - 0.3, 0.0f);

				if (rotatePart[LEFT_UPPER_THIGH][2] >= 10.0f)
				{
					rotatePart[LEFT_UPPER_THIGH][2] = 10.0f;
					if (rotatePart[LEFT_UPPER_THIGH][0] >= 0.0f)
					{
						rotatePart[LEFT_UPPER_THIGH][0] = 0.0f;
						if (rotatePart[LEFT_LOWER_THIGH][0] <= 0.0f)
						{
							translatePart[CROTCH][1] = 0.0f;
							rotatePart[LEFT_LOWER_THIGH][0] = 0.0f;
							isOpen = true;
							squat1 = false;
							squat2 = false;
							pause = true;
							SoundEngine->play2D("../Assets/music/robotstep.mp3", false, false, true);
						}
					}
				}

				rotatePart[RIGHT_UPPER_ARM][2] = -rotatePart[LEFT_UPPER_ARM][2];
				rotatePart[RIGHT_LOWER_ARM][0] = rotatePart[LEFT_LOWER_ARM][0];
				rotatePart[RIGHT_UPPER_THIGH][0] = rotatePart[LEFT_UPPER_THIGH][0];
				rotatePart[RIGHT_LOWER_THIGH][0] = rotatePart[LEFT_LOWER_THIGH][0];
				rotatePart[RIGHT_FOOT][0] = rotatePart[LEFT_FOOT][0];
				rotatePart[RIGHT_FOOT][2] = -rotatePart[LEFT_FOOT][2];
				rotatePart[RIGHT_FOOT_TOES][0] = rotatePart[LEFT_FOOT_TOES][0];
				rotatePart[RIGHT_UPPER_THIGH][2] = -rotatePart[LEFT_UPPER_THIGH][2];
			}
		}
		else
		{
			if (pause)
			{
				Sleep(300);
			}
			pause = false;


			if (!squat1)
			{
				rotatePart[LEFT_UPPER_ARM][2] -= 10.0f;
				rotatePart[LEFT_LOWER_ARM][1] -= 5.0f;
				rotatePart[LEFT_LOWER_ARM][0] += 10.0f;

				rotatePart[LEFT_UPPER_THIGH][0] -= 1.0f;
				rotatePart[LEFT_LOWER_THIGH][0] += 3.0f;

				if (rotatePart[LEFT_LOWER_ARM][0] >= 0.0f)
				{
					rotatePart[LEFT_LOWER_ARM][0] = 0.0f;
				}

				if (rotatePart[LEFT_LOWER_THIGH][0] >= 15.0f)
				{
					rotatePart[LEFT_LOWER_THIGH][0] = 15.0f;
					if (rotatePart[LEFT_UPPER_THIGH][0] <= -10.0f)
					{
						rotatePart[LEFT_UPPER_THIGH][0] = -10.0f;
						squat1 = true;
					}
				}

				translatePart[CROTCH][1] = clampValMax(translatePart[CROTCH][1] + 0.3, 1.0f);

				rotatePart[RIGHT_UPPER_ARM][2] = -rotatePart[LEFT_UPPER_ARM][2];
				rotatePart[RIGHT_LOWER_ARM][0] = rotatePart[LEFT_LOWER_ARM][0];
				rotatePart[RIGHT_UPPER_THIGH][2] = -rotatePart[LEFT_UPPER_THIGH][2];
				rotatePart[RIGHT_UPPER_THIGH][0] = rotatePart[LEFT_UPPER_THIGH][0];
				rotatePart[RIGHT_LOWER_THIGH][0] = rotatePart[LEFT_LOWER_THIGH][0];
				rotatePart[RIGHT_FOOT][0] = rotatePart[LEFT_FOOT][0];
			}
			else if (!squat2)
			{
				rotatePart[LEFT_UPPER_ARM][2] -= 10.0f;
				rotatePart[LEFT_LOWER_ARM][1] -= 10.0f;
				rotatePart[LEFT_UPPER_THIGH][0] += 1.0f;
				rotatePart[LEFT_LOWER_THIGH][0] -= 3.0f;
				rotatePart[LEFT_FOOT][0] += 10.0f;
				rotatePart[LEFT_FOOT][2] += 3.0f;
				// rotatePart[LEFT_FOOT_TOES][0] += 5.0f;

				if (rotatePart[LEFT_UPPER_ARM][2] <= 0.0f)
				{
					rotatePart[LEFT_UPPER_ARM][2] = 0.0f;
				}

				if (rotatePart[LEFT_LOWER_ARM][1] <= 0.0f)
				{
					rotatePart[LEFT_LOWER_ARM][1] = 0.0f;
				}

				if (rotatePart[LEFT_FOOT][0] >= 0.0f)
				{
					rotatePart[LEFT_FOOT][0] = 0.0f;
				}

				/*if (rotatePart[LEFT_FOOT_TOES][0] >= 0.0f)
				{
					rotatePart[LEFT_FOOT_TOES][0] = 0.0f;
				}*/

				if (rotatePart[LEFT_FOOT][2] >= 0.0f)
				{
					rotatePart[LEFT_FOOT][2] = 0.0f;
				}

				rotatePart[LEFT_UPPER_THIGH][2] -= 1.0f;

				translatePart[CROTCH][1] = clampValMin(translatePart[CROTCH][1] - 0.3, 0.0f);


				if (rotatePart[LEFT_UPPER_THIGH][2] <= 0.0f)
				{
					rotatePart[LEFT_UPPER_THIGH][2] = 0.0f;
					if (rotatePart[LEFT_UPPER_THIGH][0] >= 0.0f)
					{
						rotatePart[LEFT_UPPER_THIGH][0] = 0.0f;
						if (rotatePart[LEFT_LOWER_THIGH][0] <= 0.0f)
						{
							translatePart[CROTCH][1] = 0.0f;
							rotatePart[LEFT_LOWER_THIGH][0] = 0.0f;
							isOpen = false;
							squat1 = false;
							squat2 = false;
							pause = true;
							jjCount++;
							SoundEngine->play2D("../Assets/music/robotstep.mp3", false, false, true);
						}
					}
				}

				rotatePart[RIGHT_UPPER_ARM][2] = -rotatePart[LEFT_UPPER_ARM][2];
				rotatePart[RIGHT_LOWER_ARM][1] = -rotatePart[LEFT_LOWER_ARM][1];
				rotatePart[RIGHT_UPPER_THIGH][0] = rotatePart[LEFT_UPPER_THIGH][0];
				rotatePart[RIGHT_LOWER_THIGH][0] = rotatePart[LEFT_LOWER_THIGH][0];
				rotatePart[RIGHT_FOOT][0] = rotatePart[LEFT_FOOT][0];
				rotatePart[RIGHT_FOOT][2] = -rotatePart[LEFT_FOOT][2];
				rotatePart[RIGHT_FOOT_TOES][0] = rotatePart[LEFT_FOOT_TOES][0];
				rotatePart[RIGHT_UPPER_THIGH][2] = -rotatePart[LEFT_UPPER_THIGH][2];
			}
		}
		jjSaveStop = false;
	}
	else
	{
		jjSaveStop = true;
	}
}

float dGangnamFunction(float followSpeed, float followTarget, float imiDist)
{
	return followSpeed * imiDist / abs(followTarget);
}

void gangnamStyle()
{
	float speed1 = 2.0f;
	float speed2 = 2.2f;
	if (!setReadyGS)
	{
		// add translate part here

		rotatePart[LEFT_UPPER_THIGH][0] -= 1.0f;
		rotatePart[LEFT_UPPER_THIGH][1] += 1.3f;
		rotatePart[LEFT_LOWER_THIGH][0] += 1.3f;

		if (rotatePart[LEFT_UPPER_THIGH][0] <= -15.0f)
		{
			rotatePart[LEFT_UPPER_THIGH][0] = -15.0f;
			rotatePart[LEFT_UPPER_THIGH][1] = 20.0f;
			rotatePart[LEFT_LOWER_THIGH][0] = 20.0f;
		}

		rotatePart[RIGHT_UPPER_THIGH][0] = rotatePart[LEFT_UPPER_THIGH][0];
		rotatePart[RIGHT_UPPER_THIGH][1] = -rotatePart[LEFT_UPPER_THIGH][1];
		rotatePart[RIGHT_LOWER_THIGH][0] = rotatePart[LEFT_LOWER_THIGH][0];

		rotatePart[LEFT_UPPER_ARM][0] -= 0.8f;
		if (rotatePart[LEFT_UPPER_ARM][0] <= -10.0f)
		{
			rotatePart[LEFT_UPPER_ARM][2] = clampValMin(rotatePart[LEFT_UPPER_ARM][2] - 1.5f, -20.0f);
			rotatePart[LEFT_LOWER_ARM][0] = clampValMin(rotatePart[LEFT_LOWER_ARM][0] - 4.0f, -40.0f);
			rotatePart[LEFT_LOWER_ARM][1] = clampValMin(rotatePart[LEFT_LOWER_ARM][1] - 2.5f, -90.0f);
			// rotatePart[LEFT_LOWER_ARM][2] = clampValMin(rotatePart[LEFT_LOWER_ARM][2] - 2.5f, -30.0f);
		}

		if (rotatePart[LEFT_UPPER_ARM][0] <= -40.0f)
		{
			rotatePart[LEFT_UPPER_ARM][0] = -40.0f;
			rotatePart[LEFT_UPPER_ARM][2] = -20.0f;

			rotatePart[LEFT_LOWER_ARM][0] = -40.0f;
			rotatePart[LEFT_LOWER_ARM][1] = -90.0f;
			// rotatePart[LEFT_LOWER_ARM][2] = -30.0f;
			setReadyGS = true;
			
		}

		rotatePart[RIGHT_UPPER_ARM][0] = 0.7 * rotatePart[LEFT_UPPER_ARM][0];
		rotatePart[RIGHT_UPPER_ARM][2] = -rotatePart[LEFT_UPPER_ARM][2];
		rotatePart[RIGHT_LOWER_ARM][0] = rotatePart[LEFT_LOWER_ARM][0];
		rotatePart[RIGHT_LOWER_ARM][1] = -rotatePart[LEFT_LOWER_ARM][1];
		rotatePart[RIGHT_LOWER_ARM][2] = rotatePart[LEFT_LOWER_ARM][2];

	}
	else
	{
		float currentTime = getTime();
		/*rotatePart[LEFT_PALM][2] = sin((currentTime - startTime) * 2 * 3.14 / 0.6) * 30.0f;
		rotatePart[RIGHT_PALM][2] = -rotatePart[LEFT_PALM][2];*/

		float absSpeed = 2.0f;
		float absMaxRot = 5.0f;
		float handBend = 30.0f;
		float handTarget = 60.0f;
		float jumpVal = 0.1f;

		if (jumpLeft)
		{
			// jump left
			/*rotatePart[LEFT_UPPER_THIGH][0] = -40.0f;
			rotatePart[LEFT_UPPER_THIGH][1] = 50.0f;
			rotatePart[LEFT_UPPER_THIGH][2] = 20.0f;
			rotatePart[LEFT_LOWER_THIGH][0] = 70.0f;

			rotatePart[RIGHT_UPPER_THIGH][0] = 10.0f;
			rotatePart[RIGHT_UPPER_THIGH][1] = 30.0f;
			rotatePart[RIGHT_UPPER_THIGH][2] = -30.0f;
			rotatePart[RIGHT_LOWER_THIGH][0] = 0.0f;*/
			rotatePart[ABS][1] = clampValMax(rotatePart[ABS][1] + absSpeed, absMaxRot);

			if (isJump)
			{
				translatePart[CROTCH][1] += jumpVal;
				bool leftReady = false;
				bool rightReady = false;
				float pivotSpeed = speed1;
				float pivotTarget = -40.0f;
				float pivotSpeed2 = speed2;
				float pivotTarget2 = 10.0f;
				rotatePart[LEFT_PALM][2] = clampValMax(rotatePart[LEFT_PALM][2] + dGangnamFunction(pivotSpeed, pivotTarget, handTarget), handBend);
				rotatePart[RIGHT_PALM][2] = -rotatePart[LEFT_PALM][2];
				rotatePart[LEFT_UPPER_THIGH][0] -= pivotSpeed;
				rotatePart[LEFT_UPPER_THIGH][1] += dGangnamFunction(pivotSpeed, pivotTarget, 50.0f);
				rotatePart[LEFT_UPPER_THIGH][2] += dGangnamFunction(pivotSpeed, pivotTarget, 20.0f);
				rotatePart[LEFT_LOWER_THIGH][0] += dGangnamFunction(pivotSpeed, pivotTarget, 70.0f);

				rotatePart[RIGHT_UPPER_THIGH][0] += pivotSpeed2;
				rotatePart[RIGHT_UPPER_THIGH][1] = clampValMax(rotatePart[RIGHT_UPPER_THIGH][1] + dGangnamFunction(pivotSpeed2, pivotTarget2, 30.0f), 30.0f);
				rotatePart[RIGHT_UPPER_THIGH][2] = clampValMin(rotatePart[RIGHT_UPPER_THIGH][2] - dGangnamFunction(pivotSpeed2, pivotTarget2, 10.0f), -10.0f);

				if (rotatePart[LEFT_UPPER_THIGH][0] <= pivotTarget)
				{
					rotatePart[LEFT_UPPER_THIGH][0] = pivotTarget;
					rotatePart[LEFT_UPPER_THIGH][1] = 50.0f;
					rotatePart[LEFT_UPPER_THIGH][2] = 15.0f;
					rotatePart[LEFT_LOWER_THIGH][0] = 65.0f;
					leftReady = true;
				}

				if (rotatePart[RIGHT_UPPER_THIGH][0] >= pivotTarget2)
				{
					rotatePart[RIGHT_UPPER_THIGH][0] = pivotTarget2;
					rotatePart[RIGHT_UPPER_THIGH][1] = 30.0f;
					rotatePart[RIGHT_UPPER_THIGH][2] = -10.0f;
					rightReady = true;
				}

				if (leftReady && rightReady)
				{
					isJump = false;
				}
			}
			else
			{
				translatePart[CROTCH][1] = clampValMin(translatePart[CROTCH][1] - jumpVal, 0.0f);
				bool leftReady = false;
				bool rightReady = false;
				// return to ori pos
				float pivotSpeed = speed1;
				float pivotTarget = -15.0f;
				float pivotSpeed2 = speed2;
				float pivotTarget2 = -15.0f;
				rotatePart[LEFT_PALM][2] = clampValMin(rotatePart[LEFT_PALM][2] - dGangnamFunction(pivotSpeed, pivotTarget, handTarget), -handBend);
				rotatePart[RIGHT_PALM][2] = -rotatePart[LEFT_PALM][2];
				rotatePart[LEFT_UPPER_THIGH][0] += pivotSpeed;
				rotatePart[LEFT_UPPER_THIGH][1] = clampValMin(rotatePart[LEFT_UPPER_THIGH][1] - dGangnamFunction(pivotSpeed, pivotTarget, 35.0f), 20.0f);
				rotatePart[LEFT_UPPER_THIGH][2] = clampValMin(rotatePart[LEFT_UPPER_THIGH][2] - dGangnamFunction(pivotSpeed, pivotTarget, 15.0f), 0.0f);
				rotatePart[LEFT_LOWER_THIGH][0] = clampValMin(rotatePart[LEFT_LOWER_THIGH][0] - dGangnamFunction(pivotSpeed, pivotTarget, 35.0f), 20.0f);

				rotatePart[RIGHT_UPPER_THIGH][0] -= pivotSpeed2;
				rotatePart[RIGHT_UPPER_THIGH][1] = clampValMin(rotatePart[RIGHT_UPPER_THIGH][1] -= dGangnamFunction(pivotSpeed2, pivotTarget2, 30.0f), -20.0f);
				rotatePart[RIGHT_UPPER_THIGH][2] = clampValMax(rotatePart[RIGHT_UPPER_THIGH][2] += dGangnamFunction(pivotSpeed2, pivotTarget2, 5.0f), 0.0f);
				rotatePart[RIGHT_LOWER_THIGH][0] = clampValMax(rotatePart[RIGHT_LOWER_THIGH][0] += dGangnamFunction(pivotSpeed, pivotTarget, 10.0f), 20.0f);

				if (rotatePart[LEFT_UPPER_THIGH][0] >= pivotTarget)
				{
					rotatePart[LEFT_UPPER_THIGH][0] = pivotTarget;
					rotatePart[LEFT_UPPER_THIGH][1] = 20.0f;
					rotatePart[LEFT_UPPER_THIGH][2] = 0.0f;
					rotatePart[LEFT_LOWER_THIGH][0] = 20.0f;
					leftReady = true;
				}

				// cout << "-----> " << rotatePart[RIGHT_UPPER_THIGH][0] << endl;
				if (rotatePart[RIGHT_UPPER_THIGH][0] <= pivotTarget2)
				{
					rotatePart[RIGHT_UPPER_THIGH][0] = pivotTarget2;
					rotatePart[RIGHT_UPPER_THIGH][1] = -20.0f;
					rotatePart[RIGHT_UPPER_THIGH][2] = 0.0f;
					rotatePart[RIGHT_LOWER_THIGH][0] = 20.0f;
					rightReady = true;
				}

				if (leftReady && rightReady)
				{
					if (counter % 4 != 0) jumpLeft = !jumpLeft;
					counter++;
					isJump = true; // true
				}

				if (leftReady && rightReady)
				{
					gsSaveStop = true;
				}
				else
				{
					gsSaveStop = false;
				}
			}
		}
		else
		{
			rotatePart[ABS][1] = clampValMin(rotatePart[ABS][1] - absSpeed, -absMaxRot);
			rotatePart[RIGHT_PALM][2] = -rotatePart[LEFT_PALM][2];
			//cout << rotatePart[LEFT_UPPER_THIGH][0] << " " << rotatePart[LEFT_UPPER_THIGH][1] << " " << rotatePart[LEFT_LOWER_THIGH][0]<< endl;
			// jump right
			/*rotatePart[RIGHT_UPPER_THIGH][0] = -40.0f;
			rotatePart[RIGHT_UPPER_THIGH][1] = -50.0f;
			rotatePart[RIGHT_UPPER_THIGH][2] = -20.0f;
			rotatePart[RIGHT_LOWER_THIGH][0] = 70.0f;

			rotatePart[LEFT_UPPER_THIGH][0] = 10.0f;
			rotatePart[LEFT_UPPER_THIGH][1] = -30.0f;
			rotatePart[LEFT_UPPER_THIGH][2] = 30.0f;
			rotatePart[LEFT_LOWER_THIGH][0] = 0.0f;*/

			if (isJump)
			{
				translatePart[CROTCH][1] += jumpVal;
				bool leftReady = false;
				bool rightReady = false;
				float pivotSpeed = speed1;
				float pivotTarget = -40.0f;
				float pivotSpeed2 = speed2;
				float pivotTarget2 = 10.0f;
				rotatePart[LEFT_PALM][2] = clampValMax(rotatePart[LEFT_PALM][2] + dGangnamFunction(pivotSpeed, pivotTarget, handTarget), handBend);
				rotatePart[RIGHT_UPPER_THIGH][0] -= pivotSpeed;
				rotatePart[RIGHT_UPPER_THIGH][1] -= dGangnamFunction(pivotSpeed, pivotTarget, 50.0f);
				rotatePart[RIGHT_UPPER_THIGH][2] -= dGangnamFunction(pivotSpeed, pivotTarget, 20.0f);
				rotatePart[RIGHT_LOWER_THIGH][0] += dGangnamFunction(pivotSpeed, pivotTarget, 70.0f);

				rotatePart[LEFT_UPPER_THIGH][0] += pivotSpeed2;
				rotatePart[LEFT_UPPER_THIGH][1] = clampValMin(rotatePart[LEFT_UPPER_THIGH][1] - dGangnamFunction(pivotSpeed2, pivotTarget2, 30.0f), -30.0f);
				rotatePart[LEFT_UPPER_THIGH][2] = clampValMax(rotatePart[LEFT_UPPER_THIGH][2] + dGangnamFunction(pivotSpeed2, pivotTarget2, 10.0f), 10.0f);

				if (rotatePart[RIGHT_UPPER_THIGH][0] <= pivotTarget)
				{
					rotatePart[RIGHT_UPPER_THIGH][0] = pivotTarget;
					rotatePart[RIGHT_UPPER_THIGH][1] = -50.0f;
					rotatePart[RIGHT_UPPER_THIGH][2] = -15.0f;
					rotatePart[RIGHT_LOWER_THIGH][0] = 65.0f;
					leftReady = true;
				}

				if (rotatePart[LEFT_UPPER_THIGH][0] >= pivotTarget2)
				{
					rotatePart[LEFT_UPPER_THIGH][0] = pivotTarget2;
					rotatePart[LEFT_UPPER_THIGH][1] = -30.0f;
					rotatePart[LEFT_UPPER_THIGH][2] = 10.0f;
					rightReady = true;
				}

				if (leftReady && rightReady)
				{
					isJump = false; // false
				}
			}
			else
			{
				translatePart[CROTCH][1] = clampValMin(translatePart[CROTCH][1] - jumpVal, 0.0f);
				bool leftReady = false;
				bool rightReady = false;
				// return to ori pos
				float pivotSpeed = speed1;
				float pivotTarget = -15.0f;
				float pivotSpeed2 = speed2;
				float pivotTarget2 = -15.0f;
				rotatePart[LEFT_PALM][2] = clampValMin(rotatePart[LEFT_PALM][2] - dGangnamFunction(pivotSpeed, pivotTarget, handTarget), -handBend);
				rotatePart[RIGHT_PALM][2] = -rotatePart[LEFT_PALM][2];
				rotatePart[RIGHT_UPPER_THIGH][0] += pivotSpeed;
				rotatePart[RIGHT_UPPER_THIGH][1] = clampValMax(rotatePart[RIGHT_UPPER_THIGH][1] + dGangnamFunction(pivotSpeed, pivotTarget, 35.0f), -20.0f);
				rotatePart[RIGHT_UPPER_THIGH][2] = clampValMax(rotatePart[RIGHT_UPPER_THIGH][2] + dGangnamFunction(pivotSpeed, pivotTarget, 15.0f), 0.0f);
				rotatePart[RIGHT_LOWER_THIGH][0] = clampValMin(rotatePart[RIGHT_LOWER_THIGH][0] - dGangnamFunction(pivotSpeed, pivotTarget, 35.0f), 20.0f);

				rotatePart[LEFT_UPPER_THIGH][0] -= pivotSpeed2;
				rotatePart[LEFT_UPPER_THIGH][1] = clampValMax(rotatePart[LEFT_UPPER_THIGH][1] + dGangnamFunction(pivotSpeed2, pivotTarget2, 30.0f), 20.0f);
				rotatePart[LEFT_UPPER_THIGH][2] = clampValMin(rotatePart[LEFT_UPPER_THIGH][2] - dGangnamFunction(pivotSpeed2, pivotTarget2, 5.0f), 0.0f);
				rotatePart[LEFT_LOWER_THIGH][0] = clampValMax(rotatePart[LEFT_LOWER_THIGH][0] += dGangnamFunction(pivotSpeed, pivotTarget, 10.0f), 20.0f);

				if (rotatePart[RIGHT_UPPER_THIGH][0] >= pivotTarget)
				{
					rotatePart[RIGHT_UPPER_THIGH][0] = pivotTarget;
					rotatePart[RIGHT_UPPER_THIGH][1] = -20.0f;
					rotatePart[RIGHT_UPPER_THIGH][2] = 0.0f;
					rotatePart[RIGHT_LOWER_THIGH][0] = 20.0f;
					leftReady = true;
				}

				// cout << "-----> " << rotatePart[RIGHT_UPPER_THIGH][0] << endl;
				if (rotatePart[LEFT_UPPER_THIGH][0] <= pivotTarget2)
				{
					rotatePart[LEFT_UPPER_THIGH][0] = pivotTarget2;
					rotatePart[LEFT_UPPER_THIGH][1] = 20.0f;
					rotatePart[LEFT_UPPER_THIGH][2] = 0.0f;
					rotatePart[LEFT_LOWER_THIGH][0] = 20.0f;
					rightReady = true;
				}

				if (leftReady && rightReady)
				{
					if (counter % 4 != 0) jumpLeft = !jumpLeft;
					counter++;
					isJump = true; // true
				}

				if (leftReady && rightReady)
				{
					gsSaveStop = true;
				}
				else
				{
					gsSaveStop = false;
				}
			}

		}
	}
}

void moonWalk()
{
	if (!setReady)
	{
		// set to start position
		// rotatePart[LEFT_LOWER_THIGH][0] = 40.0f;
		// rotatePart[LEFT_FOOT_TOES][0] = -30.0f;


		rotatePart[LEFT_LOWER_THIGH][0] += 1.0f;
		rotatePart[LEFT_FOOT_TOES][0] -= 0.75f;
		rotatePart[ABS][0] += 0.25f;

		if (rotatePart[LEFT_LOWER_THIGH][0] >= 40.0f)
		{
			rotatePart[LEFT_LOWER_THIGH][0] = 40.0f;
			rotatePart[LEFT_FOOT_TOES][0] = -30.0f;
			rotatePart[ABS][0] = 10.0f;
			setReady = true;
			Sleep(300);
		}
	}
	else
	{
		if (speedDirection > 0) {
			if ((translatePart[CROTCH][2] + speedDirection) < TRANSLATEZLIMIT) {
				translatePart[CROTCH][2] += speedDirection;
			}

		}
		else{
			if ((translatePart[CROTCH][2] + speedDirection) > -TRANSLATEZLIMIT) {
				translatePart[CROTCH][2] += speedDirection;
			}
		}
		//when in between 0 and 180, don't change
		if (mwcount % 10 == 0)
		{

			rotatePart[CROTCH][1] = clampValMin(rotatePart[CROTCH][1] - spinSpeed, 0.0f);
			speedDirection = -0.2f;
		}
		else if (mwcount % 5 == 0)
		{
			rotatePart[CROTCH][1] = clampValMax(rotatePart[CROTCH][1] + spinSpeed, 180.0f);
			speedDirection = 0.2f;
		}
		float currentTime = getTime();
		float x = clampValMin(sin((currentTime - startTime) * 2 * 3.14 / 2) * 30.0f, 0.0f);
		rotatePart[HEAD][0] = -x;
		rotatePart[NECK][0] = x;
		if (!l_slide)
		{
			if (!moon_reset)
			{
				bool leftReady = false;
				bool rightReady = false;
				// left leg move front
				/*rotatePart[LEFT_UPPER_THIGH][0] = -45.0f;
				rotatePart[LEFT_FOOT][0] = 40.0f;*/

				float pivotSpeed = 1.0f;
				float pivotTarget = -25.0f;
				rotatePart[LEFT_UPPER_THIGH][0] -= pivotSpeed;
				rotatePart[LEFT_FOOT][0] = clampValMax(rotatePart[LEFT_FOOT][0] + dGangnamFunction(pivotSpeed, pivotTarget, 40.0f), 40.0f);
				rotatePart[LEFT_FOOT_TOES][0] = clampValMax(rotatePart[LEFT_FOOT_TOES][0] - dGangnamFunction(pivotSpeed, pivotTarget, 10.0f), -50.0f);

				if (rotatePart[LEFT_UPPER_THIGH][0] <= pivotTarget)
				{
					rotatePart[LEFT_UPPER_THIGH][0] = pivotTarget;
					rotatePart[LEFT_FOOT][0] = 40.0f;
					rotatePart[LEFT_FOOT_TOES][0] = -50.0f;
					leftReady = true;
				}

				float pivotSpeed2 = 1.0f;
				float pivotTarget2 = 30.0f;
				/*rotatePart[RIGHT_UPPER_THIGH][0] = 40.0f;
				rotatePart[RIGHT_FOOT][0] = -30.0f;*/

				rotatePart[RIGHT_UPPER_THIGH][0] += pivotSpeed2;
				rotatePart[RIGHT_FOOT][0] = clampValMin(rotatePart[RIGHT_FOOT][0] - dGangnamFunction(pivotSpeed2, pivotTarget2, 30.0f), -30.0f);

				if (rotatePart[RIGHT_UPPER_THIGH][0] >= pivotTarget2 || rotatePart[LEFT_UPPER_THIGH][0] <= pivotTarget)
				{
					// rotatePart[RIGHT_UPPER_THIGH][0] = pivotTarget2;
					// rotatePart[RIGHT_FOOT][0] = -30.0f;
					rightReady = true;
				}

				rotatePart[LEFT_UPPER_ARM][0] = -rotatePart[LEFT_UPPER_THIGH][0];
				rotatePart[RIGHT_UPPER_ARM][0] = -rotatePart[LEFT_UPPER_ARM][0];

				if (leftReady && rightReady)
				{
					moon_reset = true;
				}
			}
			else
			{
				bool leftReady = false;
				bool rightReady = false;
				// reset
				/*rotatePart[LEFT_UPPER_THIGH][0] = 0.0f;
				rotatePart[LEFT_LOWER_THIGH][0] = 0.0f;
				rotatePart[LEFT_FOOT][0] = 0.0f;
				rotatePart[LEFT_FOOT_TOES][0] = 0.0f;*/

				float pivotSpeed = 1.7f;
				float pivotTarget = 0.0f;
				rotatePart[LEFT_UPPER_THIGH][0] += pivotSpeed;
				rotatePart[LEFT_LOWER_THIGH][0] = clampValMin(rotatePart[LEFT_LOWER_THIGH][0] - dGangnamFunction(pivotSpeed, pivotTarget + 25.0f, 40.0f), 0.0f);
				rotatePart[LEFT_FOOT][0] = clampValMin(rotatePart[LEFT_FOOT][0] - dGangnamFunction(pivotSpeed, pivotTarget + 25.0f, 40.0f), 0.0f);
				rotatePart[LEFT_FOOT_TOES][0] = clampValMax(rotatePart[LEFT_FOOT][0] + dGangnamFunction(pivotSpeed, pivotTarget + 25.0f, 40.0f), 0.0f);

				if (rotatePart[LEFT_UPPER_THIGH][0] >= pivotTarget)
				{
					rotatePart[LEFT_UPPER_THIGH][0] = pivotTarget;
					rotatePart[LEFT_LOWER_THIGH][0] = 0.0f;
					rotatePart[LEFT_FOOT][0] = 0.0f;
					rotatePart[LEFT_FOOT_TOES][0] = 0.0f;
					leftReady = true;
				}

				/*rotatePart[RIGHT_UPPER_THIGH][0] = 0.0f;
				rotatePart[RIGHT_LOWER_THIGH][0] = 40.0f;
				rotatePart[RIGHT_FOOT][0] = 0.0f;
				rotatePart[RIGHT_FOOT_TOES][0] = -30.0f;*/

				float pivotSpeed2 = 1.7f;
				float pivotTarget2 = 0.0f; // + 30.0f
				rotatePart[RIGHT_UPPER_THIGH][0] -= pivotSpeed2;
				rotatePart[RIGHT_LOWER_THIGH][0] = clampValMax(rotatePart[RIGHT_LOWER_THIGH][0] + dGangnamFunction(pivotSpeed2, pivotTarget2 + 30.0f, 40.0f), 40.0f);
				rotatePart[RIGHT_FOOT][0] = clampValMax(rotatePart[RIGHT_FOOT][0] + dGangnamFunction(pivotSpeed2, pivotTarget2 + 30.0f, 30.0f), 0.0f);
				rotatePart[RIGHT_FOOT_TOES][0] = clampValMin(rotatePart[RIGHT_FOOT][0] - dGangnamFunction(pivotSpeed2, pivotTarget2 + 30.0f, 30.0f), -30.0f);

				if (rotatePart[RIGHT_UPPER_THIGH][0] <= pivotTarget2)
				{
					rotatePart[RIGHT_UPPER_THIGH][0] = pivotTarget2;
					rotatePart[RIGHT_LOWER_THIGH][0] = 40.0f;
					rotatePart[RIGHT_FOOT][0] = 0.0f;
					rotatePart[RIGHT_FOOT_TOES][0] = -30.0f;
					rightReady = true;
				}

				rotatePart[LEFT_UPPER_ARM][0] = -rotatePart[LEFT_UPPER_THIGH][0];
				rotatePart[RIGHT_UPPER_ARM][0] = -rotatePart[LEFT_UPPER_ARM][0];

				if (leftReady && rightReady)
				{
					l_slide = true;
					moon_reset = false;
					mwcount++;
				}

				if (leftReady && rightReady)
				{
					mwSaveStop = true;
				}
				else
				{
					mwSaveStop = false;
				}
			}
		}
		else
		{
			if (!moon_reset)
			{
				bool leftReady = false;
				bool rightReady = false;
				// left leg move front
				/*rotatePart[LEFT_UPPER_THIGH][0] = -45.0f;
				rotatePart[LEFT_FOOT][0] = 40.0f;*/

				float pivotSpeed = 1.0f;
				float pivotTarget = -25.0f;
				rotatePart[RIGHT_UPPER_THIGH][0] -= pivotSpeed;
				rotatePart[RIGHT_FOOT][0] = clampValMax(rotatePart[RIGHT_FOOT][0] + dGangnamFunction(pivotSpeed, pivotTarget, 40.0f), 40.0f);
				rotatePart[RIGHT_FOOT_TOES][0] = clampValMax(rotatePart[RIGHT_FOOT_TOES][0] - dGangnamFunction(pivotSpeed, pivotTarget, 10.0f), -50.0f);

				if (rotatePart[RIGHT_UPPER_THIGH][0] <= pivotTarget)
				{
					rotatePart[RIGHT_UPPER_THIGH][0] = pivotTarget;
					rotatePart[RIGHT_FOOT][0] = 40.0f;
					rotatePart[RIGHT_FOOT_TOES][0] = -50.0f;
					leftReady = true;
				}

				float pivotSpeed2 = 1.0f;
				float pivotTarget2 = 30.0f;
				/*rotatePart[RIGHT_UPPER_THIGH][0] = 40.0f;
				rotatePart[RIGHT_FOOT][0] = -30.0f;*/

				rotatePart[LEFT_UPPER_THIGH][0] += pivotSpeed2;
				rotatePart[LEFT_FOOT][0] = clampValMin(rotatePart[LEFT_FOOT][0] - dGangnamFunction(pivotSpeed2, pivotTarget2, 30.0f), -30.0f);

				if (rotatePart[LEFT_UPPER_THIGH][0] >= pivotTarget2 || rotatePart[RIGHT_UPPER_THIGH][0] <= pivotTarget)
				{
					// rotatePart[LEFT_UPPER_THIGH][0] = pivotTarget2;
					// rotatePart[LEFT_FOOT][0] = -30.0f;
					rightReady = true;
				}

				rotatePart[LEFT_UPPER_ARM][0] = -rotatePart[LEFT_UPPER_THIGH][0];
				rotatePart[RIGHT_UPPER_ARM][0] = -rotatePart[LEFT_UPPER_ARM][0];

				if (leftReady && rightReady)
				{
					moon_reset = true;
				}
			}
			else
			{
				bool leftReady = false;
				bool rightReady = false;
				// reset
				/*rotatePart[LEFT_UPPER_THIGH][0] = 0.0f;
				rotatePart[LEFT_LOWER_THIGH][0] = 0.0f;
				rotatePart[LEFT_FOOT][0] = 0.0f;
				rotatePart[LEFT_FOOT_TOES][0] = 0.0f;*/

				float pivotSpeed = 1.7f;
				float pivotTarget = 0.0f;
				rotatePart[RIGHT_UPPER_THIGH][0] += pivotSpeed;
				rotatePart[RIGHT_LOWER_THIGH][0] = clampValMin(rotatePart[RIGHT_LOWER_THIGH][0] - dGangnamFunction(pivotSpeed, pivotTarget + 25.0f, 40.0f), 0.0f);
				rotatePart[RIGHT_FOOT][0] = clampValMin(rotatePart[RIGHT_FOOT][0] - dGangnamFunction(pivotSpeed, pivotTarget + 25.0f, 40.0f), 0.0f);
				rotatePart[RIGHT_FOOT_TOES][0] = clampValMax(rotatePart[RIGHT_FOOT_TOES][0] + dGangnamFunction(pivotSpeed, pivotTarget + 25.0f, 40.0f), 0.0f);

				if (rotatePart[RIGHT_UPPER_THIGH][0] >= pivotTarget)
				{
					rotatePart[RIGHT_UPPER_THIGH][0] = pivotTarget;
					rotatePart[RIGHT_LOWER_THIGH][0] = 0.0f;
					rotatePart[RIGHT_FOOT][0] = 0.0f;
					rotatePart[RIGHT_FOOT_TOES][0] = 0.0f;
					leftReady = true;
				}

				/*rotatePart[RIGHT_UPPER_THIGH][0] = 0.0f;
				rotatePart[RIGHT_LOWER_THIGH][0] = 40.0f;
				rotatePart[RIGHT_FOOT][0] = 0.0f;
				rotatePart[RIGHT_FOOT_TOES][0] = -30.0f;*/

				float pivotSpeed2 = 1.7f;
				float pivotTarget2 = 0.0f; // + 30.0f
				rotatePart[LEFT_UPPER_THIGH][0] -= pivotSpeed2;
				rotatePart[LEFT_LOWER_THIGH][0] = clampValMax(rotatePart[LEFT_LOWER_THIGH][0] + dGangnamFunction(pivotSpeed2, pivotTarget2 + 30.0f, 40.0f), 40.0f);
				rotatePart[LEFT_FOOT][0] = clampValMax(rotatePart[LEFT_FOOT][0] + dGangnamFunction(pivotSpeed2, pivotTarget2 + 30.0f, 30.0f), 0.0f);
				rotatePart[LEFT_FOOT_TOES][0] = clampValMin(rotatePart[LEFT_FOOT_TOES][0] - dGangnamFunction(pivotSpeed2, pivotTarget2 + 30.0f, 30.0f), -30.0f);

				if (rotatePart[LEFT_UPPER_THIGH][0] <= pivotTarget2)
				{
					rotatePart[LEFT_UPPER_THIGH][0] = pivotTarget2;
					rotatePart[LEFT_LOWER_THIGH][0] = 40.0f;
					rotatePart[LEFT_FOOT][0] = 0.0f;
					rotatePart[LEFT_FOOT_TOES][0] = -30.0f;
					rightReady = true;
				}

				rotatePart[LEFT_UPPER_ARM][0] = -rotatePart[LEFT_UPPER_THIGH][0];
				rotatePart[RIGHT_UPPER_ARM][0] = -rotatePart[LEFT_UPPER_ARM][0];

				if (leftReady && rightReady)
				{
					l_slide = false;
					moon_reset = false;
					mwcount++;

				}

				if (leftReady && rightReady)
				{
					mwSaveStop = true;
				}
				else
				{
					mwSaveStop = false;
				}
			}
		}
	}
}

void squat()
{
	/*rotatePart[ABS][0] = 20.0f;
	rotatePart[LEFT_UPPER_ARM][0] = -90.0f;
	rotatePart[RIGHT_UPPER_ARM][0] = rotatePart[LEFT_UPPER_ARM][0];

	rotatePart[LEFT_UPPER_THIGH][0] = -55.0f;
	rotatePart[LEFT_LOWER_THIGH][0] = 100.0f;
	rotatePart[LEFT_FOOT][0] = -35.0f;
	rotatePart[RIGHT_UPPER_THIGH][0] = rotatePart[LEFT_UPPER_THIGH][0];
	rotatePart[RIGHT_LOWER_THIGH][0] = rotatePart[LEFT_LOWER_THIGH][0];
	rotatePart[RIGHT_FOOT][0] = rotatePart[LEFT_FOOT][0];
	translatePart[CROTCH][1] = -3.5f;*/
	float crotch = 3.5f;
	float crotch_s = 4.0f;

	if (!isSquatReady)
	{
		rotatePart[LEFT_UPPER_THIGH][0] -= 1.0f;
		rotatePart[LEFT_UPPER_THIGH][1] += 1.3f;
		rotatePart[LEFT_LOWER_THIGH][0] += 1.3f;

		if (rotatePart[LEFT_UPPER_THIGH][0] <= -15.0f)
		{
			rotatePart[LEFT_UPPER_THIGH][0] = -15.0f;
			rotatePart[LEFT_UPPER_THIGH][1] = 20.0f;
			rotatePart[LEFT_LOWER_THIGH][0] = 20.0f;
			isSquatReady = true;
			Sleep(300);
		}

		rotatePart[RIGHT_UPPER_THIGH][0] = rotatePart[LEFT_UPPER_THIGH][0];
		rotatePart[RIGHT_UPPER_THIGH][1] = -rotatePart[LEFT_UPPER_THIGH][1];
		rotatePart[RIGHT_LOWER_THIGH][0] = rotatePart[LEFT_LOWER_THIGH][0];
	}
	else
	{
		if (squatCount != 5)
		{
			if (isGoDown)
			{
				float pivotSpeed = 2.0f;
				float pivotTarget = 100.0f;

				translatePart[CROTCH][1] = clampValMin(translatePart[CROTCH][1] - dGangnamFunction(pivotSpeed, pivotTarget, crotch_s), -crotch);

				rotatePart[ABS][0] = clampValMax(rotatePart[ABS][0] + dGangnamFunction(pivotSpeed, pivotTarget, 25.0f), 20.0f);
				rotatePart[LEFT_UPPER_THIGH][0] = clampValMin(rotatePart[LEFT_UPPER_THIGH][0] - dGangnamFunction(pivotSpeed, pivotTarget, 55.0f), -55.0f);
				rotatePart[LEFT_LOWER_THIGH][0] += pivotSpeed;
				rotatePart[LEFT_FOOT][0] = clampValMin(rotatePart[LEFT_FOOT][0] - dGangnamFunction(pivotSpeed, pivotTarget, 40.0f), -40.0f);

				if (rotatePart[LEFT_LOWER_THIGH][0] >= 100.0f)
				{
					rotatePart[ABS][0] = 20.0f;
					rotatePart[LEFT_UPPER_THIGH][0] = -55.0f;
					rotatePart[LEFT_LOWER_THIGH][0] = 100.0f;
					rotatePart[LEFT_FOOT][0] = -40.0f;
				}

				rotatePart[LEFT_UPPER_ARM][0] = clampValMin(rotatePart[LEFT_UPPER_ARM][0] - dGangnamFunction(pivotSpeed, pivotTarget, 100.0f), -90.0f);
				rotatePart[LEFT_LOWER_ARM][1] = clampValMin(rotatePart[LEFT_LOWER_ARM][1] - dGangnamFunction(pivotSpeed, pivotTarget, 100.0f), -90.0f);

				if (rotatePart[LEFT_UPPER_ARM][0] <= -90.0f)
				{
					rotatePart[LEFT_UPPER_ARM][0] = -90.0f;
					rotatePart[LEFT_LOWER_ARM][1] = -90.0f;
					isGoDown = false;
				}

				rotatePart[RIGHT_UPPER_THIGH][0] = rotatePart[LEFT_UPPER_THIGH][0];
				rotatePart[RIGHT_LOWER_THIGH][0] = rotatePart[LEFT_LOWER_THIGH][0];
				rotatePart[RIGHT_FOOT][0] = rotatePart[LEFT_FOOT][0];
				rotatePart[RIGHT_UPPER_ARM][0] = rotatePart[LEFT_UPPER_ARM][0];
				rotatePart[RIGHT_LOWER_ARM][1] = -rotatePart[LEFT_LOWER_ARM][1];
			}
			else
			{
				float pivotSpeed = 2.0f;
				float pivotTarget = 100.0f;
				translatePart[CROTCH][1] = clampValMax(translatePart[CROTCH][1] + dGangnamFunction(pivotSpeed, pivotTarget, crotch_s + 0.5f), 0.0f);
				rotatePart[ABS][0] = clampValMin(rotatePart[ABS][0] - dGangnamFunction(pivotSpeed, pivotTarget, 30.0f), 0.0f);
				rotatePart[LEFT_UPPER_THIGH][0] = clampValMax(rotatePart[LEFT_UPPER_THIGH][0] + dGangnamFunction(pivotSpeed, pivotTarget, 50.0f), -15.0f);
				rotatePart[LEFT_LOWER_THIGH][0] -= pivotSpeed;
				rotatePart[LEFT_FOOT][0] = clampValMax(rotatePart[LEFT_FOOT][0] + dGangnamFunction(pivotSpeed, pivotTarget, 40.0f), 0.0f);

				if (rotatePart[LEFT_LOWER_THIGH][0] <= 20.0f)
				{
					rotatePart[ABS][0] = 0.0f;
					rotatePart[LEFT_UPPER_THIGH][0] = -15.0f;
					rotatePart[LEFT_LOWER_THIGH][0] = 20.0f;
					rotatePart[LEFT_FOOT][0] = 0.0f;
				}

				rotatePart[LEFT_UPPER_ARM][0] = clampValMax(rotatePart[LEFT_UPPER_ARM][0] + dGangnamFunction(pivotSpeed, pivotTarget, 100.0f), 0.0f);
				rotatePart[LEFT_LOWER_ARM][1] = clampValMax(rotatePart[LEFT_LOWER_ARM][1] + dGangnamFunction(pivotSpeed, pivotTarget, 100.0f), 0.0f);

				if (rotatePart[LEFT_UPPER_ARM][0] >= 0.0f)
				{
					rotatePart[LEFT_UPPER_ARM][0] = 0.0f;
					rotatePart[LEFT_LOWER_ARM][1] = 0.0f;
					isGoDown = true;
					Sleep(300);
					squatCount++;
				}

				rotatePart[RIGHT_UPPER_THIGH][0] = rotatePart[LEFT_UPPER_THIGH][0];
				rotatePart[RIGHT_LOWER_THIGH][0] = rotatePart[LEFT_LOWER_THIGH][0];
				rotatePart[RIGHT_FOOT][0] = rotatePart[LEFT_FOOT][0];
				rotatePart[RIGHT_UPPER_ARM][0] = rotatePart[LEFT_UPPER_ARM][0];
				rotatePart[RIGHT_LOWER_ARM][1] = -rotatePart[LEFT_LOWER_ARM][1];

				squatSaveStop = false;
			}
		}
		else
		{
			squatSaveStop = true;
		}
	}

}

void sitUp()
{
	if (!isSitUpready)
	{
		/*rotatePart[LEFT_UPPER_THIGH][0] = -130.0f;
		rotatePart[LEFT_LOWER_THIGH][0] = 130.0f;
		rotatePart[LEFT_FOOT][0] = 10.0f;*/
		// translatePart[CROTCH][1] = -8.0f;

		float pivotSpeed = 5.0f;
		float pivotTarget = -140.0f;

		rotatePart[LEFT_UPPER_THIGH][0] -= pivotSpeed;
		rotatePart[LEFT_LOWER_THIGH][0] = clampValMax(rotatePart[LEFT_LOWER_THIGH][0] + dGangnamFunction(pivotSpeed, pivotTarget, 140.0f), 140.0f);
		rotatePart[LEFT_FOOT][0] = clampValMax(rotatePart[LEFT_FOOT][0] + dGangnamFunction(pivotSpeed, pivotTarget, 10.0f), 10.0f);

		if (rotatePart[LEFT_UPPER_THIGH][0] >= -40.0f)
		{
			translatePart[CROTCH][1] = clampValMin(translatePart[CROTCH][1] - dGangnamFunction(pivotSpeed, pivotTarget, 5.0f), -8.0f);
		}
		else if (rotatePart[LEFT_UPPER_THIGH][0] <= -50.0f)
		{
			translatePart[CROTCH][1] = clampValMin(translatePart[CROTCH][1] - dGangnamFunction(pivotSpeed, pivotTarget, 10.0f), -8.0f);
		}
		else
		{
			translatePart[CROTCH][1] = clampValMin(translatePart[CROTCH][1] - dGangnamFunction(pivotSpeed, pivotTarget, 3.0f), -8.0f);
		}

		if (rotatePart[LEFT_UPPER_THIGH][0] <= pivotTarget)
		{
			// rotatePart[CROTCH][0] = pivotTarget;
			rotatePart[LEFT_UPPER_THIGH][0] = pivotTarget;
			rotatePart[LEFT_LOWER_THIGH][0] = 140.0f;
			rotatePart[LEFT_FOOT][0] = 10.0f;
			isSitUpready = true;
		}

		rotatePart[RIGHT_UPPER_THIGH][0] = rotatePart[LEFT_UPPER_THIGH][0];
		rotatePart[RIGHT_LOWER_THIGH][0] = rotatePart[LEFT_LOWER_THIGH][0];
		rotatePart[RIGHT_FOOT][0] = rotatePart[LEFT_FOOT][0];
	}
	else if (!isSitUpready2)
	{
		/*rotatePart[LEFT_UPPER_THIGH][0] = -60.0f;
		rotatePart[LEFT_LOWER_THIGH][0] = 140.0f;
		rotatePart[LEFT_UPPER_ARM][0] = 90.0f;*/

		float pivotSpeed = 3.0f;
		float pivotTarget = -90.0f;

		translatePart[CROTCH][1] = clampValMin(translatePart[CROTCH][1] - dGangnamFunction(pivotSpeed, pivotTarget, 5.0f), -9.0f);

		rotatePart[CROTCH][0] -= pivotSpeed;
		rotatePart[LEFT_UPPER_THIGH][0] = clampValMax(rotatePart[LEFT_UPPER_THIGH][0] + dGangnamFunction(pivotSpeed, pivotTarget, 100.0f), -40.0f);
		rotatePart[LEFT_LOWER_THIGH][0] = clampValMin(rotatePart[LEFT_LOWER_THIGH][0] - dGangnamFunction(pivotSpeed, pivotTarget, 30.0f), 110.0f);
		rotatePart[LEFT_UPPER_ARM][0] = clampValMin(rotatePart[LEFT_UPPER_ARM][0] - dGangnamFunction(pivotSpeed, pivotTarget, 70.0f), -70.0f);
		rotatePart[LEFT_UPPER_ARM][1] = clampValMax(rotatePart[LEFT_UPPER_ARM][1] + dGangnamFunction(pivotSpeed, pivotTarget, 50.0f), 50.0f);
		rotatePart[LEFT_LOWER_ARM][0] = clampValMin(rotatePart[LEFT_LOWER_ARM][0] - dGangnamFunction(pivotSpeed, pivotTarget, 150.0f), -150.0f);
		rotatePart[LEFT_LOWER_ARM][1] = clampValMin(rotatePart[LEFT_LOWER_ARM][1] - dGangnamFunction(pivotSpeed, pivotTarget, 20.0f), -20.0f);



		if (rotatePart[CROTCH][0] <= pivotTarget)
		{
			rotatePart[CROTCH][0] = -90.0f;
			rotatePart[LEFT_UPPER_THIGH][0] = -40.0f;
			rotatePart[LEFT_LOWER_THIGH][0] = 110.0f;
			rotatePart[LEFT_UPPER_ARM][0] = -90.0f;
			rotatePart[LEFT_LOWER_ARM][0] = -150.0f;
			isSitUpready2 = true;
		}

		rotatePart[RIGHT_UPPER_THIGH][0] = rotatePart[LEFT_UPPER_THIGH][0];
		rotatePart[RIGHT_LOWER_THIGH][0] = rotatePart[LEFT_LOWER_THIGH][0];
		rotatePart[RIGHT_FOOT][0] = rotatePart[LEFT_FOOT][0];

		rotatePart[RIGHT_UPPER_ARM][0] = rotatePart[LEFT_UPPER_ARM][0];
		rotatePart[RIGHT_UPPER_ARM][1] = -rotatePart[LEFT_UPPER_ARM][1];
		rotatePart[RIGHT_LOWER_ARM][0] = rotatePart[LEFT_LOWER_ARM][0];
		rotatePart[RIGHT_LOWER_ARM][1] = -rotatePart[LEFT_LOWER_ARM][1];
	}
	else
	{
		if (sleepOnce)
		{
			Sleep(300);
			sleepOnce = !sleepOnce;
		}

		if (times != 5)
		{
			if (isUP)
			{
				float pivotSpeed = 1.5f;
				float pivotTarget = -50.0f;

				rotatePart[CROTCH][0] += pivotSpeed;
				rotatePart[LEFT_UPPER_THIGH][0] = clampValMin(rotatePart[LEFT_UPPER_THIGH][0] - dGangnamFunction(pivotSpeed, pivotTarget, 55.0f), -85.0f);
				rotatePart[ABS][0] = clampValMax(rotatePart[ABS][0] + dGangnamFunction(pivotSpeed, pivotTarget, 40.0f), 20.0f);
				rotatePart[UPPER_BODY][0] = clampValMax(rotatePart[UPPER_BODY][0] + dGangnamFunction(pivotSpeed, pivotTarget, 40.0f), 20.0f);


				if (rotatePart[CROTCH][0] >= pivotTarget)
				{
					rotatePart[CROTCH][0] = pivotTarget;
					rotatePart[LEFT_UPPER_THIGH][0] = -85.0f;
					rotatePart[ABS][0] = 20.0f;
					rotatePart[UPPER_BODY][0] = 20.0f;
					isUP = false;
				}

				rotatePart[RIGHT_UPPER_THIGH][0] = rotatePart[LEFT_UPPER_THIGH][0];
			}
			else
			{
				float pivotSpeed = 1.0f;
				float pivotTarget = -90.0f;

				rotatePart[CROTCH][0] -= pivotSpeed;
				rotatePart[LEFT_UPPER_THIGH][0] = clampValMax(rotatePart[LEFT_UPPER_THIGH][0] + dGangnamFunction(pivotSpeed, pivotTarget, 110.0f), -40.0f);
				rotatePart[ABS][0] = clampValMin(rotatePart[ABS][0] - dGangnamFunction(pivotSpeed, pivotTarget, 50.0f), 0.0f);
				rotatePart[UPPER_BODY][0] = clampValMin(rotatePart[UPPER_BODY][0] - dGangnamFunction(pivotSpeed, pivotTarget, 50.0f), 0.0f);

				if (rotatePart[CROTCH][0] <= pivotTarget)
				{
					rotatePart[CROTCH][0] = pivotTarget;
					rotatePart[LEFT_UPPER_THIGH][0] = -40.0f;
					rotatePart[ABS][0] = 0.0f;
					rotatePart[UPPER_BODY][0] = 0.0f;
					isUP = true;
					times++;
				}

				rotatePart[RIGHT_UPPER_THIGH][0] = rotatePart[LEFT_UPPER_THIGH][0];
			}
		}
		else
		{
			if (!isDone)
			{
				float pivotSpeed = 3.0f;
				float pivotTarget = 0.0f;

				rotatePart[CROTCH][0] += pivotSpeed;
				translatePart[CROTCH][1] = clampValMax(translatePart[CROTCH][1] + dGangnamFunction(pivotSpeed, pivotTarget + 90.0f, 5.0f), -8.0f);
				rotatePart[LEFT_UPPER_THIGH][0] = clampValMin(rotatePart[LEFT_UPPER_THIGH][0] - dGangnamFunction(pivotSpeed, pivotTarget + 90.0f, 100.0f), -140.0f);
				rotatePart[LEFT_LOWER_THIGH][0] = clampValMax(rotatePart[LEFT_LOWER_THIGH][0] + dGangnamFunction(pivotSpeed, pivotTarget + 90.0f, 30), 140.0f);
				rotatePart[LEFT_FOOT][0] = clampValMin(rotatePart[LEFT_FOOT][0] - dGangnamFunction(pivotSpeed, pivotTarget + 90.0f, 10), 0.0f);

				if (rotatePart[CROTCH][0] >= pivotTarget)
				{
					rotatePart[CROTCH][0] = pivotTarget;
					rotatePart[LEFT_UPPER_THIGH][0] = -140.0f;
					rotatePart[LEFT_LOWER_THIGH][0] = 140.0f;
					rotatePart[LEFT_FOOT][0] = 0.0f;
					isDone = true;
				}

				rotatePart[RIGHT_UPPER_THIGH][0] = rotatePart[LEFT_UPPER_THIGH][0];
				rotatePart[RIGHT_LOWER_THIGH][0] = rotatePart[LEFT_LOWER_THIGH][0];
				rotatePart[RIGHT_FOOT][0] = rotatePart[LEFT_FOOT][0];
			}
			else if (!isDone2)
			{
				float pivotSpeed = 4.0f;
				float pivotTarget = 0.0f;

				rotatePart[LEFT_UPPER_THIGH][0] += pivotSpeed;
				rotatePart[LEFT_LOWER_THIGH][0] = clampValMin(rotatePart[LEFT_LOWER_THIGH][0] - dGangnamFunction(pivotSpeed, pivotTarget + 140.0f, 140.0f), 0.0f);
				rotatePart[LEFT_UPPER_ARM][0] = clampValMax(rotatePart[LEFT_UPPER_ARM][0] + dGangnamFunction(pivotSpeed, pivotTarget + 140.0f, 100.0f), 0.0f);
				rotatePart[LEFT_UPPER_ARM][1] = clampValMin(rotatePart[LEFT_UPPER_ARM][1] - dGangnamFunction(pivotSpeed, pivotTarget + 140.0f, 50.0f), 0.0f);
				rotatePart[LEFT_LOWER_ARM][0] = clampValMax(rotatePart[LEFT_LOWER_ARM][0] + dGangnamFunction(pivotSpeed, pivotTarget + 140.0f, 200.0f), 0.0f);
				rotatePart[LEFT_LOWER_ARM][1] = clampValMax(rotatePart[LEFT_LOWER_ARM][1] + dGangnamFunction(pivotSpeed, pivotTarget + 140.0f, 20.0f), 0.0f);

				if (rotatePart[LEFT_UPPER_THIGH][0] >= -40.0f)
				{
					translatePart[CROTCH][1] = clampValMax(translatePart[CROTCH][1] + dGangnamFunction(pivotSpeed, pivotTarget + 140.0f, 5.0f), 0.0f);
				}
				else if (rotatePart[LEFT_UPPER_THIGH][0] <= -50.0f)
				{
					translatePart[CROTCH][1] = clampValMax(translatePart[CROTCH][1] + dGangnamFunction(pivotSpeed, pivotTarget + 140.0f, 10.0f), 0.0f);
				}
				else
				{
					translatePart[CROTCH][1] = clampValMax(translatePart[CROTCH][1] + dGangnamFunction(pivotSpeed, pivotTarget + 140.0f, 3.0f), 0.0f);
				}


				if (rotatePart[LEFT_UPPER_THIGH][0] >= pivotTarget)
				{
					rotatePart[LEFT_UPPER_THIGH][0] = pivotTarget;
					rotatePart[LEFT_LOWER_THIGH][0] = 0.0f;
					rotatePart[LEFT_UPPER_ARM][0] = 0.0f;
					rotatePart[LEFT_LOWER_ARM][0] = 0.0f;
					rotatePart[LEFT_UPPER_ARM][1] = 0.0f;
					rotatePart[LEFT_LOWER_ARM][1] = 0.0f;
					isDone2 = true;
					suSaveStop = true;
				}

				rotatePart[RIGHT_UPPER_THIGH][0] = rotatePart[LEFT_UPPER_THIGH][0];
				rotatePart[RIGHT_LOWER_THIGH][0] = rotatePart[LEFT_LOWER_THIGH][0];
				rotatePart[RIGHT_UPPER_ARM][0] = rotatePart[LEFT_UPPER_ARM][0];
				rotatePart[RIGHT_UPPER_ARM][1] = -rotatePart[LEFT_UPPER_ARM][1];
				rotatePart[RIGHT_LOWER_ARM][0] = rotatePart[LEFT_LOWER_ARM][0];
				rotatePart[RIGHT_LOWER_ARM][1] = -rotatePart[LEFT_LOWER_ARM][1];

			}
		}
	}
}

void pushUp()
{
	if (!isPushUpready)
	{
		/*rotatePart[LEFT_UPPER_THIGH][0] = -130.0f;
		rotatePart[LEFT_LOWER_THIGH][0] = 130.0f;
		rotatePart[LEFT_FOOT][0] = 10.0f;
		rotatePart[LEFT_PALM][0] = -45.0f;
		rotatePart[LEFT_PALM][2] = 90.0f;*/
		float pivotSpeed = 5.0f;
		float pivotTarget = -140.0f;

		rotatePart[LEFT_UPPER_THIGH][0] -= pivotSpeed;
		rotatePart[CROTCH][0] = clampValMax(rotatePart[CROTCH][0] + dGangnamFunction(pivotSpeed, pivotTarget, 80.0f), 80.0f);
		rotatePart[LEFT_LOWER_THIGH][0] = clampValMax(rotatePart[LEFT_LOWER_THIGH][0] + dGangnamFunction(pivotSpeed, pivotTarget, 140.0f), 140.0f);
		rotatePart[LEFT_FOOT][0] = clampValMax(rotatePart[LEFT_FOOT][0] + dGangnamFunction(pivotSpeed, pivotTarget, 10.0f), 10.0f);
		rotatePart[LEFT_UPPER_ARM][0] = clampValMin(rotatePart[LEFT_UPPER_ARM][0] - dGangnamFunction(pivotSpeed, pivotTarget, 80.0f), -80.0f);
		rotatePart[LEFT_PALM][0] = clampValMin(rotatePart[LEFT_PALM][0] - dGangnamFunction(pivotSpeed, pivotTarget, 45.0f), -45.0f);
		rotatePart[LEFT_PALM][2] = clampValMax(rotatePart[LEFT_PALM][2] + dGangnamFunction(pivotSpeed, pivotTarget, 90.0f), 90.0f);

		// translatePart[CROTCH][1] = clampValMin(translatePart[CROTCH][1] - dGangnamFunction(pivotSpeed, pivotTarget, 7.0f), -6.0f);

		if (rotatePart[LEFT_UPPER_THIGH][0] >= -40.0f)
		{
			translatePart[CROTCH][1] = clampValMin(translatePart[CROTCH][1] - dGangnamFunction(pivotSpeed, pivotTarget, 3.0f), -6.0f);
		}
		else if (rotatePart[LEFT_UPPER_THIGH][0] <= -50.0f)
		{
			translatePart[CROTCH][1] = clampValMin(translatePart[CROTCH][1] - dGangnamFunction(pivotSpeed, pivotTarget, 7.0f), -6.0f);
		}
		else
		{
			translatePart[CROTCH][1] = clampValMin(translatePart[CROTCH][1] - dGangnamFunction(pivotSpeed, pivotTarget, 1.0f), -6.0f);
		}

		if (rotatePart[LEFT_UPPER_THIGH][0] <= pivotTarget)
		{
			// rotatePart[CROTCH][0] = pivotTarget;
			rotatePart[LEFT_UPPER_THIGH][0] = pivotTarget;
			rotatePart[LEFT_LOWER_THIGH][0] = 140.0f;
			rotatePart[LEFT_FOOT][0] = 10.0f;
			rotatePart[LEFT_PALM][0] = -45.0f;
			rotatePart[LEFT_PALM][2] = 90.0f;
			isPushUpready = true;
		}

		rotatePart[RIGHT_UPPER_ARM][0] = rotatePart[LEFT_UPPER_ARM][0];
		rotatePart[RIGHT_UPPER_THIGH][0] = rotatePart[LEFT_UPPER_THIGH][0];
		rotatePart[RIGHT_LOWER_THIGH][0] = rotatePart[LEFT_LOWER_THIGH][0];
		rotatePart[RIGHT_FOOT][0] = rotatePart[LEFT_FOOT][0];
		rotatePart[RIGHT_PALM][0] = rotatePart[LEFT_PALM][0];
		rotatePart[RIGHT_PALM][2] = -rotatePart[LEFT_PALM][2];
	}
	else if (!isPushUpready2)
	{
		float pivotSpeed = 3.0f;
		float pivotTarget = -5.0f;

		// rotatePart[LEFT_UPPER_THIGH][0] += pivotSpeed;
		rotatePart[LEFT_UPPER_THIGH][0] = clampValMax(rotatePart[LEFT_UPPER_THIGH][0] + dGangnamFunction(pivotSpeed, pivotTarget + 140.0f, 140.0f + 50.0f), -5.0f);
		rotatePart[LEFT_LOWER_THIGH][0] = clampValMin(rotatePart[LEFT_LOWER_THIGH][0] - dGangnamFunction(pivotSpeed, pivotTarget + 140.0f, 140.0f), 0.0f);
		rotatePart[LEFT_FOOT_TOES][0] = clampValMin(rotatePart[LEFT_FOOT_TOES][0] - dGangnamFunction(pivotSpeed, pivotTarget + 140.0f, 50.0f), -60.0f);

		if (rotatePart[LEFT_UPPER_THIGH][0] <= -80.0f)
		{
			rotatePart[CROTCH][0] = clampValMax(rotatePart[CROTCH][0] + dGangnamFunction(pivotSpeed, pivotTarget + 140.0f, 140.0f), 250.0f);
			rotatePart[LEFT_UPPER_ARM][0] = clampValMin(rotatePart[LEFT_UPPER_ARM][0] - dGangnamFunction(pivotSpeed, pivotTarget + 140.0f, 110.0f), -170.0f);
			translatePart[CROTCH][1] = clampValMax(translatePart[CROTCH][1] + dGangnamFunction(pivotSpeed, pivotTarget + 140.0f, 10.0f), -2.0f);
		}
		else
		{
			rotatePart[CROTCH][0] = clampValMin(rotatePart[CROTCH][0] - dGangnamFunction(pivotSpeed, pivotTarget + 140.0f, 130.0f), 80.0f);
			rotatePart[LEFT_UPPER_ARM][0] = clampValMax(rotatePart[LEFT_UPPER_ARM][0] + dGangnamFunction(pivotSpeed, pivotTarget + 140.0f, 100.0f), -80.0f);
			translatePart[CROTCH][1] = clampValMin(translatePart[CROTCH][1] - dGangnamFunction(pivotSpeed, pivotTarget + 140.0f, 9.0f), -5.9);
		}

		// if (rotatePart[LEFT_UPPER_THIGH][0] >= pivotTarget)
		if (rotatePart[LEFT_LOWER_THIGH][0] <= 0.0f)
		{
			rotatePart[LEFT_UPPER_THIGH][0] = pivotTarget;
			rotatePart[LEFT_LOWER_THIGH][0] = 0.0f;
			rotatePart[LEFT_UPPER_ARM][0] = -80.0f;
			rotatePart[LEFT_UPPER_THIGH][0] = -5.0f;
			rotatePart[CROTCH][0] = 80.0f;
			translatePart[CROTCH][1] = -5.9f;
			isPushUpready2 = true;
		}

		rotatePart[RIGHT_UPPER_THIGH][0] = rotatePart[LEFT_UPPER_THIGH][0];
		rotatePart[RIGHT_LOWER_THIGH][0] = rotatePart[LEFT_LOWER_THIGH][0];
		rotatePart[RIGHT_UPPER_ARM][0] = rotatePart[LEFT_UPPER_ARM][0];
		rotatePart[RIGHT_UPPER_THIGH][0] = rotatePart[LEFT_UPPER_THIGH][0];
		rotatePart[RIGHT_FOOT_TOES][0] = rotatePart[LEFT_FOOT_TOES][0];
	}
	else
	{
		if (pTimes != 5)
		{
			if (pushDown)
			{
				float pivotSpeed = 0.3f;
				float pivotTarget = 10.0f;
				rotatePart[CROTCH][0] += pivotSpeed;
				/*rotatePart[LEFT_UPPER_ARM][0] = -10.0f;
				rotatePart[LEFT_LOWER_ARM][0] = -120.0f;
				rotatePart[LEFT_PALM][1] = -40.0f;*/
				rotatePart[LEFT_UPPER_ARM][0] = clampValMax(rotatePart[LEFT_UPPER_ARM][0] + dGangnamFunction(pivotSpeed, pivotTarget, 70.0f), -10.0f);
				rotatePart[LEFT_LOWER_ARM][0] = clampValMin(rotatePart[LEFT_LOWER_ARM][0] - dGangnamFunction(pivotSpeed, pivotTarget, 120.0f), -120.0f);
				rotatePart[LEFT_PALM][1] = clampValMin(rotatePart[LEFT_PALM][1] - dGangnamFunction(pivotSpeed, pivotTarget, 40.0f), -40.0f);

				if (rotatePart[LEFT_UPPER_ARM][0] <= -45.0f)
				{
					translatePart[CROTCH][1] = clampValMin(translatePart[CROTCH][1] - dGangnamFunction(pivotSpeed, pivotTarget, 0.5f), -8.0f);
					rotatePart[LEFT_UPPER_THIGH][0] = clampValMin(rotatePart[LEFT_UPPER_THIGH][0] - dGangnamFunction(pivotSpeed, pivotTarget, 8.0f), -10.0f);
				}
				else
				{
					translatePart[CROTCH][1] = clampValMin(translatePart[CROTCH][1] - dGangnamFunction(pivotSpeed, pivotTarget, 4.0f), -8.0f);
					rotatePart[LEFT_UPPER_THIGH][0] = clampValMax(rotatePart[LEFT_UPPER_THIGH][0] + dGangnamFunction(pivotSpeed, pivotTarget, 8.0f), -5.0f);;
				}

				if (rotatePart[CROTCH][0] >= 90.0f)
				{
					rotatePart[CROTCH][0] = 90.0f;
					translatePart[CROTCH][1] = -8.0f;
					rotatePart[LEFT_UPPER_ARM][0] = -10.0f;
					rotatePart[LEFT_LOWER_ARM][0] = -120.0f;
					rotatePart[LEFT_PALM][1] = -40.0f;
					pushDown = false;
				}

				rotatePart[RIGHT_UPPER_ARM][0] = rotatePart[LEFT_UPPER_ARM][0];
				rotatePart[RIGHT_LOWER_ARM][0] = rotatePart[LEFT_LOWER_ARM][0];
				rotatePart[RIGHT_PALM][1] = -rotatePart[LEFT_PALM][1];
				rotatePart[RIGHT_UPPER_THIGH][0] = rotatePart[LEFT_UPPER_THIGH][0];
			}
			else
			{
				float pivotSpeed = 0.2f;
				float pivotTarget = 10.0f;
				rotatePart[CROTCH][0] -= pivotSpeed;
				rotatePart[LEFT_UPPER_ARM][0] = clampValMin(rotatePart[LEFT_UPPER_ARM][0] - dGangnamFunction(pivotSpeed, pivotTarget, 70.0f), -80.0f);
				rotatePart[LEFT_LOWER_ARM][0] = clampValMax(rotatePart[LEFT_LOWER_ARM][0] + dGangnamFunction(pivotSpeed, pivotTarget, 120.0f), 0.0f);
				rotatePart[LEFT_PALM][1] = clampValMax(rotatePart[LEFT_PALM][1] + dGangnamFunction(pivotSpeed, pivotTarget, 40.0f), 0.0f);

				// translatePart[CROTCH][1] = clampValMax(translatePart[CROTCH][1] + dGangnamFunction(pivotSpeed, pivotTarget, 2.0f), -6.0f);
				if (rotatePart[LEFT_UPPER_ARM][0] <= -45.0f)
				{
					translatePart[CROTCH][1] = clampValMax(translatePart[CROTCH][1] + dGangnamFunction(pivotSpeed, pivotTarget, 0.5f), -5.7f);
					rotatePart[LEFT_UPPER_THIGH][0] = clampValMax(rotatePart[LEFT_UPPER_THIGH][0] + dGangnamFunction(pivotSpeed, pivotTarget, 8.0f), -5.0f);
				}
				else
				{
					translatePart[CROTCH][1] = clampValMax(translatePart[CROTCH][1] + dGangnamFunction(pivotSpeed, pivotTarget, 4.0f), -5.7f);
					rotatePart[LEFT_UPPER_THIGH][0] = clampValMin(rotatePart[LEFT_UPPER_THIGH][0] - dGangnamFunction(pivotSpeed, pivotTarget, 8.0f), -10.0f);
				}

				if (rotatePart[CROTCH][0] <= 80.0f)
				{
					rotatePart[CROTCH][0] = 80.0f;
					translatePart[CROTCH][1] = -5.7f;
					pushDown = true;
					pTimes++;
				}

				rotatePart[RIGHT_UPPER_ARM][0] = rotatePart[LEFT_UPPER_ARM][0];
				rotatePart[RIGHT_LOWER_ARM][0] = rotatePart[LEFT_LOWER_ARM][0];
				rotatePart[RIGHT_PALM][1] = -rotatePart[LEFT_PALM][1];
				rotatePart[RIGHT_UPPER_THIGH][0] = rotatePart[LEFT_UPPER_THIGH][0];
			}
		}
		else
		{
			if (!isPDone)
			{
				float pivotSpeed = 3.0f;
				float pivotTarget = -140.0f;

				rotatePart[LEFT_UPPER_THIGH][0] -= pivotSpeed;
				rotatePart[LEFT_LOWER_THIGH][0] = clampValMax(rotatePart[LEFT_LOWER_THIGH][0] + dGangnamFunction(pivotSpeed, pivotTarget, 140.0f), 140.0f);
				rotatePart[LEFT_FOOT_TOES][0] = clampValMax(rotatePart[LEFT_FOOT_TOES][0] + dGangnamFunction(pivotSpeed, pivotTarget, 60.0f), 0.0f);

				if (rotatePart[LEFT_UPPER_THIGH][0] > -60.0f)
				{
					rotatePart[CROTCH][0] = clampValMax(rotatePart[CROTCH][0] + dGangnamFunction(pivotSpeed, pivotTarget, 140.0f), 250.0f);
					rotatePart[LEFT_UPPER_ARM][0] = clampValMin(rotatePart[LEFT_UPPER_ARM][0] - dGangnamFunction(pivotSpeed, pivotTarget, 110.0f), -170.0f);
					translatePart[CROTCH][1] = clampValMax(translatePart[CROTCH][1] + dGangnamFunction(pivotSpeed, pivotTarget, 10.0f), -2.0f);
				}
				else
				{
					rotatePart[CROTCH][0] = clampValMin(rotatePart[CROTCH][0] - dGangnamFunction(pivotSpeed, pivotTarget, 130.0f), 80.0f);
					rotatePart[LEFT_UPPER_ARM][0] = clampValMax(rotatePart[LEFT_UPPER_ARM][0] + dGangnamFunction(pivotSpeed, pivotTarget, 100.0f), -80.0f);
					translatePart[CROTCH][1] = clampValMin(translatePart[CROTCH][1] - dGangnamFunction(pivotSpeed, pivotTarget, 9.0f), -6.0f);
				}

				if (rotatePart[LEFT_UPPER_THIGH][0] <= pivotTarget)
				{
					rotatePart[LEFT_UPPER_THIGH][0] = pivotTarget;
					rotatePart[LEFT_LOWER_THIGH][0] = 140.0f;
					rotatePart[LEFT_UPPER_ARM][0] = -80.0f;
					rotatePart[CROTCH][0] = 80.0f;
					translatePart[CROTCH][1] = -6.0f;
					isPDone = true;
				}

				rotatePart[RIGHT_UPPER_THIGH][0] = rotatePart[LEFT_UPPER_THIGH][0];
				rotatePart[RIGHT_LOWER_THIGH][0] = rotatePart[LEFT_LOWER_THIGH][0];
				rotatePart[RIGHT_UPPER_ARM][0] = rotatePart[LEFT_UPPER_ARM][0];
				rotatePart[RIGHT_UPPER_THIGH][0] = rotatePart[LEFT_UPPER_THIGH][0];
				rotatePart[RIGHT_FOOT_TOES][0] = rotatePart[LEFT_FOOT_TOES][0];
			}
			else if (!isPDone2)
			{
				float pivotSpeed = 5.0f;
				float pivotTarget = 0.0f;

				rotatePart[LEFT_UPPER_THIGH][0] += pivotSpeed;
				rotatePart[CROTCH][0] = clampValMin(rotatePart[CROTCH][0] - dGangnamFunction(pivotSpeed, pivotTarget + 140.0f, 80.0f), 0.0f);
				rotatePart[LEFT_LOWER_THIGH][0] = clampValMin(rotatePart[LEFT_LOWER_THIGH][0] - dGangnamFunction(pivotSpeed, pivotTarget + 140.0f, 140.0f), 0.0f);
				rotatePart[LEFT_FOOT][0] = clampValMin(rotatePart[LEFT_FOOT][0] - dGangnamFunction(pivotSpeed, pivotTarget + 140.0f, 10.0f), 0.0f);
				rotatePart[LEFT_UPPER_ARM][0] = clampValMax(rotatePart[LEFT_UPPER_ARM][0] + dGangnamFunction(pivotSpeed, pivotTarget + 140.0f, 80.0f), 0.0f);
				rotatePart[LEFT_PALM][0] = clampValMax(rotatePart[LEFT_PALM][0] + dGangnamFunction(pivotSpeed, pivotTarget + 140.0f, 45.0f), 0.0f);
				rotatePart[LEFT_PALM][2] = clampValMin(rotatePart[LEFT_PALM][2] - dGangnamFunction(pivotSpeed, pivotTarget + 140.0f, 90.0f), 0.0f);

				translatePart[CROTCH][1] = clampValMax(translatePart[CROTCH][1] + dGangnamFunction(pivotSpeed, pivotTarget + 140.0f, 7.0f), 0.0f);

				if (rotatePart[LEFT_UPPER_THIGH][0] >= pivotTarget)
				{
					// rotatePart[CROTCH][0] = pivotTarget;
					rotatePart[LEFT_UPPER_THIGH][0] = pivotTarget;
					rotatePart[LEFT_LOWER_THIGH][0] = 0.0f;
					rotatePart[LEFT_FOOT][0] = 0.0f;
					rotatePart[LEFT_PALM][0] = 0.0f;
					rotatePart[LEFT_PALM][2] = 0.0f;
					isPDone2 = true;
					puSaveStop = true;
				}

				rotatePart[RIGHT_UPPER_THIGH][0] = rotatePart[LEFT_UPPER_THIGH][0];
				rotatePart[RIGHT_LOWER_THIGH][0] = rotatePart[LEFT_LOWER_THIGH][0];
				rotatePart[RIGHT_FOOT][0] = rotatePart[LEFT_FOOT][0];
				rotatePart[RIGHT_PALM][0] = rotatePart[LEFT_PALM][0];
				rotatePart[RIGHT_PALM][2] = -rotatePart[LEFT_PALM][2];
				rotatePart[RIGHT_UPPER_ARM][0] = rotatePart[LEFT_UPPER_ARM][0];

				
			}
		}
	}
}