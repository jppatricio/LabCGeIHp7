#version 330 core

in vec2 out_tex;
out vec4 color;
uniform sampler2D ourTexture;

void main(){
    //Cambiamos el color por la textura
    color = texture(ourTexture, out_tex);
    //color = vec4(0.3, 0.5, 0.4, 1.0);
    if(color.a <= 0.1){
        discard;
    }

}