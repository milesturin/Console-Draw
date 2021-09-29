#include "Canvas.hpp"

#include <algorithm>

const double PI = 3.14159265358979323846264;
const double TWO_PI = PI * 2.0;
const double PI_OVER_TWO = PI / 2.0;

const int SPACE_WIDTH = Canvas::CONSOLE_WIDTH;
const int SPACE_HEIGHT = Canvas::CONSOLE_HEIGHT;
const int LONGER_DIMENSION = std::max<int>(SPACE_WIDTH, SPACE_HEIGHT);//still need?
const int TOTAL_WIDTH = SPACE_WIDTH + 2;
const int TOTAL_HEIGHT = SPACE_HEIGHT + 2;
const int TOTAL_LENGTH = TOTAL_WIDTH * TOTAL_HEIGHT;
const int LEFTMOST_SPACE = 1;
const int RIGHTMOST_SPACE = TOTAL_WIDTH - 2;
const int TOPMOST_SPACE = 1;
const int BOTTOMMOST_SPACE = TOTAL_HEIGHT - 2;

const int GAUSS_SEIDEL_ITERATIONS = 20;

const double TIME_STEP = 0.04;
const double DIFFUSION_RATE = 0.0;// 02;
const double VISCOSITY = 0.0001;

struct Cell
{
	double density, xVelocity, yVelocity;
};

Cell &getCell(Cell *arr, int row, int col) { return arr[row * TOTAL_WIDTH + col]; }

//TEST VERSION
template<double Cell:: *var>
void swapArrays(Cell *current, Cell *previous)
{
	/*Cell *temp = current;
	current = previous;
	previous = temp;*/
	for(int i = 0; i < TOTAL_LENGTH; ++i)
	{
		double temp = current[i].*var;
		current[i].*var = previous[i].*var;
		previous[i].*var = temp;
	}
}

template<double Cell::*var>
void addSource(Cell* current, Cell *previous)
{
	for(int i = 0; i < TOTAL_LENGTH; ++i) { current[i].*var += previous[i].*var * TIME_STEP; }
}

template<double Cell::*var>
void setBounds(Cell* current, int b)
{
	double m = b == 2 ? -1.0 : 1.0;
	for(int i = TOPMOST_SPACE; i <= BOTTOMMOST_SPACE; ++i)
	{
		getCell(current, i, 0).*var = m * getCell(current, i, LEFTMOST_SPACE).*var;
		getCell(current, i, TOTAL_WIDTH - 1).*var = m * getCell(current, i, RIGHTMOST_SPACE).*var;
	}
	m = b == 1 ? -1.0 : 1.0;
	for(int i = LEFTMOST_SPACE; i <= RIGHTMOST_SPACE; ++i)
	{
		getCell(current, 0, i).*var = m * getCell(current, TOPMOST_SPACE, i).*var;
		getCell(current, TOTAL_HEIGHT - 1, i).*var = m * getCell(current, BOTTOMMOST_SPACE, i).*var;
	}
	getCell(current, 0, 0).*var = 0.5 * (getCell(current, TOPMOST_SPACE, 0).*var + getCell(current, 0, LEFTMOST_SPACE).*var);
	getCell(current, 0, TOTAL_WIDTH - 1).*var = 0.5 * (getCell(current, TOPMOST_SPACE, TOTAL_WIDTH - 1).*var + getCell(current, 0, RIGHTMOST_SPACE).*var);
	getCell(current, TOTAL_HEIGHT - 1, 0).*var = 0.5 * (getCell(current, BOTTOMMOST_SPACE, 0).*var + getCell(current, TOTAL_HEIGHT - 1, LEFTMOST_SPACE).*var);
	getCell(current, TOTAL_HEIGHT - 1, TOTAL_WIDTH - 1).*var = 0.5 * (getCell(current, BOTTOMMOST_SPACE, TOTAL_WIDTH - 1).*var + getCell(current, TOTAL_HEIGHT - 1, RIGHTMOST_SPACE).*var);
}

