#include <ctime>

#include "Canvas.hpp"

const double PI = 3.14159265358979323846264;
const double TWO_PI = PI * 2.0;
const double PI_OVER_TWO = PI / 2.0;

const int BALL_AMOUNT = 10;
const double MIN_BALL_SPEED = 0.5;
const double MAX_BALL_SPEED = 0.8;
const double MIN_BALL_RADIUS = 1.0;
const double MAX_BALL_RADIUS = 4.0;

const double HIGHLIGHT_LENGTH = 0.5;
const double HIGHLIGHT_INTENSITY = 1.1;

const int TARGET_FPS = 140;
const int MS_PER_FRAME = 1000 / TARGET_FPS;

struct Ball
{
	double x, y, angle, speed, radius, hue;
};

double randDouble()
{
	return static_cast<double>(rand()) / RAND_MAX;
}

double distance(double x1, double y1, double x2, double y2)
{
	return sqrt(pow(x2 - x1, 2.0) + pow(y2 - y1, 2.0));
}

double distance(double x, double y, const Ball *ball)
{
	return sqrt(pow(ball->x - x, 2.0) + pow(ball->y - y, 2.0));
}

double distance(const Ball *b1, const Ball *b2)
{
	return sqrt(pow(b1->x - b2->x, 2.0) + pow(b1->y - b2->y, 2.0));
}

void collision(double phi, double t1, double t2, double v1, double v2, double m1, double m2, double &angle, double &speed)
{
	double coe = (v1 * cos(t1 - phi) * (m1 - m2) + 2.0 * m2 * v2 * cos(t2 - phi)) / (m1 + m2);
	double v1x = coe * cos(phi) + v1 * sin(t1 - phi) * cos(phi + PI_OVER_TWO);
	double v1y = coe * sin(phi) + v1 * sin(t1 - phi) * sin(phi + PI_OVER_TWO);
	angle = atan(v1y / v1x);
	speed = v1x / cos(angle);
}
/*
int main()
{
	srand(time(NULL));
	
	Canvas canvas;
	double spaceWidth = Canvas::CONSOLE_WIDTH * Canvas::CHARACTER_ASPECT_RATIO;
	double spaceHeight = Canvas::CONSOLE_HEIGHT;

	Ball *balls[BALL_AMOUNT];

	for(int i = 0; i < BALL_AMOUNT; ++i)
	{
		balls[i] = new Ball;
		balls[i]->x = randDouble() * spaceWidth;
		balls[i]->y = randDouble() * spaceHeight;
		balls[i]->angle = randDouble() * TWO_PI;
		balls[i]->speed = randDouble() * (MAX_BALL_SPEED - MIN_BALL_SPEED) + MIN_BALL_SPEED;
		balls[i]->radius = randDouble() * (MAX_BALL_RADIUS - MIN_BALL_RADIUS) + MIN_BALL_RADIUS;
		balls[i]->hue = randDouble() * 360.0;
	}

	while(true)
	{
		canvas.clearBuffer();
		
		for(int i = 0; i < BALL_AMOUNT; ++i)
		{
			Ball *ball = balls[i];
			ball->x += cos(ball->angle) * ball->speed;
			ball->y += sin(ball->angle) * ball->speed;
			if(ball->x - ball->radius < 0.0)
			{
				ball->x = ball->radius;
				ball->angle = -ball->angle - PI;
			}
			else if(ball->x + ball->radius > spaceWidth)
			{
				ball->x = spaceWidth - ball->radius;
				ball->angle = -ball->angle - PI;
			}
			if(ball->y - ball->radius < 0.0)
			{
				ball->y = ball->radius;
				ball->angle = -ball->angle;
			}
			else if(ball->y + ball->radius > spaceHeight)
			{
				ball->y = spaceHeight - ball->radius;
				ball->angle = -ball->angle;
			}
			for(int j = i + 1; j < BALL_AMOUNT; ++j)
			{
				Ball *other = balls[j];
				double overlap = (ball->radius + other->radius) - distance(ball, other);
				if(overlap > 0.0)
				{
					double collisionAngle = atan2(other->y - ball->y, other->x - ball->x);
					double ballAngle = ball->angle;
					double otherAngle = other->angle;

					double tempAngle, tempSpeed;
					collision(collisionAngle, ballAngle, otherAngle, ball->speed, other->speed, ball->radius, other->radius, tempAngle, tempSpeed);
					collision(collisionAngle, otherAngle, ballAngle, other->speed, ball->speed, other->radius, ball->radius, other->angle, other->speed);
					ball->angle = tempAngle;
					ball->speed = tempSpeed;

					double ballInterpolation, otherInterpolation;
					if(ball->radius > other->radius)
					{
						otherInterpolation = other->radius / ball->radius;
						ballInterpolation = 1.0 - otherInterpolation;
					}
					else
					{
						ballInterpolation = ball->radius / other->radius;
						otherInterpolation = 1.0 - ballInterpolation;
					}
					ball->x -= cos(collisionAngle) * overlap * ballInterpolation;
					ball->y -= sin(collisionAngle) * overlap * ballInterpolation;
					other->x += cos(collisionAngle) * overlap * otherInterpolation;
					other->y += sin(collisionAngle) * overlap * otherInterpolation;
					break;
				}
			}
		}
		
		for(int i = 0; i < Canvas::CONSOLE_HEIGHT; ++i)
		{
			for(int j = 0; j < Canvas::CONSOLE_WIDTH; ++j)
			{
				double tX = static_cast<double>(j) * Canvas::CHARACTER_ASPECT_RATIO + 0.5;
				double tY = static_cast<double>(i) + 0.5;

				for(Ball *ball : balls)
				{
					double dist = distance(tX, tY, ball);
					if(dist < ball->radius)
					{
						double highlightAngle = -atan2(ball->y, ball->x);
						double highlightLength = ball->radius * HIGHLIGHT_LENGTH;
						double highlightDist = distance(tX, tY, ball->x + cos(highlightAngle) * highlightLength, ball->y + sin(highlightAngle) * highlightLength);
						canvas.setCharacterHSV(i, j, 1.0 - dist / ball->radius, ball->hue, highlightDist / (ball->radius * HIGHLIGHT_INTENSITY), 1.0);
						break;
					}
				}
			}
		}

		canvas.clearScreen();
		canvas.render();
		//Sleep(std::max<int>(MS_PER_FRAME - canvas.render(), 0));
	}

	for(Ball *ball : balls) { delete ball; }

	return 0;
}*/