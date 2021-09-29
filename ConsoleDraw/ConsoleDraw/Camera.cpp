#include "Canvas.hpp"
#include "escapi.cpp"
#include <cassert>

const int CAMERA_WIDTH = 1280;
const int CAMERA_HEIGHT = 720;
const double CAMERA_ASPECT_RATIO = CAMERA_WIDTH / CAMERA_HEIGHT;
const int CAMERA_DEVICE = 0;
const int BUFFER_LENGTH = CAMERA_WIDTH * CAMERA_HEIGHT;
const int ADJUSTED_CAMERA_HEIGHT = CAMERA_WIDTH / Canvas::CONSOLE_PIXEL_ASPECT_RATIO;
const int SCAN_OFFSET = (CAMERA_HEIGHT - ADJUSTED_CAMERA_HEIGHT) / 2;
const int CHARACTER_WIDTH = ceil(CAMERA_WIDTH / static_cast<double>(Canvas::CONSOLE_WIDTH));
const int CHARACTER_HEIGHT = ceil(ADJUSTED_CAMERA_HEIGHT / static_cast<double>(Canvas::CONSOLE_HEIGHT));
const int CHARACTER_AREA = CHARACTER_WIDTH * CHARACTER_HEIGHT;

const double RED_MULTIPLIER =  0.2126 / 255.0;
const double GREEN_MULTIPLIER =  0.7152 / 255.0;
const double BLUE_MULTIPLIER =  0.0722 / 255.0;

int main()
{
	Canvas canvas(false);

	if(!setupESCAPI()) { return 1; }

	SimpleCapParams capture;
	capture.mWidth = CAMERA_WIDTH;
	capture.mHeight = CAMERA_HEIGHT;
	capture.mTargetBuf = new int[capture.mWidth * capture.mHeight];

	initCapture(CAMERA_DEVICE, &capture);

	while(true)
	{
		doCapture(CAMERA_DEVICE);
		while(!isCaptureDone(CAMERA_DEVICE));
		
		double accumulators[canvas.CONSOLE_HEIGHT][canvas.CONSOLE_WIDTH][4] = {};
		for(int i = 0; i < canvas.CONSOLE_HEIGHT; ++i)
		{
			for(int j = 0; j < canvas.CONSOLE_WIDTH; ++j)
			{
				accumulators[i][j][0] = 0.0;
				accumulators[i][j][1] = 0.0;
				accumulators[i][j][2] = 0.0;
				accumulators[i][j][3] = 0.0;
			}
		}
		for(int i = 0; i < ADJUSTED_CAMERA_HEIGHT; ++i)
		{
			for(int j = 0; j < CAMERA_WIDTH; ++j)
			{
				int pixel = capture.mTargetBuf[(i + SCAN_OFFSET) * CAMERA_WIDTH + j];
				int y = i / CHARACTER_HEIGHT;
				int x = j / CHARACTER_WIDTH;
				unsigned char r, g, b;
				r = (pixel >> 16) & 0xff;
				g = (pixel >> 8) & 0xff;
				b = (pixel >> 0) & 0xff;
				accumulators[y][x][0] += r;
				accumulators[y][x][1] += g;
				accumulators[y][x][2] += b;
				accumulators[y][x][3] += r * RED_MULTIPLIER + g * GREEN_MULTIPLIER + b * BLUE_MULTIPLIER;
			}
		}
		double paletteStep = 1.0 / canvas.getPalette().size();
		for(int n = 0; n < 3; ++n)
		{
			for(int i = 0; i < canvas.CONSOLE_HEIGHT; ++i)
			{
				for(int j = 0; j < canvas.CONSOLE_WIDTH; ++j)
				{
					switch(n)
					{
					case 0:
					{
						accumulators[i][j][0] /= CHARACTER_AREA;
						accumulators[i][j][1] /= CHARACTER_AREA;
						accumulators[i][j][2] /= CHARACTER_AREA;
						accumulators[i][j][3] /= CHARACTER_AREA;
						break;
					}

					case 1:
					{
						double pixel = accumulators[i][j][3];
						double error = pixel - floor(pixel / paletteStep) * paletteStep;
						if(j < canvas.CONSOLE_WIDTH - 1)
							accumulators[i][j + 1][3] += error * 7.0 / 16.0;
						if(i < canvas.CONSOLE_HEIGHT - 1)
							accumulators[i + 1][j][3] += error * 5.0 / 16.0;
						if(i < canvas.CONSOLE_HEIGHT - 1 && j > 0)
							accumulators[i + 1][j - 1][3] += error * 3.0 / 16.0;
						if(i < canvas.CONSOLE_HEIGHT - 1 && j < canvas.CONSOLE_WIDTH - 1)
							accumulators[i + 1][j + 1][3] += error * 1.0 / 16.0;
						break;
					}

					case 2:
					{
						canvas.setCharacterRGB(i, j, accumulators[i][j][3], accumulators[i][j][0], accumulators[i][j][1], accumulators[i][j][2]);
						break;
					}
					}
				}
			}
		}

		canvas.clearScreen();
		canvas.render();
	}

	deinitCapture(CAMERA_DEVICE);
	return 0;
}