template<double Cell::*var>
void diffuse(Cell *current, Cell *previous, double rate, int b)
{
	double a = rate * TIME_STEP * SPACE_WIDTH * SPACE_HEIGHT;

	for(int i = 0; i < GAUSS_SEIDEL_ITERATIONS; ++i)
	{
		for(int j = TOPMOST_SPACE; j <= BOTTOMMOST_SPACE; ++j)
		{
			for(int k = LEFTMOST_SPACE; k <= RIGHTMOST_SPACE; ++k)
			{
				getCell(current, j, k).*var = (getCell(previous, j, k).*var + a *
					(getCell(current, j - 1, k).*var + getCell(current, j + 1, k).*var +
					getCell(current, j, k - 1).*var + getCell(current, j, k + 1).*var)) / (1 + 4 * a);
			}
		}
		setBounds<var>(current, b);
	}
}

template<double Cell::*var, bool usePrevVel>
void advect(Cell *current, Cell *previous, int b)
{
	const double dtx = TIME_STEP * SPACE_WIDTH;
	const double dty = TIME_STEP * SPACE_HEIGHT;
	for(int i = TOPMOST_SPACE; i <= BOTTOMMOST_SPACE; ++i)
	{
		for(int j = LEFTMOST_SPACE; j <= RIGHTMOST_SPACE; ++j)
		{
			double y = i - dty * getCell(usePrevVel ? previous : current, i, j).yVelocity;
			double x = j - dtx * getCell(usePrevVel ? previous : current, i, j).xVelocity;
			y = std::clamp(y, 0.5, SPACE_HEIGHT + 0.5);
			x = std::clamp(x, 0.5, SPACE_WIDTH + 0.5);
			int i0 = y;
			int i1 = i0 + 1;
			int j0 = x;
			int j1 = j0 + 1;
			double s1 = y - i0;
			double s0 = 1.0 - s1;
			double t1 = x - j0;
			double t0 = 1.0 - t1;
			getCell(current, i, j).*var = s0 * (t0 * getCell(previous, i0, j0).*var + t1 *
				getCell(previous, i0, j1).*var) + s1 * (t0 * getCell(previous, i1, j0).*var +
					t1 * getCell(previous, i1, j1).*var);
		}
	}
	setBounds<var>(current, b);
}

void project(Cell *current, Cell *previous)
{
	double h = 1.0 / LONGER_DIMENSION;
	for(int i = TOPMOST_SPACE; i <= BOTTOMMOST_SPACE; ++i)
	{
		for(int j = LEFTMOST_SPACE; j <= RIGHTMOST_SPACE; ++j)
		{
			getCell(previous, i, j).yVelocity = -0.5 * h * (getCell(current, i + 1, j).xVelocity - getCell(current, i - 1, j).xVelocity + 
				getCell(current, i, j + 1).yVelocity - getCell(current, i, j - 1).yVelocity);
			getCell(previous, i, j).xVelocity = 0.0;
		}
	}
	setBounds<&Cell::xVelocity>(previous, 0);
	setBounds<&Cell::yVelocity>(previous, 0);

	for(int i = 0; i < GAUSS_SEIDEL_ITERATIONS; ++i)
	{
		for(int j = TOPMOST_SPACE; j <= BOTTOMMOST_SPACE; ++j)
		{
			for(int k = LEFTMOST_SPACE; k <= RIGHTMOST_SPACE; ++k)
			{
				getCell(previous, j, k).xVelocity = (getCell(previous, j, k).yVelocity + getCell(previous, j - 1, k).xVelocity + 
					getCell(previous, j + 1, k).xVelocity + getCell(previous, j, k - 1).xVelocity + getCell(previous, j, k + 1).xVelocity) / 4.0;
			}
		}
		setBounds<&Cell::xVelocity>(previous, 0);
	}

	for(int i = TOPMOST_SPACE; i <= BOTTOMMOST_SPACE; ++i)
	{
		for(int j = LEFTMOST_SPACE; j <= RIGHTMOST_SPACE; ++j)
		{
			getCell(current, i, j).xVelocity -= 0.5 * (getCell(previous, i + 1, j).xVelocity - getCell(previous, i - 1, j).xVelocity) / h;
			getCell(current, i, j).yVelocity -= 0.5 * (getCell(previous, i, j + 1).xVelocity - getCell(previous, i, j - 1).xVelocity) / h;
		}
	}
	setBounds<&Cell::xVelocity>(current, 1);
	setBounds<&Cell::yVelocity>(current, 2);
}

