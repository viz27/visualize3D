#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <queue>
#include <map>
#include <algorithm>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "file_utils.h"
#include "math_utils.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
using namespace std;


struct GridCell
{
	int p[8];
	float min_iso, max_iso;
	GridCell(int a, int b, int c, int d, int e, int f, int g, int h)
	{
		p[0] = a;
		p[1] = b;
		p[2] = c;
		p[3] = d;
		p[4] = e;
		p[5] = f;
		p[6] = g;
		p[7] = h;
	}
};


struct iso_list_item
{
	int cell_id;
	int parent_id;
	iso_list_item(int a, int b)
	{
		cell_id = a;
		parent_id = b;
	}
};


struct seed_item
{
	int cell_id;
	int count;
	seed_item(int a, int b)
	{
		cell_id = a;
		count = b;
	}
};


/********************************************************************/
/*   Variables */
bool fastMode = 1;                          //If enabled, contor propagation will be done wothout seed set
vector <GridCell> GlobalGridCells;          //list of all grid cells in the dataset
vector <float> isolist;                     //list of iso values for which seed set will be calculated
int isolist_len;
vector <iso_list_item> cells_of_iso[102];   //list of index of grid cells per iso values
vector <seed_item> seeds_of_iso[102];             //seed set per iso value
int GlobalCells_x, GlobalCells_y, GlobalCells_z;
float normalize_scale = 1;                  //normalize points to a box from -1 to 1 on all axes
string dataFile;                            //Dataset Used
string configFile;
string seedFile;
char spacing[100];
const int ANIMATION_DELAY = 20; /* milliseconds between rendering */
int count=0;
const char* pVSFileName1 = "shader.vs";
const char* pFSFileName1 = "shader.fs";
const char* pVSFileName2 = "shader2.vs";
const char* pFSFileName2 = "shader2.fs";
const char* pVSFileName3 = "shader3.vs";
const char* pFSFileName3 = "shader3.fs";
char OffFile[] = "teapot.off";
float slice_axis = 0.0;//z axis
float iso_value = 0.1;//iso value based on which contours will be created
vector<Vector3f> GlobalVertices, Vertices, Normals, Vertices2, Vertices3;
vector<Vector4f> Colors, Colors2, Colors3;
vector<float> GlobalScalars;
const char * theProgramTitle = "Sample";
int theWindowWidth = 700, theWindowHeight = 700;
int theWindowPositionX = 40, theWindowPositionY = 40;
bool isFullScreen = false;
bool isAnimating = true;
float yrotation = 0.0f;
float xrotation = 0.0f;
float mouseposx, mouseposy;
GLuint VBO, VAO, VAO2, VBO2, VAO3, VBO3, VAOT, VBOT;
GLuint gWorldLocation, gViewLocation, gProjLocation, viewLocationvec;
GLuint posID, posID2, posID3, colorID, normalID;
int vrtxCount, scrollVrtxCount;
GLuint ShaderProgram1, ShaderProgram2, ShaderProgram3;
float scrollBarEndx = -1;
float scrollBarEndy = 1;
int globalx=0, globaly=0;
float cameraPosy = 2.0;


/********************************************************************
  Utility functions
 */
vector<Vector3f> scrollVertices11, scrollVertices12, scrollVertices13;
void generateScrollbar(float xleft, float xright, float ytop, float ybottom, float scrollFillGap, float s10, float s12, float s13, float vc)
{
	if (xright > scrollBarEndx) {scrollBarEndx = xright;}
	if (ybottom < scrollBarEndy) {scrollBarEndy = ybottom;}
	scrollVertices11.push_back(Vector3f(xleft, ytop, -1));
	scrollVertices11.push_back(Vector3f(xright, ytop, -1));
	scrollVertices11.push_back(Vector3f(xright, ytop, -1));
	scrollVertices11.push_back(Vector3f(xright, ybottom, -1));
	scrollVertices11.push_back(Vector3f(xright, ybottom, -1));
	scrollVertices11.push_back(Vector3f(xleft, ybottom, -1));
	scrollVertices11.push_back(Vector3f(xleft, ybottom, -1));
	scrollVertices11.push_back(Vector3f(xleft, ytop, -1));
	
	scrollVertices11.push_back(Vector3f(xleft+scrollFillGap, ytop-scrollFillGap, -1));
	scrollVertices11.push_back(Vector3f(xleft+scrollFillGap, ybottom+scrollFillGap, -1));
	scrollVertices11.push_back(Vector3f(xleft+2*scrollFillGap, ytop-scrollFillGap, -1));
	scrollVertices11.push_back(Vector3f(xleft+scrollFillGap, ybottom+scrollFillGap, -1));
	scrollVertices11.push_back(Vector3f(xleft+2*scrollFillGap, ytop-scrollFillGap, -1));
	scrollVertices11.push_back(Vector3f(xleft+2*scrollFillGap, ybottom+scrollFillGap, -1));
	scrollVertices11[10].x = s10;
	scrollVertices11[12].x = s12;
	scrollVertices11[13].x = s13;
	scrollVrtxCount = vc;

}


void generateScrollbar3(float xleft, float xright, float ytop, float ybottom, float scrollFillGap, float s10, float s12, float s13, float vc)
{
	if (xright > scrollBarEndx) {scrollBarEndx = xright;}
	if (ybottom < scrollBarEndy) {scrollBarEndy = ybottom;}
	scrollVertices13.push_back(Vector3f(xleft, ytop, -1));
	scrollVertices13.push_back(Vector3f(xright, ytop, -1));
	scrollVertices13.push_back(Vector3f(xright, ytop, -1));
	scrollVertices13.push_back(Vector3f(xright, ybottom, -1));
	scrollVertices13.push_back(Vector3f(xright, ybottom, -1));
	scrollVertices13.push_back(Vector3f(xleft, ybottom, -1));
	scrollVertices13.push_back(Vector3f(xleft, ybottom, -1));
	scrollVertices13.push_back(Vector3f(xleft, ytop, -1));
	
	scrollVertices13.push_back(Vector3f(xleft+scrollFillGap, ytop-scrollFillGap, -1));
	scrollVertices13.push_back(Vector3f(xleft+scrollFillGap, ybottom+scrollFillGap, -1));
	scrollVertices13.push_back(Vector3f(xleft+2*scrollFillGap, ytop-scrollFillGap, -1));
	scrollVertices13.push_back(Vector3f(xleft+scrollFillGap, ybottom+scrollFillGap, -1));
	scrollVertices13.push_back(Vector3f(xleft+2*scrollFillGap, ytop-scrollFillGap, -1));
	scrollVertices13.push_back(Vector3f(xleft+2*scrollFillGap, ybottom+scrollFillGap, -1));
	scrollVertices13[10].x = s10;
	scrollVertices13[12].x = s12;
	scrollVertices13[13].x = s13;
	scrollVrtxCount = vc; 

}


vector<string> split (const string &s, char delim) {
    vector<string> result;
    stringstream ss (s);
    string item;
    
    while (getline (ss, item, delim)) {
		result.push_back (item);
    }
    return result;
}


