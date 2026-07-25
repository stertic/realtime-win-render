#ifndef UNICODE
#define UNICODE
#endif

#include <iostream>
#include <Windows.h>
#include <windowsx.h>
#include <wingdi.h>
#include <stdarg.h>
#include <WinUser.h>
#include <profileapi.h>

#include <cmath>
#include <vector>

#define nearPlane 1

//LV = location vector
//RV = rotation vector, could be unit vector or not a unit vector
//all vectors are stored in double type
//all angles are done in radians
//for pixel coordinates, higher x = right, higher y = down
//ID starts from 0
//Count starts from 1

//has alpha because DIB function(setDIBtoDevice) only supports 32 bits per pixel(one byte stride)
typedef struct {
	byte r, g, b, a;
} RGBA;

//this struct was created with the intent of only passing around 'pass' struct
//and not the entire scene since indexing passes require the width of window
typedef struct {
	unsigned int w, h;
	RGBA* pass;
} pass;

//created for use in depth buffering
typedef struct {
	unsigned int w, h;
	double* pass;
} fieldDouble;

typedef struct {
	double x, y;
} XY;

typedef struct {
	double u, v, w;
} UV;

typedef struct vect2 {
	double x = 0, y = 0;
} vect2;

struct vect4;

typedef struct vect3 {
	double x = 0, y = 0, z = 0;

	vect3() {
		x = 0;
		y = 0;
		z = 0;
	}
	vect3(const vect4& v);
	vect3(const double& a, const double& b, const double& c) {
		x = a;
		y = b;
		z = c;
	}

	vect3 operator+(const vect3& v) const {
		return { x + v.x, y + v.y, z + v.z };
	}
	vect3 operator-(const vect3& v) const {
		return { x - v.x, y - v.y, z - v.z };
	}
	vect3 operator*(const double& scalar) const {
		return { x * scalar, y * scalar, z * scalar };
	}
	vect3 operator-() const {
		return { -x, -y, -z };
	}
	vect3 operator/(const double& scalar) const {
		return { x / scalar, y / scalar, z / scalar };
	}
	void operator+=(const vect3& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return;
	}
	void operator-=(const vect3& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return;
	}
} vect3;

struct matrix44;

typedef struct vect4 {
	double x = 0, y = 0, z = 0, w = 1;

	vect4() {
		x = 0;
		y = 0;
		z = 0;
		w = 1;
	}
	vect4(const vect3& v) {
		x = v.x;
		y = v.y;
		z = v.z;
		w = 1;
	}
	vect4(const double& a, const double& b, const double& c, const double& d) {
		x = a;
		y = b;
		z = c;
		w = d;
	}

	vect4 operator+(const vect4& v) const {
		return { x + v.x, y + v.y, z + v.z, w + v.w };
	}
	vect4 operator-(const vect4& v) const {
		return { x - v.x, y - v.y, z - v.z, w - v.w };
	}
	vect4 operator*(const matrix44& mat) const;
	//IS NOT A DOT PRODUCT (only scaling)
	vect4 operator*(const double& scalar) const {
		return { x * scalar, y * scalar, z * scalar, w * scalar };
	}
	//IS NOT A DOT PRODUCT (only scaling)
	vect4 operator/(const double& scalar) const {
		return { x / scalar, y / scalar, z / scalar, w / scalar };
	}
	void operator+=(const vect4& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return;
	}
	void operator-=(const vect4& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return;
	}
	void operator*=(const double& scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		w *= scalar;
		return;
	}
	void operator/=(const double& scalar) {
		x /= scalar;
		y /= scalar;
		z /= scalar;
		w /= scalar;
		return;
	}
	void normalize() {
		double len = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
		x /= len;
		y /= len;
		z /= len;
	}
	vect3 truncate() const {
		return vect3(x, y, z);
	}
} vect4;

vect3::vect3(const vect4& v) {
	x = v.x;
	y = v.y;
	z = v.z;
}

//scanthrough order
// 0 1 2 = row 0
// 3 4 5 = row 1
// 6 7 8 = row 2
typedef struct matrix33 {
	double n[9]{};
	matrix33 operator+(const matrix33& m) const {
		return { {n[0] + m.n[0], n[1] + m.n[1], n[2] + m.n[2],
			n[3] + m.n[3], n[4] + m.n[4], n[5] + m.n[5],
			n[6] + m.n[6], n[7] + m.n[7], n[8] + m.n[8] } };
	}
	vect3 operator*(const vect3& v) const {
		return { v.x * n[0] + v.y * n[1] + v.z * n[2],
			v.x * n[3] + v.y * n[4] + v.z * n[5],
			v.x * n[6] + v.y * n[7] + v.z * n[8] };
	}
	double det() const {
		return n[0] * n[4] * n[8]
			+ n[3] * n[7] * n[2]
			+ n[6] * n[1] * n[5]
			- n[2] * n[4] * n[6]
			- n[5] * n[7] * n[0]
			- n[8] * n[1] * n[3];
	}
} matrix33;

//scanthrough order
// 0  1  2  3 = row 0
// 4  5  6  7 = row 1
// 8  9  10 11 = row 2
// 12 13 14 15 = row 3
typedef struct matrix44 {
	double n[16]{};

	matrix44 operator+(const matrix44& m) const {
		return { { n[0] + m.n[0], n[1] + m.n[1], n[2] + m.n[2], n[3] + m.n[3],
			n[4] + m.n[4], n[5] + m.n[5], n[6] + m.n[6], n[7] + m.n[7],
			n[8] + m.n[8], n[9] + m.n[9], n[10] + m.n[10], n[11] + m.n[11],
			n[12] + m.n[12], n[13] + m.n[13], n[14] + m.n[14], n[15] + m.n[15] } };
	}
	matrix44 operator*(const double& a) const {
		return { a * n[0], a * n[1], a * n[2], a * n[3],
			a * n[4], a * n[5], a * n[6], a * n[7],
			a * n[8], a * n[9], a * n[10], a * n[11],
			a * n[12], a * n[13], a * n[14], a * n[15] };
	}
	matrix44 operator*(const matrix44& m) const {
		matrix44 castM44;
		for (int row = 0; row < 4; row++) {
			for (int col = 0; col < 4; col++) {
				castM44.n[row * 4 + col] = n[row * 4] * m.n[col] + n[row * 4 + 1] * m.n[col + 4] + n[row * 4 + 2] * m.n[col + 8] + n[row * 4 + 3] * m.n[col + 12];
			}
		}
		return castM44;
	}
	vect4 operator*(const vect4& v) const {
		return { v.x * n[0] + v.y * n[1] + v.z * n[2] + v.w * n[3],
			v.x * n[4] + v.y * n[5] + v.z * n[6] + v.w * n[7],
			v.x * n[8] + v.y * n[9] + v.z * n[10] + v.w * n[11],
			v.x * n[12] + v.y * n[13] + v.z * n[14] + v.w * n[15] };
	}
	double det() const {
		return n[0] * n[5] * n[10] * n[15]
			+ n[4] * n[9] * n[14] * n[3]
			+ n[8] * n[13] * n[2] * n[7]
			+ n[12] * n[1] * n[6] * n[11]
			- n[3] * n[6] * n[9] * n[12]
			- n[7] * n[10] * n[13] * n[0]
			- n[11] * n[14] * n[1] * n[4]
			- n[15] * n[2] * n[5] * n[8];
	}
	matrix44 inv() {
		double inv[16], det;

		inv[0] =	n[5] * n[10] * n[15]	- n[5] * n[11] * n[14]	- n[9] * n[6] * n[15]	+ n[9] * n[7] * n[14]	+ n[13] * n[6] * n[11]	- n[13] * n[7] * n[10];
		inv[4] =	-n[4] * n[10] * n[15]	+ n[4] * n[11] * n[14]	+ n[8] * n[6] * n[15]	- n[8] * n[7] * n[14]	- n[12] * n[6] * n[11]	+ n[12] * n[7] * n[10];
		inv[8] =	n[4] * n[9] * n[15]		- n[4] * n[11] * n[13]	- n[8] * n[5] * n[15]	+ n[8] * n[7] * n[13]	+ n[12] * n[5] * n[11]	- n[12] * n[7] * n[9];
		inv[12] =	-n[4] * n[9] * n[14]	+ n[4] * n[10] * n[13]	+ n[8] * n[5] * n[14]	- n[8] * n[6] * n[13]	- n[12] * n[5] * n[10]	+ n[12] * n[6] * n[9];

		inv[1] =	-n[1] * n[10] * n[15]	+ n[1] * n[11] * n[14]	+ n[9] * n[2] * n[15]	- n[9] * n[3] * n[14]	- n[13] * n[2] * n[11]	+ n[13] * n[3] * n[10];
		inv[5] =	n[0] * n[10] * n[15]	- n[0] * n[11] * n[14]	- n[8] * n[2] * n[15]	+ n[8] * n[3] * n[14]	+ n[12] * n[2] * n[11]	- n[12] * n[3] * n[10];
		inv[9] =	-n[0] * n[9] * n[15]	+ n[0] * n[11] * n[13]	+ n[8] * n[1] * n[15]	- n[8] * n[3] * n[13]	- n[12] * n[1] * n[11]	+ n[12] * n[3] * n[9];
		inv[13] =	n[0] * n[9] * n[14]		- n[0] * n[10] * n[13]	- n[8] * n[1] * n[14]	+ n[8] * n[2] * n[13]	+ n[12] * n[1] * n[10]	- n[12] * n[2] * n[9];

		inv[2] =	n[1] * n[6] * n[15]		- n[1] * n[7] * n[14]	- n[5] * n[2] * n[15]	+ n[5] * n[3] * n[14]	+ n[13] * n[2] * n[7]	- n[13] * n[3] * n[6];
		inv[6] =	-n[0] * n[6] * n[15]	+ n[0] * n[7] * n[14]	+ n[4] * n[2] * n[15]	- n[4] * n[3] * n[14]	- n[12] * n[2] * n[7]	+ n[12] * n[3] * n[6];
		inv[10] =	n[0] * n[5] * n[15]		- n[0] * n[7] * n[13]	- n[4] * n[1] * n[15]	+ n[4] * n[3] * n[13]	+ n[12] * n[1] * n[7]	- n[12] * n[3] * n[5];
		inv[14] =	-n[0] * n[5] * n[14]	+ n[0] * n[6] * n[13]	+ n[4] * n[1] * n[14]	- n[4] * n[2] * n[13]	- n[12] * n[1] * n[6]	+ n[12] * n[2] * n[5];

		inv[3] =	-n[1] * n[6] * n[11]	+ n[1] * n[7] * n[10]	+ n[5] * n[2] * n[11]	- n[5] * n[3] * n[10]	- n[9] * n[2] * n[7]	+ n[9] * n[3] * n[6];
		inv[7] =	n[0] * n[6] * n[11]		- n[0] * n[7] * n[10]	- n[4] * n[2] * n[11]	+ n[4] * n[3] * n[10]	+ n[8] * n[2] * n[7]	- n[8] * n[3] * n[6];
		inv[11] =	-n[0] * n[5] * n[11]	+ n[0] * n[7] * n[9]	+ n[4] * n[1] * n[11]	- n[4] * n[3] * n[9]	- n[8] * n[1] * n[7]	+ n[8] * n[3] * n[5];
		inv[15] =	n[0] * n[5] * n[10]		- n[0] * n[6] * n[9]	- n[4] * n[1] * n[10]	+ n[4] * n[2] * n[9]	+ n[8] * n[1] * n[6]	- n[8] * n[2] * n[5];

		det = n[0] * inv[0] + n[1] * inv[4] + n[2] * inv[8] + n[3] * inv[12];

		if (det == 0)
			return matrix44{};

		det = 1.0 / det;

		matrix44 invOut;
		for (int i = 0; i < 16; i++)
			invOut.n[i] = inv[i] * det;
		return invOut;
	}

	inline matrix33 ditchW() const {
		return { { n[0], n[1], n[2], n[4], n[5], n[6], n[8], n[9], n[10] } };
	}
	matrix44 T() const {
		matrix44 cast;
		//0  1  2  3  -> 0  4  8  12
		//4  5  6  7  -> 1  5  9  13
		//8  9  10 11 -> 2  6  10 14
		//12 13 14 15 -> 3  7  11 15

		for (int i = 0; i < 16; i++) {
			cast.n[i] = n[i % 4 * 4 + i / 4];
		}
		return cast;
	}
} matrix44;

