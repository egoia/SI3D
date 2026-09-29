
//! \file tp.cpp code de depart minimaliste


#include "wavefront.h"
#include "texture.h"

#include "draw.h"        
#include "app_camera.h"        // classe Application a deriver#include "program.h"
#include "program.h"
#include "uniforms.h"


// utilitaire. creation d'une grille / repere. permet de visualiser l'espace autour des objets.
Mesh make_grid( const int n= 10 )
{
    Mesh grid= Mesh(GL_LINES);
    
    // grille
    grid.color(White());
    float w= float(n-1) / 2;
    for(int x= 0; x < n; x++)
    {
        grid.vertex(x -w, 0, -w); 
        grid.vertex(x -w, 0,  w);
    }
    for(int z= 0; z < n; z++)
    {
        grid.vertex(-w, 0, z -w); 
        grid.vertex( w, 0, z -w); 
    }
    
    // axes XYZ
    grid.color(Red());
    grid.vertex(Point(0, .1, 0));
    grid.vertex(Point(1, .1, 0));
    
    grid.color(Green());
    grid.vertex(Point(0, .1, 0));
    grid.vertex(Point(0, 1, 0));
    
    grid.color(Blue());
    grid.vertex(Point(0, .1, 0));
    grid.vertex(Point(0, .1, 1));
    
    glLineWidth(2);
    
    return grid;
}

Mesh make_grid_camera( )
{
    Mesh camera= Mesh(GL_LINES);
    
    // pyramide de vision de la camera
    camera.color(Yellow());
    camera.vertex(0,0,0);
    camera.vertex(-0.5, -0.5, -1);
    camera.vertex(0,0,0);
    camera.vertex(-0.5, 0.5, -1);
    camera.vertex(0,0,0);
    camera.vertex(0.5, 0.5, -1);
    camera.vertex(0,0,0);
    camera.vertex(0.5, -0.5, -1);
    
    camera.vertex(-0.5, -0.5, -1);
    camera.vertex(-0.5, 0.5, -1);
 
    camera.vertex(-0.5, 0.5, -1);
    camera.vertex(0.5, 0.5, -1);
 
    camera.vertex(0.5, 0.5, -1);
    camera.vertex(0.5, -0.5, -1);
    
    camera.vertex(0.5, -0.5, -1);
    camera.vertex(-0.5, -0.5, -1);
    
     //axes
    camera.color(Red());
    camera.vertex(Point(0, .1, 0));
    camera.vertex(Point(1, .1, 0));
    
    camera.color(Green());
    camera.vertex(Point(0, .1, 0));
    camera.vertex(Point(0, 1, 0));
    
    camera.color(Blue());
    camera.vertex(Point(0, .1, 0));
    camera.vertex(Point(0, .1, 1));
    
    glLineWidth(2);
    
    return camera;
}


