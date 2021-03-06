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
#include "controller.h"
#include "WindowManager.h"
#include "Shape.h"
#include <algorithm>
#include "line.h"
#include "bone.h"
#define GEOM_COUNT 2
#define ROT_MAX 1.570796
#define ROT_TIME .005;
#define EMITTER_DISTANCE 100
#define EMITTER_OFFSET 20
#define EMITTER_HEIGHT 2
#define BULLET_SPEED 15
#define SEEKER_PROB 3
#define GRAVITY_C -5
#define PARTICLE_SPEED 1
#define PARTICLE_SPREAD 2
#define PARTICLE_NUM 10;
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
	vec3 pos = vec3(0.0f,0.0f,0.0f);
	player()
	{
		left, right,state,ff,rr = 0;
	}
};
class particle
{
public:
	vec3 pos, v;
	particle(vec3 pos_init) {
		pos = pos_init;
		v = vec3(get_rand_f(), get_rand_f(), get_rand_f());
	}
	void process(double ftime) {
		v.y += GRAVITY_C * ftime;
		pos += v * (float)ftime;
	}
private:
	float get_rand_f() {
		float num = PARTICLE_SPREAD * ((float)rand() / RAND_MAX * 2 - 1);
		return num;
	}
};

class bullet
{
public:
	glm::vec3 pos, v;
	int type;
	bullet(vec3 ppos) {
		type = rand() % SEEKER_PROB;
		pos = vec3(EMITTER_OFFSET * (rand() % 3) - EMITTER_OFFSET, EMITTER_HEIGHT, ppos.z - EMITTER_DISTANCE);
		if (type  != 0) {
			v = vec3(0, 0, BULLET_SPEED);
		}
		else {
			v = ppos - pos;
			v.z = BULLET_SPEED;
		}
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
    std::shared_ptr<Program> shape, prog, particlesh, bulletsh;
    
    GLuint VertexArrayID[GEOM_COUNT];
	GLuint VertexBufferID[GEOM_COUNT];
	GLuint VertexBufferIDimat[GEOM_COUNT];
    GLuint VertexNormDBox, VertexTexBox, IndexBufferIDBox;
    GLuint SkyTex;
    
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

	//particle vector
	vector<particle> particles;

	//controller global
	CXBOXController *gamepad = new CXBOXController(1);

	//texture data
	GLuint Texture;

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
        // Create Geometry Sky Sphere
        //===================================================================================================
        
        /*
        //shape->loadMesh(resourceDirectory + "/sphere.obj");
        //shape->resize();
        //shape->init();
        
        int width, height, channels;
        char filepath[1000];
        
        //Sky Sphere
        string str = resourceDirectory + "/stars_milky_way.jpg";
        strcpy(filepath, str.c_str());
        unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &NightTex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, NightTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        */
        
		//====================================================================================================
		// Create Geometry for Bullet
		//===================================================================================================
		bulletG = make_shared<Shape>();
		bulletG->loadMesh(resourceDirectory + "/bullet.obj");
		bulletG->resize();
		bulletG->init();

		//====================================================================================================
		// Load Animations
		//===================================================================================================

		for (int ii = 0; ii < 200; ii++)
			animmat[ii] = mat4(1);
		readtobone(resourceDirectory + "/walk.FBX", &all_animation, &root, 1);
		readtobone(resourceDirectory + "/run.FBX", &all_animation, &root, 0);
		root->set_animations(&all_animation, animmat, animmatsize);

		glGenVertexArrays(1, &VertexArrayID[0]);
		glBindVertexArray(VertexArrayID[0]);
		/*aniamtion IDLE*/
		glGenBuffers(1, &VertexBufferID[0]);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID[0]);
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
		glGenBuffers(1, &VertexBufferIDimat[0]);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferIDimat[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uint)*imatIDLE.size(), imatIDLE.data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 0, (void*)0);


		glBindVertexArray(0);
		//glUseProgram(prog->pid);

		

		//====================================================================================================
		// Create Geometry for Particle 
		//===================================================================================================

		//generate the VAO
		glGenVertexArrays(1, &VertexArrayID[1]);
		glBindVertexArray(VertexArrayID[1]);
		
		glGenBuffers(1, &VertexBufferID[1]);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID[1]);

		GLfloat cube_vertices[] = {
			// front
			-1.0, -1.0,  1.0,//LD
			1.0, -1.0,  1.0,//RD
			1.0,  1.0,  1.0,//RU
			-1.0,  1.0,  1.0,//LU
		};
		//make it a bit smaller
		for (int i = 0; i < 12; i++)
			cube_vertices[i] *= 0.5;
		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_DYNAMIC_DRAW);

		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//color
		GLfloat cube_norm[] = {
			// front colors
			0.0, 0.0, 1.0,
			0.0, 0.0, 1.0,
			0.0, 0.0, 1.0,
			0.0, 0.0, 1.0,

		};
		glGenBuffers(1, &VertexNormDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexNormDBox);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_norm), cube_norm, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//color
		glm::vec2 cube_tex[] = {
			// front colors
			glm::vec2(0.0, 1.0),
			glm::vec2(1.0, 1.0),
			glm::vec2(1.0, 0.0),
			glm::vec2(0.0, 0.0),

		};
		glGenBuffers(1, &VertexTexBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexTexBox);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_tex), cube_tex, GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &IndexBufferIDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		GLushort cube_elements[] = {

			// front
			0, 1, 2,
			2, 3, 0,
		};
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

		glBindVertexArray(0);

		//====================================================================================================
		// Bind Texture for Particle 
		//===================================================================================================

		int width, height, channels;
		char filepath[1000];

		//texture 1
		string str = resourceDirectory + "/Blue_Giant.jpg";
		strcpy(filepath, str.c_str());
		unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		GLuint Tex1Location = glGetUniformLocation(particlesh->pid, "tex");//tex, tex2... sampler in the fragment shader
		// Then bind the uniform samplers to texture units:
		glUseProgram(particlesh->pid);
		glUniform1i(Tex1Location, 0);
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
		bulletsh->addUniform("type");
		bulletsh->addUniform("campos");
		bulletsh->addAttribute("vertPos");
		bulletsh->addAttribute("vertNor");
		bulletsh->addAttribute("vertTex");

		particlesh = std::make_shared<Program>();
		particlesh->setVerbose(true);
		particlesh->setShaderNames(resourceDirectory + "/shader_vertex_particle.glsl", resourceDirectory + "/shader_fragment_particle.glsl");
		if (!particlesh->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		particlesh->addUniform("P");
		particlesh->addUniform("V");
		particlesh->addUniform("M");
		particlesh->addUniform("campos");
		particlesh->addAttribute("vertPos");
		particlesh->addAttribute("vertNor");
		particlesh->addAttribute("vertTex");
	}

    glm::mat4 getPerspectiveMatrix() {
        float fov = 3.14159f / 4.0f;
        float aspect = windowManager->getAspect();
        return glm::perspective(fov, aspect, 0.01f, 10000.0f);
    }
	
	struct mysortfunction
	{
		inline bool operator() (particle& p1, particle& p2) {
			vec3 v1 = p1.pos;
			vec3 v2 = p2.pos;
			float l1 = sqrtf(std::pow(v1.x + mycam.pos.x, 2) + std::pow(v1.y + mycam.pos.y, 2) + std::pow(v1.z + mycam.pos.z, 2));
			float l2 = sqrtf(std::pow(v2.x + mycam.pos.x, 2) + std::pow(v2.y + mycam.pos.y, 2) + std::pow(v2.z + mycam.pos.z, 2));
			return l1 > l2;
		}
	};


	void render()
	{


        //====================================================================================================
        // INIT
        //====================================================================================================
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.3f, 0.7f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        
        //====================================================================================================
        // Frame Data
        //====================================================================================================
        double frametime = get_last_elapsed_time();
        static double totaltime_ms=0;
        totaltime_ms += frametime*1000.0;
        static double totaltime_untilframe_ms = 0;
        totaltime_untilframe_ms += frametime*1000.0;
        

		//====================================================================================================
		// Game Pad
		//====================================================================================================
		if (gamepad->IsConnected())
		{

			//	BUTTON PRESS
			if (gamepad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y)
				myplayer.left = 1;
			else
				myplayer.left = 0;
			if (gamepad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)
			{
				myplayer.right = 1;
			}
			else
				myplayer.right = 0;

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
		// Fire Bullet
		//====================================================================================================
		static int ticks = 0;
		/*fire bullet every second*/
		if (totaltime_ms / 10000 * 8 > ticks) {
			//cout << to_string(totaltime_ms / 1000) << " " << to_string(ticks) << endl;
			ticks++;
			bullet x(myplayer.pos);
			bullets.push_back(x);
		}
		/*process bullet*/
		for (int bi = 0; bi < bullets.size(); bi++) {
			bullets.at(bi).process(frametime);
			if (bullets.at(bi).pos.z > -10) {
				for (int j = 0; j < 10; j++) {
					particle x = particle(bullets.at(bi).pos);
					particles.push_back(x);
				}
				bullets.erase(bullets.begin() + bi);
			}
		}

		/* process particles */
		for (int i = 0; i < particles.size(); i++) {
			particles.at(i).process(frametime);
			if (particles.at(i).pos.y < 0) {
				particles.erase(particles.begin() + i);
			}
		}


        //animation frame system
        int anim_step_width_ms = 8490 / 204;
        static int frame = 0;
        if (totaltime_untilframe_ms >= anim_step_width_ms)
        {
            totaltime_untilframe_ms = 0;
            frame++;
        }
        
		for (int ii = 0; ii < 200; ii++)
			animmat[ii] = mat4(1);
		
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

		//loop animation
		if ( myplayer.ff )
			frametime *= 2;
		if ( myplayer.rr )
			frametime /= 2;
		
		fframe += (frametime * 1000.0 / anim_step_width_ms);
		root->play_animation(&fframe, 0, 1, myplayer.state);

		//rotate player
        glm::mat4 player_rotate = glm::rotate(glm::mat4(1), -myplayer.lr, glm::vec3(0, 1, 0));
        
        
        static float lateral_trans = 0.0;
        static float progressive_trans = 0;
        static int highscore = 0.0;
        
        highscore = (int)(progressive_trans * -100);
        
        cout << highscore << endl;

        if(myplayer.lr > 0.0) {
            lateral_trans += .2;
        }
        else if(myplayer.lr < 0.0) {
            
            lateral_trans -= .2;
        }
        else {
            progressive_trans -= .05;
        }
        
        
        //====================================================================================================
        // Setup Matrices
        //====================================================================================================
        glm::mat4 V, M, P;
        P = getPerspectiveMatrix();
        V = mycam.process(frametime);
        M = glm::mat4(1);
        
        glm::mat4 TransZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -8 + progressive_trans));
        glm::mat4 TransX = glm::translate(glm::mat4(1.0f), glm::vec3(lateral_trans, 0.0f, 0.0f));
        glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f));
        M =  TransZ * TransX * player_rotate * S;
        

        
        //====================================================================================================
        // Send to Shaders and draw Animation
        //====================================================================================================
        
		glBindVertexArray(VertexArrayID[0]);
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
			glUniform1i(bulletsh->getUniform("type"),bullets.at(bi).type);
			glUniform3fv(bulletsh->getUniform("campos"), 1, &mycam.pos[0]);
			bulletG->draw(bulletsh, false);			//render!!!!!!!
		}
		bulletsh->unbind();
		
		//====================================================================================================
		// Send to Shaders and draw Particles 
		//====================================================================================================
		particlesh->bind();


		//send the matrices to the shaders
		glUniformMatrix4fv(particlesh->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(particlesh->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(particlesh->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(particlesh->getUniform("campos"), 1, &mycam.pos[0]);



		glBindVertexArray(VertexArrayID[1]);
		//actually draw from vertex 0, 3 vertices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
		mat4 Vi = glm::transpose(V);
		Vi[0][3] = 0;
		Vi[1][3] = 0;
		Vi[2][3] = 0;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);

		//sort
		if (particles.size() > 0) {
			std::sort(particles.begin(), particles.end(), mysortfunction());
		}
		//draw
		for (int i = 0; i < particles.size(); i++)
		{
			glm::mat4 TransZ = glm::translate(glm::mat4(1.0f), particles.at(i).pos);
			//TransZ = glm::translate(glm::mat4(1.0f), vec3(0,0,-5));
			M = TransZ * Vi;


			glUniformMatrix4fv(particlesh->getUniform("M"), 1, GL_FALSE, &M[0][0]);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);

		}
		glBindVertexArray(0);


		particlesh->unbind();
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
