#pragma once

template <>
struct std::hash<sf::Color> {
  std::size_t operator()(const sf::Color& c) const {
        std::size_t res = 17;
        res = res * 31 + hash<char>()(c.r);
        res = res * 31 + hash<char>()(c.g);
        res = res * 31 + hash<char>()(c.b);
        res = res * 31 + hash<char>()(c.a);
        return res;
    }
};

inline sf::Color brightenColor(const sf::Color& color) {
	if((color.r + color.g + color.b) >= (255*3/2))
		return sf::Color::Black;
	return sf::Color::White;
}

namespace sf {
	struct HSVColor {
		float h; // Hue component, range: [0.f, 360.f)
		float s; // Saturation component, range: [0.f, 1.f)
		float v; // Value component, range: [0.f, 1.f)

		HSVColor(float hue, float saturation, float value) : h(hue), s(saturation), v(value) {}
		HSVColor(const Color& rgb) {
			HSVColor hsv = fromRgb(rgb);
			(*this) = hsv;
		}

		operator Color() const {
			return toRgb();
		}
		
		bool operator ==(const HSVColor& other) const {
			return h == other.h && s == other.s && v == other.v;
		}
		
		bool operator !=(const HSVColor& other) const {
			return !(*(this) == other);
		}

		HSVColor& operator =(const HSVColor& other) {
			h = other.h;
			s = other.s;
			v = other.v;
			return (*this);
		}

		// credits to https://gist.github.com/marukrap/7c361f2c367eaf40537a8715e3fd952as
		// for the conversion between rgb and hsv.

		static HSVColor fromRgb(const Color& rgb) {
			float R = rgb.r / 255.f;
			float G = rgb.g / 255.f;
			float B = rgb.b / 255.f;

			float M = std::max({ R, G, B });
			float m = std::min({ R, G, B });
			float C = M - m; // Chroma

			float H = 0.f; // Hue
			float S = 0.f; // Saturation
			float V = 0.f; // Value

			if(C != 0.f) {
				if(M == R) H = std::fmod(((G - B) / C), 6.f);
				else if(M == G) H = ((B - R) / C) + 2;
				else if(M == B) H = ((R - G) / C) + 4;
				H *= 60;
			}
			if(H < 0.f) H += 360;
			V = M;
			if(V != 0.f) S = C / V;
			return HSVColor(H, S, V);
		}
		
		Color toRgb() const {
			float C = s * v; // Chroma
			float HPrime = std::fmod(h / 60, 6.f); // H'
			float X = C * (1 - std::fabs(std::fmod(HPrime, 2.f) - 1));
			float M = v - C;

			float R = 0.f;
			float G = 0.f;
			float B = 0.f;

			switch(static_cast<int>(HPrime)) {
				case 0: R = C; G = X;        break; // [0, 1)
				case 1: R = X; G = C;        break; // [1, 2)
				case 2:        G = C; B = X; break; // [2, 3)
				case 3:        G = X; B = C; break; // [3, 4)
				case 4: R = X;        B = C; break; // [4, 5)
				case 5: R = C;        B = X; break; // [5, 6)
			}
			R += M;
			G += M;
			B += M;
			return Color(
				static_cast<sf::Uint8>(std::round(R * 255)),
				static_cast<sf::Uint8>(std::round(G * 255)),
				static_cast<sf::Uint8>(std::round(B * 255))
			);	
		}
	};
}