#version 330

// Input
in vec2 texcoord;
in float frag_time;
in vec3 frag_color;
in vec3 world_position;
in vec3 world_normal;

// Uniform properties
uniform sampler2D texture_1;

// Uniforms for light properties
uniform vec3 eye_position;
uniform vec3 globe_position;
uniform float phase;

uniform vec3 conePos[4];
uniform vec3 coneColor[4];
uniform vec3 coneDir[4];

uniform vec3 tilePos[64];
uniform vec3 tileColor[64];
uniform vec3 tileDir[64];

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

// Declare various other uniforms
uniform int use_texture;

// Output
layout(location = 0) out vec4 out_color;


vec3 PointLightContribution(vec3 lightPos, vec3 lightColor, vec3 lightDir, int typeOfLight, float ambientFactor,
								float diffuseFactor, float specularFactor, float attenFactor, int div, int div2)
{
	vec3 N = normalize(world_normal);
	vec3 L = normalize(lightPos - world_position);
	vec3 V = normalize(eye_position - world_position);
	vec3 H = normalize(L + V);
	vec3 R = reflect(-L, N);

	vec3 light_color = lightColor;
	
	vec3 ambient_light = ambientFactor * light_color / div;

	// Compute diffuse light component
	float diff = material_kd * max(dot(normalize(N), L), 0) * diffuseFactor;
	vec3 diffuse_light = diff * light_color / div2;

	float specular = 0;
	// Compute specular light component

	if (diff > 0) {
		specular = material_ks * pow(max(dot(normalize(N), H), 0), material_shininess);
	}
	
	vec3 specular_light = specularFactor * specular * light_color / div; 
	
	float d = distance(world_position, lightPos);
	float atten = 1/ (attenFactor * pow(d, 2));

	vec3 light;
	if (typeOfLight == 1)
	{
		float cut_off_rad		= radians(9.f);
		float spot_light		= dot(-L, lightDir);
		float spot_light_limit	= cos(cut_off_rad);
		
		if (spot_light > spot_light_limit)
		{	 
			// Quadratic attenuation
			float linear_att		= (spot_light - spot_light_limit) / (1 - spot_light_limit);
			float light_att_factor	= 3.5 * pow(linear_att, 2);
			light					= ambient_light + light_att_factor * (diffuse_light + specular_light);
		} else
		{
			light = ambient_light;  // There is no spot light, but there is light from other objects
		}
	} else
	{
		light = ambient_light + atten * (diffuse_light + specular_light);
	}
		
	return light;
}

vec3 lightGlobe(){
	vec3 light_dir = world_position - globe_position;
	
	vec2 tex_coord = texcoord;
	tex_coord.x = (1.0 / (2 * 3.14159)) * atan (light_dir.x, light_dir.z);
	tex_coord.y = (1.0 / 3.14159) * acos (light_dir.y / length (light_dir));
	tex_coord = vec2(tex_coord.x - frag_time / 20.f, tex_coord.y);

	vec3 light_color = texture (texture_1, tex_coord).xyz;
	return PointLightContribution(globe_position, light_color, light_dir, 0, 0.55, 1, 0.25, 0.2, 1, 1);
}

vec3 lightDancering(){
	vec3 light = vec3(0);
	for (int i = 0; i < 64; i++){
		light += PointLightContribution(tilePos[i], tileColor[i], tileDir[i], 0, 0, 1.5, 0.2, 0.15, 64, 48);
	}
	return light;
}

vec3 lightCones(){
	vec3 light = vec3(0);
	for (int i = 0; i < 4; i++){
		light += PointLightContribution(conePos[i], coneColor[i], coneDir[i], 1, 0.03, 1, 0.2, 0.1, 1,1);
	}
	return light;
}


void main()
{
	vec3 finalLight = vec3(0);

	if (phase == 0) {
		// All lights
		finalLight += lightGlobe();
		finalLight += lightDancering();
		finalLight += lightCones();
	} else if (phase == 1) {
		// Globe lighting
		finalLight += lightGlobe();
	} else if (phase == 2) {
		// Dance ring lighting
		finalLight += lightDancering();
	} else{
		// Cones lighting
		finalLight += lightCones();
	}

	if (use_texture == 1) {
		if (phase == 0 || phase == 3) {
			out_color = vec4(texture(texture_1, texcoord).xyz * finalLight, 0.35);
		} else {
			out_color = vec4(texture(texture_1, texcoord).xyz * finalLight, 1);
		}
   } else {
		if (phase == 0 || phase == 3) {
			out_color = vec4(finalLight * frag_color, 0.35);
		} else {
			out_color = vec4(finalLight * frag_color, 1);
		}
   }	
}
