// const int MAX_SELECTED_PROVINCES 1000;

uniform sampler2D texture;
uniform sampler2D provincesTexture;
uniform float time;
uniform int mapMode;

uniform vec4 selectedProvinces[1000];
uniform int selectedProvincesCount;

bool IsSelected(vec4 color) {
    bool selected = false;
    for(int i = 0; i < selectedProvincesCount; i++) {
        if (color == selectedProvinces[i]) {
            return true;
        }
    }
    return false;
}

void main() {
    vec4 pixelColor = texture2D(texture, gl_TexCoord[0].xy);
    vec4 provincePixelColor = texture2D(provincesTexture, gl_TexCoord[0].xy);
    bool selected = IsSelected(pixelColor);

    if(!selected) {
        gl_FragColor = gl_Color * pixelColor;
        return;
    }
    // uint colorId = (pixelColor.r << 16) + (pixelColor.g << 8) + (pixelColor.b);

    float v = abs(sin(2.0*time)+3.0)/6.0;
    gl_FragColor = vec4(v, v, v, 1.0);
}