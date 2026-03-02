uniform sampler2D texture;
uniform sampler2D provincesTexture;
uniform sampler2D baronyTexture;
uniform sampler2D countyTexture;
uniform sampler2D duchyTexture;
uniform sampler2D kingdomTexture;
uniform sampler2D empireTexture;
uniform sampler2D hegemonyTexture;
uniform vec2 textureSize;

uniform float time;
uniform int mapMode;
uniform bool displayBorders;

const int PROVINCE = 0;
const int BARONY   = 1;
const int COUNTY   = 2;
const int DUCHY    = 3;
const int KINGDOM  = 4;
const int EMPIRE   = 5;
const int HEGEMONY = 6;

const int MAPMODE_PROVINCES = 0;
const int MAPMODE_HEIGHTMAP = 1;
const int MAPMODE_RIVERS = 2;
const int MAPMODE_TERRAIN = 3;
const int MAPMODE_WINTER_SEVERITY = 4;
const int MAPMODE_CULTURE = 5;
const int MAPMODE_RELIGION = 6;
const int MAPMODE_BARONY = 7;
const int MAPMODE_COUNTY = 8;
const int MAPMODE_DUCHY = 9;
const int MAPMODE_KINGDOM = 10;
const int MAPMODE_EMPIRE = 11;
const int MAPMODE_HEGEMONY = 12;

// The last (4th) element of the array is used to distinguish the type defined above.
uniform vec4 selectedEntities[1000];
uniform int selectedEntitiesCount;

bool IsSelected(int type, vec4 color) {
    bool selected = false;
    vec3 rgb = color.rgb;
    for(int i = 0; i < selectedEntitiesCount; i++) {
        if(int(selectedEntities[i].a) != type)
            continue;
        if(rgb == selectedEntities[i].rgb)
            return true;
    }
    return false;
}

bool IsBorder(sampler2D tex) {
    // Calculate the texel size based on the texture dimensions
    vec2 texelSize = 1.0 / textureSize;

    // Sample the current pixel and its neighbors
    vec4 currentColor = texture2D(tex, gl_TexCoord[0].xy);
    vec4 leftColor = texture2D(tex, gl_TexCoord[0].xy + vec2(-texelSize.x, 0.0));
    vec4 rightColor = texture2D(tex, gl_TexCoord[0].xy + vec2(texelSize.x, 0.0));
    vec4 topColor = texture2D(tex, gl_TexCoord[0].xy + vec2(0.0, texelSize.y));
    vec4 bottomColor = texture2D(tex, gl_TexCoord[0].xy + vec2(0.0, -texelSize.y));

    // Check if the current pixel differs from any of its neighbors
    return currentColor != leftColor
        || currentColor != rightColor
        || currentColor != topColor
        || currentColor != bottomColor;
    
    // return currentColor != rightColor || currentColor != bottomColor;
}

vec4 GetEntityColor(int type) {
    if(type == PROVINCE) return texture2D(provincesTexture, gl_TexCoord[0].xy);
    if(type == BARONY) return texture2D(baronyTexture, gl_TexCoord[0].xy);
    if(type == COUNTY) return texture2D(countyTexture, gl_TexCoord[0].xy);
    if(type == DUCHY) return texture2D(duchyTexture, gl_TexCoord[0].xy);
    if(type == KINGDOM) return texture2D(kingdomTexture, gl_TexCoord[0].xy);
    if(type == EMPIRE) return texture2D(empireTexture, gl_TexCoord[0].xy);
    if(type == HEGEMONY) return texture2D(hegemonyTexture, gl_TexCoord[0].xy);
    return vec4(0.0, 0.0, 0.0, 1.0);
}

int GetBorderTier() {
    // Check if it is the border of a province first to avoid unless calculation for highter tier.
    if(!IsBorder(provincesTexture)) return -1;
    if(mapMode >= MAPMODE_HEGEMONY && IsBorder(hegemonyTexture)) return 6;
    if(mapMode >= MAPMODE_EMPIRE && IsBorder(empireTexture)) return 5;
    if(mapMode >= MAPMODE_KINGDOM && IsBorder(kingdomTexture)) return 4;
    if(mapMode >= MAPMODE_DUCHY && IsBorder(duchyTexture)) return 3;
    if(mapMode >= MAPMODE_COUNTY && IsBorder(countyTexture)) return 2;
    if(mapMode >= MAPMODE_BARONY && IsBorder(baronyTexture)) return 1;
    return 0;
}

void main() {
    vec2 pixelPos = gl_TexCoord[0].xy;
    vec4 pixelColor = texture2D(texture, pixelPos);
    vec4 provincePixelColor = texture2D(provincesTexture, pixelPos);

    // Final color that will be used for the pixel.
    vec4 color = gl_Color * pixelColor;
    float alpha = color.a;

    for(int i = HEGEMONY; i >= PROVINCE; i--) {
        bool isSelected = IsSelected(i, GetEntityColor(i));
        if(isSelected) {
            float v = abs(sin(2.0*time)+3.0)/6.0;
            if (mapMode == MAPMODE_WINTER_SEVERITY) color = vec4(v, 0.0, 0.0, 1.0);
            else color = vec4(v, v, v, 1.0);
            break;
        }
    }

    if(displayBorders) {
        int borderTier = GetBorderTier();

        if(borderTier >= 0) {
            // The highter the tier, the darker the borders.
            float a = max(1.0, float(borderTier)) / (max(1.0, float(mapMode-MAPMODE_BARONY+1)));
            float t = (a*a) / 1.7;
            
            // Take the farthest color from the current color.
            vec4 borderColor = vec4(0.0, 0.0, 0.0, 1.0);
            if(color.r + color.g + color.b <= 0.3) borderColor = vec4(1.0, 1.0, 1.0, 1.0);
            if (mapMode == MAPMODE_WINTER_SEVERITY) t /= 5.0;

            color = mix(color, borderColor, t);
        }
    }

    if((mapMode == MAPMODE_CULTURE || mapMode == MAPMODE_RELIGION) && alpha == 0.0) {
        color.a = 1.0;
        if(cos(10000.0*(pixelPos.x+pixelPos.y)) >= 0.5) {
            color = mix(color, vec4(0.0, 0.0, 0.0, 1.0), 0.75);
        }
    }

    gl_FragColor = color;
}