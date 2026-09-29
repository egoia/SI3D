
#version 330

const float pi= 3.1415926;

#ifdef VERTEX_SHADER
layout(location= 0) in vec3 position;
uniform mat4 mvpMatrix;

#ifdef USE_TEXCOORD
    layout(location= 1) in vec2 texcoord;
    out vec2 vertex_texcoord;
#endif

#ifdef USE_NORMAL
    layout(location= 2) in vec3 normal;
    uniform mat4 normalMatrix;
    out vec3 vertex_normal;
#endif
#if defined USE_SHADING || !defined USE_NORMAL
    uniform mat4 mvMatrix;
    out vec3 vertex_position;
#endif

#ifdef USE_COLOR
    layout(location= 3) in vec4 color;
    out vec4 vertex_color;
#endif

void main( )
{
    gl_Position= mvpMatrix * vec4(position, 1);
    
#ifdef USE_TEXCOORD
    vertex_texcoord= texcoord;
#endif

#ifdef USE_NORMAL
    vertex_normal= mat3(normalMatrix) * normal;
#endif
#if defined USE_SHADING || !defined USE_NORMAL
    vertex_position= vec3(mvMatrix * vec4(position, 1));
#endif

#ifdef USE_COLOR
    vertex_color= color;
#endif
}
#endif


#ifdef FRAGMENT_SHADER

#if defined USE_TEXCOORD
    in vec2 vertex_texcoord;
    uniform sampler2D diffuse_texture;
#endif

#ifdef USE_ALPHATEST
    uniform float alpha_min= 0.3;
#endif

#ifdef USE_SHADING
    #ifdef USE_LIGHT
        uniform vec3 light;
        uniform vec4 light_color;
    #endif

    #ifdef USE_SUN
        uniform vec3 sun= vec3(0.5, 0.25, 0);
        uniform vec4 sun_color= vec4(1);
    #endif

    #ifdef USE_SKY
        uniform vec3 sky= vec3(0, 1, 0);
        uniform vec4 sky_color= vec4(1);
    #endif

    #ifdef USE_MATERIAL
        uniform vec4 diffuse_color;
        uniform vec4 specular_color;
        uniform float ns;
        #ifdef USE_TEXCOORD
            uniform sampler2D specular_texture;
            uniform sampler2D ns_texture;
        #endif
    #endif
#endif

#ifdef USE_NORMAL
    in vec3 vertex_normal;
#endif
#if defined USE_SHADING || !defined USE_NORMAL
    in vec3 vertex_position;
#endif

#ifdef USE_COLOR
    in vec4 vertex_color;
#endif

uniform vec4 mesh_color= vec4(0.5, 0.5, 0.5, 1);

out vec4 fragment_color;

void main( )
{
    vec4 color= mesh_color;
#ifdef USE_COLOR
    color= vertex_color;
#endif
#ifdef USE_MATERIAL
    color= diffuse_color;
#endif

#ifdef USE_TEXCOORD
    color= color * texture(diffuse_texture, vertex_texcoord);
    #ifdef USE_ALPHATEST
        if(color.a < alpha_min)
            discard;
    #endif
#endif

#ifdef USE_NORMAL
    vec3 normal= normalize(vertex_normal);
#else
    vec3 t= dFdx(vertex_position);
    vec3 b= dFdy(vertex_position);
    vec3 normal= normalize( cross(t, b) );
#endif
    
#ifdef USE_SHADING
    vec4 pixel= vec4(0);

    #ifdef USE_MATERIAL
        vec3 o= normalize(- vertex_position);       // Vector( camera, p )
        
        float glossy= ns;
        vec4 highlight= specular_color;
        #ifdef USE_TEXCOORD
            highlight= highlight * texture(specular_texture, vertex_texcoord);
            glossy= glossy * texture(ns_texture, vertex_texcoord).r;
        #endif
    #endif
        
    #ifdef USE_LIGHT
        // source ponctuelle
        float cos_light= max(0, dot(normal, normalize(light - vertex_position)));
        pixel= pixel + color * light_color * cos_light / distance(light, vertex_position);
        #ifdef USE_MATERIAL
        {
            vec3 hlight= normalize( o + normalize(light - vertex_position) );
            float cos_hlight= max( 0, dot( normal, hlight ) );
            float blinn= (glossy+8) / (8*pi) * pow(cos_hlight, glossy);
            pixel= pixel + blinn * highlight * light_color  * cos_light / distance(light, vertex_position);
        }
        #endif
    #endif
    #ifdef USE_SUN
        // source directionnelle 
        float cos_sun= max(0, dot(normal, normalize(sun)));
        pixel= pixel + color * sun_color * cos_sun;
        #ifdef USE_MATERIAL
        {
            vec3 hsun= normalize( o + normalize(sun) );
            float cos_hsun= max( 0, dot( normal, hsun ) );
            float blinn= (glossy+8) / (8*pi) * pow(cos_hsun, glossy);
            pixel= pixel + blinn * highlight * sun_color * cos_sun;
        }
        #endif
    #endif
    #ifdef USE_SKY
        // source dome / hemisphere
        float cos_sky= (1 + max(0, dot(normal, normalize(sky)))) / 2;
        pixel= pixel + color * sky_color * cos_sky;
    #endif
    
#else
    float cos_theta= abs(normal.z);
    vec4 pixel= color * cos_theta;
#endif

    // hachure les triangles mal orientes
    if(gl_FrontFacing == false)
    {
        ivec2 fragment= ivec2(gl_FragCoord.xy / 4) % ivec2(2, 2);
        if((fragment.x ^ fragment.y) == 0)
            pixel= vec4(0.8, 0.4, 0, 1);
    }
    
    fragment_color= vec4(pixel.rgb, 1);
}
#endif
