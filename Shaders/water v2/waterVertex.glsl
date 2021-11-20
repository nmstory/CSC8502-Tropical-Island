#version 410

in vec4 position;
uniform sampler2D terrain;

out Vertex {
	vec2 texCoord;
} OUT;

void main(void){
    vec2 tc = position.xy;
    float height = texture(terrain, tc).a;
    vec4 displaced = vec4(
        position.x, position.y,
        height, 1.0);
    gl_Position = displaced; // change this so displaced is never needed?
	OUT.texCoord = tc;
}
