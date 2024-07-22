uniform sampler2D texture;
uniform float time;
uniform vec4 selectedProvinces[100];
uniform int selectedProvincesCount;

bool isSelected(vec4 color) {
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
    bool selected = isSelected(pixelColor);
    
    if(!selected) {
        gl_FragColor = gl_Color * pixelColor;
        return;
    }

    float v = abs(sin(2.0*time)+3.0)/6.0;
    gl_FragColor = vec4(v, v, v, 1.0);
}