vect4 vect4::operator*(const matrix44& mat) const {
	return { mat.n[0] * x + mat.n[1] * y + mat.n[2] * z + mat.n[3] * w,
		mat.n[4] * x + mat.n[5] * y + mat.n[6] * z + mat.n[7] * w,
		mat.n[8] * x + mat.n[9] * y + mat.n[10] * z + mat.n[11] * w,
		mat.n[12] * x + mat.n[13] * y + mat.n[14] * z + mat.n[15] * w };
}

typedef double deg;
typedef double rad;

#define n(col, row) col + row * 4

const vect3 zeroVect3 = { 0, 0, 0 }; //x, y, z
const vect4 initVect4 = { 0, 0, 0, 1 }; //x, y, z, w
const vect4 zeroVect4 = { 0, 0, 0, 0 }; //x, y, z, w

constexpr deg DEGTORAD = 0.017453292;
constexpr rad RADTODEG = 57.2957795;
//2 PI = TAU
constexpr rad TAU = 6.2831853071795864;
constexpr rad PI = 3.1415926535897932;

//floating point for subpixel precision, converted to int/LONG right before drawing to screen
typedef struct {
	int x, y;
} screenXY;

//screenspace fragment, aggregate of data required to render a triangle on screen
typedef struct {
	//location vectors(screenspace)
	//needs to be in counterclockwise order
	screenXY a, b, c;

	//depth storing
	double az, bz, cz;
} screenFrag;

//data inside struct 'face' are all indices, NOT LOCATION VECTORS
//tri structs hold NO REAL DATA, they only hold indicies
//negative indices are interpreted into a positive index at parsing function
typedef struct tri {
	//indices start at 1, 0 means unused
	//IDs can be used without subtracting 1 and are global (ex) scene.vPData[scene.obj[0].tris[5].vPID[0]]
	//normals represented with counterclockwise vertex
	//the indices are in order (ex) vert normal for point[i] is normal[i]
	//vertex position ID, vertex texture ID, vertex normal ID, face normal ID
	unsigned int vPID[3], vTID[3], vNID[3], fNID;
} tri;

//object structs hold NO REAL DATA, they only hold indicies for values inside scene struct
//the three 'Data' pointers points to an array each(vertexData, vertNormalData, vertTextureData)
//data inside the data arrays contain actual position vectors
typedef struct object {
	char name[31];
	bool shadeSmooth;
	//counts are counts of data inside individual object
	unsigned int triCount, vPCount, vNCount, vTCount;
	//original vert data, must not be modified when rendering(only modify to modify the mesh permanently)
	tri* tris;

	//worldspace orientation
	vect3 worldP;
	vect3 worldR;
} object;

typedef struct renderObj {
	//tVPData array length is vertCount
	//transformed data
	vect4* tVPData, *tVNData, *tFNData;

	//tVPData, vertCull array lengths are both vertCount
	bool* vCull;

	//VERSION2 CULLING
	/*
	//culled data
	vect4* cVPData;
	unsigned int cVPCount;
	UV* cVTData;
	*/
	tri* triRender;
	unsigned int triRenderCount;
} renderObj;

typedef struct physicsObj {
	vect3 p{}, v{}, a{};
	double m;
	
	physicsObj() {
		stop();
		m = 1;
	}
	physicsObj(const double& mass) {
		stop();
		m = mass;
	}

	void step(const double& dt) {
		v += a * dt;
		p += v * dt;
		return;
	}
	void F(const vect3& force) {
		a += force / m;
		return;
	}
	void stop() {
		p = zeroVect3;
		v = zeroVect3;
		a = zeroVect3;
		return;
	}
} physicsObj;

//assumed symmeteric frustum
typedef struct camera {
	//frustum properties
	//distN is distance from camera's location to the near projection plane
	//distF is distance from camera's location to the far projection plane
	//width is half of the width of the near projection plane
	//height is half of the height of the near projection plane
	double distN{}, distF{}, width{}, height{};
	rad pitch{}, yaw{}, roll{};
	//camera location vector and lookat target vector
	//lookat target vector is calculated with the camera positioned at the origin
	vect4 LVect = initVect4, TVect = initVect4;
	//horizontal field of view
	rad hFOV;

	physicsObj player;

	void update(screenXY resolution, deg fov) {
		hFOV = fov * DEGTORAD;
		distN = nearPlane;
		//distF = 100;
		//CODE WRITTEN ASSUMING INFINITE FAR PLANE
		//viewportMatrix must be modified for finite far plane
		width = tan(hFOV) * distN;
		height = ((double)resolution.y / (double)resolution.x) * width;
	}
} camera;

typedef struct scene {
	camera cam{};
	//screen midpoint is screen coordinate, winmidpoint is window's local coordinate (stored since movecursor uses it all the time)
	screenXY screenRes{}, screenMidPoint{}, winMidPoint{};
	renderObj render{};
	//all counts are sum of counts of all objects of the obj array
	unsigned int objCount{}, vPCount{}, vNCount{}, vTCount{}, fNCount{};
	//an array of object structs
	object* obj{};

	//REAL DATA, MUST NOT BE MODIFIED AFTER PARSE UNLESS PERMANENT TRANSFORM
	//vertex position data, vertex normal data, vertex texture data
	vect4* vPData{}, * vNData{}, * fNData{};
	UV* vTData{};
	//face normal data

	//will write to renderPass and setdibits the bitmap over to provided hdc in once
	//hdc provided to updateOpenBL IS the buffer bitmap
	//depth buffer initialization and freeing are done in parallel with the renderpass
	pass renderPass{};
	BITMAPINFO renderPassDIBINFO{};
	fieldDouble depthBuffer{};

	scene() {
		objCount = 0;
		obj = nullptr;

		render.tVPData = nullptr;
		render.tVNData = nullptr;
		render.tFNData = nullptr;

		cam.hFOV = 0 * DEGTORAD;
		cam.distN = 0;
		cam.width = tan(cam.hFOV) * cam.distN;
		cam.height = ((double)screenRes.y / (double)screenRes.x) * cam.width;

		cam.roll = 0 * DEGTORAD;
		cam.pitch = 0 * DEGTORAD;
		cam.yaw = 0 * DEGTORAD;
		cam.LVect = initVect4;
		cam.TVect.x = -1 * sin(cam.roll) * cos(cam.pitch);
		cam.TVect.y = cos(cam.roll) * cos(cam.pitch);
		cam.TVect.z = sin(cam.pitch);
		cam.TVect.w = 1.0;

		fNCount = 0;
		vTCount = 0;
		vNCount = 0;
		vPCount = 0;
		fNData = nullptr;
		vNData = nullptr;
		vPData = nullptr;
		vTData = nullptr;
	}
} scene;