class TP : public AppCamera
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    TP( ) : AppCamera(1024, 640) {}
    
    
    // creation des objets de l'application
    int init( )
    {
        camera().lookat(Point(0,0,0), 100);
        // decrire un repere / grille 
        m_repere= make_grid(10);
        
        // charge un objet
        //~ m_objet= read_mesh("data/cube.obj");
        m_robot= read_indexed_mesh("data/robot.obj");
        m_robot_frame_2 = read_indexed_mesh("data/robot.obj");
        m_bigguy= read_mesh("data/bigguy.obj");
        m_cube= read_mesh("data/cube.obj");
        
        // etat openGL par defaut
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre
        
        glClearDepth(1.f);                          // profondeur par defaut
        glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
        glEnable(GL_DEPTH_TEST);                    // activer le ztest

        program= 0;
 
        program = read_program("projets/shader.glsl");
        program_print_errors(program);
        

        return 0;   // pas d'erreur, sinon renvoyer -1
    }
    
    // destruction des objets de l'application
    int quit( )
    {
        m_robot.release();
        m_repere.release();
        m_bigguy.release();
        m_cube.release();
        return 0;   // pas d'erreur
    }
    
    // dessiner une nouvelle image
    int render( )
    {
        // transformations standards
        Transform model= Identity();
        Transform view= camera().view();
        Transform projection= camera().projection(float(window_width()), float(window_height()), 45);
        
        // Transform viewport= Viewport(window_width(), window_height());
        glViewport(0, 0, window_width(), window_height());
        
        // couleur et profondeur par defaut
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        vec3 green(0.2,0.8,0.2);
        vec3 yellow(0.2, 0.8,0.8);
        
        vec3 red(0.8, 0.2,0.2);
        
        //Robot
        Transform robotTransform = Translation( 3, 0, 2);

        Transform mvMatrix = view * robotTransform;
        Transform mvpMatrix = projection * mvMatrix;

        glUseProgram(program);
        program_uniform(program, "mvpMatrix", mvpMatrix);
        program_uniform(program, "mvMatrix", mvMatrix);

        auto groups = m_robot.groups();
        assert(groups.size()==3);
        for(size_t i = 0; i<groups.size(); i++){
            if(i==0){
                program_uniform(program, "color", green);
            }
            else if (i==1){
                program_uniform(program, "color", yellow);
            }
            else{
                program_uniform(program, "color", vec3(1,1,1));
            }
            m_robot.draw(groups[i].first, groups[i].n, program);
        }

        GLuint VAO = 0;
        GLuint VBO = 0;
        GLuint EBO = 0;

        std::vector<vec3> positions = m_robot.positions();
        std::vector<unsigned int> indices = m_robot.indices();

        // 1. Génération des 3 objets
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        // 2. Lier le VAO d'abord
        glBindVertexArray(VAO);

        // 3. VBO (Positions)
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 
                     positions.size() * sizeof(vec3), 
                     positions.data(), 
                     GL_STATIC_DRAW);

        // 4. EBO (Indices) -> lié dans le VAO actif
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                     indices.size() * sizeof(unsigned int), 
                     indices.data(), 
                     GL_STATIC_DRAW);


        // 5. Attribut de position (location = 0)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0, 
            3, 
            GL_FLOAT, 
            GL_FALSE, 
            sizeof(vec3), 
            (void*)0
        );

        // 6. Déliage propre
        glBindVertexArray(0); // Détache le VAO EN PREMIER
        glBindBuffer(GL_ARRAY_BUFFER, 0);             
        m_robot.draw(program);

        
        program_uniform(program, "color", red);

        //Cube
        Point minBoundCube;
        Point maxBoundCube;

        m_cube.bounds(minBoundCube, maxBoundCube);

        Transform cubeTransformUp = Translation(0, (maxBoundCube.y - minBoundCube.y)/2.0f, 0); 
        
        Transform toTheBackground = Translation(-3,0,-3);

        mvMatrix = view * toTheBackground * cubeTransformUp;
        mvpMatrix = projection * mvMatrix;

        
        program_uniform(program, "mvpMatrix", mvpMatrix);
        program_uniform(program, "mvMatrix", mvMatrix);

        m_cube.draw(program);
        
        //BigGuy
        Point minBoundBigGuy;
        Point maxBoundBigGuy;

        m_bigguy.bounds(minBoundBigGuy, maxBoundBigGuy);

        Transform bigguyScale = Scale(0.2f) ;
        minBoundBigGuy = bigguyScale(minBoundBigGuy); 
        maxBoundBigGuy = bigguyScale(maxBoundBigGuy); 

        Transform bigguyUp = Translation(0, (maxBoundBigGuy.y - minBoundBigGuy.y)/2.0f, 0);

        mvMatrix = view * toTheBackground * cubeTransformUp * bigguyUp * bigguyScale;
        mvpMatrix = projection * mvMatrix;

        program_uniform(program, "mvpMatrix", mvpMatrix);
        program_uniform(program, "mvMatrix", mvMatrix);

        m_bigguy.draw(program);
        
        // dessine le repere, placé au centre du monde
        draw(m_repere, /* model */ Identity(), camera());
        return 1;
    }

protected:
    Mesh m_robot;
    Mesh m_robot_frame_2;
    Mesh m_bigguy;
    Mesh m_repere;
    Mesh m_cube;
    GLuint program;
};


int main( int argc, char **argv )
{
    // il ne reste plus qu'a creer un objet application et la lancer 
    TP tp;
    tp.run();
    
    return 0;
}