float dot(Vector3f a, Vector3f b)
{
	return a.x*b.x+a.y*b.y+a.z*b.z;
}


double ABS(double x)
{
	if(x<0) return -x;
	return x;
}


bool file_exists(const char *filename) {
  std::ifstream ifile(filename);
  return (bool)ifile;
}


Vector3f get_normal(Vector3f x1, Vector3f x2, Vector3f x3)
{
	Vector3f planeNormal;
	Vector3f a = x1-x3;
    Vector3f b = x2-x1;
    planeNormal.x = a.y*b.z-a.z*b.y;
    planeNormal.y = a.z*b.x-a.x*b.z;
    planeNormal.z = a.x*b.y-a.y*b.x;
    return planeNormal;
}


void normalizeVertices()
{
	int i, n;
	n = GlobalVertices.size();
	float x_min, x_max, y_min, y_max, z_min, z_max;
	float x_mid, y_mid, z_mid, max_diff;
	x_min = GlobalVertices[0].x;
	y_min = GlobalVertices[0].y;
	z_min = GlobalVertices[0].z;
	x_max = x_min;
	y_max = y_min;
	z_max = z_min;
	for(i=1;i<n;i++)
	{
		if(GlobalVertices[i].x<x_min) x_min = GlobalVertices[i].x;
		if(GlobalVertices[i].y<y_min) y_min = GlobalVertices[i].y;
		if(GlobalVertices[i].z<z_min) z_min = GlobalVertices[i].z;

		if(GlobalVertices[i].x>x_max) x_max = GlobalVertices[i].x;
		if(GlobalVertices[i].y>y_max) y_max = GlobalVertices[i].y;
		if(GlobalVertices[i].z>z_max) z_max = GlobalVertices[i].z;
	}
	x_mid = (x_max+x_min)/2;
	y_mid = (y_max+y_min)/2;
	z_mid = (z_max+z_min)/2;
	/* Scale down the values to lie in a cube of size 0.8
	 * Ensure that the proportion dist between point pairs
	 * doent't change while normalizing*/
	 max_diff = x_max-x_min;
	 if(y_max-y_min > max_diff) max_diff = y_max-y_min;
	 if(z_max-z_min > max_diff) max_diff = z_max-z_min;
	for(i=0;i<n;i++)
	{
		float x_normalize_scale, y_normalize_scale, z_normalize_scale;
		x_normalize_scale = ((x_max-x_min)/max_diff)*normalize_scale;
		y_normalize_scale = ((y_max-y_min)/max_diff)*normalize_scale;
		z_normalize_scale = ((z_max-z_min)/max_diff)*normalize_scale;
		GlobalVertices[i].x = ((GlobalVertices[i].x - x_mid)*x_normalize_scale)/(x_max-x_mid);
		GlobalVertices[i].y = ((GlobalVertices[i].y - y_mid)*y_normalize_scale)/(y_max-y_mid);
		GlobalVertices[i].z = ((GlobalVertices[i].z - z_mid)*z_normalize_scale)/(z_max-z_mid);
	}
}


Vector3f VertexInterp(double isolevel, int vert1, int vert2, double valp1, double valp2)
{
   double mu;
   Vector3f p, p1, p2;
   p1 = GlobalVertices[vert1];
   p2 = GlobalVertices[vert2];

   if (ABS(isolevel-valp1) < 0.00001)
      return GlobalVertices[vert1];
   if (ABS(isolevel-valp2) < 0.00001)
      return GlobalVertices[vert2];
   if (ABS(valp1-valp2) < 0.00001)
      return GlobalVertices[vert1];
   mu = (isolevel - valp1) / (valp2 - valp1);
   p.x = p1.x + mu * (p2.x - p1.x);
   p.y = p1.y + mu * (p2.y - p1.y);
   p.z = p1.z + mu * (p2.z - p1.z);

   return p ;
}