/////////////////////
///    globals    ///
/////////////////////

scene projScene;

bool quit = false;
bool pause = true;
bool keyboard[256] = { 0 };
//use x y when paused and mouse is free to move for mouse position
//use dx dy for ingame looking around(delta x and y)
struct mouse{
	int x, y;
	int dx, dy;
	uint8_t button;
} mouse;
enum { MOUSEL = 0b1, MOUSER = 0b10, MOUSEM = 0b100, MOUSEX1 = 0b1000, MOUSEX2 = 0b10000 };
long long fps;
deg fov = 60;

#define MAXMSGPERFRAME 10

void initBuffer(HWND hwnd, HDC& bufferHDC, HBITMAP& bufferhBM, scene& inpScene);
void freeBuffer(HDC& bufferHDC, HBITMAP& bufferHBM, scene& inpScene);
void printDebug(HDC hdc, int x, int y, COLORREF col, const wchar_t* format, ...);

double vect4Dist(const vect4& vA, const vect4& vB) {
	return sqrt(pow(vA.x - vB.x, 2) + pow(vA.y - vB.y, 2) + pow(vA.z - vB.z, 2));
}

//a debugging function, IS NOT EFFICIENT
//must be called after clipping
void updateFaceNormalDisplay(scene& inpScene, matrix44& projectionMatrix, matrix44& modelViewMatrix, HDC hdc) {
	HPEN lineTri = CreatePen(PS_SOLID, 1, RGB(10, 200, 200));
	HPEN prevPen = (HPEN)SelectObject(hdc, lineTri);

	vect4 base, end;
	POINT renderPoint;
	tri* tarTri;
	for (unsigned int tarObjID = 0; tarObjID < inpScene.objCount; tarObjID++) {
		for (unsigned int tarTriID = 0; tarTriID < inpScene.obj[tarObjID].triCount; tarTriID++) {
			tarTri = &(inpScene.obj[tarObjID].tris[tarTriID]);
			base = (inpScene.vPData[tarTri->vPID[0]] +
					inpScene.vPData[tarTri->vPID[1]] +
					inpScene.vPData[tarTri->vPID[2]]) / 3;
			end = base + inpScene.fNData[tarTri->fNID];
			end.w = 1;

			base = (base * modelViewMatrix) * projectionMatrix;
			end = (end * modelViewMatrix) * projectionMatrix;

			base /= base.w;
			end /= end.w;

			renderPoint.x = (int)((base.x + 0.5f) * inpScene.screenRes.x);
			renderPoint.y = (int)((base.y + 0.5f) * inpScene.screenRes.y);
			MoveToEx(hdc, renderPoint.x, renderPoint.y, NULL);

			renderPoint.x = (int)((end.x + 0.5f) * inpScene.screenRes.x);
			renderPoint.y = (int)((end.y + 0.5f) * inpScene.screenRes.y);
			LineTo(hdc, renderPoint.x, renderPoint.y);
		}
	}
	SelectObject(hdc, prevPen);
	DeleteObject(lineTri);
	return;
}

//both rasters the silhouette of triangles and performs depth buffering
void drawTrig(pass& writePass, fieldDouble& depthBuffer, const screenFrag& frag) {
	int xmax, ymax, xmin, ymin;

	// each the result of the edge function of one side of the triangle
	int evalAB, evalBC, evalCA, evalABC;

	// coordinate of the pixel to check
	int evalX, evalY;

	// since evalX and evalY are incremented by 1 and since addition is cheaper than multiplication, we
	// add the 'inc___' variables to the results instead of calculating the edge function every time.
	int incABx, incABy, incBCx, incBCy, incCAx, incCAy;

	//the inverse of distance from point on triangle to camera
	double depthBary;

	RGBA fillCol = { 244, 242, 243, 0 };

	//getting the smallest encasing rectangle for the triangle
	if (frag.a.x > frag.b.x) {
		xmax = frag.a.x;
		xmin = frag.b.x;
	}
	else {
		xmax = frag.b.x;
		xmin = frag.a.x;
	}
	if (frag.c.x > xmax)
		xmax = frag.c.x;
	if (frag.c.x < xmin)
		xmin = frag.c.x;

	if (frag.a.y > frag.b.y) {
		ymax = frag.a.y;
		ymin = frag.b.y;
	}
	else {
		ymax = frag.b.y;
		ymin = frag.a.y;
	}
	if (frag.c.y > ymax)
		ymax = frag.c.y;
	if (frag.c.y < ymin)
		ymin = frag.c.y;

	//prevent out of bounds
	if (xmin < 0)
		xmin = 0;
	if (ymin < 0)
		ymin = 0;

	if (xmax >= (int)writePass.w)
		xmax = writePass.w - 1;
	if (ymax >= (int)writePass.h)
		ymax = writePass.h - 1;

	// the edge function only needs to be computed here once.
	evalAB = (xmin - frag.a.x) * (frag.b.y - frag.a.y) - (ymin - frag.a.y - 1) * (frag.b.x - frag.a.x);
	evalBC = (xmin - frag.b.x) * (frag.c.y - frag.b.y) - (ymin - frag.b.y - 1) * (frag.c.x - frag.b.x);
	evalCA = (xmin - frag.c.x) * (frag.a.y - frag.c.y) - (ymin - frag.c.y - 1) * (frag.a.x - frag.c.x);

	//triangle of area 0 are not rendered(see below on why this is ok)
	//positive evalABC means dot product with the camera view vector is positive(pointing in the same direction)
	//positive triangles are also rendered because face normal culling isn't a simple comparison between view vector and face normal.
	//this means correcting sign of evalABC is required.
	evalABC = (frag.c.x - frag.a.x) * (frag.b.y - frag.a.y) - (frag.c.y - frag.a.y) * (frag.b.x - frag.a.x);
	if (!evalABC) {
		return;
	}
	if (evalABC > 0) {
		evalABC = -evalABC;
	}

	incABx = frag.b.x - frag.a.x;
	incABy = frag.b.y - frag.a.y;

	incBCx = frag.c.x - frag.b.x;
	incBCy = frag.c.y - frag.b.y;

	incCAx = frag.a.x - frag.c.x;
	incCAy = frag.a.y - frag.c.y;
	//evalY after for loop is gonna overshoot by 1, so in for loop evalY++/-- is done at start
	evalY = ymin - 1;

	//evalX and y checks are INCLUSIVE FOR BOTH MIN AND MAX ie the edge pixels are rendered
	//this means for manifold meshes not rendering triangles with area of 0(line) is ok since
	//the MINMAX edge of the triangle adjacent to the 'line' triangle will cover the 'line' of said triangle.

	// the for loops are for roving through the pixels continuously.
	for (evalX = xmin; evalX <= xmax; evalX++) {
		//going down through current column
		for (evalY++; evalY <= ymax; evalY++) {
			if (evalAB <= 0 && evalBC <= 0 && evalCA <= 0) {
				//for vertex coloring
				/*fillCol.r = ((double)evalAB / (double)evalABC * 255);
				fillCol.g = ((double)evalBC / (double)evalABC * 255);
				fillCol.b = ((double)evalCA / (double)evalABC * 255);*/

				//depthBary is INVERSE of distance!
				depthBary = 1.0 / (
					(1 / frag.cz) * ((double)evalAB / (double)evalABC) +
					(1 / frag.az) * ((double)evalBC / (double)evalABC) +
					(1 / frag.bz) * ((double)evalCA / (double)evalABC));

				if (depthBary <= depthBuffer.pass[evalX + evalY * depthBuffer.w]) {
					depthBuffer.pass[evalX + evalY * depthBuffer.w] = depthBary;
					fillCol.r = (byte)(255 * depthBary);
					fillCol.g = (byte)(255 * depthBary);
					fillCol.b = (byte)(255 * depthBary);
					writePass.pass[evalX + evalY * writePass.w] = fillCol;
				}
			}
			//for barycentric coordinates:
			//ecalAB = area of triangle A B P(check point) * 2
			//barycentric coordinate = area of subtriangle / area of whole triangle
			evalAB -= incABx;
			evalBC -= incBCx;
			evalCA -= incCAx;
		}
		//inc__x are for reverting y coordinate of value of evaluation function
		//inc__y are for incrementing the x coordinate by 1
		evalAB += incABx + incABy;
		evalBC += incBCx + incBCy;
		evalCA += incCAx + incCAy;
		evalX++;

		if (evalX > xmax)
			break;
		//going up through the current column
		for (evalY--; evalY >= ymin; evalY--) {
			if (evalAB <= 0 && evalBC <= 0 && evalCA <= 0) {
				/*fillCol.r = ((double)evalAB / (double)evalABC * 255);
				fillCol.g = ((double)evalBC / (double)evalABC * 255);
				fillCol.b = ((double)evalCA / (double)evalABC * 255);*/

				depthBary = 1.0 / (
					(1 / frag.cz) * ((double)evalAB / (double)evalABC) +
					(1 / frag.az) * ((double)evalBC / (double)evalABC) +
					(1 / frag.bz) * ((double)evalCA / (double)evalABC));

				if (depthBary <= depthBuffer.pass[evalX + evalY * depthBuffer.w]) {
					depthBuffer.pass[evalX + evalY * depthBuffer.w] = depthBary;
					fillCol.r = (byte)(255 * depthBary);
					fillCol.g = (byte)(255 * depthBary);
					fillCol.b = (byte)(255 * depthBary);
					writePass.pass[evalX + evalY * writePass.w] = fillCol;
				}
			}
			evalAB += incABx;
			evalBC += incBCx;
			evalCA += incCAx;
		}
		//originally -= inc__x and += inc__y
		evalAB -= incABx - incABy;
		evalBC -= incBCx - incBCy;
		evalCA -= incCAx - incCAy;
	}
	return;
}

