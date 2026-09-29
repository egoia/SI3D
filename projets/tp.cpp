
//! \file tp.cpp code de depart minimaliste


#include "wavefront.h"
#include "texture.h"

#include "draw.h"        
#include "app_camera.h"        // classe Application a deriver#include "program.h"
#include "program.h"
#include "uniforms.h"
#include <algorithm>


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
        m_robot_frame_1= read_mesh("data/robot_quaternius/run/Robot_000001.obj");
        m_robot_frame_2 = read_mesh("data/robot_quaternius/run/Robot_000005.obj");

        groups = m_robot_frame_2.groups();
        m_robot_frame_1.groups();

        vao = 0;
        ebo = 0;
        vbo_frame_1 = 0;
        vbo_frame_2 = 0;

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo_frame_1);
        glGenBuffers(1, &vbo_frame_2);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo_frame_1);
        glBufferData(GL_ARRAY_BUFFER, m_robot_frame_1.positions().size() * sizeof(vec3), m_robot_frame_1.positions().data(), GL_STATIC_DRAW);
        
        glVertexAttribPointer(
            0,3,GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0
        );
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, vbo_frame_2);
        glBufferData(GL_ARRAY_BUFFER, m_robot_frame_2.positions().size() * sizeof(vec3), m_robot_frame_2.positions().data(), GL_STATIC_DRAW);

        glVertexAttribPointer(
            1,3,GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0
        );
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        // etat openGL par defaut
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre
        
        glClearDepth(1.f);                          // profondeur par defaut
        glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
        glEnable(GL_DEPTH_TEST);                    // activer le ztest

        program = 0;
 
        program = read_program("projets/shader_anim.glsl");
        program_print_errors(program);

        last_time = global_time();

        return 0;   // pas d'erreur, sinon renvoyer -1
    }
    
    // destruction des objets de l'application
    int quit( )
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &ebo);
        glDeleteBuffers(1, &vbo_frame_1);
        glDeleteBuffers(1, &vbo_frame_2);
        m_robot_frame_1.release();
        m_robot_frame_2.release();
        m_repere.release();
        glDeleteProgram(program);
        return 0;   // pas d'erreur
    }
    
    // dessiner une nouvelle image
    int render( )
    {
        // transformations standards
        Transform model= Identity();
        Transform view= camera().view();
        Transform projection= camera().projection(float(window_width()), float(window_height()), 45);
        

        Transform mvMatrix = view * model;
        Transform mvpMatrix = projection * mvMatrix;

        // Transform viewport= Viewport(window_width(), window_height());
        glViewport(0, 0, window_width(), window_height());
        
        // couleur et profondeur par defaut
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        

        glUseProgram(program);
        glBindVertexArray(vao);

        /*//TODO set les uniforms
        GLint location_mvpMatrix = glGetUniformLocation(program, "mvpMatrix");
        glUniformMatrix4fv(location_mvpMatrix, 1, GL_FALSE, mvpMatrix.data());
*/
        program_uniform(program, "mvpMatrix", mvpMatrix);
        program_uniform(program, "mvMatrix", mvMatrix);

        compteur += global_time() - last_time;
        float interpolation_value = clamp(compteur / frame_duration, 0.0f, 1.0f); // Valeur d'interpolation entre les deux frames (0.0 à 1.0)
        program_uniform(program, "interpolation_value", interpolation_value);
        if(compteur >= frame_duration){
            compteur = 0.0f;
        }
        last_time = global_time();
        std::cout << "Nombre de groupes : " << groups.size() << std::endl;

        const Materials& materials = m_robot_frame_2.materials();
        for (auto const& group : groups) {
            const Material& material = materials(group.index);
            program_uniform(program, "color", material.diffuse);
            std::cout << material.diffuse.r << material.diffuse.g << material.diffuse.b << std::endl;
            
            glDrawArrays(
                GL_TRIANGLES,group.first,
                group.n
            );

        }
                
        glBindVertexArray(0);
        glUseProgram(0);

        // dessine le repere, placé au centre du monde
        draw(m_repere, /* model */ Identity(), camera());
        return 1;
    }

protected:
    GLuint vao;
    GLuint ebo;
    GLuint vbo_frame_1;
    GLuint vbo_frame_2;
    Mesh m_robot_frame_1;
    Mesh m_robot_frame_2;
    Mesh m_repere;
    GLuint program;
    float frame_duration = 1000.0f;//milliseconds
    float last_time = 0.0f;
    float compteur = 0.0f;
    std::vector<TriangleGroup> groups;

};


int main( int argc, char **argv )
{
    // il ne reste plus qu'a creer un objet application et la lancer 
    TP tp;
    tp.run();
    
    return 0;
}
