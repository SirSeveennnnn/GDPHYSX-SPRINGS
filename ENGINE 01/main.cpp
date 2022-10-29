#include<glad/glad.h>
#include <GLFW/glfw3.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include<string>
#include<iostream>

#include"MyShaders.h"
#include"Camera.h"

#include"Particle.h"
#include"Fireworks.h"
#include"ParticleGravity.h"
#include"ParticleForceRegistry.h"
#include"BasicSpringForce.h"
#include"AnchoredSpringForce.h"
#include"ElasticBungeeForce.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include"tiny_obj_loader.h"


using namespace std;

//Init callbacks
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);


//Init Particle and Fireworks pool
vector<Particle*> particles1;
vector<Particle*> particles2;
vector<Fireworks*> fireworks;
ParticleForceRegistry* registry = new ParticleForceRegistry();

//forces
ParticleGravity* gravity = new ParticleGravity(glm::vec3(0, -5.f, 0));

//Particle Counter and Pool Size Limit
int particleLimit = 30;
int particleCounter1 = 1; // index 0 will be a place holder
int particleCounter2 = 1; // index 0 will be a place holder

//Firwork Counter and Pool Size Limit
int fireworksLimit = 30;
int fireworksCounter = 0;

int main(void)
{
    // width and height of the screen
    float width = 1080;
    float height = 720;
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Jae Macuha", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();

    glViewport(0, 0, width, height);

    //Init mouse and key callbacks
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetKeyCallback(window, KeyCallBack);

    //Create Shaders
    MyShaders* shader = new MyShaders("Shaders/shader.vert", "Shaders/shader.frag");

    // Delta Time
    float lastTime = glfwGetTime();

    //create camera
    Camera camera(width,height);

    //Create Particles
    for (int i = 0; i < particleLimit; i++)
    {
        particles1.push_back(new Particle(glm::vec3(-50, -50, -30)));
    }

    for (int i = 0; i < particleLimit; i++)
    {
        particles2.push_back(new Particle(glm::vec3(50, -50, -30)));
    }
    

    //Create Fireworks
    for (int i = 0; i < fireworksLimit; i++)
    {
        fireworks.push_back(new Fireworks(glm::vec3(-50, -50, -30)));
    }

    for (int i = 0; i < particleLimit; i++)
    {
        registry->add(particles1[i], gravity);
    }


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // Delta Time
        //current time
        GLfloat currTime = glfwGetTime();
        float deltaTime = currTime - lastTime;
        //Add Forces
        for (int i = 0; i < registry->Size(); i++)
        {
            registry->UpdateForces(deltaTime);
        }


        //Physics Updates go here
        for (int i = 0; i < particles1.size(); i++)
        {
            particles1[i]->Update(deltaTime);
        }

        for (int i = 0; i < particles2.size(); i++)
        {
            particles2[i]->Update(deltaTime);
        }

        for (int i = 0; i < fireworks.size(); i++)
        {
            fireworks[i]->Update(deltaTime, fireworks, fireworksCounter, fireworksLimit);
        }

        //Draw Models
        for (int i = 0; i < particles1.size(); i++)
        {
            particles1[i]->Draw(shader->GetShaderProgram(), camera);
        }
        for (int i = 0; i < particles2.size(); i++)
        {
            particles2[i]->Draw(shader->GetShaderProgram(), camera);
        }

        for (int i = 0; i < fireworks.size(); i++)
        {
            fireworks[i]->Draw(shader->GetShaderProgram(), camera);
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        lastTime = currTime; // delta time
    }
    //Free Memory
    for (int i = 0; i < particles1.size(); i++)
    {
        particles1[i]->Destroy();
    }

    glfwTerminate();
    return 0;

}


// current ballistic selected
BallisticType currentBallistic = Pistol;

