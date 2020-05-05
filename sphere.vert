attribute vec3 a_Vertex;
attribute vec2 a_Texture;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
varying vec2 qt_TexCoord0;

void main(void)
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(a_Vertex,1.0);
    qt_TexCoord0 = a_Texture;
}