////////// VERSION2 CULLING
/*
void clipTri(scene& inpScene, tri* tarTri) {
	vect4 *checkPoint[3];
	//clipPoly may not be pointers as intersections do not have preexisting memory locations
	vect4 clipPoly[4];
	unsigned int clipPolyVert;
	double wAbs[3], slide;
	bool vCulled[3]{};
	vect4 intersect;

	//this section needs to construct

	//vect4* cVPData;
	//unsigned int cVPCount;
	//UV* cVTData;
	//tri* triRender;
	//unsigned int triRenderCount;

	checkPoint[0] = &inpScene.render.tVPData[tarTri->vPID[0]];
	wAbs[0] = abs(checkPoint[0]->w);

	checkPoint[1] = &inpScene.render.tVPData[tarTri->vPID[0]];
	wAbs[1] = abs(checkPoint[1]->w);

	checkPoint[2] = &inpScene.render.tVPData[tarTri->vPID[0]];
	wAbs[2] = abs(checkPoint[2]->w);

	clipPolyVert = 3;
	clipPoly[0] = *checkPoint[0];
	clipPoly[1] = *checkPoint[1];
	clipPoly[2] = *checkPoint[2];

	//<near >far, right left, top bottom(>pos, <neg)
	
	//clip -> clip = add none
	//clip -> not clip = add intersect
	//not clip -> clip = add first and intersect
	//not clip -> not clip = add first

	//Z neg cull
	vCulled[0] = checkPoint[0]->z < -wAbs[0];
	vCulled[1] = checkPoint[1]->z < -wAbs[1];
	vCulled[2] = checkPoint[2]->z < -wAbs[2];

	if (vCulled[0] && vCulled[1] && vCulled[2]) {
		//entire trig culled - don't perform dot cull and subsequent culling on other axis
		clipPolyVert = 0;
	}
	else if (!vCulled[0] || !vCulled[1] || !vCulled[2]) {
		clipPolyVert = 0;

		//0 to 1
		if (vCulled[0]) {
			if(!vCulled[1]) {
				//add intersection
				slide = abs(checkPoint[0]->z - wAbs[0]) + abs(checkPoint[1]->z - wAbs[1]);
				slide = abs(checkPoint[0]->z - wAbs[0]) / slide;
				intersect = (*checkPoint[0] * (1 - slide)) + (*checkPoint[1] * slide);
				clipPoly[clipPolyVert] = intersect;
				clipPolyVert += 1;
			}
			//does nothing if both vertices are culled
		}
		else {
			if (vCulled[1]) {
				//add first and intersection
				slide = abs(checkPoint[0]->z - checkPoint[0]->w) + abs(checkPoint[1]->z - checkPoint[1]->w);
				slide = abs(checkPoint[0]->z - checkPoint[0]->w) / slide;
				intersect = (*checkPoint[0] * (1 - slide)) + (*checkPoint[1] * slide);
				clipPoly[clipPolyVert] = *checkPoint[0];
				clipPoly[clipPolyVert + 1] = intersect;
				clipPolyVert += 2;
			}
			else {
				//add both
				clipPoly[clipPolyVert] = *checkPoint[0];
				clipPoly[clipPolyVert + 1] = *checkPoint[1];
				clipPolyVert += 2;
			}
		}
	}
	//leave triangle alone

	//X neg cull
	vCulled[0] = checkPoint[0]->x < -wAbs[0];
	vCulled[1] = checkPoint[1]->x < -wAbs[1];
	vCulled[2] = checkPoint[2]->x < -wAbs[2];

	if (vCulled[0] && vCulled[1] && vCulled[2]) {
		//entire trig culled - don't perform dot cull
		clipPolyVert = 0;
	}
	else if (!vCulled[0] || !vCulled[1] || !vCulled[2]) {
		//perform 01 12 20
	}
	//leave triangle alone

	//X pos cull
	vCulled[0] = checkPoint[0]->x > wAbs[0];
	vCulled[1] = checkPoint[1]->x > wAbs[1];
	vCulled[2] = checkPoint[2]->x > wAbs[2];

	if (vCulled[0] && vCulled[1] && vCulled[2]) {
		//entire trig culled - don't perform dot cull
		clipPolyVert = 0;
	}
	else if (!vCulled[0] || !vCulled[1] || !vCulled[2]) {
		//perform 01 12 20
	}
	//leave triangle alone

	//Y neg cull
	vCulled[0] = checkPoint[0]->y < -wAbs[0];
	vCulled[1] = checkPoint[1]->y < -wAbs[1];
	vCulled[2] = checkPoint[2]->y < -wAbs[2];

	if (vCulled[0] && vCulled[1] && vCulled[2]) {
		//entire trig culled - don't perform dot cull
		clipPolyVert = 0;
	}
	else if (!vCulled[0] || !vCulled[1] || !vCulled[2]) {
		//perform 01 12 20
	}
	//leave triangle alone

	//Y pos cull
	vCulled[0] = checkPoint[0]->y > wAbs[0];
	vCulled[1] = checkPoint[1]->y > wAbs[1];
	vCulled[2] = checkPoint[2]->y > wAbs[2];

	if (vCulled[0] && vCulled[1] && vCulled[2]) {
		//entire trig culled - don't perform dot cull
		clipPolyVert = 0;
	}
	else if (!vCulled[0] || !vCulled[1] || !vCulled[2]) {
		//perform 01 12 20
	}
	//leave triangle alone
	return;
}
*/

//frees memories allocated by parseObj
//cofunction of parseObj
void freeScene(scene& inpScene) {
	for (unsigned int tarObj = 0; tarObj < inpScene.objCount; tarObj++) {
		delete[](inpScene.obj[tarObj]).tris;
	}
	delete[] inpScene.obj;

	delete[] inpScene.vPData;
	delete[] inpScene.vNData;
	delete[] inpScene.vTData;
	delete[] inpScene.fNData;
	return;
}

//must be called after parsing new obj files
//is not correlated to pixels or passes. only needs to be called when the mesh itself changes
//input is the scene since data counts are stored in scenes
void initRenderObj(scene& inpScene) {
	inpScene.render.vCull = new bool[inpScene.vPCount];
	inpScene.render.tVPData = new vect4[inpScene.vPCount];
	inpScene.render.tVNData = new vect4[inpScene.vNCount];
	inpScene.render.tFNData = new vect4[inpScene.fNCount];
	return;
}

//frees coresponding scene's renderObj
//cofunction of initRenderObj
void freeRenderObj(scene& inpScene) {
	renderObj& tarRender = inpScene.render;
	delete[] tarRender.vCull;
	delete[] tarRender.tVPData;
	delete[] tarRender.tVNData;
	delete[] tarRender.tFNData;
}

void m44Transform(const vect4* inpArr, vect4* outArr, const matrix44& mat, unsigned int vertCount);
void m44Transform(vect4* ioArr, const matrix44& mat, const unsigned int vertCount);
void m33Transform(const vect3* inpArr, vect3* outArr, const matrix33& mat, unsigned int vertCount);
void m33Transform(vect3 * ioArr, const matrix33 & mat, const unsigned int vertCount);

inline double vect3Dot(vect3 v1, vect3 v2) {
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}
vect3 vect3Cross(vect3 v1, vect3 v2);
inline double vect4Dot(vect4 v1, vect4 v2) {
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z) + (v1.w * v2.w);
}

int parseObj(const char* filename, scene& inpScene);

