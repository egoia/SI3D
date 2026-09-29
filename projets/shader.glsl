#version 330
 
#ifdef VERTEX_SHADER
// doit calculer la position d'un sommet dans le repere projectif
uniform mat4 mvpMatrix;
layout(location=0) in vec3 position;
layout(location=2) in vec3 normal;

out vec3 vertex_normal;
void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);                   // a completer
    vertex_normal = normal;
}
#endif
 
#ifdef FRAGMENT_SHADER
// doit calculer la couleur du fragment
uniform mat4 mvMatrix;

uniform vec3 color;

in vec3 vertex_normal;
const vec3 l =vec3(1,1,1);
void main( )
{
    vec3 d = vec3(mvMatrix * vec4(vertex_normal,0));
    vec3 lbis = vec3(mvMatrix * vec4(l,0));
    float light = max(dot(normalize(d), normalize(lbis)),0);
    float light_value = step(0.9, light)*0.9;
    light_value = max(light_value,step(0.7, light)*0.7);
    light_value = max(light_value,step(0.3, light)*0.3);
    light_value = max(light_value,step(0.5, light)*0.5);
    light_value = max(light_value,step(0.1, light)*0.1);
    
    gl_FragColor = vec4(color * light_value, 1);
}
#endif