#pragma once

#include <string>
#include <chrono>
#include <Windows.h>

//Potential palettes:
/*
.'`^",:;Il!i><~+_-?][}{1)(|\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$
.`',"-~^:;!>+?[}(/*#&$@
.`',\"-~^:;!>+?[}(/XZ*#MW&B$@
*/

class Canvas
{
public:
	static constexpr wchar_t ANSI_COLOR[] = L"\x1B[38;2;255;255;255m\0";
	static constexpr int ANSI_CHARACTER_OFFSET = sizeof(ANSI_COLOR) / sizeof(wchar_t);//wcslen(ANSI_COLOR);
	static constexpr int ANSI_COLOR_RED_OFFSET = 7;
	static constexpr int ANSI_COLOR_GREEN_OFFSET = 11;
	static constexpr int ANSI_COLOR_BLUE_OFFSET = 15;
	static constexpr wchar_t ANSI_CLEAR[] = L"\x1B[30M\x1b[0;0H\0";//should change based on height constant
	static constexpr int ANSI_CLEAR_SIZE = sizeof(ANSI_CLEAR) / sizeof(wchar_t);//wcslen(ANSI_CLEAR);
	static constexpr int ANSI_WORD_SIZE = ANSI_CHARACTER_OFFSET + 1;
	static constexpr int ASCII_CHARACTER_OFFSET = 48;
	static constexpr double CHARACTER_ASPECT_RATIO = 0.5;
	static constexpr wchar_t EMPTY_CHARACTER = L' ';
	static constexpr int CONSOLE_WIDTH = 120;
	static constexpr int CONSOLE_HEIGHT = 30;
	static constexpr int CONSOLE_SIZE = CONSOLE_WIDTH * CONSOLE_HEIGHT;
	static constexpr int CONSOLE_BUFFER_SIZE = CONSOLE_SIZE * ANSI_WORD_SIZE + 1;
	static constexpr double CONSOLE_ASPECT_RATIO = CONSOLE_WIDTH / CONSOLE_HEIGHT;
	static constexpr double CONSOLE_PIXEL_ASPECT_RATIO = CONSOLE_WIDTH * CHARACTER_ASPECT_RATIO / CONSOLE_HEIGHT;

	Canvas(bool showRenderTime = false, std::wstring characterPalette = L".'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$");
	~Canvas();

	int render();
	void clearScreen();
	void clearBuffer();
	void setCharacterRGB(int row, int column, double shade = 1.0, unsigned char r = 255, unsigned char g = 255, unsigned char b = 255);
	void setCharacterHSV(int row, int column, double shade = 1.0, double h = 0.0, double s = 0.0, double v = 1.0);

	std::wstring getPalette() const;
	int getRows() const;
	int getColumns() const;
	wchar_t getCharacter(int row, int column) const;
	//WIP
	//wchar_t getCharacterRGB(int row, int column, unsigned char &r, unsigned char &g, unsigned char &b) const;
	//wchar_t getCharacterHSV(int row, int column, double &h, double &s, double &v) const;

private:
	wchar_t *consoleBuffer;
	HANDLE consoleHandle;
	std::wstring characterPalette;

	bool showRenderTime;
	std::chrono::steady_clock::time_point lastFrame;
	uint64_t renderTime;
};