#version 330

// Input
in vec2 texcoord;
in vec3 frag_color;

// Uniform properties
uniform sampler2D texture_1;

// Declare various other uniforms
uniform int use_texture;

// Output
layout(location = 0) out vec4 out_color;

void main()
{
    // Calculate the out_color using the texture2D() function.
    vec4 color = texture2D(texture_1, texcoord);
    
    if (color.a < 0.8) {
        discard;
    }


   if (use_texture == 1) {
        out_color = color;
   } else {
        out_color = vec4(frag_color, 0.5);
   }
}
