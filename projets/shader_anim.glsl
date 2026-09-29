#version 330
 
#ifdef VERTEX_SHADER
// doit calculer la position d'un sommet dans le repere projectif
uniform mat4 mvpMatrix;
layout(location=0) in vec3 position_frame_1;
layout(location=1) in vec3 position_frame_2;
uniform float interpolation_value;

void main( )
{
    gl_Position= mvpMatrix * vec4(mix(position_frame_1,position_frame_2, interpolation_value),1);                   // a completer
}
#endif
 
#ifdef FRAGMENT_SHADER
// doit calculer la couleur du fragment
uniform mat4 mvMatrix;

uniform vec4 color;

const vec3 l =vec3(1,1,1);
void main( )
{
    //vec3 d = vec3(mvMatrix * vec4(vertex_normal,0));
    //vec3 lbis = vec3(mvMatrix * vec4(l,0));
    float light = 1.0; //max(dot(normalize(d), normalize(lbis)),0);
    
    gl_FragColor = color * light;
}
#endif