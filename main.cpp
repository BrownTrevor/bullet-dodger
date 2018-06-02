/*
CPE/CSC 474 Lab base code Eckhardt/Dahl
based on CPE/CSC 471 Lab base code Wood/Dunn/Eckhardt
*/

#include <iostream>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

#include "WindowManager.h"
#include "Shape.h"
#include "line.h"
#include "bone.h"
#include "controller.h"
#define ANIMATION_COUNT 2
#define IDLE 0
#define STEP 1
#define ROT_MAX 1.570796
#define ROT_TIME .005;
using namespace std;
using namespace glm;
shared_ptr<Shape> shape;
shared_ptr<Shape> bulletG;
mat4 linint_between_two_orientations(vec3 ez_aka_lookto_1, vec3 ey_aka_up_1, vec3 ez_aka_lookto_2, vec3 ey_aka_up_2, float t);


//*************************************************************************************************
//
//			TEST YOUR ANIMATION
//
//*************************************************************************************************
mat4 test_animation()
	{
	static float t = 0.0;
	t += 0.01;
	//float f = (sin(t)+1.)/2.0;//
	mat4 mt;
	vec3 ez1, ey1, ez2, ey2, ez3, ey3, ez4, ey4;

	ez1 = vec3(0, 0, 1);
	ey1 = vec3(0, 1, 0);

	ez2 = vec3(0.157114, -0.0207484, 0.987362);
	ey2 = vec3(-0.978864, 0.129268, 0.158478);

	ez3 = vec3(0.799965, -0.0758681, 0.595231);
	ey3 = vec3(-0.356247, 0.73818, 0.572869);

	ez4 = vec3(0.883246, -0.0758681, -0.462732);
	ey4 = vec3(0.361728, 0.73818, 0.569423);



	if (t <= 1)					mt = linint_between_two_orientations(ez1, ey1, ez2, ey2, t);
	else if (t <= 2)			mt = linint_between_two_orientations(ez2, ey2, ez3, ey3, t - 1.);
	else if (t <= 3)			mt = linint_between_two_orientations(ez3, ey3, ez4, ey4, t - 2.);
	else if (t <= 4)			mt = linint_between_two_orientations(ez4, ey4, ez1, ey1, t - 3.);
	else
		{
		t = 0;
		mt = linint_between_two_orientations(ez1, ey1, ez2, ey2, t);
		}


	return mt;
	}
//*************************************************************************************************
//
//			END - TEST YOUR ANIMATION
//
//*************************************************************************************************

mat4 linint_between_two_orientations(vec3 ez_aka_lookto_1, vec3 ey_aka_up_1, vec3 ez_aka_lookto_2, vec3 ey_aka_up_2, float t)
	{
	mat4 m1, m2;
	quat q1, q2;
	vec3 ex, ey, ez;
	ey = ey_aka_up_1;
	ez = ez_aka_lookto_1;
	ex = cross(ey, ez);
	m1[0][0] = ex.x;		m1[0][1] = ex.y;		m1[0][2] = ex.z;		m1[0][3] = 0;
	m1[1][0] = ey.x;		m1[1][1] = ey.y;		m1[1][2] = ey.z;		m1[1][3] = 0;
	m1[2][0] = ez.x;		m1[2][1] = ez.y;		m1[2][2] = ez.z;		m1[2][3] = 0;
	m1[3][0] = 0;			m1[3][1] = 0;			m1[3][2] = 0;			m1[3][3] = 1.0f;
	ey = ey_aka_up_2;
	ez = ez_aka_lookto_2;
	ex = cross(ey, ez);
	m2[0][0] = ex.x;		m2[0][1] = ex.y;		m2[0][2] = ex.z;		m2[0][3] = 0;
	m2[1][0] = ey.x;		m2[1][1] = ey.y;		m2[1][2] = ey.z;		m2[1][3] = 0;
	m2[2][0] = ez.x;		m2[2][1] = ez.y;		m2[2][2] = ez.z;		m2[2][3] = 0;
	m2[3][0] = 0;			m2[3][1] = 0;			m2[3][2] = 0;			m2[3][3] = 1.0f;
	q1 = quat(m1);
	q2 = quat(m2);
	quat qt = slerp(q1, q2, t); //<---
	qt = normalize(qt);
	mat4 mt = mat4(qt);
	//mt = transpose(mt);		 //<---
	return mt;
	}