//openBLinit must be called beforehand
//called after cam.roll / pitch / yaw is changed externally
int openBLupdate(HDC hdc, scene& inpScene) {
	//cam___ variables are only cast for code conciseness - replace if needed
	double camRoll = inpScene.cam.roll, camPitch = inpScene.cam.pitch, camYaw = inpScene.cam.yaw;
	//rotation matrix(pre-transpose)
	//A, B, C
	//pitch, roll, yaw
	//pitch, yaw, roll
	matrix44 viewRotMatrix = { {
		cos(camRoll) * cos(camYaw) - sin(camRoll) * sin(camPitch) * sin(camYaw) , -sin(camRoll) * cos(camPitch) , cos(camRoll) * sin(camYaw) + sin(camRoll) * sin(camPitch) * cos(camYaw), 0,
		sin(camRoll) * cos(camYaw) + cos(camRoll) * sin(camPitch) * sin(camYaw) , cos(camRoll) * cos(camPitch), sin(camRoll) * sin(camYaw) - cos(camRoll) * sin(camPitch) * cos(camYaw)	, 0,
		-cos(camPitch) * sin(camYaw), sin(camPitch), cos(camPitch) * cos(camYaw), 0,
		0, 0, 0, 1 } };

	inpScene.cam.TVect = viewRotMatrix * vect4{ 0, 0, -1, 1 };
	inpScene.cam.TVect.normalize();

	matrix44 viewTransMatrix = { {
		1, 0, 0, -inpScene.cam.LVect.x,
		0, 1, 0, -inpScene.cam.LVect.y,
		0, 0, 1, -inpScene.cam.LVect.z,
		0, 0, 0, 1 } };

	matrix44 modelViewMatrix = viewRotMatrix * viewTransMatrix;
	matrix44 projectionMatrix = { {
		inpScene.cam.distN / inpScene.cam.width, 0, 0, 0,
		0, inpScene.cam.distN / inpScene.cam.height, 0, 0,
		0, 0, -1, -2 * inpScene.cam.distN,
		0, 0, -1, 0 } };

	//1. modelView transforms to eyespace
	//2. projection transforms to clipspace
	m44Transform(inpScene.vPData, inpScene.render.tVPData, projectionMatrix * modelViewMatrix, inpScene.vPCount);
	//in eye space (modelViewMatrix)
	//in clip space (projectionMatrix)

	//VERSION3 CULLING
	vect4* tarVert;
	double wAbs;
	for (unsigned int tarVertID = 0; tarVertID < inpScene.vPCount; tarVertID++) {
		tarVert = &inpScene.render.tVPData[tarVertID];
		wAbs = abs(tarVert->w);
		if (tarVert->x > wAbs || tarVert->x < -wAbs ||
			tarVert->y > wAbs || tarVert->y < -wAbs ||
			tarVert->z < -wAbs) {
			inpScene.render.vCull[tarVertID] = true;
		}
		else {
			inpScene.render.vCull[tarVertID] = false;
		}
	}

	vect4 cullVect;
	tri* tarTri;

	//compute vertex by inoutoutin
	//facecull if valid vertex exists
	//triangulate

	//this section needs further development

	//VERSION2 CULLING
	/*
	vect4* cVPData;
	unsigned int cVPCount;
	UV* cVTData;
	tri* triRender;
	unsigned int triRenderCount;
	
	inpScene.render.cVPCount = 0;
	inpScene.render.cVPData = new vect4[inpScene.vPCount];
	*/

	/*
	////////// CULLING REVISIONS //////////
	VERSION1 CULLING
	formerly culling was done by first culling individual vertices after the transform into clipspace.
	after that vector required for culling based on face normals was calculated, and then in the loop that
	goes through all faces, first it checked if all vertices were culled and second it performed the facenormal check.
	if any of the three vertices wasn't culled the triangle was to be rendered.

	VERSION2 CULLING
	after some development I eventually tried implementing proper culling of the triangles via Sotherland-Hodgeman algo.
	I failed however after attempting to implement all edge cases manually and eventually abandoned it.
	in this attempt I tried defining a new set of array of triangles whose members were the triangles to be rendered and
	which has gone through clipping. This meant the vertex-wise culling array wasn't needed, so i tried removing the
	associated code entirely.

	VERSION3 CULLING	
	now I am trying to go back to the old culling method.
	this unfortunately means things like 'discontinued culling' is possibly in need and required to be brought back.
	for clarity the versions will be called VERSION1, VERSION2, VERSION3 culling in chronological order.
	version3 is named separately in case there were modifications between ver1 and ver3, but in essence ver3 is
	a recreation(resurrection if you will) of ver1.
	*/

	inpScene.render.triRenderCount = 0;
	//fNCount = count of face = count of face normals
	inpScene.render.triRender = new tri[inpScene.fNCount];
	//if speed issues occur due to excessive resizing, will implement 2^n growth allocation
	for (unsigned int tarObjID = 0; tarObjID < inpScene.objCount; tarObjID++) {
		for (unsigned int tarTriID = 0; tarTriID < inpScene.obj[tarObjID].triCount; tarTriID++) {
			tarTri = &inpScene.obj[tarObjID].tris[tarTriID];
			cullVect = inpScene.vPData[tarTri->vPID[0]] - inpScene.cam.LVect;

			//REMAINS OF VERSION2 CULLING
			//if (vect3Dot(cullVect, inpScene.fNData[tarTri->fNID]) <= 0) {
			//	the commented line under was supposed to be the starting point of culling code
			//	inpScene.render.triRender[tarTri->fNID]
			//	inpScene.render.triRenderCount++;
			//}
			
			//VERSION3 CULLING
			if (!inpScene.render.vCull[tarTri->vPID[0]] ||
				!inpScene.render.vCull[tarTri->vPID[1]] ||
				!inpScene.render.vCull[tarTri->vPID[2]]) {
				if (vect3Dot(cullVect, inpScene.fNData[tarTri->fNID]) <= 0) {
					inpScene.render.triRender[inpScene.render.triRenderCount] = *tarTri;
					inpScene.render.triRenderCount++;
				}
			}
		}
	}

	//w is set to 1 for viewport matrix transform
	for (unsigned int tarVert = 0; tarVert < inpScene.vPCount; tarVert++) {
		inpScene.render.tVPData[tarVert].x /= inpScene.render.tVPData[tarVert].w;
		inpScene.render.tVPData[tarVert].y /= inpScene.render.tVPData[tarVert].w;
		inpScene.render.tVPData[tarVert].z /= inpScene.render.tVPData[tarVert].w;
		inpScene.render.tVPData[tarVert].w = 1;
	}
	//in NDC space

	//near and far are set to 0, 1(for depth testing)
	//near, far planes are in NDC coordinates(-1 ~ 1)
	//in camspace coordinates near plane is distN, far plane is distF
	matrix44 viewportMatrix = { {
		(double)inpScene.screenRes.x / 2, 0, 0, (double)inpScene.screenRes.x / 2,
		0, (double)inpScene.screenRes.y / 2, 0, (double)inpScene.screenRes.y / 2,
		0, 0, 0.5, 0.5, 
		0, 0, 0, 1 } };

	m44Transform(inpScene.render.tVPData, viewportMatrix, inpScene.vPCount);
	//in window space

	memset(inpScene.renderPass.pass, 10, inpScene.renderPassDIBINFO.bmiHeader.biSizeImage);
	memset(inpScene.depthBuffer.pass, 0b0111'1111, sizeof(double) * inpScene.depthBuffer.w * inpScene.depthBuffer.h);
	screenFrag renderFag;
	for (unsigned int tarTriRender = 0; tarTriRender < inpScene.render.triRenderCount; tarTriRender++) {
		tarTri = &inpScene.render.triRender[tarTriRender];
		renderFag.a.x = (int)inpScene.render.tVPData[tarTri->vPID[0]].x;
		renderFag.a.y = (int)inpScene.render.tVPData[tarTri->vPID[0]].y;

		renderFag.b.x = (int)inpScene.render.tVPData[tarTri->vPID[1]].x;
		renderFag.b.y = (int)inpScene.render.tVPData[tarTri->vPID[1]].y;

		renderFag.c.x = (int)inpScene.render.tVPData[tarTri->vPID[2]].x;
		renderFag.c.y = (int)inpScene.render.tVPData[tarTri->vPID[2]].y;

		renderFag.az = inpScene.render.tVPData[tarTri->vPID[0]].z;
		renderFag.bz = inpScene.render.tVPData[tarTri->vPID[1]].z;
		renderFag.cz = inpScene.render.tVPData[tarTri->vPID[2]].z;

		drawTrig(inpScene.renderPass, inpScene.depthBuffer, renderFag);
	}

	//updateFaceNormalDisplay(inpScene, projectionMatrix, modelViewMatrix, hdc);

	SetDIBitsToDevice(hdc, 0, 0, inpScene.screenRes.x, inpScene.screenRes.y, 0, 0, 0, inpScene.screenRes.y, inpScene.renderPass.pass, &inpScene.renderPassDIBINFO, DIB_RGB_COLORS);
	
	/*
	//for debugging each face
	for (unsigned int tarTriRender = 0; tarTriRender < inpScene.render.triRenderCount; tarTriRender++) {
		tarTri = &inpScene.render.triRender[tarTriRender];
		renderFag.a.x = (int)inpScene.render.tVPData[tarTri->vPID[0]].x;
		renderFag.a.y = (int)inpScene.render.tVPData[tarTri->vPID[0]].y;

		renderFag.b.x = (int)inpScene.render.tVPData[tarTri->vPID[1]].x;
		renderFag.b.y = (int)inpScene.render.tVPData[tarTri->vPID[1]].y;

		renderFag.c.x = (int)inpScene.render.tVPData[tarTri->vPID[2]].x;
		renderFag.c.y = (int)inpScene.render.tVPData[tarTri->vPID[2]].y;

		renderFag.az = inpScene.render.tVPData[tarTri->vPID[0]].z;
		renderFag.bz = inpScene.render.tVPData[tarTri->vPID[1]].z;
		renderFag.cz = inpScene.render.tVPData[tarTri->vPID[2]].z;

		printDebug(hdc, renderFag.a.x, renderFag.a.y, RGB(255, 0, 0), L"%.2f", renderFag.az);
		printDebug(hdc, renderFag.b.x, renderFag.b.y, RGB(255, 0, 0), L"%.2f", renderFag.bz);
		printDebug(hdc, renderFag.c.x, renderFag.c.y, RGB(255, 0, 0), L"%.2f", renderFag.cz);
	}
	*/
	/*
	//for debugging each vertex
	for (unsigned int tarVertRender = 0; tarVertRender < inpScene.vPCount; tarVertRender++) {
		if (!inpScene.render.vCull[tarVertRender]) {
			printDebug(hdc, (int)inpScene.render.tVPData[tarVertRender].x, (int)inpScene.render.tVPData[tarVertRender].y, RGB(255, 0, 0), L"%.2f", inpScene.render.tVPData[tarVertRender].z);
		}
	}
	*/
	return 0;
}

LRESULT CALLBACK winProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow) {
	WNDCLASS wc = { };
	wc.lpfnWndProc = winProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"graphics main";
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(
		0, //optional window style
		L"graphics main", //window class name
		L"openbl", //window display text
		WS_OVERLAPPEDWINDOW, //window style
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, // position
		NULL, //parent window
		NULL, //menu
		hInstance, //instance handle
		NULL //additional data
	);
	if (hwnd == NULL) {
		return -1;
	}

	LARGE_INTEGER precisionClockFreq;
	LARGE_INTEGER precisionCurTime, precisionPrevTime;
	LONGLONG precisionFrameTime;

	QueryPerformanceFrequency(&precisionClockFreq);
	QueryPerformanceCounter(&precisionPrevTime);

	ShowWindow(hwnd, nCmdShow);

	MSG msg = { 0 };
	int msgPerFrame;
	vect3 wishVect;
	long double dt = 0.001, accum = 0;
	projScene.cam.player = physicsObj(1.0);
	double speed;
	physicsObj& player = projScene.cam.player;

	while (!quit) {
		msgPerFrame = 0;
		while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)) {
			DispatchMessage(&msg);
			msgPerFrame++;
		}

		QueryPerformanceCounter(&precisionCurTime);
		precisionFrameTime = precisionCurTime.QuadPart - precisionPrevTime.QuadPart;
		precisionPrevTime = precisionCurTime;
		fps = precisionClockFreq.QuadPart / precisionFrameTime;

		accum += (long double)precisionFrameTime / precisionClockFreq.QuadPart;

		while (accum >= dt) {
			//0.1 per second
			if (!pause) {
				if (keyboard['W']) {
					wishVect = { -sin(projScene.cam.yaw), 0, cos(projScene.cam.yaw) };
					player.v -= wishVect * speed;
				}
				if (keyboard['S']) {
					wishVect = { -sin(projScene.cam.yaw), 0, cos(projScene.cam.yaw) };
					player.v += wishVect * speed;
				}
				if (keyboard['A']) {
					wishVect = { cos(projScene.cam.yaw), 0, sin(projScene.cam.yaw) };
					player.v -= wishVect * speed;
				}
				if (keyboard['D']) {
					wishVect = { cos(projScene.cam.yaw), 0, sin(projScene.cam.yaw) };
					player.v += wishVect * speed;
				}
				if (keyboard['O']) {
					projScene.cam.player.p = zeroVect3;
				}
				if (keyboard[VK_CONTROL])
					speed = 2;
				else
					speed = 0.4;

				if (keyboard[VK_SPACE]) {
					player.v = vect3(0, -10, 0);
				}
				if (keyboard[VK_SHIFT]) {
					player.v = vect3(0, 10, 0);
				}
			}

			projScene.cam.player.v -= projScene.cam.player.v / 10;
			projScene.cam.player.step(dt);

			projScene.cam.LVect = projScene.cam.player.p;
			accum -= dt;
		}

		if (mouse.dx != 0) {
			projScene.cam.yaw += ((double)mouse.dx / (double)projScene.screenRes.x) * PI;
			mouse.dx = 0;
		}
		if (mouse.dy != 0) {
			projScene.cam.pitch -= (((double)mouse.dy / (double)projScene.screenRes.y)) * PI;
			mouse.dy = 0;
		}

		InvalidateRect(hwnd, NULL, FALSE);
	}
	return 0;
}

