uniform sampler2D texture;
uniform sampler2D provincesIndexTexture;

uniform sampler2D provinceIdentityPalette;
uniform sampler2D contentPalette;
uniform sampler2D baronyPalette;
uniform sampler2D countyPalette;
uniform sampler2D duchyPalette;
uniform sampler2D kingdomPalette;
uniform sampler2D empirePalette;
uniform sampler2D hegemonyPalette;

// palette[i] = the highlight color for province i if it (or a title/region/adjacency covering it)
// is currently selected, with alpha 0 otherwise.
uniform sampler2D selectionPalette;

// paletteSize is shared by every palette above: they're all indexed by the same province index.
uniform vec2 paletteSize;

// Size (in texels) of the currently bound provincesIndexTexture chunk.
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
const int MAPMODE_FLAGS = 3;
const int MAPMODE_TERRAIN = 4;
const int MAPMODE_CLIMATE = 5;
const int MAPMODE_WINTER_SEVERITY = 6;
const int MAPMODE_CULTURE = 7;
const int MAPMODE_RELIGION = 8;
const int MAPMODE_BARONY = 9;
const int MAPMODE_COUNTY = 10;
const int MAPMODE_DUCHY = 11;
const int MAPMODE_KINGDOM = 12;
const int MAPMODE_EMPIRE = 13;
const int MAPMODE_HEGEMONY = 14;

// Decodes a province render index (0..65535) packed into the R+G channels
// of a texel sampled from provincesIndexTexture.
int DecodeIndex(vec4 texel) {
    return int(texel.r * 255.0 + 0.5) + int(texel.g * 255.0 + 0.5) * 256;
}

// Maps a province render index to the UV of its entry in any palette texture -
// every palette shares the same layout, given by paletteSize.
vec2 PaletteUV(int index) {
    float x = mod(float(index), paletteSize.x);
    float y = floor(float(index) / paletteSize.x);
    return (vec2(x, y) + 0.5) / paletteSize;
}

// Resolves the color of an entity (a province, or the title governing it at a
// given tier) at an arbitrary UV: decode the province index from the shared,
// static index texture, then look up that entity type's palette.
vec4 GetEntityColorAt(int type, vec2 uv) {
    int index = DecodeIndex(texture2D(provincesIndexTexture, uv));
    vec2 paletteUV = PaletteUV(index);

    if(type == PROVINCE) return texture2D(provinceIdentityPalette, paletteUV);
    if(type == BARONY) return texture2D(baronyPalette, paletteUV);
    if(type == COUNTY) return texture2D(countyPalette, paletteUV);
    if(type == DUCHY) return texture2D(duchyPalette, paletteUV);
    if(type == KINGDOM) return texture2D(kingdomPalette, paletteUV);
    if(type == EMPIRE) return texture2D(empirePalette, paletteUV);
    if(type == HEGEMONY) return texture2D(hegemonyPalette, paletteUV);
    return vec4(0.0, 0.0, 0.0, 1.0);
}

vec4 GetEntityColor(int type) {
    return GetEntityColorAt(type, gl_TexCoord[0].xy);
}

bool IsBorderForTier(int type) {
    // Calculate the texel size based on the province index texture dimensions.
    vec2 texelSize = 1.0 / textureSize;
    vec2 uv = gl_TexCoord[0].xy;

    // Sample the current pixel's resolved entity color and its neighbors'.
    vec4 currentColor = GetEntityColorAt(type, uv);
    vec4 leftColor = GetEntityColorAt(type, uv + vec2(-texelSize.x, 0.0));
    vec4 rightColor = GetEntityColorAt(type, uv + vec2(texelSize.x, 0.0));
    vec4 topColor = GetEntityColorAt(type, uv + vec2(0.0, texelSize.y));
    vec4 bottomColor = GetEntityColorAt(type, uv + vec2(0.0, -texelSize.y));

    // Check if the current pixel differs from any of its neighbors.
    return currentColor != leftColor
        || currentColor != rightColor
        || currentColor != topColor
        || currentColor != bottomColor;
}

int GetBorderTier() {
    // Check if it is the border of a province first to avoid unless calculation for highter tier.
    if(!IsBorderForTier(PROVINCE)) return -1;
    if(mapMode >= MAPMODE_HEGEMONY && IsBorderForTier(HEGEMONY)) return 6;
    if(mapMode >= MAPMODE_EMPIRE && IsBorderForTier(EMPIRE)) return 5;
    if(mapMode >= MAPMODE_KINGDOM && IsBorderForTier(KINGDOM)) return 4;
    if(mapMode >= MAPMODE_DUCHY && IsBorderForTier(DUCHY)) return 3;
    if(mapMode >= MAPMODE_COUNTY && IsBorderForTier(COUNTY)) return 2;
    if(mapMode >= MAPMODE_BARONY && IsBorderForTier(BARONY)) return 1;
    return 0;
}

void main() {
    vec2 pixelPos = gl_TexCoord[0].xy;

    // Base color:
    // - heightmap and rivers are plain images with no province concept.
    // - title-tier modes reuse their tier's resolved color directly..
    // - every other province-driven mode looks up the content palette.
    vec4 pixelColor;
    if(mapMode == MAPMODE_HEIGHTMAP || mapMode == MAPMODE_RIVERS) {
        pixelColor = texture2D(texture, pixelPos);
    }
    else if(mapMode >= MAPMODE_BARONY) {
        pixelColor = GetEntityColor(mapMode - MAPMODE_BARONY + BARONY);
    }
    else {
        int index = DecodeIndex(texture2D(provincesIndexTexture, pixelPos));
        pixelColor = texture2D(contentPalette, PaletteUV(index));
    }

    // Final color that will be used for the pixel.
    vec4 color = gl_Color * pixelColor;
    float alpha = color.a;

    // Alpha serves as a signal for the selection palette:
    // - 1.0 means an actively highlighted entity that pulses.
    // - ]0.0, 1.0[ means a solid/static color (e.g. land/sea while an adjacency is selected).
    // - 0.0 means that the entity isn't selected and should just be ignored.
    if(mapMode != MAPMODE_HEIGHTMAP && mapMode != MAPMODE_RIVERS) {
        int selectionIndex = DecodeIndex(texture2D(provincesIndexTexture, pixelPos));
        vec4 selection = texture2D(selectionPalette, PaletteUV(selectionIndex));
        if(selection.a > 0.75) {
            float pulse = (sin(3.0*time) + 1.0) * 0.5; // 0..1
            vec3 brightened = mix(selection.rgb, vec3(1.0), 0.35);
            color = vec4(mix(selection.rgb, brightened, pulse), 1.0);
        }
        else if(selection.a > 0.0) {
            color = vec4(selection.rgb, 1.0);
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

    if((mapMode == MAPMODE_CULTURE || mapMode == MAPMODE_RELIGION || mapMode == MAPMODE_FLAGS) && alpha == 0.0) {
        color.a = 1.0;
        if(cos(10000.0*(pixelPos.x+pixelPos.y)) >= 0.5) {
            color = mix(color, vec4(0.0, 0.0, 0.0, 1.0), 0.75);
        }
    }

    gl_FragColor = color;
}
