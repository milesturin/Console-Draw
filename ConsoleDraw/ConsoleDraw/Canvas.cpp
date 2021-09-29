#include "Canvas.hpp"

#include <algorithm>

Canvas::Canvas(bool showRenderTime, std::wstring characterPalette)
{
	this->showRenderTime = showRenderTime;
	this->characterPalette = characterPalette;

	consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	consoleBuffer = new wchar_t[CONSOLE_BUFFER_SIZE];
	consoleBuffer[CONSOLE_BUFFER_SIZE - 1] = L'\0';

	for(int i = 0; i < CONSOLE_BUFFER_SIZE - 1; ++i)
	{
		int index = i % ANSI_WORD_SIZE;
		consoleBuffer[i] = index == ANSI_WORD_SIZE - 1 ? EMPTY_CHARACTER : ANSI_COLOR[index];
	}
	
	DWORD bytesWritten;
	const wchar_t *settings = L"\x1B[?3l\0";
	WriteConsoleW(consoleHandle, consoleBuffer, 6, &bytesWritten, nullptr);
}

Canvas::~Canvas()
{
	delete[] consoleBuffer;
}

int Canvas::render()
{
	//WIP
	/*if(showRenderTime)
	{
		int timeLength = log10(renderTime) + 1;
		for(int i = 0; i < timeLength + 2; ++i)
		{
			setCharacterRGB(0, timeLength + 2 - i, 0.0, 255, 255, 255);
			wchar_t character;
			if(i == 0) { character = L's'; }
			else if(i == 1) { character = L'm'; }
			else { character = ((renderTime / (i + 1)) % 10) + ASCII_CHARACTER_OFFSET; }
			consoleBuffer[ANSI_WORD_SIZE * (timeLength + 2 - i) + ANSI_CHARACTER_OFFSET] = character;
		}
	}*/
	DWORD bytesWritten;
	WriteConsoleW(consoleHandle, consoleBuffer, CONSOLE_BUFFER_SIZE, &bytesWritten, nullptr);
	auto currentFrame = std::chrono::steady_clock::now();
	renderTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentFrame - lastFrame).count();
	lastFrame = currentFrame;
	return renderTime;
}

void Canvas::clearScreen()
{
	DWORD bytesWritten;
	WriteConsoleW(consoleHandle, ANSI_CLEAR, ANSI_CLEAR_SIZE, &bytesWritten, nullptr);
}

void Canvas::clearBuffer()
{
	for(int i = 0; i < CONSOLE_SIZE; ++i)
	{
		consoleBuffer[ANSI_WORD_SIZE * i - 1] = EMPTY_CHARACTER;
	}
}

void Canvas::setCharacterRGB(int row, int column, double shade, unsigned char r, unsigned char g, unsigned char b)
{
	row = std::clamp(row, 0, CONSOLE_HEIGHT - 1);
	column = std::clamp(column, 0, CONSOLE_WIDTH - 1);
	shade = std::clamp(shade, 0.0, 1.0);

	int index = (row * CONSOLE_WIDTH + column) * ANSI_WORD_SIZE;
	consoleBuffer[index + ANSI_COLOR_RED_OFFSET + 2] = r % 10 + ASCII_CHARACTER_OFFSET;
	consoleBuffer[index + ANSI_COLOR_RED_OFFSET + 1] = (r / 10) % 10 + ASCII_CHARACTER_OFFSET;
	consoleBuffer[index + ANSI_COLOR_RED_OFFSET] = (r / 100) % 10 + ASCII_CHARACTER_OFFSET;
	consoleBuffer[index + ANSI_COLOR_GREEN_OFFSET + 2] = g % 10 + ASCII_CHARACTER_OFFSET;
	consoleBuffer[index + ANSI_COLOR_GREEN_OFFSET + 1] = (g / 10) % 10 + ASCII_CHARACTER_OFFSET;
	consoleBuffer[index + ANSI_COLOR_GREEN_OFFSET] = (g / 100) % 10 + ASCII_CHARACTER_OFFSET;
	consoleBuffer[index + ANSI_COLOR_BLUE_OFFSET + 2] = b % 10 + ASCII_CHARACTER_OFFSET;
	consoleBuffer[index + ANSI_COLOR_BLUE_OFFSET + 1] = (b / 10) % 10 + ASCII_CHARACTER_OFFSET;
	consoleBuffer[index + ANSI_COLOR_BLUE_OFFSET] = (b / 100) % 10 + ASCII_CHARACTER_OFFSET;
	

	consoleBuffer[index + ANSI_CHARACTER_OFFSET] = characterPalette[std::min<int>(floor(shade * characterPalette.size()), characterPalette.size() - 1)];
}

void Canvas::setCharacterHSV(int row, int column, double shade, double h, double s, double v)
{
	while(h > 360.0) { h -= 360.0; }
	while(h < 0.0) { h += 360.0; }
	h /= 60.0;
	s = std::clamp(s, 0.0, 1.0);
	v = std::clamp(v, 0.0, 1.0);
	
	int i = h;
	double f = h - i;
	double p = v * (1.0 - s);
	double q = v * (1.0 - (s * f));
	double t = v * (1.0 - (s * (1.0 - f)));

	v *= 255.0;
	t *= 255.0;
	p *= 255.0;
	q *= 255.0;

	unsigned char r, g, b;

	switch(i)
	{
	case 0:
		r = v;
		g = t;
		b = p;
		break;
	case 1:
		r = q;
		g = v;
		b = p;
		break;
	case 2:
		r = p;
		g = v;
		b = t;
		break;
	case 3:
		r = p;
		g = q;
		b = v;
		break;
	case 4:
		r = t;
		g = p;
		b = v;
		break;
	default:
		r = v;
		g = p;
		b = q;
		break;
	}
	
	setCharacterRGB(row, column, shade, r, g, b);
}

std::wstring Canvas::getPalette() const { return characterPalette; }

int Canvas::getRows() const { return CONSOLE_HEIGHT; }

int Canvas::getColumns() const { return CONSOLE_WIDTH;  }

wchar_t Canvas::getCharacter(int row, int column) const { return consoleBuffer[(row * CONSOLE_WIDTH + column) * ANSI_WORD_SIZE + ANSI_CHARACTER_OFFSET]; }

//TODO
/*
wchar_t Canvas::getCharacterRGB(int row, int column, unsigned char &r, unsigned char &g, unsigned char &b) const
{
	
}

wchar_t Canvas::getCharacterHSV(int row, int column, double &h, double &s, double &v) const
{

}*/