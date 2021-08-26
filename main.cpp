#include <iostream>
#include <vector>
#include <cmath>
#include <GL/gl.h>
#include <freeglut/GL/glut.h>

using namespace std;

#define radius 5
#define precision 0.001

typedef pair<int, int> p;

vector<p> points;
vector<p> tmppoints;
vector<p> bezierpoints;

auto movpoint = points.end();

auto findPoint(int x, int y)
{
	for(auto it = points.begin(); it < points.end(); it++)
		if(((it->first + radius >= x) && (it->first - radius <= x)) && ((it->second + radius >= y) && (it->second - radius <= y)))
			return it;

	return points.end();
}

void renderCircle(int x, int y)
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
	if(points.empty())
		return;
		
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
			auto point = findPoint(x, y);
			if(point != points.end())
			{
				points.erase(point);
				movpoint = points.end();
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