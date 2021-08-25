#include <iostream>
#include <vector>
#include <cmath>
#include <GL/gl.h>
#include <freeglut/GL/glut.h>

using namespace std;

#define radius 5
#define precision 0.001

typedef pair<int, int> p;

vector<vector<p>> points(1, vector<p>({}));
vector<vector<p>> tmppoints;
vector<p> bezierpoints;
auto movpoint = points[0].end();

auto findPoint(int x, int y)
{
	for(auto it = points[0].begin(); it < points[0].end(); it++)
		if(((it->first + radius >= x) && (it->first - radius <= x)) && ((it->second + radius >= y) && (it->second - radius <= y)))
			return it;

	return points[0].end();
}

void renderCircle(int x, int y)
{
	glBegin(GL_LINE_LOOP);
	glColor3f(1.0, 1.0, 1.0);

	for(int dx = -radius; dx <= radius; dx++)
	{
		glVertex2i(x + dx, y + sqrt(radius*radius - dx*dx));
		glVertex2i(x + dx, y - sqrt(radius*radius - dx*dx));
	}

	glEnd();
}

void renderBezierCurve()
{
	if(points[0].empty())
		return;

	bezierpoints.clear();
	for(double t = 0.0; t <= 1.0; t+=precision)
	{
		tmppoints.clear();
		tmppoints.push_back(points[0]);

		for(int layer = 1; layer < tmppoints[0].size(); ++layer)
		{
			tmppoints.push_back(vector<p>());
			for(auto point = tmppoints[layer-1].begin()+1; point < tmppoints[layer-1].end(); ++point)
				tmppoints[layer].push_back({(point-1)->first + round(((double)(point->first  - (point-1)->first))*t), 
										 	(point-1)->second + round(((double)(point->second - (point-1)->second))*t)});
		}
		bezierpoints.push_back(tmppoints.back()[0]);
	}
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

	for (auto p : points[0])
		glVertex2i(p.first, p.second);

	glEnd();
	
	for (auto p : points[0])
		renderCircle(p.first, p.second);

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
			auto point = findPoint(x, y);
			
			if(movpoint != points[0].end())
			{
				movpoint->first = x;
				movpoint->second = y;
				movpoint = points[0].end();

				return;
			}

			if(point == points[0].end())
			{
				points[0].push_back({x, y});
				movpoint = points[0].end();
			}
			else
				movpoint = point;
		}
	}
	else if(button == GLUT_RIGHT_BUTTON)
	{
		if(state == GLUT_DOWN)
		{
			auto point = findPoint(x, y);
			if(point != points[0].end())
			{
				points[0].erase(point);
				movpoint = points[0].end();
			}
		}
	}
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Bezier curves");

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);

	glutMainLoop();

	return 0;
}