void draw(Canvas &canvas, Cell *current)
{
	for(int i = 0; i < Canvas::CONSOLE_HEIGHT; ++i)
	{
		for(int j = 0; j < Canvas::CONSOLE_WIDTH; ++j)
		{
			Cell &cell = getCell(current, TOPMOST_SPACE + i, LEFTMOST_SPACE + j);
			double ang = atan2(cell.yVelocity, cell.xVelocity);
			canvas.setCharacterHSV(i, j, cell.density, ang / TWO_PI * 360.0, 1.0, 1.0);
		}
	}
}
/*
int main()
{
	Canvas canvas;

	Cell *current = new Cell[TOTAL_LENGTH];
	Cell *previous = new Cell[TOTAL_LENGTH];

	for(int i = 0; i < TOTAL_HEIGHT; ++i)
	{
		for(int j = 0; j < TOTAL_WIDTH; ++j)
		{
			int z = (i - 1) * Canvas::CONSOLE_ASPECT_RATIO + j - 1;
			double dist = sqrt(pow(i - TOTAL_HEIGHT / 2, 2.0) + pow(j - TOTAL_WIDTH / 2, 2.0)) / sqrt(pow(TOTAL_HEIGHT / 2, 2.0) + pow(TOTAL_WIDTH / 2, 2.0));
			double ang = atan2(i - TOTAL_HEIGHT / 2, j - TOTAL_WIDTH / 2) + PI;
			getCell(current, i, j).density = 0.0;
			getCell(current, i, j).xVelocity = 0.0;
			getCell(current, i, j).yVelocity = 0.0;
			getCell(previous, i, j).density = (i - 1 == (j - 1) / static_cast<int>(Canvas::CONSOLE_ASPECT_RATIO) || z > RIGHTMOST_SPACE - 5 && z < RIGHTMOST_SPACE);
			getCell(previous, i, j).xVelocity = 7.0 * dist * cos(ang);
			getCell(previous, i, j).yVelocity = 7.0 * dist * sin(ang);
		}
	}
	
	while(true)
	{
		canvas.clearBuffer();

		addSource<&Cell::xVelocity>(current, previous);
		addSource<&Cell::yVelocity>(current, previous);
		swapArrays<&Cell::xVelocity>(current, previous);
		swapArrays<&Cell::yVelocity>(current, previous);
		diffuse<&Cell::xVelocity>(current, previous, VISCOSITY, 1);
		diffuse<&Cell::yVelocity>(current, previous, VISCOSITY, 2);
		project(current, previous);
		swapArrays<&Cell::xVelocity>(current, previous);
		swapArrays<&Cell::yVelocity>(current, previous);
		advect<&Cell::xVelocity, true>(current, previous, 1);
		advect<&Cell::yVelocity, true>(current, previous, 2);
		project(current, previous);
		
		addSource<&Cell::density>(current, previous);
		swapArrays<&Cell::density>(current, previous);
		diffuse<&Cell::density>(current, previous, DIFFUSION_RATE, 0);
		swapArrays<&Cell::density>(current, previous);
		advect<&Cell::density, false>(current, previous, 0);

		draw(canvas, current);

		canvas.clearScreen();
		canvas.render();
		Sleep(10);
	}

	delete[] current;
	delete[] previous;

	return 0;
}*/