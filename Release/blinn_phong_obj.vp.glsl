#version 430

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;


uniform MatVP{
	mat4 View;
	mat4 Projection;
};
uniform mat4 Model;
uniform vec3 vLightPosition;
uniform vec3 vViewPos;
uniform mat4 lightSpaceMatrix;

// to fragment shader
out vec3 inverseTransposeNormal;
out vec2 UV;
out vec3 viewDir;
out vec3 halfwayDir;
out vec4 lightSpacePosition;
out vec3 fragPos;
out vec3 lightDir;

void main(void) 
{ 
    //place the coordinate according to the transformation matrix
    gl_Position = Projection * View * Model * vec4(vPosition,1.0);

    //take care the uv and normal from obj file
    inverseTransposeNormal = transpose(inverse(mat3(Model))) * vertexNormal;
    inverseTransposeNormal = normalize(inverseTransposeNormal);
	UV = vertexUV;
    
    //for shadow
    fragPos = vec3(Model*vec4(vPosition,1.0)); //purely on world coordinate, not based on camera coordinate or screen coordinate
    lightSpacePosition = lightSpaceMatrix * (vec4(fragPos,1.0)); //now, find fragment position based on the light, to map for shadow mapping

    //light direction vector with respect to each fragment position
    lightDir = normalize(vLightPosition - fragPos);
    //count viewDir, viewDirection compared to each vertices
    viewDir = normalize(vViewPos - fragPos);
    //count halfwayDir, for Blinn-Phong
    halfwayDir = normalize(lightDir + viewDir);

}
