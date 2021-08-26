#include <iostream>
#include <vector>
#include <cmath>
#include <GL/gl.h>
#include <glut.h>

using namespace std;

#define base_radius 5
#define precision 0.001

typedef pair<int, int> p;

vector<p> points;
vector<p> tmppoints;
vector<p> bezierpoints;

auto movpoint = points.end();
bool isrendersubbezier = false;
double subbezierT = 0.5;

auto findPoint(int x, int y, int radius)
{
	for(auto it = points.begin(); it < points.end(); it++)
		if(((it->first + radius >= x) && (it->first - radius <= x)) && ((it->second + radius >= y) && (it->second - radius <= y)))
			return it;

	return points.end();
}

void renderCircle(int x, int y, int radius)
{
	glBegin(GL_LINE_LOOP);

	for(int dx = -radius; dx <= radius; dx++)
	{
		glVertex2i(x + dx, y + sqrt(radius*radius - dx*dx));
		glVertex2i(x + dx, y - sqrt(radius*radius - dx*dx));
	}

	glEnd();
}

void renderBezierPoint(double t)
{	
	if(points.empty())
		return;

	tmppoints.clear();
	tmppoints.resize((points.size()*(points.size() + 1)) / 2);

	memcpy(tmppoints.data(), points.data(), points.size()*sizeof(p));

	int stack = points.size();
	for(auto layer = points.size()-1; layer > 0; layer--)
	{
		for(auto point = stack; point < stack+layer; point++)
		{	
			tmppoints[point].first = tmppoints[point-layer-1].first + round((tmppoints[point-layer].first - tmppoints[point-layer-1].first)*t);
			tmppoints[point].second = tmppoints[point-layer-1].second + round((tmppoints[point-layer].second - tmppoints[point-layer-1].second)*t);
		}

		stack += layer;
	}
	bezierpoints.push_back(tmppoints.end()[-1]);
}

void renderBezierCurve()
{		
	bezierpoints.clear();

	for(double t = 0; t < 1; t += precision)
		renderBezierPoint(t);
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, h, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glLineWidth(2);
	glBegin(GL_LINE_STRIP);

	glColor3f(1.0, 1.0, 1.0);

	for (auto p : points)
		glVertex2i(p.first, p.second);

	glEnd();

	for (auto p : points)
		renderCircle(p.first, p.second, base_radius);

	if(isrendersubbezier)
	{
		renderBezierPoint(subbezierT);
		srand(0x12e15e35b500f16e);

		int stack = 0;
		for(auto layer = points.size(); layer > 0; layer--)
		{
			glBegin(GL_LINE_STRIP);
			glColor3ub(rand()%255, rand()%255, rand()%255);

			for(int point = stack; point < stack+layer; point++)
				glVertex2i(tmppoints[point].first, tmppoints[point].second);
				
			glEnd();

			for(int point = stack; point < stack+layer; point++)
				renderCircle(tmppoints[point].first, tmppoints[point].second, base_radius-1);

			stack += layer;
		}
	}

	renderBezierCurve();
	glBegin(GL_LINE_STRIP);

	glColor3f(1.0, 0, 0);
	for(auto p : bezierpoints)
		glVertex2i(p.first, p.second);

	glEnd();

	glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			auto point = findPoint(x, y, base_radius);
			
			if(movpoint != points.end())
			{
				movpoint->first = x;
				movpoint->second = y;
				movpoint = points.end();

				return;
			}

			if(point == points.end())
			{
				points.push_back({x, y});
				movpoint = points.end();
			}
			else
				movpoint = point;
		}
	}
	else if(button == GLUT_RIGHT_BUTTON)
	{
		if(state == GLUT_DOWN)
		{
			auto point = findPoint(x, y, base_radius);
			if(point != points.end())
			{
				points.erase(point);
				movpoint = points.end();
			}
		}
	}
}

void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 'q':
			isrendersubbezier = !isrendersubbezier;
		break;

		case 'a':
			if((subbezierT - precision) >= 0)
				subbezierT -= precision;
		break;

		case 'd':
			if((subbezierT + precision)<= 1)
				subbezierT += precision;
		break;

		case 'w':
			if((subbezierT + 0.01) <= 1)
				subbezierT += 0.01;
		break;

		case 's':
			if((subbezierT - 0.01) >= 0)
				subbezierT -= 0.01;
		break;
	}
	display();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1000, 800);
	glutCreateWindow("Bezier curves");

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);

	glutMainLoop();

	return 0;
}