double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime =glfwGetTime();
	double difference = actualtime- lasttime;
	lasttime = actualtime;
	return difference;
}
class camera
{
public:
	glm::vec3 pos, rot;
	int w, a, s, d;
	camera()
	{
		w = a = s = d = 0;
		pos = rot = glm::vec3(0, 0, 0);
	}
	glm::mat4 process(double ftime)
	{
		float speed = 0;
		if (w == 1)
		{
			speed = 10*ftime;
		}
		else if (s == 1)
		{
			speed = -10*ftime;
		}
		float yangle=0;
		if (a == 1)
			yangle = -3*ftime;
		else if(d==1)
			yangle = 3*ftime;
		rot.y += yangle;
		glm::vec4 dir = glm::vec4(0, 0, speed,1);
		glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
		dir = dir*R;
		pos += glm::vec3(dir.x, dir.y, dir.z);
		glm::mat4 T = glm::translate(glm::mat4(1), pos);
		return R*T;
	}
};
class player
{
public:
	int left, right,ff,rr;
	float lr, state;
	
	player()
	{
		left, right,state,ff,rr = 0;
	}
};
class bullet
{
public:
	glm::vec3 pos, v;
	bullet() {
		pos = vec3(0, 3, -100);
		v = vec3((rand() % 4) -2, (rand() % 4) - 2, 12);
	}
	void process(double ftime){
		pos += v * (float)ftime;
	}
	mat4 getRot() {
		mat4 Matrix;
		vec3 Z = normalize(v);
		vec3 X = normalize(cross(vec3(0,1,0),Z));
		vec3 Y = normalize(cross(Z, X));
		Matrix[0][0] = X.x; Matrix[0][1] = X.y; Matrix[0][2] = X.z; Matrix[0][3] = 0;
		Matrix[1][0] = Y.x; Matrix[1][1] = Y.y; Matrix[1][2] = Y.z; Matrix[1][3] = 0;
		Matrix[2][0] = Z.x; Matrix[2][1] = Z.y; Matrix[2][2] = Z.z; Matrix[2][3] = 0;
		Matrix[3][0] = 0; Matrix[3][1] = 0; Matrix[3][2] = 0; Matrix[3][3] = 1.0f;
		return Matrix * rotate(mat4(1), (float)(3.1415 / 2), vec3(1, 0, 0));;
	}
};

camera mycam;
player myplayer;

class Application : public EventCallbacks
{

public:

    WindowManager *windowManager = nullptr;
    
    // Our shader program
    std::shared_ptr<Program> shape, prog, bulletsh;
    
    GLuint VertexArrayID;
	GLuint VertexBufferID[ANIMATION_COUNT];
	GLuint VertexBufferIDimat[ANIMATION_COUNT];
    GLuint VertexNormDBox, VertexTexBox, IndexBufferIDBox;
    
    
    //animation matrices:
    mat4 animmat[200];
    int animmatsize=0;
    
    string filename;
    double gametime = 0;
    bool mousePressed = false;
    bool mouseCaptured = false;
    glm::vec2 mouseMoveOrigin = glm::vec2(0);
    glm::vec3 mouseMoveInitialCameraRot;
	bone *root = NULL;
    int size_stick = 0;
    all_animations all_animation;
    
	//bullet vector
	vector<bullet> bullets;

