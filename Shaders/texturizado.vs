#version 330 core
layout (location = 0) in vec3 in_position;
// CAMBIO DESCOMENTAR LAYOUT
layout (location = 2) in vec2 in_tex;
// CAMBIO out__tex
out vec2 out_tex;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(){

	gl_Position = projection * view * model * vec4(in_position, 1.0);
	// CAMBIO SE ASIGNA
	out_tex = in_tex;

}