void Polygonise(GridCell grid, double isolevel)
{
	int i;
	int cubeindex;
	Vector3f vertlist[12];

	int edgeTable[256]={
	0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
	0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
	0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
	0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
	0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
	0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
	0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
	0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
	0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
	0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
	0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
	0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
	0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
	0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
	0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
	0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
	0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
	0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
	0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
	0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
	0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
	0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
	0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
	0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
	0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
	0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
	0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
	0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
	0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
	0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
	0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
	0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0   };
	int triTable[256][16] =
	{{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
	{3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
	{3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
	{3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
	{9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
	{9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
	{2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
	{8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
	{9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
	{4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
	{3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
	{1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
	{4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
	{4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
	{5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
	{2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
	{9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
	{0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
	{2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
	{10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
	{5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
	{5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
	{9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
	{0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
	{1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
	{10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
	{8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
	{2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
	{7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
	{2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
	{11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
	{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
	{11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
	{11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
	{1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
	{9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
	{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
	{2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
	{5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
	{6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
	{3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
	{6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
	{5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
	{1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
	{10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
	{6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
	{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
	{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
	{3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
	{5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
	{0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
	{9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
	{8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
	{5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
	{0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
	{6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
	{10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
	{10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
	{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
	{1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
	{0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
	{10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
	{3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
	{6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
	{9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
	{8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
	{3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
	{6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
	{0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
	{10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
	{10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
	{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
	{7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
	{7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
	{2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
	{1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
	{11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
	{8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
	{0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
	{7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
	{10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
	{2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
	{6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
	{7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
	{2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
	{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
	{10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
	{10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
	{0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
	{7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
	{6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
	{8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
	{9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
	{6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
	{4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
	{10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
	{8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
	{0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
	{1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
	{8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
	{10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
	{4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
	{10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
	{5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
	{11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
	{9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
	{6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
	{7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
	{3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
	{7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
	{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
	{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
	{9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
	{1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
	{4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
	{7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
	{6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
	{3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
	{0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
	{6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
	{0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
	{11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
	{6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
	{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
	{9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
	{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
	{1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
	{10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
	{0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
	{5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
	{10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
	{11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
	{9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
	{7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
	{2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
	{8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
	{9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
	{9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
	{1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
	{9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
	{9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
	{5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
	{0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
	{10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
	{2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
	{0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
	{0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
	{9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
	{5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
	{3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
	{5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
	{8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
	{0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
	{9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
	{1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
	{3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
	{4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
	{9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
	{11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
	{11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
	{2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
	{9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
	{3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
	{1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
	{4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
	{3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
	{0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
	{9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
	{1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};

   /*
      Determine the index into the edge table which
      tells us which vertices are inside of the surface
   */
   cubeindex = 0;
   if (GlobalScalars[grid.p[0]] < isolevel) cubeindex |= 1;
   if (GlobalScalars[grid.p[1]] < isolevel) cubeindex |= 2;
   if (GlobalScalars[grid.p[2]] < isolevel) cubeindex |= 4;
   if (GlobalScalars[grid.p[3]] < isolevel) cubeindex |= 8;
   if (GlobalScalars[grid.p[4]] < isolevel) cubeindex |= 16;
   if (GlobalScalars[grid.p[5]] < isolevel) cubeindex |= 32;
   if (GlobalScalars[grid.p[6]] < isolevel) cubeindex |= 64;
   if (GlobalScalars[grid.p[7]] < isolevel) cubeindex |= 128;

   /* Cube is entirely in/out of the surface */
   if (edgeTable[cubeindex] == 0)
      return;

   /* Find the vertices where the surface intersects the cube */
   if (edgeTable[cubeindex] & 1)
      vertlist[0] =
         VertexInterp(isolevel,grid.p[0],grid.p[1],GlobalScalars[grid.p[0]],GlobalScalars[grid.p[1]]);
   if (edgeTable[cubeindex] & 2)
      vertlist[1] =
         VertexInterp(isolevel,grid.p[1],grid.p[2],GlobalScalars[grid.p[1]],GlobalScalars[grid.p[2]]);
   if (edgeTable[cubeindex] & 4)
      vertlist[2] =
         VertexInterp(isolevel,grid.p[2],grid.p[3],GlobalScalars[grid.p[2]],GlobalScalars[grid.p[3]]);
   if (edgeTable[cubeindex] & 8)
      vertlist[3] =
         VertexInterp(isolevel,grid.p[3],grid.p[0],GlobalScalars[grid.p[3]],GlobalScalars[grid.p[0]]);
   if (edgeTable[cubeindex] & 16)
      vertlist[4] =
         VertexInterp(isolevel,grid.p[4],grid.p[5],GlobalScalars[grid.p[4]],GlobalScalars[grid.p[5]]);
   if (edgeTable[cubeindex] & 32)
      vertlist[5] =
         VertexInterp(isolevel,grid.p[5],grid.p[6],GlobalScalars[grid.p[5]],GlobalScalars[grid.p[6]]);
   if (edgeTable[cubeindex] & 64)
      vertlist[6] =
         VertexInterp(isolevel,grid.p[6],grid.p[7],GlobalScalars[grid.p[6]],GlobalScalars[grid.p[7]]);
   if (edgeTable[cubeindex] & 128)
      vertlist[7] =
         VertexInterp(isolevel,grid.p[7],grid.p[4],GlobalScalars[grid.p[7]],GlobalScalars[grid.p[4]]);
   if (edgeTable[cubeindex] & 256)
      vertlist[8] =
         VertexInterp(isolevel,grid.p[0],grid.p[4],GlobalScalars[grid.p[0]],GlobalScalars[grid.p[4]]);
   if (edgeTable[cubeindex] & 512)
      vertlist[9] =
         VertexInterp(isolevel,grid.p[1],grid.p[5],GlobalScalars[grid.p[1]],GlobalScalars[grid.p[5]]);
   if (edgeTable[cubeindex] & 1024)
      vertlist[10] =
         VertexInterp(isolevel,grid.p[2],grid.p[6],GlobalScalars[grid.p[2]],GlobalScalars[grid.p[6]]);
   if (edgeTable[cubeindex] & 2048)
      vertlist[11] =
         VertexInterp(isolevel,grid.p[3],grid.p[7],GlobalScalars[grid.p[3]],GlobalScalars[grid.p[7]]);

   /* Create the triangle */
   for (i=0;triTable[cubeindex][i]!=-1;i+=3) {
		Vertices.push_back(vertlist[triTable[cubeindex][i  ]]);
		Vertices.push_back(vertlist[triTable[cubeindex][i+1]]);
		Vertices.push_back(vertlist[triTable[cubeindex][i+2]]);
		Colors.push_back(Vector4f(1.0,0.7,0.8,1));
		Colors.push_back(Vector4f(1.0,0.7,0.8,1));
		Colors.push_back(Vector4f(1.0,0.7,0.8,1));
		Vector3f face_norm1 = get_normal(vertlist[triTable[cubeindex][i]], vertlist[triTable[cubeindex][i+1]], vertlist[triTable[cubeindex][i+2]]);
		Normals.push_back(face_norm1);
		Normals.push_back(face_norm1);
		Normals.push_back(face_norm1);
   }
   return;
}


int readConfigFile() {
	FILE * input;
	int ret;
	assert((file_exists(configFile.c_str()) && "Config file doesn't exist"));
	input = fopen(configFile.c_str(), "r");
	//cout<<"Config File looks like"<<endl;
	ret = fscanf(input, "%s", spacing);
	//cout<<spacing<<endl;
	ret = fscanf(input, "%f", &iso_value);
	//cout<<iso_value<<endl;
	int extra_isos;
	ret = fscanf(input, "%d", &extra_isos);
	//cout<<extra_isos<<' ';
	for(int i=0;i<extra_isos;i++)
	{
		float temp;
		ret = fscanf(input, "%f", &temp);
		isolist.push_back(temp);
		//cout<<temp<<' ';
	}
	cout<<endl;
	fclose(input);
	return ret;
}


void writeSeedFile() {
	FILE * output;
	output = fopen(seedFile.c_str(), "w");
	for(int i=0;i<isolist_len;i++)
	{
		int length = seeds_of_iso[i].size();
		fprintf(output, "%d ", length);
		for(int j=0;j<length;j++)
		{
			fprintf(output, "%d ", seeds_of_iso[i][j].cell_id);
		}
		fprintf(output, "\n");
	}
	fclose(output);
	return;
}


int readSeedFile() {
	FILE * input;
	int ret = 0;
	input = fopen(seedFile.c_str(), "r");
	for(int i=0;i<isolist_len;i++)
	{
		int length;
		ret = fscanf(input, "%d", &length);
		for(int j=0;j<length;j++)
		{
			int temp;
			ret = fscanf(input, "%d", &temp);
			seeds_of_iso[i].push_back(seed_item(temp, 0));
		}
	}
	fclose(input);
	return ret;
}


void readRawFile(){
	assert((file_exists(dataFile.c_str()) && "Data file doesn't exist"));
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	GlobalVertices.clear();
	//split source file using '_' as delimiter to get the x,y,z extants and type of data
        vector<string> params = split(dataFile, '_');
    
        string xyzext = params[1];
        string formatspec = params[2];

        //remove the .raw extension
        vector<string> format = split(formatspec,'.');

        //split the xyz extants using 'x' as delimiter and convert them to integers
        vector<string> xyz = split(xyzext,'x');

        int x = stoi(xyz[0]);
        int y = stoi(xyz[1]);
        int z = stoi(xyz[2]);
        //cout<<x<<' '<<y<<' '<<z<<endl;

        //calculate the size of the data
        long long size = x*y*z;
        vector <string> space = split(spacing,'x');
 
        ifstream infile; 

        //open the raw file to read it's contents
        infile.open(dataFile, ios::binary | ios::in) ;
        char *datac = NULL;
        int *datai = NULL;
        if(format[0] == "uint8") {
        	datac = new char[size];
        	infile.read(datac, size);
        }
        else if(format[0] == "uint16")
        {
        	datai = new int[size];
    	    infile.read((char *)datai, size);
        }
        else
        {
		cout<<"UNSUPPORTED FORMAT!!"<<endl;
        }
        int dp = 0;
        infile.close();
        int i = 0;
	long long finalcubes;//finalvertexsize
	GlobalCells_x = (x+stoi(space[0])-1)/stoi(space[0]);
	GlobalCells_y = (y+stoi(space[1])-1)/stoi(space[1]);
	GlobalCells_z = (z+stoi(space[2])-1)/stoi(space[2]);
	//finalvertexsize = GlobalCells_x*GlobalCells_y*GlobalCells_z;
	finalcubes = (GlobalCells_x-1)*(GlobalCells_y-1)*(GlobalCells_z-1);
	    if (finalcubes<1)
    {
	cout<<"NO CUBES AT THE END AFTER REMOVING VERTIES!!"<<endl;
    }
	for(int r=0;r<GlobalCells_z;r++)
	for(int q=0;q<GlobalCells_y;q++)
	for(int p=0;p<GlobalCells_x;p++)
	{
		if ((p%stoi(space[0]))!=0) {i++;continue;}
		if ((q%stoi(space[1]))!=0) {i++;continue;}
		if ((r%stoi(space[2]))!=0) {i++;continue;}
		if(format[0] == "uint8") 
		{
			dp = datac[i];
			if(dp < 0) dp += 256;
		}
		else if(format[0] == "uint16") 
		{
			dp = datai[i];
			if(dp < 0) dp += 65536;
		}
		GlobalVertices.push_back(Vector3f(p, q, r));
		GlobalScalars.push_back((float)dp/256);
		i++;
	}
	normalizeVertices();
	/* Read the Polygons */	
	//cout<<GlobalCells_z<<" "<<GlobalCells_y<<" "<<GlobalCells_x<<endl;
	for(int r=0;r<GlobalCells_z-1;r++)
 	for(int q=0;q<GlobalCells_y-1;q++)
	for(int p=0;p<GlobalCells_x-1;p++)
	{	
		int x_stride = GlobalCells_x;
		int xy_stride = GlobalCells_x*GlobalCells_y;
		int curr = r*xy_stride+q*x_stride+p;
		int polys[8];
		polys[0]=curr; polys[1]=curr+1; polys[2]=curr+x_stride; polys[3]=curr+x_stride+1;
		curr+=xy_stride;
		polys[4]=curr; polys[5]=curr+1; polys[6]=curr+x_stride; polys[7]=curr+x_stride+1;
		GridCell grid = GridCell(polys[0], polys[1], polys[3], polys[2], polys[4], polys[5], polys[7], polys[6]);
		GlobalGridCells.push_back(grid);
	}

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	cout <<"Time taken = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]\n";
	//cout <<std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]\n";
	GlobalCells_x-=1;	GlobalCells_y-=1;	GlobalCells_z-=1;
}


bool vertexeq(Vector3f a, Vector3f b)
{
	if (a.x!=b.x) return 0;
	if (a.y!=b.y) return 0;
	if (a.z!=b.z) return 0;
	return 1;
}


int get_cell_index(int j, int id)
{
	int left, right, mid, mid_cell_id;
	left = 0;
	right = cells_of_iso[j].size()-1;
	while(1)
	{
		if(left>right)
		{
			cout<<"SOME ERROR!! CANNOT FIND "<<id<<" in iso list of "<<isolist[j]<<endl;
			return -1;
		}
		mid = (left+right)/2;
		mid_cell_id = cells_of_iso[j][mid].cell_id;
		if(mid_cell_id == id) return mid;
		if(mid_cell_id < id) left = mid+1;
		else right = mid-1;
	}
}


int get_parent_cell(int j, int index)
{
	if(cells_of_iso[j][index].parent_id != index)
	{
		cells_of_iso[j][index].parent_id = get_parent_cell(j, cells_of_iso[j][index].parent_id);
	}
	return cells_of_iso[j][index].parent_id;
}


void generate_seed_set()
{
	int stride_xy = GlobalCells_x*GlobalCells_y;
	int stride_x = GlobalCells_x;
	
	for(int z=0;z<GlobalCells_z;z++)
	for(int y=0;y<GlobalCells_y;y++)
	for(int x=0;x<GlobalCells_x;x++)
	{
		int curr_cell_id = z*stride_xy + y*stride_x + x;
		
		int neighbours[7] = {-1, -1, -1, -1, -1, -1, -1};//neighbours to right, down and top.. ignore left, up and bottom
		
		int x_right = x+1;
		int y_down = y+1;
		int z_top = z+1;
		
		if(x_right<GlobalCells_x)
		{
			neighbours[0] = z*stride_xy + y*stride_x + x_right;
			if(y_down<GlobalCells_y)
			{
				neighbours[1] = z*stride_xy + y_down*stride_x + x;
				neighbours[2] = z*stride_xy + y_down*stride_x + x_right;
			}
		}
		else if(y_down<GlobalCells_y) neighbours[1] = z*stride_xy + y_down*stride_x + x;
		
		if(z_top<GlobalCells_z)
		{
			neighbours[3] = z_top*stride_xy + y*stride_x + x;
			if(x_right<GlobalCells_x)
			{
				neighbours[4] = z_top*stride_xy + y*stride_x + x_right;
				if(y_down<GlobalCells_y)
				{
					neighbours[5] = z_top*stride_xy + y_down*stride_x + x;
					neighbours[6] = z_top*stride_xy + y_down*stride_x + x_right;
				}
			}
			else if(y_down<GlobalCells_y) neighbours[5] = z_top*stride_xy + y_down*stride_x + x;
		}
		
		for(int i=0;i<7;i++)
		{
			if(neighbours[i] == -1) continue;
			GridCell curr_cell = GlobalGridCells[curr_cell_id];
			GridCell neighbour_cell = GlobalGridCells[neighbours[i]];
			
			float min_iso_common = curr_cell.min_iso;
			if(neighbour_cell.min_iso > min_iso_common) min_iso_common =  neighbour_cell.min_iso;
			
			float max_iso_common = curr_cell.max_iso;
			if(neighbour_cell.max_iso < max_iso_common) max_iso_common =  neighbour_cell.max_iso;
			
			for(int j=0;j<isolist_len;j++)
			{
				if(isolist[j]>max_iso_common) break;
				if(isolist[j]<min_iso_common) continue;
				
				//connect the two cells in the cells_of_iso[j] list
				int curr_cell_index = get_cell_index(j, curr_cell_id);
				int neighbour_cell_index = get_cell_index(j, neighbours[i]);
				int neighbour_parent = get_parent_cell(j, neighbour_cell_index);
				int curr_parent = get_parent_cell(j, curr_cell_index);
				if(neighbour_parent!=curr_parent)
				{
					if(curr_parent<neighbour_parent) cells_of_iso[j][neighbour_parent].parent_id = curr_parent;
					else cells_of_iso[j][curr_parent].parent_id = neighbour_parent;
				}
			}
		}
	}
	
	for(unsigned int i=0;i<isolist.size();i++)
	{
		for(unsigned int j=0;j<cells_of_iso[i].size();j++)
		{
			int pid = get_parent_cell(i, j);
			if(pid==(int)j)
			{
				seeds_of_iso[i].push_back(seed_item(cells_of_iso[i][pid].cell_id, 1));
			}
			else
			{
				for(unsigned int k=0;k<seeds_of_iso[i].size();k++)
				{
					if(seeds_of_iso[i][k].cell_id == cells_of_iso[i][pid].cell_id)
					{
						seeds_of_iso[i][k].count++;
						break;
					}
				}
			}
		}
	}
	
	for(unsigned int i=0;i<isolist.size();i++)
	{
		int sumsum = 0;
		for(unsigned int j=0;j<seeds_of_iso[i].size();j++)
		{
			//cout<<seeds_of_iso[i][j].cell_id<<" : "<<seeds_of_iso[i][j].count<<endl;
			sumsum += seeds_of_iso[i][j].count;
		}
		cout<<isolist[i]<<" : "<<seeds_of_iso[i].size()<<" : "<<sumsum<<endl;
	}
	
	cout<<"IDs and counts of 0.1\n";
	for(unsigned int j=0;j<seeds_of_iso[9].size();j++)
	{
		cout<<seeds_of_iso[9][j].cell_id<<" : "<<seeds_of_iso[9][j].count<<endl;
	}
}


int BFS(int cellid, float extract_iso)
{
	int total_cells = 0;
	queue <int> q;
	map <int, int> traversed;
	q.push(cellid);
	while(!q.empty())
    {
        int curr_cell = q.front();
        q.pop();
        if(traversed[curr_cell]==1) continue;
        total_cells++;
        //cout<<"Extracting from "<<curr_cell<<' '<<total_cells<<' '<<traversed[curr_cell]<<endl;
        traversed[curr_cell]=1;
        Polygonise(GlobalGridCells[curr_cell],iso_value);

        int x,y,z;
		x = curr_cell%GlobalCells_x;
		y = ((curr_cell-x)/GlobalCells_x)%GlobalCells_y;
		z = (curr_cell-(y*GlobalCells_x+x))/(GlobalCells_y*GlobalCells_x);

		for(int k=z-1;k<=z+1;k++)
		for(int j=y-1;j<=y+1;j++)
		for(int i=x-1;i<=x+1;i++)
		{
			if(i<0 || i>=GlobalCells_x) continue;
			if(j<0 || j>=GlobalCells_y) continue;
			if(k<0 || k>=GlobalCells_z) continue;
			int neighbor_cell_id = k*(GlobalCells_x*GlobalCells_y) + j*GlobalCells_x + i;
			if(traversed[neighbor_cell_id]!=0) continue;
			if(GlobalGridCells[neighbor_cell_id].min_iso>extract_iso) continue;
			if(GlobalGridCells[neighbor_cell_id].max_iso<extract_iso) continue;
			//cout<<"Pushing "<<neighbor_cell_id<<endl;
			q.push(neighbor_cell_id);
		}
    }
    traversed.clear();
    //cout<<"Returning "<<total_cells<<" cells\n\n";
    return total_cells;
}


int propagate_contours(float iso)
{
	int iso_index = 0;
	int total_cells = 0;
	Vertices.clear();
	Colors.clear();
	Normals.clear();
	for(int i=0;i<isolist_len;i++)
	{
		//cout<<i<<"\t: "<<isolist[i]<<' '<<cells_of_iso[i].size()<<endl;
		float diff = ABS(isolist[i]-iso_value);
		if(diff<=0.00001) iso_index=i;
	}

	cout<<"iso_index = "<<iso_index<<endl;
	for(unsigned int i=0;i<seeds_of_iso[iso_index].size();i++)
	{
		int cellid = seeds_of_iso[iso_index][i].cell_id;
		//Polygonise(GlobalGridCells[cellid],iso_value);
		//cout<<"Seed cell start "<<cellid<<endl;
		total_cells += BFS(cellid, iso);
	}
	return total_cells;
}


int propagate_contours_fast(float iso)
{
	int iso_index = 0;
	Vertices.clear();
	Colors.clear();
	Normals.clear();
	for(int i=0;i<isolist_len;i++)
	{
		//cout<<i<<"\t: "<<isolist[i]<<' '<<cells_of_iso[i].size()<<endl;
		float diff = ABS(isolist[i]-iso_value);
		if(diff<=0.00001) iso_index=i;
	}

	cout<<"iso_index = "<<iso_index<<endl;
	for(unsigned int i=0;i<cells_of_iso[iso_index].size();i++)
	{
		int cellid = cells_of_iso[iso_index][i].cell_id;
		Polygonise(GlobalGridCells[cellid],iso_value);
	}
	return cells_of_iso[iso_index].size();
}


/* post: compute frames per second and display in window's title bar */
void computeFPS() {
	static int frameCount = 0;
	static int lastFrameTime = 0;
	static char * title = NULL;
	int currentTime;

	if (!title)
		title = (char*) malloc((strlen(theProgramTitle) + 20) * sizeof (char));
	frameCount++;
	currentTime = glutGet((GLenum) (GLUT_ELAPSED_TIME));
	if (currentTime - lastFrameTime > 1000) {
		sprintf(title, "%s [ FPS: %4.2f ]",
			theProgramTitle,
			frameCount * 1000.0 / (currentTime - lastFrameTime));
		glutSetWindowTitle(title);
		lastFrameTime = currentTime;
		frameCount = 0;
	}
}


void run_preprocessing()
{
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	
	cout<<"Reading data\n";
	readConfigFile();
	readRawFile();

	for(unsigned int i=0; i<GlobalGridCells.size();i++)
	{
		float min, max, vals[8];
		int j;
		
		for(j=0;j<8;j++) vals[j] = GlobalScalars[GlobalGridCells[i].p[j]];
		min = vals[0];
		max = vals[0];
		for(j=1;j<8;j++)
		{
			if(vals[j]<min) min=vals[j];
			if(vals[j]>max) max=vals[j];
		}
		GlobalGridCells[i].min_iso = min;
		GlobalGridCells[i].max_iso = max;
	}
	
	int cubeCount = GlobalGridCells.size();
	assert (cubeCount == GlobalCells_x*GlobalCells_y*GlobalCells_z);
	cout<<"Grid cell COUNT IS:" <<cubeCount<<endl;
	
	for(float i = 0.01;i<=1;i+=0.01) isolist.push_back(i);
	isolist_len = isolist.size();
	sort(isolist.begin(), isolist.begin()+isolist_len);
	
	if(fastMode == 1)
	{
		cout<<"\nStarting to identify grid cells for every iso value\n";
		for(unsigned int i=0; i<GlobalGridCells.size();i++)
		{
			for(int j=0;j<isolist_len;j++)
			{
				if(isolist[j]>GlobalGridCells[i].max_iso) break;
				if(isolist[j]<GlobalGridCells[i].min_iso) continue;
				int parent_val = cells_of_iso[j].size();
				cells_of_iso[j].push_back(iso_list_item(i, parent_val));
				//pbs++;
			}
		}
	}
	else if(!file_exists(seedFile.c_str()) && fastMode == 0)
	{
		cout<<"\nSeed file not found.. Starting to extract seed cells\n";
		//unsigned long long pbs = 0;
		for(unsigned int i=0; i<GlobalGridCells.size();i++)
		{	
			for(int j=0;j<isolist_len;j++)
			{
				if(isolist[j]>GlobalGridCells[i].max_iso) break;
				if(isolist[j]<GlobalGridCells[i].min_iso) continue;
				int parent_val = cells_of_iso[j].size();
				cells_of_iso[j].push_back(iso_list_item(i, parent_val));
				//pbs++;
			}
		}
		std::chrono::steady_clock::time_point time_now = std::chrono::steady_clock::now();
		cout << "Identified cells per iso value.. Time = " << std::chrono::duration_cast<std::chrono::seconds>(time_now - begin).count() << "[s], ";

		cout<<"Generating Seed Set..\n";
		generate_seed_set();
		time_now = std::chrono::steady_clock::now();
		cout << "Seed Set Generated.. Time = " << std::chrono::duration_cast<std::chrono::seconds>(time_now - begin).count() << "[s]\n";
		for(int j=0;j<isolist_len;j++)
		{
			cells_of_iso[j].clear();
		}
		writeSeedFile();
	}
	else
	{
		cout<<"\nSeed file found.. Reading seed cells from there\n";
		readSeedFile();
		cout<<"\nNumber of Seeds per iso value found"<<endl;
		for(unsigned int i=0;i<isolist.size();i++)
		{
			cout<<isolist[i]<<" : "<<seeds_of_iso[i].size()<<endl;
		}
	}
}


static void CreateVertexBuffer() {
	glGenVertexArrays(1, &VAO);
	//cout << "VAO: " << VAO << endl;
	glBindVertexArray(VAO);
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	
	run_preprocessing();
	
	cout<<"\nPropagating contours for iso value: "<<iso_value<<endl;
	int contur_cells;
	if(fastMode == 0) contur_cells = propagate_contours(iso_value);
	else contur_cells = propagate_contours_fast(iso_value);
	cout<<"Extracted contours from "<<contur_cells<<" cells\n";
	std::chrono::steady_clock::time_point time_now = std::chrono::steady_clock::now();
    cout << "Time = " << std::chrono::duration_cast<std::chrono::seconds>(time_now - begin).count() << "[s], ";

	
	vrtxCount = Vertices.size();
	cout<<"VERTEX COUNT IS:" <<vrtxCount<<endl;

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vrtxCount*(sizeof(Vector3f)+sizeof(Vector3f)+sizeof(Vector4f)), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vrtxCount*sizeof(Vector3f), &Vertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER, vrtxCount*sizeof(Vector3f), vrtxCount*sizeof(Vector3f), &Normals[0]);
	glBufferSubData(GL_ARRAY_BUFFER, vrtxCount*(2*sizeof(Vector3f)), vrtxCount*sizeof(Vector4f), &Colors[0]);
	

	glEnableVertexAttribArray(posID);
	glVertexAttribPointer(posID, 3, GL_FLOAT, GL_FALSE, 0, 0);//0'th attrib(position), 3 values, all are float, don't normalize, skip 0 bytes, offset 0 bytes
	
	glEnableVertexAttribArray(normalID);
	glVertexAttribPointer(normalID, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vrtxCount*sizeof(Vector3f)));
	
	glEnableVertexAttribArray(colorID);
	glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(2*vrtxCount*sizeof(Vector3f)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}


static void CreateVertexBuffer2() {
	glGenVertexArrays(1, &VAO2);
	glBindVertexArray(VAO2);
	glGenBuffers(1, &VBO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	
	generateScrollbar(-0.99, -0.7, 0.99, 0.95, 0.005, -0.8, -0.8, -0.8, 14);
//	generateScrollbar(-0.7, -0.5, 0.99, 0.9, 0.005, -0.8, -0.8, -0.8, 14);
	
	glBufferData(GL_ARRAY_BUFFER, scrollVrtxCount*(sizeof(Vector3f)), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, scrollVrtxCount*sizeof(Vector3f), &scrollVertices11[0]);
	glEnableVertexAttribArray(posID2);
	glVertexAttribPointer(posID2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);
	
		glBindVertexArray(VAO2);
		glGenBuffers(1, &VBO2);
		glBindBuffer(GL_ARRAY_BUFFER, VBO2);
		glBufferData(GL_ARRAY_BUFFER, scrollVrtxCount*(sizeof(Vector3f)), NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, scrollVrtxCount*sizeof(Vector3f), &scrollVertices11[0]);
		glEnableVertexAttribArray(posID2);
		glVertexAttribPointer(posID2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindVertexArray(0);
}


static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType) {
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(0);
	}	

	const GLchar * p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	glShaderSource(ShaderObj, 1, p, Lengths);
	glCompileShader(ShaderObj);
	GLint success;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		exit(1);
	}

	glAttachShader(ShaderProgram, ShaderObj);
}


static void CompileShaders(int type) {
	GLuint ShaderProgram = glCreateProgram();

	if (type == 1)
	{
		//cout<<"Compiling shader type "<<type<<' '<<ShaderProgram<<endl;
		ShaderProgram1 = ShaderProgram;
	}
	if (type == 2)
	{
		//cout<<"Compiling shader type "<<type<<' '<<ShaderProgram<<endl;
		ShaderProgram2 = ShaderProgram;
	}
	if (type == 3)
	{
		//cout<<"Compiling shader type "<<type<<' '<<ShaderProgram<<endl;
		ShaderProgram3 = ShaderProgram;
	}

	if (ShaderProgram == 0) {
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}
	string vs, fs;
	if (type == 1)
	{
		if (!ReadFile(pVSFileName1, vs)) {
			exit(1);
		}

		if (!ReadFile(pFSFileName1, fs)) {
			exit(1);
		}

		AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);
		AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);
	}
	if (type == 2)
	{
		if (!ReadFile(pVSFileName2, vs)) {
			exit(1);
		}

		if (!ReadFile(pFSFileName2, fs)) {
			exit(1);
		}

		AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);
		AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);
	}
	if (type == 3)
	{
		if (!ReadFile(pVSFileName3, vs)) {
			exit(1);
		}

		if (!ReadFile(pFSFileName3, fs)) {
			exit(1);
		}

		AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);
		AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);
	}
	
	GLint Success = 0;
	GLchar ErrorLog[1024] = {0};

	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(ShaderProgram, sizeof (ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof (ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	glUseProgram(ShaderProgram);
	
	glUseProgram(ShaderProgram);
	if(type == 1)
	{
		gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
		gViewLocation = glGetUniformLocation(ShaderProgram, "gView");
		gProjLocation = glGetUniformLocation(ShaderProgram, "gProj");
		viewLocationvec = glGetUniformLocation(ShaderProgram, "viewPos");
		posID =    glGetAttribLocation(ShaderProgram, "Position");
		colorID =  glGetAttribLocation(ShaderProgram, "Color");
		normalID = glGetAttribLocation(ShaderProgram, "aNormal");
		//cout<<posID<<' '<<colorID<<' '<<normalID<<' '<<viewLocationvec<<endl;
	}
	if(type == 2)
	{
		posID2 =    glGetAttribLocation(ShaderProgram, "Position");
		//colorID2 =  glGetAttribLocation(ShaderProgram, "Color");
	}
	if(type == 3)
	{
		posID3 =    glGetAttribLocation(ShaderProgram, "Position");
		//colorID2 =  glGetAttribLocation(ShaderProgram, "Color");
	}
}


/********************************************************************
 Callback Functions
 These functions are registered with the glut window and called 
 when certain events occur.
 */
void onInit(int argc, char * argv[])
/* pre:  glut window has been initialized
   post: model has been initialized */ {
	/* by default the back ground color is black */
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        CompileShaders(1);
	CreateVertexBuffer();
	CompileShaders(2);
	CreateVertexBuffer2();
//	CompileShaders(3);
//	CreateVertexBuffer3();
	//glDisable(GL_DEPTH_TEST); 
}
void drawBitmapText(string s,float x,float y) 
{ 	const char *c;
	glRasterPos2f(x, y);
	for (c=s.c_str(); *c != '\0'; c++) 
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *c);
	}
}


vector< string > iso_input(1);
static void onDisplay() {
	glUseProgram(ShaderProgram1);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS); 
	glPointSize(5);
	
	glm::mat4 World, View, Proj;
	World = glm::mat4(1.0f);
	World = glm::rotate(World, glm::radians(yrotation), glm::vec3(0.0f, 1.0f, 0.0f));
	World = glm::rotate(World, glm::radians(xrotation), glm::vec3(1.0f, 0.0f, 0.0f));
	Vector3f viewpos = Vector3f(0.0f, 0.0f, cameraPosy);
	View = glm::lookAt(
	    glm::vec3(viewpos.x, viewpos.y, viewpos.z),
	    glm::vec3(0.0f, 0.0f, 0.0f),
	    glm::vec3(0.0f, 1.0f, 0.0f)
	    );
        //Proj = glm::perspective(glm::radians(80.0f), 800.0f / 600.0f, -1.0f, 1.0f);
	Proj = glm::perspective(glm::radians(80.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	glUniformMatrix4fv(gWorldLocation, 1, GL_FALSE, glm::value_ptr(World));
	glUniformMatrix4fv(gViewLocation, 1, GL_FALSE, glm::value_ptr(View));
	glUniformMatrix4fv(gProjLocation, 1, GL_FALSE, glm::value_ptr(Proj));
	glUniform3fv(viewLocationvec, 1, Vector3f(0.0f, 0.0f, 2.0f));

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vrtxCount); //mode, first, count
	//glDrawArrays(GL_LINES, 0, vrtxCount);
	glBindVertexArray(0);
	
	glUseProgram(ShaderProgram2);
	glBindVertexArray(VAO2);
	glDrawArrays(GL_LINES, 0, 8); //mode, first, count
	glDrawArrays(GL_TRIANGLES, 8, 6);
	//glDrawArrays(GL_POINTS, 0, cubeVrtxCount);
	glBindVertexArray(0);
	//glDisableVertexAttribArray(0);
	std::string mouse_X = std::to_string(cameraPosy); 	
	std::string iso_val = std::to_string(iso_value);
	int n=mouse_X.length();		
	int o=iso_val.length();
	char n_X[n+1];		
	char o_Y[o+1];
	strcpy(n_X, mouse_X.c_str());		
	strcpy(o_Y, iso_val.c_str());
	drawBitmapText("X:", -0.68,0.96); 
	drawBitmapText(n_X, -0.64, 0.96);
	drawBitmapText("RENDERING AT ISOVALUE:", -0.99,0.91); 
	drawBitmapText(o_Y, -0.58,0.91); 
	drawBitmapText("ENTER ISOVALUE:", -0.99,0.86);
	for( size_t i = 0; i < iso_input.size(); ++i )
        {
        	ostringstream oss;
        	oss << iso_input[i];
        //	if(i>=1) iso_value=std::stof(iso_input[i]);
        	//cout<<iso_input[i]<<endl;
        	//const int fontHeight = glutBitmapHeight( GLUT_BITMAP_TIMES_ROMAN_10 );
        	glRasterPos2f( -0.68,0.86 );
        	glutBitmapString( GLUT_BITMAP_TIMES_ROMAN_10, (const unsigned char*)( oss.str().c_str() ) );
        } 
	//cout<<iso_value<<"*******"<<endl;
/*	glUseProgram(ShaderProgram3);
	glBindVertexArray(VAO3);
	glDrawArrays(GL_LINES, 0, 8); //mode, first, count
	glDrawArrays(GL_TRIANGLES, 8, 6);
	//glDrawArrays(GL_POINTS, 0, cubeVrtxCount);
	glBindVertexArray(0);   
	//glDisableVertexAttribArray(0);	
	// check for any errors when rendering */
	GLenum errorCode = glGetError();
	if (errorCode == GL_NO_ERROR) {
		/* double-buffering - swap the back and front buffers */
		glutSwapBuffers();
		glFlush();
	} else {
		fprintf(stderr, "OpenGL rendering error %d\n", errorCode);
	}
	/*cout<<"end of display"<<endl;*/
}


/* pre:  glut window has been resized
 */
static void onReshape(int width, int height) {
	glViewport(0, 0, width, height);
	if (!isFullScreen) {
		theWindowWidth = width;
		theWindowHeight = height;
	}
	// update scene based on new aspect ratio....
}


/* pre:  glut window is not doing anything else
   post: scene is updated and re-rendered if necessary */
static void onIdle() {
	static int oldTime = 0;
	if (isAnimating) {
		int currentTime = glutGet((GLenum) (GLUT_ELAPSED_TIME));
		// Ensures fairly constant framerate 
		if (currentTime - oldTime > ANIMATION_DELAY) {
			// do animation....
			yrotation += 0.3f;

			oldTime = currentTime;
			// compute the frame rate 
			computeFPS();
			// notify window it has to be repainted 
			glutPostRedisplay();
	 	}
	}	
}


/* pre:  mouse is dragged (i.e., moved while button is pressed) within glut window
   post: scene is updated and re-rendered  */
static void onMouseMotion(int x, int y) {
	/* notify window that it has to be re-rendered */
	//cout<<"Absolute:"<<x<<' '<<y<<endl;
	mouseposx = ((float)x*2/theWindowWidth) - 1;
	mouseposy = 1 - ((float)y*2/theWindowHeight);
	//cout<<"Window:"<<mouseposx<<' '<<mouseposy<<endl;
	//cout<<scrollBarEndx<<' '<<scrollBarEndy<<endl;
	if (mouseposx < scrollBarEndx && mouseposy>scrollBarEndy)
	{
	  /*  if(mouseposx<=-0.7 && mouseposx>=-0.99 && mouseposy<=0.99 && mouseposy>=0.95 )*/{
		scrollVertices11[10].x = mouseposx;
		scrollVertices11[12].x = mouseposx;
		scrollVertices11[13].x = mouseposx;
		cameraPosy = (mouseposx+1)*3/0.3;
		glBindVertexArray(VAO2);
		glGenBuffers(1, &VBO2);
		glBindBuffer(GL_ARRAY_BUFFER, VBO2);
		glBufferData(GL_ARRAY_BUFFER, scrollVrtxCount*(sizeof(Vector3f)), NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, scrollVrtxCount*sizeof(Vector3f), &scrollVertices11[0]);
		glEnableVertexAttribArray(posID2);
		glVertexAttribPointer(posID2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindVertexArray(0);
		}
	}
	int diffx = x - globalx;
	int diffy = y - globaly;
	if(diffx>=10 || diffy >=10)
	{
		diffx = 0;
		diffy = 0;
	}
	//cout<<diffx<<' '<<diffy<<endl;
	globalx = x;
	globaly = y;
	yrotation += diffx;
	xrotation += diffy;
	glutPostRedisplay();
}


/* pre:  mouse button has been pressed while within glut window
   post: scene is updated and re-rendered */
static void onMouseButtonPress(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// Left button pressed
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP){
		// Left button un pressed
	}
	/* notify window that it has to be re-rendered */
	glutPostRedisplay();
}


/* pre:  key has been pressed
   post: scene is updated and re-rendered */
static void onAlphaNumericKeyPress(unsigned char key, int x, int y) {
    if( key == 13 )
    {
        // enter key
        iso_value=stof(iso_input[iso_input.size()-1]);
        if(fastMode==0) propagate_contours(iso_value);
        else propagate_contours_fast(iso_value);
	vrtxCount = Vertices.size();
	cout<<"VERTEX COUNT IS:" <<vrtxCount<<endl;
        glDeleteBuffers(1, &VBO);
//	glGenVertexArray(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER,vrtxCount*(sizeof(Vector3f)+sizeof(Vector3f)+sizeof(Vector4f)),NULL,GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vrtxCount*sizeof(Vector3f), &Vertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER, vrtxCount*sizeof(Vector3f), vrtxCount*sizeof(Vector3f), &Normals[0]);
	glBufferSubData(GL_ARRAY_BUFFER, vrtxCount*(2*sizeof(Vector3f)), vrtxCount*sizeof(Vector4f), &Colors[0]);
	glEnableVertexAttribArray(posID);
	glVertexAttribPointer(posID, 3, GL_FLOAT, GL_FALSE, 0, 0);//0'th attrib(position), 3 values, all are float, don't normalize, skip 0 bytes, offset 0 bytes	
	glEnableVertexAttribArray(normalID);
	glVertexAttribPointer(normalID, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vrtxCount*sizeof(Vector3f)));	
	glEnableVertexAttribArray(colorID);
	glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(2*vrtxCount*sizeof(Vector3f)));
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
        iso_input.clear();
        iso_input.resize(1);        
    }
    else if( key == 8 )
    {
        // backspace
        iso_input.back().pop_back();
    }
    else if( key == 32){
    	isAnimating = !isAnimating;
    }
    else if (key==46 || (key>=48 && key<=57))
    {
        // regular text
        iso_input.back().push_back( key );
        
    }
	/* notify window that it has to be re-rendered */
	glutPostRedisplay();
}


/* pre:  arrow or function key has been pressed
   post: scene is updated and re-rendered */
static void onSpecialKeyPress(int key, int x, int y) {
	/* please do not change function of these keys */
	switch (key) {
			/* toggle full screen mode */
		case GLUT_KEY_F1:
			isFullScreen = !isFullScreen;
			if (isFullScreen) {
				theWindowPositionX = glutGet((GLenum) (GLUT_WINDOW_X));
				theWindowPositionY = glutGet((GLenum) (GLUT_WINDOW_Y));
				glutFullScreen();
			} else {
				glutReshapeWindow(theWindowWidth, theWindowHeight);
				glutPositionWindow(theWindowPositionX, theWindowPositionY);
			}
			break;
	}
	/* notify window that it has to be re-rendered */
	glutPostRedisplay();
}


/* pre:  glut window has just been iconified or restored 
   post: if window is visible, animate model, otherwise don't bother */
static void onVisible(int state) {
	if (state == GLUT_VISIBLE) {
		/* tell glut to show model again */
		glutIdleFunc(onIdle);
	} else {
		glutIdleFunc(NULL);
	}
}


static void InitializeGlutCallbacks() {
	/* tell glut how to display model */
	glutDisplayFunc(onDisplay);
	/* tell glutwhat to do when it would otherwise be idle */
	glutIdleFunc(onIdle);
	/* tell glut how to respond to changes in window size */
	glutReshapeFunc(onReshape);
	/* tell glut how to handle changes in window visibility */
	glutVisibilityFunc(onVisible);
	/* tell glut how to handle key presses */
	glutKeyboardFunc(onAlphaNumericKeyPress);
	glutSpecialFunc(onSpecialKeyPress);
	/* tell glut how to handle the mouse */
	glutMotionFunc(onMouseMotion);
	glutMouseFunc(onMouseButtonPress);
}


int main(int argc, char** argv) {
//	cout<<"Enter scalar value(0 to 1 expected): ";
//	cin>>iso_value;
//	cout<<"Displaying object at scalar value "<<iso_value<<endl;
	
	if ((argc != 2 && argc != 3)||(argc == 3 && strcmp(argv[2], "seed_mode")))
	{
        std::cerr << "Usage: " << argv[0] << " <inputfile(.raw part excluded)>" << std::endl;
        std::cerr << "OR\n";
        std::cerr << "Usage: " << argv[0] << " <inputfile(.raw part excluded)> seed_mode" << std::endl;
        return 1;
    }
    
    if(argc == 3) fastMode=0;

    dataFile = argv[1];
    dataFile += ".raw";
    configFile = argv[1];
    configFile += ".config";
    seedFile = argv[1];
    seedFile += ".seed";
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	glutInit(&argc, argv);

	/* request initial window size and position on the screen */
	glutInitWindowSize(theWindowWidth, theWindowHeight);
	glutInitWindowPosition(theWindowPositionX, theWindowPositionY);
	/* request full color with double buffering and depth-based rendering */
	glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH | GLUT_RGBA);
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	/* create window whose title is the name of the executable */
	glutCreateWindow(theProgramTitle);

	InitializeGlutCallbacks();

	// Must be done after glut is initialized!
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	printf("GL version: %s\n", glGetString(GL_VERSION));
	cout<<"Data File: "<<dataFile<<endl;
    cout<<"Config File: "<<configFile<<endl;
    cout<<"Seed File: "<<seedFile<<endl<<endl;
	

	/* initialize model */
	onInit(argc, argv);
	
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	cout << "Preprocessing time = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s], ";
	cout<<std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]\n";
	cout<<"Starting Rendering..."<<endl;

	/* give control over to glut to handle rendering and interaction  */
	glutMainLoop();

	/* program should never get here */
	return 0;
}
