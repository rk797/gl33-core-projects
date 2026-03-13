#version 330 core
layout (location = 0) in vec3 positionIn;
layout (location = 1) in vec2 uvIn;

out vec2 uvOut;

uniform mat3 transformMatrix;

void main()
{
    uvOut = uvIn;
    
    // convert to homogeneous coordinates for the 2D transformation
    vec3 homPos = vec3(positionIn.x, positionIn.y, 1.0);
    
    // apply the affine transformation matrix
    vec3 transformedPos = transformMatrix * homPos;
    
    // pass the transformed position to gl_Position
    gl_Position = vec4(transformedPos.x, transformedPos.y, 0.0, 1.0);
}