	//controller global
	CXBOXController *gamepad = new CXBOXController(1);

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			mycam.w = 1;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			mycam.w = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			mycam.s = 1;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			mycam.s = 0;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			mycam.a = 1;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			mycam.a = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			mycam.d = 1;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			mycam.d = 0;
		}
		if (key == GLFW_KEY_Q && action == GLFW_PRESS)
		{
			myplayer.left = 1;
		}
		if (key == GLFW_KEY_Q && action == GLFW_RELEASE)
		{
			myplayer.left = 0;
		}
		if (key == GLFW_KEY_E && action == GLFW_PRESS)
		{
			myplayer.right = 1;
		}
		if (key == GLFW_KEY_E && action == GLFW_RELEASE)
		{
			myplayer.right = 0;
		}
		if (key == GLFW_KEY_O && action == GLFW_PRESS)
		{
			myplayer.rr = 1;
		}
		if (key == GLFW_KEY_O && action == GLFW_RELEASE)
		{
			myplayer.rr = 0;
		}
		if (key == GLFW_KEY_P && action == GLFW_PRESS)
		{
			myplayer.ff = 1;
		}
		if (key == GLFW_KEY_P && action == GLFW_RELEASE)
		{
			myplayer.ff = 0;
		}
		
	}

	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		//DO NOTHING RIGHT NOW
	}
    
    void mouseMoveCallback(GLFWwindow *window, double xpos, double ypos) {
        //DO NOTHING RIGHT NOW
    }

	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}

	/*Note that any gl calls must always happen after a GL state is initialized */
	
	void initGeom(const std::string& resourceDirectory)
	{
		//====================================================================================================
		// Create Geometry for Bullet
		//===================================================================================================
		bulletG = make_shared<Shape>();
		bulletG->loadMesh(resourceDirectory + "/bullet.obj");
		bulletG->resize();
		bulletG->init();

		for (int ii = 0; ii < 200; ii++)
			animmat[ii] = mat4(1);
		readtobone(resourceDirectory + "/walk.FBX", &all_animation, &root, 1);
		readtobone(resourceDirectory + "/run.FBX", &all_animation, &root, 0);
		root->set_animations(&all_animation, animmat, animmatsize);

	

		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);
		/*aniamtion IDLE*/
		glGenBuffers(1, &VertexBufferID[IDLE]);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID[IDLE]);
		vector<vec3> posIDLE;
		vector<unsigned int> imatIDLE;
		root->write_to_VBOs(vec3(0, 0, 0), posIDLE, imatIDLE);
		size_stick = posIDLE.size();

		
		
		

		//====================================================================================================
		// Allocate Space for Bones
		//====================================================================================================
		/*animation idle*/
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*posIDLE.size(), posIDLE.data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		

		//====================================================================================================
		// Allocate Space for Animations
		//====================================================================================================
		/*animation idle*/
		glGenBuffers(1, &VertexBufferIDimat[IDLE]);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferIDimat[IDLE]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uint)*imatIDLE.size(), imatIDLE.data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 0, (void*)0);


		glBindVertexArray(0);
		glUseProgram(prog->pid);
	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
		prog = std::make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
		if (!prog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("Manim");
		prog->addUniform("campos");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertimat");

		bulletsh = std::make_shared<Program>();
		bulletsh->setVerbose(true);
		bulletsh->setShaderNames(resourceDirectory + "/plane_vertex.glsl", resourceDirectory + "/plane_frag.glsl");
		if (!bulletsh->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		bulletsh->addUniform("P");
		bulletsh->addUniform("V");
		bulletsh->addUniform("M");
		bulletsh->addUniform("campos");
		bulletsh->addAttribute("vertPos");
		bulletsh->addAttribute("vertNor");
		bulletsh->addAttribute("vertTex");

	}

    glm::mat4 getPerspectiveMatrix() {
        float fov = 3.14159f / 4.0f;
        float aspect = windowManager->getAspect();
        return glm::perspective(fov, aspect, 0.01f, 10000.0f);
    }
	
	void render()
	{


<<<<<<< HEAD
        //====================================================================================================
        // INIT
        //====================================================================================================
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.3f, 0.7f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        
        //====================================================================================================
        // Frame Data
        //====================================================================================================
        double frametime = get_last_elapsed_time();
        static double totaltime_ms=0;
        totaltime_ms += frametime*1000.0;
        static double totaltime_untilframe_ms = 0;
        totaltime_untilframe_ms += frametime*1000.0;
        
        for (int ii = 0; ii < 200; ii++)
            animmat[ii] = mat4(1);
        
        //animation frame system
        int anim_step_width_ms = 8490 / 204;
        static int frame = 0;
        if (totaltime_untilframe_ms >= anim_step_width_ms)
        {
            totaltime_untilframe_ms = 0;
            frame++;
        }
        root->play_animation(frame,"axisneurontestfile_Avatar00");    //name of current animation
        
=======
		//====================================================================================================
		// INIT
		//====================================================================================================
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClearColor(0.3f, 0.7f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//====================================================================================================
		// Game Pad
		//====================================================================================================

		if (gamepad->IsConnected())
		{

			//	BUTTON PRESS
			if (gamepad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y)
				mycam.w = 1;
			else
				mycam.w = 0;
			if (gamepad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)
			{
				mycam.s = 1;
			}
			else
				mycam.s = 0;

		}
		// ANALOG STICKS
		SHORT lx = gamepad->GetState().Gamepad.sThumbLX;
		SHORT ly = gamepad->GetState().Gamepad.sThumbLY;

		if (abs(ly) > 3000)
		{
			float angle_x = (float)ly / 32000.0;
			angle_x *= 0.05;
			mycam.rot.x -= angle_x;
		}
		if (abs(lx) > 3000)
		{
			float angle_y = (float)lx / 32000.0;
			angle_y *= 0.05;
			mycam.rot.y -= angle_y;
		}


		//====================================================================================================
		// Frame Data
		//====================================================================================================
		double frametime = get_last_elapsed_time();
		static double totaltime_ms = 0;
		totaltime_ms += frametime * 1000.0;
		static double totaltime_untilframe_ms = 0;
		totaltime_untilframe_ms += frametime * 1000.0;
		//====================================================================================================
		// Fire Bullet
		//====================================================================================================
		static int ticks = 0;
		/*fire bullet every second*/
		if (totaltime_ms / 10000 * 8 > ticks) {
			cout << to_string(totaltime_ms / 1000) << " " << to_string(ticks) << endl;
			ticks++;
			bullet x;
			bullets.push_back(x);
		}
		/*process bullet*/
		for (int bi = 0; bi < bullets.size(); bi++) {
			bullets.at(bi).process(frametime);
			if (bullets.at(bi).pos.z > 0) {
				bullets.erase(bullets.begin() + bi);
			}
		}


		/*idle animation*/
		for (int ii = 0; ii < 200; ii++)
			animmat[ii] = mat4(1);

		//animation frame system
		int anim_step_width_ms = 8490 / 204;
		static int frame = 0; 
		
		//find average step
		float avgkfn = (root->animation[0]->keyframes.size() + root->animation[1]->keyframes.size()) / 2;
		static float fframe = 0;
		if (myplayer.left || myplayer.right) {
			//rotate left -
			if (myplayer.left) {
				if (myplayer.lr  > -ROT_MAX) {
					myplayer.lr -= frametime * 1000.0 * ROT_TIME;
				}
			}
			//rotate right +
			else {
				if (myplayer.lr  < ROT_MAX) {
					myplayer.lr += frametime * 1000.0 * ROT_TIME;
				}
			}
			//switch to running
			if (myplayer.state < .999) {
				myplayer.state += 1 / avgkfn;
			}
		}
		else {
			if (myplayer.state > .001) {
				myplayer.state -= 1 / avgkfn;
			}
			//decay rotation
			float decay_step = frametime * 1000.0 * ROT_TIME;
			if (abs(myplayer.lr) < decay_step) {
				myplayer.lr = 0;
			}
			else if (myplayer.lr > 0) {
				myplayer.lr -= decay_step;
			}
			else {
				myplayer.lr += decay_step;
			}
		}
<<<<<<< HEAD
>>>>>>> 3b481b8d4bc89117c61feea59bbdf2d22bde9759
       
=======
		//loop animation
		if ( myplayer.ff )
			frametime *= 2;
		if ( myplayer.rr )
			frametime /= 2;
		
		fframe += (frametime * 1000.0 / anim_step_width_ms);
		root->play_animation(&fframe, 0, 1, myplayer.state);

		//rotate player
		glm::mat4 player_rotate = glm::rotate(glm::mat4(1), myplayer.lr, glm::vec3(0, 1, 0));
>>>>>>> 7e10af6e3cc6dd508d09b48483ff6c368d802de5
		
        //====================================================================================================
        // Setup Matrices
        //====================================================================================================
        glm::mat4 V, M, P;
        P = getPerspectiveMatrix();
        V = mycam.process(frametime);
        M = glm::mat4(1);
        
        glm::mat4 TransZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -8));
        glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f));
        M =  TransZ * player_rotate * S;
        

        
        //====================================================================================================
        // Send to Shaders and draw Animation
        //====================================================================================================
        
		glBindVertexArray(VertexArrayID);
        prog->bind();
        glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        glUniformMatrix4fv(prog->getUniform("Manim"), 200, GL_FALSE, &animmat[0][0][0]);
        glDrawArrays(GL_LINES, 4, size_stick-4);
        glBindVertexArray(0);
        prog->unbind();
        
		//====================================================================================================
		// Send to Shaders and draw Bullet
		//====================================================================================================

		bulletsh->bind();
		for (int bi = 0; bi < bullets.size(); bi++) {
			mat4 transbullet = glm::translate(glm::mat4(1.0f), bullets.at(bi).pos);
			M = transbullet * bullets.at(bi).getRot();
			glUniformMatrix4fv(bulletsh->getUniform("P"), 1, GL_FALSE, &P[0][0]);
			glUniformMatrix4fv(bulletsh->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(bulletsh->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniform3fv(bulletsh->getUniform("campos"), 1, &mycam.pos[0]);
			bulletG->draw(bulletsh, false);			//render!!!!!!!
		}
		bulletsh->unbind();
    

	}

};
//******************************************************************************************
int main(int argc, char **argv)
{
	std::string resourceDir = "../resources"; // Where the resources are loaded from
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager * windowManager = new WindowManager();
	windowManager->init(1920, 1080);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
	// Initialize scene.
	application->init(resourceDir);
	application->initGeom(resourceDir);

	// Loop until the user closes the window.
	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
