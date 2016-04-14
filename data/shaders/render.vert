#version 450

layout (location = 0) in vec3 worldPos;
layout (location = 1) in vec3 normal;

layout (binding = 0) uniform UBO {
	mat4 projection;
	mat4 view;
	mat4 model;
} ubo;

void main(){
	gl_Position = ubo.projection * ubo.view * ubo.model * vec4(worldPos.xyz, 1.0);
}