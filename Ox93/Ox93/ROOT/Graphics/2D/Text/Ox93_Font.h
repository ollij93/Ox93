#pragma once

#ifndef OX93_FONT_H__
#define OX93_FONT_H__

// Includes...
#include "Ox93_Core.h"

// Constants...
static const Ox93_Hash uOX93_FONT_DEFAULT = 0xBF4C6E21; // "Default"

// Forward Declarations...
class Ox93_BitMap;

class Ox93_Font
{
public:
	enum Ox93_Font_Characters
	{
		OX93_FONT_CHARACTER_SPACE,
		OX93_FONT_CHARACTER_EXCLAMATION_MARK,
		OX93_FONT_CHARACTER_SPEACH_MARKS,
		OX93_FONT_CHARACTER_HASH,
		OX93_FONT_CHARACTER_DOLAR,
		OX93_FONT_CHARACTER_PERCENTAGE,
		OX93_FONT_CHARACTER_AMPERSAND,
		OX93_FONT_CHARACTER_APOSTROPHE,
		OX93_FONT_CHARACTER_OPEN_BRACKET,
		OX93_FONT_CHARACTER_CLOSE_BRACKET,
		OX93_FONT_CHARACTER_ASTERISK,
		OX93_FONT_CHARACTER_PLUS,
		OX93_FONT_CHARACTER_COMMA,
		OX93_FONT_CHARACTER_HYPHEN,
		OX93_FONT_CHARACTER_FULLSTOP,
		OX93_FONT_CHARACTER_FORWARD_SLASH,
		OX93_FONT_CHARACTER_NUM_0,
		OX93_FONT_CHARACTER_NUM_1,
		OX93_FONT_CHARACTER_NUM_2,
		OX93_FONT_CHARACTER_NUM_3,
		OX93_FONT_CHARACTER_NUM_4,
		OX93_FONT_CHARACTER_NUM_5,
		OX93_FONT_CHARACTER_NUM_6,
		OX93_FONT_CHARACTER_NUM_7,
		OX93_FONT_CHARACTER_NUM_8,
		OX93_FONT_CHARACTER_NUM_9,
		OX93_FONT_CHARACTER_COLON,
		OX93_FONT_CHARACTER_SEMICOLON,
		OX93_FONT_CHARACTER_LESSTHAN,
		OX93_FONT_CHARACTER_EQUALS,
		OX93_FONT_CHARACTER_GREATERTHAN,
		OX93_FONT_CHARACTER_QUESTION_MARK,
		OX93_FONT_CHARACTER_AT,
		OX93_FONT_CHARACTER_UPPER_A,
		OX93_FONT_CHARACTER_UPPER_B,
		OX93_FONT_CHARACTER_UPPER_C,
		OX93_FONT_CHARACTER_UPPER_D,
		OX93_FONT_CHARACTER_UPPER_E,
		OX93_FONT_CHARACTER_UPPER_F,
		OX93_FONT_CHARACTER_UPPER_G,
		OX93_FONT_CHARACTER_UPPER_H,
		OX93_FONT_CHARACTER_UPPER_I,
		OX93_FONT_CHARACTER_UPPER_J,
		OX93_FONT_CHARACTER_UPPER_K,
		OX93_FONT_CHARACTER_UPPER_L,
		OX93_FONT_CHARACTER_UPPER_M,
		OX93_FONT_CHARACTER_UPPER_N,
		OX93_FONT_CHARACTER_UPPER_O,
		OX93_FONT_CHARACTER_UPPER_P,
		OX93_FONT_CHARACTER_UPPER_Q,
		OX93_FONT_CHARACTER_UPPER_R,
		OX93_FONT_CHARACTER_UPPER_S,
		OX93_FONT_CHARACTER_UPPER_T,
		OX93_FONT_CHARACTER_UPPER_U,
		OX93_FONT_CHARACTER_UPPER_V,
		OX93_FONT_CHARACTER_UPPER_W,
		OX93_FONT_CHARACTER_UPPER_X,
		OX93_FONT_CHARACTER_UPPER_Y,
		OX93_FONT_CHARACTER_UPPER_Z,
		OX93_FONT_CHARACTER_SQUARE_OPEN,
		OX93_FONT_CHARACTER_BACK_SLASH,
		OX93_FONT_CHARACTER_SQUARE_CLOSE,
		OX93_FONT_CHARACTER_CARET,
		OX93_FONT_CHARACTER_UNDERSCORE,
		OX93_FONT_CHARACTER_GRAVE,
		OX93_FONT_CHARACTER_LOWER_A,
		OX93_FONT_CHARACTER_LOWER_B,
		OX93_FONT_CHARACTER_LOWER_C,
		OX93_FONT_CHARACTER_LOWER_D,
		OX93_FONT_CHARACTER_LOWER_E,
		OX93_FONT_CHARACTER_LOWER_F,
		OX93_FONT_CHARACTER_LOWER_G,
		OX93_FONT_CHARACTER_LOWER_H,
		OX93_FONT_CHARACTER_LOWER_I,
		OX93_FONT_CHARACTER_LOWER_J,
		OX93_FONT_CHARACTER_LOWER_K,
		OX93_FONT_CHARACTER_LOWER_L,
		OX93_FONT_CHARACTER_LOWER_M,
		OX93_FONT_CHARACTER_LOWER_N,
		OX93_FONT_CHARACTER_LOWER_O,
		OX93_FONT_CHARACTER_LOWER_P,
		OX93_FONT_CHARACTER_LOWER_Q,
		OX93_FONT_CHARACTER_LOWER_R,
		OX93_FONT_CHARACTER_LOWER_S,
		OX93_FONT_CHARACTER_LOWER_T,
		OX93_FONT_CHARACTER_LOWER_U,
		OX93_FONT_CHARACTER_LOWER_V,
		OX93_FONT_CHARACTER_LOWER_W,
		OX93_FONT_CHARACTER_LOWER_X,
		OX93_FONT_CHARACTER_LOWER_Y,
		OX93_FONT_CHARACTER_LOWER_Z,
		OX93_FONT_CHARACTER_CURLY_OPEN,
		OX93_FONT_CHARACTER_VERTICAL_BAR,
		OX93_FONT_CHARACTER_CURLY_CLOSE,
		OX93_FONT_CHARACTER_TILDE,

		OX93_NUM_FONT_CHARACTERS,
	};

public:
	static void CreateFontFromFile(const WCHAR* pwszFontname);

	static const Ox93_Font* GetFontByHash(Ox93_Hash uHash);

	Ox93_BitMap* CreateBitMapForCharacter(u_int uCharacter) const;

protected:
	Ox93_Font();
	~Ox93_Font() {}
	Ox93_Font(const Ox93_Font& xOther) {}

	bool LoadFontFromFile(const WCHAR* pwszFontname);

private:
	struct Ox93_Font_CharacterHandle
	{
		float m_fLeft;
		float m_fRight;
		u_int m_uWidth;
	};

	Ox93_Hash m_uHash;
	Ox93_Hash m_uTextureHash;
	u_int m_uHeight;
	Ox93_Font_CharacterHandle m_axCharacterHandles[OX93_NUM_FONT_CHARACTERS];

	static std::list<Ox93_Font*> s_lpxFontList;
};

#endif // OX93_FONT_H__

