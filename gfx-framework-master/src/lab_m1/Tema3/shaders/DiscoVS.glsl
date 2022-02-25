#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 object_color;
uniform float time;
uniform float isGlobe;
uniform float invertNormals;


// Output
out vec2 texcoord;
out vec3 frag_color;
out float frag_time;
out vec3 world_position;
out vec3 world_normal;

void main()
{
    // Pass v_texture_coord as output to fragment shader
    frag_time = time;
    frag_color = object_color;
    texcoord = v_texture_coord;
    
    vec3 result = v_normal;
    if (invertNormals == 1) {
        result = (dot(result,result.xyz) < 0.0) ? result : -result;
    }
    
    world_position = vec3(Model * vec4(v_position, 1.0));
    world_normal = vec3(Model * vec4(result, 0.0));

    if (isGlobe == 1) {
		texcoord = vec2(texcoord.x - time / 20.f, texcoord.y);
	} 

    gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