LRESULT CALLBACK winProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static HDC bufferHDC;
	static HBITMAP bufferHBM;

	static bool hasFocus = true;

	switch (uMsg) {
	case WM_CREATE: {
		ShowCursor(TRUE);
		initBuffer(hwnd, bufferHDC, bufferHBM, projScene);
		parseObj("Resource\\input.obj", projScene);
		initRenderObj(projScene);
		break;
	}
	case WM_MOVE:
	case WM_SIZE: {
		freeBuffer(bufferHDC, bufferHBM, projScene);
		initBuffer(hwnd, bufferHDC, bufferHBM, projScene);
		break;
	}
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		RECT clientRect;
		GetClientRect(hwnd, &clientRect);
		int width = clientRect.right - clientRect.left,
			height = clientRect.bottom - clientRect.top;

		openBLupdate(bufferHDC, projScene);

		if (pause) {
			printDebug(bufferHDC, 10, 10, RGB(150, 150, 150), L"PAUSED");
			printDebug(bufferHDC, 10, 25, RGB(150, 150, 150), L"fps: %lld, pos: [ %lf %lf %lf ]", fps, projScene.cam.player.p.x, projScene.cam.player.p.y, projScene.cam.player.p.z);
			printDebug(bufferHDC, 10, 40, RGB(150, 150, 150), L"tri count: %d, res: %d x %d, fov: %lf", projScene.render.triRenderCount, projScene.screenRes.x, projScene.screenRes.y, fov);

			printDebug(bufferHDC, -10, 10, RGB(150, 150, 150), L"move : [WASD]");
			printDebug(bufferHDC, -10, 25, RGB(150, 150, 150), L"sprint : [ctrl]");
			printDebug(bufferHDC, -10, 40, RGB(150, 150, 150), L"up : [space]");
			printDebug(bufferHDC, -10, 55, RGB(150, 150, 150), L"down : [shift]");
			printDebug(bufferHDC, -10, 70, RGB(150, 150, 150), L"pause : [P]");
			printDebug(bufferHDC, -10, 85, RGB(150, 150, 150), L"reset position : [O]");
			printDebug(bufferHDC, -10, 100, RGB(150, 150, 150), L"exit : [esc]");
			
			printDebug(bufferHDC, -10, 100, RGB(150, 150, 150), L"fov +/-/reset : [j]/[k]/[l]");
		}
		else {

			printDebug(bufferHDC, 10, 10, RGB(150, 150, 150), L"fps: %lld", fps);
		}

		BitBlt(hdc, 0, 0, width, height, bufferHDC, 0, 0, SRCCOPY);
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_KILLFOCUS:
		hasFocus = false;
		memset(keyboard, 0, sizeof(keyboard));
		mouse.button = 0;
		break;
	case WM_SETFOCUS :
		hasFocus = true;
		break;
	case WM_SYSKEYDOWN :
	case WM_SYSKEYUP :
	case WM_KEYDOWN :
	case WM_KEYUP:
		if (hasFocus) {
			bool keyDownCur, keyDownPrev;
			keyDownPrev = ((lParam & ((uint32_t)1 << 30)) != 0);
			keyDownCur = ((lParam & ((uint32_t)1 << 31)) == 0);
			if (keyDownPrev != keyDownCur) {
				keyboard[(uint8_t)wParam] = keyDownCur;
				//instant key processing
				if (keyDownCur) {
					switch (wParam) {
					case VK_ESCAPE :
						quit = true;
						break;
					case 'P' :
						if (pause) {
							pause = false;
						}
						else {
							pause = true;
						}
						break;
					case 'J' :
						fov++;
						freeBuffer(bufferHDC, bufferHBM, projScene);
						initBuffer(hwnd, bufferHDC, bufferHBM, projScene);
						break;
					case 'K' :
						fov--;
						freeBuffer(bufferHDC, bufferHBM, projScene);
						initBuffer(hwnd, bufferHDC, bufferHBM, projScene);
						break;
					case 'L' :
						fov = 60;
						freeBuffer(bufferHDC, bufferHBM, projScene);
						initBuffer(hwnd, bufferHDC, bufferHBM, projScene);
						break;
					}
				}
			}
		}
		break;

	case WM_MOUSEMOVE :
		if (pause) {
			mouse.x = GET_X_LPARAM(lParam);
			mouse.y = GET_Y_LPARAM(lParam);
		}
		else {
			mouse.dx = GET_X_LPARAM(lParam) - projScene.winMidPoint.x;
			mouse.dy = GET_Y_LPARAM(lParam) - projScene.winMidPoint.y;
			if (!pause) {
				SetCursorPos(projScene.screenMidPoint.x, projScene.screenMidPoint.y);
			}
		}
		break;

	case WM_LBUTTONDOWN: mouse.button |= MOUSEL; break;
	case WM_LBUTTONUP: mouse.button &= ~MOUSEL; break;
	case WM_MBUTTONDOWN: mouse.button |= MOUSEM; break;
	case WM_MBUTTONUP: mouse.button &= ~MOUSEM; break;
	case WM_RBUTTONDOWN: mouse.button |= MOUSER; break;
	case WM_RBUTTONUP: mouse.button &= ~MOUSER; break;

	case WM_XBUTTONDOWN :
		if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
			mouse.button |= MOUSEX1;
		else
			mouse.button |= MOUSEX2;
		break;

	case WM_XBUTTONUP :
		if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
			mouse.button &= ~MOUSEX1;
		else
			mouse.button &= ~MOUSEX2;
		break;
	// did not implement WM_MOUSEWHEEL(scrolling)

	case WM_DESTROY:
		freeBuffer(bufferHDC, bufferHBM, projScene);
		freeRenderObj(projScene);
		freeScene(projScene);
		quit = true;
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
		break;
	}
	return 0;
}

