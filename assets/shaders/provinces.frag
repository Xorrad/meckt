// const int MAX_SELECTED_PROVINCES 1000;

uniform sampler2D texture;
uniform sampler2D provincesTexture;
uniform vec2 provincesTextureSize;
uniform float time;
uniform int mapMode;
uniform bool displayBorders;

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

bool IsBorder(sampler2D tex) {
    // Calculate the texel size based on the texture dimensions
    vec2 texelSize = 1.0 / provincesTextureSize;

    // Sample the current pixel and its neighbors
    vec4 currentColor = texture2D(tex, gl_TexCoord[0].xy);
    // vec4 leftColor = texture2D(tex, gl_TexCoord[0].xy + vec2(-texelSize.x, 0.0));
    vec4 rightColor = texture2D(tex, gl_TexCoord[0].xy + vec2(texelSize.x, 0.0));
    // vec4 topColor = texture2D(tex, gl_TexCoord[0].xy + vec2(0.0, texelSize.y));
    vec4 bottomColor = texture2D(tex, gl_TexCoord[0].xy + vec2(0.0, -texelSize.y));

    // Check if the current pixel differs from any of its neighbors
    // return currentColor != leftColor
    //     || currentColor != rightColor
    //     || currentColor != topColor
    //     || currentColor != bottomColor;
    
    return currentColor != rightColor || currentColor != bottomColor;
}

void main() {
    vec4 pixelColor = texture2D(texture, gl_TexCoord[0].xy);
    vec4 provincePixelColor = texture2D(provincesTexture, gl_TexCoord[0].xy);
    vec4 color = gl_Color * pixelColor;
    
    bool isSelected = IsSelected(pixelColor);

    if(displayBorders) {
        bool isProvinceBorder = IsBorder(provincesTexture);
        bool isTitleBorder = IsBorder(texture);
        
        if(isTitleBorder) {
            gl_FragColor = mix(color, vec4(0.0, 0.0, 0.0, 1.0), 0.5);
            return;
        }
        
        if(isProvinceBorder) {
            gl_FragColor = mix(color, vec4(0.0, 0.0, 0.0, 1.0), 0.1);
            return;
        }
    }

    if(!isSelected) {
        gl_FragColor = color;
        return;
    }

    float v = abs(sin(2.0*time)+3.0)/6.0;
    gl_FragColor = vec4(v, v, v, 1.0);
}