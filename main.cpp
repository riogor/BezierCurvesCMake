#include <iostream>
#include <vector>
#include <cmath>

#include <GL/gl.h>
#include <glut.h>

using namespace std;

const int         base_radius     = 5;
const long double precision       = 0.001L;
const int         precisionpoints = 1.0L/precision;

typedef pair<int,         int>         pointI;
typedef pair<long double, long double> pointLD;

vector<pointI > basepoints;
vector<pointLD> tmppoints;
vector<pointI > bezierpoints;

auto        movingpoint       = basepoints.end();
bool        isrendersubbezier = false;
long double subbezierT        = 0.5L;

namespace calculate
{
	auto findPointOnClickPos(int x, int y, int radius)
	{
		for(auto point = basepoints.begin(); point < basepoints.end(); point++)
			if(((point->first  + radius >= x) && (point->first  - radius <= x)) 
			&& ((point->second + radius >= y) && (point->second - radius <= y)))
				return point;

		return basepoints.end();
	}

	void calculateBezierPoint(long double t, int point)
	{	
		if(basepoints.empty())
			return;

		tmppoints.clear();
		tmppoints.resize((basepoints.size()*(basepoints.size() + 1)) / 2);

		for(int p = 0; p < basepoints.size(); p++)
			tmppoints[p] = basepoints[p];

		int stacked = basepoints.size();
		for(auto layer = basepoints.size()-1; layer > 0; layer--)
		{
			for(size_t point = stacked; point < stacked+layer; point++)
			{	
				tmppoints[point].first  = tmppoints[point-layer-1].first + 
				    (tmppoints[point-layer].first  - tmppoints[point-layer-1].first)*t;

				tmppoints[point].second = tmppoints[point-layer-1].second + 
				    (tmppoints[point-layer].second - tmppoints[point-layer-1].second)*t;
			}

			stacked += layer;
		}

		if(point != -1) //calculating subbezier
			bezierpoints[point] = tmppoints.end()[-1];
	}

	void calculateBezierCurve()
	{		
		bezierpoints.clear();
		bezierpoints.resize(precisionpoints+1);

		long double t = 0.0L;
		for(int point = 0; point <= precisionpoints; point++, t += precision)
			calculateBezierPoint(t, point);
	}

} // namespace calculate

namespace render
{
	void renderCircle(int x, int y, int radius)
	{
		glLineWidth(1);
		glBegin(GL_LINE_LOOP);

		for(int dx = -radius; dx <= radius; dx++)
		{
			glVertex2i(x + dx, y + sqrt(radius*radius - dx*dx));
			glVertex2i(x + dx, y - sqrt(radius*radius - dx*dx));
		}

		glEnd();
	}

} // namespace render

namespace eventHandler
{
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

		for (auto p : basepoints)
			glVertex2i(p.first, p.second);

		glEnd();

		for (auto p : basepoints)
			render::renderCircle(p.first, p.second, base_radius);

		if(isrendersubbezier)
		{
			calculate::calculateBezierPoint(subbezierT, -1);
			srand(0x12e15e35b500f16e);

			int stacked = basepoints.size();
			for(auto layer = basepoints.size()-1; layer > 0; layer--)
			{
				glBegin(GL_LINE_STRIP);
				glColor3ub(rand()%255, rand()%255, rand()%255);

				for(int point = stacked; point < stacked+layer; point++)
					glVertex2i(tmppoints[point].first, tmppoints[point].second);
					
				glEnd();

				for(int point = stacked; point < stacked+layer; point++)
					render::renderCircle(tmppoints[point].first, 
										tmppoints[point].second, base_radius-1);

				stacked += layer;
			}
		}

		calculate::calculateBezierCurve();
		
		glLineWidth(2);
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
				auto point_on_click = calculate::findPointOnClickPos(x, y, base_radius);
				
				if(movingpoint != basepoints.end())
				{
					movingpoint->first = x;
					movingpoint->second = y;

					movingpoint = basepoints.end();

					return;
				}

				if(point_on_click == basepoints.end())
				{
					basepoints.push_back({x, y});

					movingpoint = basepoints.end();
				}
				else
					movingpoint = point_on_click;
			}
		}
		else if(button == GLUT_RIGHT_BUTTON)
		{
			if(state == GLUT_DOWN)
			{
				auto point_on_click = calculate::findPointOnClickPos(x, y, base_radius);
				if(point_on_click != basepoints.end())
				{
					basepoints.erase(point_on_click);
					
					movingpoint = basepoints.end();
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
				display();
			break;

			case 'a':
				if((subbezierT - precision) >= 0.0L)
				{
					subbezierT -= precision;
					display();
				}
			break;

			case 'd':
				if((subbezierT + precision)<= 1.0L)
				{
					subbezierT += precision;
					display();
				}
			break;

			case 'w':
				if((subbezierT + 0.01L) <= 1.0L)
				{
					subbezierT += 0.01L;
					display();
				}
			break;

			case 's':
				if((subbezierT - 0.01L) >= 0.0L)
				{
					subbezierT -= 0.01L;
					display();
				}
			break;
		}
	}
} // namespace eventHandler

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1000, 800);
	glutCreateWindow("Bezier curves");

	glutReshapeFunc(eventHandler::reshape);
	glutDisplayFunc(eventHandler::display);
	glutMouseFunc(eventHandler::mouse);
	glutKeyboardFunc(eventHandler::keyboard);

	glutMainLoop();

	return 0;
}