//10, 10 for x and y are default
//negative x or y means offset from right and bottom respectively
void printDebug(HDC hdc, int x, int y, COLORREF col, const wchar_t* format, ...) {
	va_list args;
	va_start(args, format);

	BITMAP structBitmapHeader;
	memset(&structBitmapHeader, 0, sizeof(BITMAP));
	HGDIOBJ hBitmap = GetCurrentObject(hdc, OBJ_BITMAP);
	GetObject(hBitmap, sizeof(BITMAP), &structBitmapHeader);

	int textPosX, textPosY;

	// bmWidth - (-x) = bmWidth + x
	if (x < 0)
		textPosX = structBitmapHeader.bmWidth + x;
	else
		textPosX = x;

	if (y < 0)
		textPosY = structBitmapHeader.bmHeight + y;
	else
		textPosY = y;

	RECT textBound = {
			textPosX,	//left
			textPosY,	//top
			textPosX,	//right
			textPosY,	//bottom
	};
	int prevBkMode = SetBkMode(hdc, TRANSPARENT), bufferSize = 128;
	COLORREF prevTextCol = SetTextColor(hdc, col);

	wchar_t* printBuffer = new wchar_t[bufferSize];
	vswprintf_s(printBuffer, bufferSize, format, args);
	if (x >= 0 && y >= 0) {
		//top left
		DrawText(hdc, printBuffer, -1, &textBound, DT_LEFT | DT_TOP | DT_NOCLIP);
	}
	else if (x < 0 && y >= 0) {
		//top right
		DrawText(hdc, printBuffer, -1, &textBound, DT_RIGHT | DT_TOP | DT_NOCLIP);
	}
	else if (x >= 0 && y < 0) {
		//bottom left
		DrawText(hdc, printBuffer, -1, &textBound, DT_LEFT | DT_BOTTOM | DT_NOCLIP);
	}
	else {
		//bottom right
		DrawText(hdc, printBuffer, -1, &textBound, DT_RIGHT | DT_BOTTOM | DT_NOCLIP);
	}
	delete[] printBuffer;

	va_end(args);
	SetBkMode(hdc, prevBkMode);
	SetTextColor(hdc, prevTextCol);
	return;
}

//must alse be called when window or rendering area is resized
//for initialization call once, for resize call freeBuffer and call this
void initBuffer(HWND hwnd, HDC& bufferHDC, HBITMAP& bufferHBM, scene& inpScene) {
	RECT clientRect;
	GetClientRect(hwnd, &clientRect);
	//client rectangle is window coordinates, so left and top are always 0
	//right and bottoms are width and heights
	int w = clientRect.right, h = clientRect.bottom;

	HDC hdc = GetDC(hwnd);
	bufferHDC = CreateCompatibleDC(hdc);
	bufferHBM = CreateCompatibleBitmap(hdc, w, h);
	SelectObject(bufferHDC, bufferHBM);
	ReleaseDC(hwnd, hdc);

	inpScene.renderPass.w = w;
	inpScene.renderPass.h = h;
	inpScene.depthBuffer.w = w;
	inpScene.depthBuffer.h = h;
	inpScene.screenRes.x = w;
	inpScene.screenRes.y = h;
	inpScene.winMidPoint.x = w / 2;
	inpScene.winMidPoint.y = h / 2;
	POINT cast = { inpScene.winMidPoint.x, inpScene.winMidPoint.y };
	ClientToScreen(hwnd, &cast);
	inpScene.screenMidPoint.x = cast.x;
	inpScene.screenMidPoint.y = cast.y;
	inpScene.cam.update(screenXY{ w, h }, fov);

	int pixelCount = w * h;
	//if nonzero
	if (pixelCount) {
		inpScene.renderPass.pass = new RGBA[pixelCount];
		inpScene.depthBuffer.pass = new double[pixelCount];
	}

	inpScene.renderPassDIBINFO.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	GetDIBits(bufferHDC, bufferHBM, 0, h, NULL, (LPBITMAPINFO)&inpScene.renderPassDIBINFO, DIB_RGB_COLORS);

	BITMAPINFOHEADER& sceneBMINFO = inpScene.renderPassDIBINFO.bmiHeader;

	sceneBMINFO.biWidth = w;
	sceneBMINFO.biHeight = -h; //top-down
	sceneBMINFO.biBitCount = 32;
	sceneBMINFO.biCompression = BI_RGB;
	sceneBMINFO.biSizeImage = pixelCount * 4;
	sceneBMINFO.biClrUsed = false;
	sceneBMINFO.biPlanes = 1;
	return;
}

void freeBuffer(HDC& bufferHDC, HBITMAP& bufferHBM, scene& inpScene) {
	DeleteObject(bufferHBM);
	DeleteDC(bufferHDC);
	//if nonzero
	if (inpScene.renderPassDIBINFO.bmiHeader.biSizeImage) {
		delete[] inpScene.renderPass.pass;
		delete[] inpScene.depthBuffer.pass;
	}
	return;
}

//transforms a chunk of vertex location data array with linear transform(matrix multiplication) and stores it into outArr vertex array chunk
void m44Transform(const vect4* inpArr, vect4* outArr, const matrix44& mat, const unsigned int vertCount) {
	const vect4* tarDat;
	for (unsigned int tarVert = 0; tarVert < vertCount; tarVert++) {
		tarDat = &inpArr[tarVert];
		outArr[tarVert].x = mat.n[0] * tarDat->x + mat.n[1] * tarDat->y + mat.n[2] * tarDat->z + mat.n[3] * tarDat->w;
		outArr[tarVert].y = mat.n[4] * tarDat->x + mat.n[5] * tarDat->y + mat.n[6] * tarDat->z + mat.n[7] * tarDat->w;
		outArr[tarVert].z = mat.n[8] * tarDat->x + mat.n[9] * tarDat->y + mat.n[10] * tarDat->z + mat.n[11] * tarDat->w;
		outArr[tarVert].w = mat.n[12] * tarDat->x + mat.n[13] * tarDat->y + mat.n[14] * tarDat->z + mat.n[15] * tarDat->w;
	}
	return;
}

//applies transformation directly
void m44Transform(vect4* ioArr, const matrix44& mat, const unsigned int vertCount) {
	vect4* tarDat;
	vect4 castVect4;
	for (unsigned int tarVert = 0; tarVert < vertCount; tarVert++) {
		tarDat = &ioArr[tarVert];
		castVect4.x = mat.n[0] * tarDat->x + mat.n[1] * tarDat->y + mat.n[2] * tarDat->z + mat.n[3] * tarDat->w;
		castVect4.y = mat.n[4] * tarDat->x + mat.n[5] * tarDat->y + mat.n[6] * tarDat->z + mat.n[7] * tarDat->w;
		castVect4.z = mat.n[8] * tarDat->x + mat.n[9] * tarDat->y + mat.n[10] * tarDat->z + mat.n[11] * tarDat->w;
		castVect4.w = mat.n[12] * tarDat->x + mat.n[13] * tarDat->y + mat.n[14] * tarDat->z + mat.n[15] * tarDat->w;
		*tarDat = castVect4;
	}
	return;
}

void m33Transform(const vect3* inpArr, vect3* outArr, const matrix33& mat, const unsigned int vertCount) {
	const vect3* tarDat;
	for (unsigned int tarVert = 0; tarVert < vertCount; tarVert++) {
		tarDat = &inpArr[tarVert];
		outArr[tarVert].x = mat.n[0] * tarDat->x + mat.n[1] * tarDat->y + mat.n[2] * tarDat->z;
		outArr[tarVert].y = mat.n[3] * tarDat->x + mat.n[4] * tarDat->y + mat.n[5] * tarDat->z;
		outArr[tarVert].z = mat.n[6] * tarDat->x + mat.n[7] * tarDat->y + mat.n[8] * tarDat->z;
	}
}