//Callback for Mouse Inputs
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        if (currentBallistic  == Pistol || currentBallistic == Artillery || currentBallistic == Fireball || currentBallistic == Laser)
        {
            particles1[particleCounter1]->FireBallistic(currentBallistic);
            particleCounter1++;
            if (particleCounter1 > (particleLimit - 1)) 
            {
                particleCounter1 = 1; //We use 1 because the first particle is a placeholder
            }
        }
        else if (currentBallistic == FireworkEnum)
        {
            fireworks[fireworksCounter]->FireFireworks(Parent, 0, nullptr);
            fireworksCounter++;
            if (fireworksCounter > fireworksLimit - 1)
            {
                fireworksCounter = 0;
            }
        }
        else if (currentBallistic == BasicSpring)
        {
            particles1[particleCounter1]->FireSpring();
            particles2[particleCounter2]->FireSpring();

            //first particle
            BasicSpringForce* first = new BasicSpringForce(particles2[particleCounter2], 0.1f, 70);
            registry->add(particles1[particleCounter1], first);

            BasicSpringForce* second = new BasicSpringForce(particles1[particleCounter1], 0.1f, 70);
            registry->add(particles2[particleCounter2], second);
          
            
            particleCounter1++;
            if (particleCounter1 > (particleLimit - 1))
            {
                particleCounter1 = 1; //We use 1 because the first particle is a placeholder
            }

            particleCounter2++;
            if (particleCounter2 > (particleLimit - 1))
            {
                particleCounter2 = 1; //We use 1 because the first particle is a placeholder
            }
        }
        else if (currentBallistic == AnchoredSpring)
        {
            particles1[particleCounter1]->FireSpring();
            AnchoredSpringForce* anchor = new AnchoredSpringForce(glm::vec3(-50, -50, -30), 0.1f, 50);
            registry->add(particles1[particleCounter1], anchor);

            particleCounter1++;
            if (particleCounter1 > (particleLimit - 1))
            {
                particleCounter1 = 1; //We use 1 because the first particle is a placeholder
            }

            particles2[particleCounter2]->FireSpring();
            AnchoredSpringForce* anchor2 = new AnchoredSpringForce(glm::vec3(50, -50, -30), 0.1f, 50);
            registry->add(particles2[particleCounter2], anchor2);

            particleCounter2++;
            if (particleCounter2 > (particleLimit - 1))
            {
                particleCounter2 = 1; //We use 1 because the first particle is a placeholder
            }
        }
        else if (currentBallistic == ElasticSpring)
        {
            particles1[particleCounter1]->FireSpring();
            particles2[particleCounter2]->FireSpring();

            //first particle
            ElasticBungeeForce* first = new ElasticBungeeForce(particles2[particleCounter2], 0.1f, 50);
            registry->add(particles1[particleCounter1], first);

            ElasticBungeeForce* second = new ElasticBungeeForce(particles1[particleCounter1], 0.1f, 50);
            registry->add(particles2[particleCounter2], second);


            particleCounter1++;
            if (particleCounter1 > (particleLimit - 1))
            {
                particleCounter1 = 1; //We use 1 because the first particle is a placeholder
            }

            particleCounter2++;
            if (particleCounter2 > (particleLimit - 1))
            {
                particleCounter2 = 1; //We use 1 because the first particle is a placeholder
            }
        }
        
    }

}

//Callback for Keyboard Inputs
void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    registry->clear();

    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        currentBallistic = Pistol;
    }
    else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        currentBallistic = Artillery;
    }
    else if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
        currentBallistic = Fireball;
    }
    else if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
        currentBallistic = Laser;
    }
    else if (key == GLFW_KEY_5 && action == GLFW_PRESS) {
        currentBallistic = FireworkEnum;
    }
    else if (key == GLFW_KEY_6 && action == GLFW_PRESS) {
        currentBallistic = BasicSpring;
    }
    else if (key == GLFW_KEY_7 && action == GLFW_PRESS) {
        currentBallistic = AnchoredSpring;
    }
    else if (key == GLFW_KEY_8 && action == GLFW_PRESS) {
        currentBallistic = ElasticSpring;
    }

}
