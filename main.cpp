#include <vart/scene.h>
#include <vart/light.h>
#include <vart/arrow.h>
#include <vart/meshobject.h>
#include <vart/sphere.h>
#include <vart/contrib/viewerglutogl.h>
#include <vart/contrib/mousecontrol.h>
#include <cmath>

#include <iostream>

using namespace std;
using namespace VART;

#define NUM_CHAIR 10
#define ANGLE_BETWEEN_OF_CHAIRS (2 * M_PI) / NUM_CHAIR      // 360/NUM_CHAIR
#define RADIUS_WHEEL 70

class MyIHClass : public VART::ViewerGlutOGL::IdleHandler { // Funcao chamada pelo "main.loop()"
    public:
        double rotationAngle = 0;

        vector<Transform*> trans_tChair;
        Transform* trans_rWheel;

        virtual ~MyIHClass() {}
        virtual void OnIdle() {
            trans_rWheel->MakeRotation(Point4D::X(), rotationAngle);

            double pX = 0, pY = 0, pZ = 0;
            
            for(int i = 0; i < NUM_CHAIR; i++) {
                pY = (RADIUS_WHEEL * sin(rotationAngle + (i * ANGLE_BETWEEN_OF_CHAIRS)));
                pZ = RADIUS_WHEEL * -(cos(rotationAngle + (i * ANGLE_BETWEEN_OF_CHAIRS)));
                trans_tChair[i]->MakeTranslation(pX, pY, pZ);
            }
            
            rotationAngle += 1;
            viewerPtr->PostRedisplay();
        }
};

int main(int argc, char* argv[]) {
    ViewerGlutOGL::Init(&argc, argv); // Initialize GLUT
    static Scene scene; // create a scene
    static ViewerGlutOGL viewer; // create a viewer (application window)
    // create a camera (scene observer)
    Camera camera(Point4D(200,0,0),Point4D::ORIGIN(),Point4D::Y());
    camera.SetFarPlaneDistance(300);

    // Add lights and cameras to the scene
    scene.AddLight(Light::BRIGHT_AMBIENT());
    scene.AddCamera(&camera);

    // --------------------------------------------- ARROW
    // Create some objects
    int tamArrow = 100;
    Arrow arrowX(tamArrow);
    Arrow arrowY(Point4D::ORIGIN(), Point4D::Y()*tamArrow);
    Arrow arrowZ(Point4D::ORIGIN(), Point4D::Z()*tamArrow);

    // Initialize scene objects
    arrowX.SetMaterial(Material::PLASTIC_RED());
    arrowY.SetMaterial(Material::PLASTIC_GREEN());
    arrowZ.SetMaterial(Material::PLASTIC_BLUE());

    // Build the scene graph
    scene.AddObject(&arrowX);
    scene.AddObject(&arrowY);
    scene.AddObject(&arrowZ);
    // ---------------------------------------------

    // --------------------------------------------- Wheel
    list<MeshObject*> listObjs;
    MeshObject::ReadFromOBJ("ferris-wheel.obj", &listObjs);
    
    vector<MeshObject*> objects = {begin(listObjs), end(listObjs)};

    MeshObject* chair;      // Cadeira
    MeshObject* sup;        // Suporte
    MeshObject* wheel;      // Roda

    // Recebendo os intens
    for(unsigned int i = 0; i < objects.size(); ++i){
        if(objects[i]->GetDescription() ==  "chair")
            chair = objects[i];
        if(objects[i]->GetDescription() ==  "support")
            sup = objects[i];
        if(objects[i]->GetDescription() ==  "wheel")
            wheel = objects[i];
    }

    vector<Transform*> trans_chairs;
    for(int i = 0; i < NUM_CHAIR; i++) {
        Transform* trans_chair = new Transform();
        trans_chair->MakeIdentity();
        trans_chair->AddChild(*chair);
        trans_chairs.push_back(trans_chair);
        scene.AddObject(trans_chairs[i]);
    }
    

    Transform* trans_wheel = new Transform();
    trans_wheel->MakeIdentity();
    trans_wheel->AddChild(*wheel);

    scene.AddObject(sup);
    scene.AddObject(trans_wheel);
    // -----------------------------------------------------------

    // Set up the viewer
    MyIHClass idle;
    idle.trans_rWheel = trans_wheel;
    idle.trans_tChair = trans_chairs;

    viewer.SetTitle("Roda Gigante");
    viewer.SetScene(scene); // attach the scene
    viewer.SetIdleHandler(&idle);

    // Run application
    scene.DrawLightsOGL(); // Set OpenGL's lights' state
    ViewerGlutOGL::MainLoop(); // Enter main loop (event loop) and never return
    return 0;
}
