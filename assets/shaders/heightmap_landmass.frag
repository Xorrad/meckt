uniform sampler2D texture;
uniform vec2 textureSize;
uniform float waterLevel;

void main() {
    vec2 pixelPos = gl_TexCoord[0].xy;
    vec4 pixelColor = texture2D(texture, pixelPos);
    float elevation = ((pixelColor.r + pixelColor.g + pixelColor.b)/3.0)*100.0;

    gl_FragColor = (elevation <= waterLevel) ? vec4(0.0, 0.0, 1.0, 1.0) : vec4(1.0, 1.0, 1.0, 1.0);
}