//applies transformation directly
void m33Transform(vect3* ioArr, const matrix33& mat, const unsigned int vertCount) {
	vect3* tarDat;
	vect3 castVect3;
	for (unsigned int tarVert = 0; tarVert < vertCount; tarVert++) {
		tarDat = &ioArr[tarVert];
		castVect3.x = mat.n[0] * tarDat->x + mat.n[1] * tarDat->y + mat.n[2] * tarDat->z;
		castVect3.y = mat.n[3] * tarDat->x + mat.n[4] * tarDat->y + mat.n[5] * tarDat->z;
		castVect3.z = mat.n[6] * tarDat->x + mat.n[7] * tarDat->y + mat.n[8] * tarDat->z;
		*tarDat = castVect3;
	}
	return;
}

vect3 vect3Cross(vect3 v1, vect3 v2) {
	vect3 res;
	res.x = v1.y * v2.z - v1.z * v2.y;
	res.y = v1.z * v2.x - v1.x * v2.z;
	res.z = v1.x * v2.y - v1.y - v2.x;
	return res;
}

//takes the root of an object array and reallocates it to concatanate new objects
//count of newly parsed object is added to objCount
//the int return is only for error states
//indices are global in wavefront formatting, but are parsed to be local to the object
int parseObj(const char* filename, scene& inpScene) {
	FILE* inputObj;
	fopen_s(&inputObj, filename, "rt");
	if (inputObj == NULL) {
		printf("failed to open .obj file(first read)\n");
		return -1;
	}

	//assuming triangulated mesh
	void* heapMemPtr;
	int castIntToBool, argCount;
	const unsigned int initialObjCount = inpScene.objCount;
	char inputBuffer[256], indexBuffer[32];
	vect3 castVect3 = { };
	vect4 castVect4 = initVect4;
	UV castUV = { };
	object* currentObj = &inpScene.obj[initialObjCount];

	fgets(inputBuffer, 256, inputObj);
	while (feof(inputObj) == 0) {
		switch (inputBuffer[0]) {
		case 'v':
			if (inputBuffer[1] == 'n') {
				currentObj->vNCount++;
			}
			else if (inputBuffer[1] == 't') {
				currentObj->vTCount++;
			}
			else {
				currentObj->vPCount++;
			}
			break;
		case 'f':
			currentObj->triCount++;
			break;
		case 'o':
			//object
			inpScene.objCount++;
			heapMemPtr = realloc(inpScene.obj, sizeof(object) * (inpScene.objCount));
			if (heapMemPtr == NULL) {
				printf("error while allocating memory for sceneObjects\n");
				fclose(inputObj);
				return -1;
			}
			inpScene.obj = (object*)heapMemPtr;
			currentObj = &inpScene.obj[inpScene.objCount - 1];
			currentObj->shadeSmooth = false;
			strcpy_s(currentObj->name, inputBuffer);
			currentObj->triCount = 0;
			currentObj->vPCount = 0;
			currentObj->vNCount = 0;
			currentObj->vTCount = 0;
			currentObj->tris = NULL;
			break;
		case 's':
			//shade smooth
			argCount = sscanf_s(inputBuffer, "s %d", &castIntToBool);
			if (argCount != 1) {
				printf("error while reading shade smooth options from provided obj file. the file may be damaged or corrupted.\n");
				fclose(inputObj);
				return -1;
			}
			currentObj->shadeSmooth = castIntToBool;
			break;
		}
		fgets(inputBuffer, 256, inputObj);
	}
	fclose(inputObj);

	for (unsigned int tarObj = initialObjCount; tarObj < inpScene.objCount; tarObj++) {
		inpScene.vPCount += inpScene.obj[tarObj].vPCount;
		inpScene.vNCount += inpScene.obj[tarObj].vNCount;
		inpScene.vTCount += inpScene.obj[tarObj].vTCount;
		inpScene.fNCount += inpScene.obj[tarObj].triCount;
		inpScene.obj[tarObj].tris = (tri*)malloc(sizeof(tri) * inpScene.obj[tarObj].triCount);
	}

	inpScene.vPData = (vect4*)malloc(inpScene.vPCount * sizeof(vect4));
	inpScene.vNData = (vect4*)malloc(inpScene.vNCount * sizeof(vect4));
	inpScene.vTData = (UV*)malloc(inpScene.vTCount * sizeof(UV));
	inpScene.fNData = (vect4*)malloc(inpScene.fNCount * sizeof(vect4));

	fopen_s(&inputObj, filename, "rt");
	if (inputObj == NULL) {
		printf("failed to open .obj file(second read)\n");
		return -1;
	}

	int indexType, indexBufferCursor, inpBufferCursor, vertAttribute[3];
	int triNum = 0, triVertNum = 0;
	unsigned int objNum = initialObjCount, vPNum = 0, vNNum = 0, vTNum = 0, fNNum = 0;
	currentObj = &inpScene.obj[initialObjCount];
	tri* currentTri = currentObj->tris;

	fgets(inputBuffer, 256, inputObj);
	while (feof(inputObj) == 0) {
		switch (inputBuffer[0]) {
		case 'v':
			if (inputBuffer[1] == 'n') {
				//vertex normal
				argCount = sscanf_s(inputBuffer, "vn %lf %lf %lf", &castVect3.x, &castVect3.y, &castVect3.z);
				if (argCount != 3) {
					printf("error while reading vertex normals from provided obj file. the file may be damaged or corrupted.\n");
					fclose(inputObj);
					return -1;
				}
				inpScene.vNData[vNNum] = castVect3;
				vNNum++;
			}
			else if (inputBuffer[1] == 't') {
				//vertex texture coordinates
				argCount = sscanf_s(inputBuffer, "vt %lf %lf %lf", &castUV.u, &castUV.v, &castUV.w);
				if (argCount == 1) {
					castUV.v = 0;
					castUV.w = 0;
				}
				else if (argCount == 2) {
					castUV.w = 0;
				}
				else if (argCount != 3) {
					printf("error while reading vertex texture coordinates from provided obj file. the file may be damaged or corrupted.\n");
					fclose(inputObj);
					return -1;
				}
				inpScene.vTData[vTNum] = castUV;
				vTNum++;
			}
			else {
				//vertex
				argCount = sscanf_s(inputBuffer, "v %lf %lf %lf", &castVect4.x, &castVect4.y, &castVect4.z);
				if (argCount != 3) {
					printf("error while reading vertex coordinates from provided obj file. the file may be damaged or corrupted.\n");
					fclose(inputObj);
					return -1;
				}
				inpScene.vPData[vPNum] = castVect4;
				vPNum++;
			}
			break;
		case 'f':
			//face
			//initializations
			currentTri = &(currentObj->tris[triNum]);
			inpBufferCursor = 1;
			indexType = 0;
			while (inputBuffer[inpBufferCursor] == ' ')
				inpBufferCursor++;
			//if number wasn't read, represents it unused by leaving the values 0
			vertAttribute[0] = 0;
			vertAttribute[1] = 0;
			vertAttribute[2] = 0;
			triVertNum = 0;
			//start of proper parsing of current line
			while (inputBuffer[inpBufferCursor] != '\0') {
				//check if cursor is at dividing character
				if (inputBuffer[inpBufferCursor] == '/') {
					indexType++;
					inpBufferCursor++;
				}
				//check if cursor is at a number
				else if (inputBuffer[inpBufferCursor] <= 57 && inputBuffer[inpBufferCursor] >= 48) {
					//manual integer read to buffer, and then read with sscanf for stability
					memset(indexBuffer, 0, sizeof(indexBuffer));
					indexBufferCursor = 0;
					while (inputBuffer[inpBufferCursor] <= 57 && inputBuffer[inpBufferCursor] >= 48) {
						indexBuffer[indexBufferCursor] = inputBuffer[inpBufferCursor];
						indexBufferCursor++;
						inpBufferCursor++;
					}
					argCount = sscanf_s(indexBuffer, "%d", &vertAttribute[indexType]);
					if (argCount != 1) {
						printf("erroneous data in index buffer while parsing obj file. the file may be damaged or corrupted.\n");
						fclose(inputObj);
						return -1;
					}
				}
				//assuming space or whitespace character
				else {
					currentTri->vPID[triVertNum] = vertAttribute[0] - 1;
					currentTri->vTID[triVertNum] = vertAttribute[1] - 1;
					currentTri->vNID[triVertNum] = vertAttribute[2] - 1;
					indexType = 0;
					triVertNum++;
					inpBufferCursor++;
					while (inputBuffer[inpBufferCursor] == ' ')
						inpBufferCursor++;
				}
			}
			//wavefront faces are in order of location - texture - normals
			currentTri->fNID = fNNum;
			inpScene.fNData[fNNum] = inpScene.vNData[currentTri->vNID[0]] + inpScene.vNData[currentTri->vNID[1]] + inpScene.vNData[currentTri->vNID[2]];
			inpScene.fNData[fNNum] = inpScene.fNData[fNNum] * 0.333;
			fNNum++;
			triNum++;
			break;
		case 'o':
			currentObj = &inpScene.obj[objNum];
			objNum++;
			triNum = 0;
			break;
		}
		fgets(inputBuffer, 256, inputObj);
	}
	fclose(inputObj);
	return 0;
}