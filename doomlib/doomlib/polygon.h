#pragma once
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cassert>

/* POLYGON.H: Header file for polygon-filling code, also includes
a number of useful items for 3-D animation. */

#define MAX_OBJECTS  100   /* max simultaneous # objects supported */
#define MAX_POLY_LENGTH 4  /* four vertices is the max per poly */
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

#define NUM_CUBE_VERTS 8              /* # of vertices per cube */
#define NUM_CUBE_FACES 6              /* # of faces per cube */
/* Ratio: distance from viewpoint to projection plane / width of
projection plane. Defines the width of the field of view. Lower
absolute values = wider fields of view; higher values = narrower */
#define PROJECTION_RATIO -2.0 /* negative because visible Z
coordinates are negative */
/* Draws the polygon described by the point list PointList in color
Color with all vertices offset by (X,Y) */
#define DRAW_POLYGON(PointList,NumPoints,Color,X,Y)   Polygon.Length = NumPoints; Polygon.PointPtr = PointList;  FillConvexPolygon(&Polygon, Color, X, Y);




typedef int32_t Fixedpoint;

constexpr static inline int32_t INT_TO_FIXED(int32_t x) { return x << 16; }
constexpr static inline int32_t DOUBLE_TO_FIXED(double x) { return  static_cast<int32_t>((x * 65536.0 + 0.5)); }

static constexpr size_t Fixedpoint_FRACBITS = 16;
static constexpr size_t Fixedpoint_FRACHALF = (Fixedpoint_FRACBITS / 2);
static constexpr Fixedpoint Fixedpoint_FRACUNIT = (1 << Fixedpoint_FRACBITS);
static constexpr Fixedpoint Fixedpoint_ONE = (1 << Fixedpoint_FRACBITS);
static constexpr Fixedpoint Fixedpoint_HALF = (1 << (Fixedpoint_FRACBITS - 1));
static constexpr Fixedpoint  Fixedpoint_MAX = std::numeric_limits<Fixedpoint>::max();
static constexpr Fixedpoint  Fixedpoint_MIN = std::numeric_limits<Fixedpoint>::min();
static constexpr Fixedpoint Fixedpoint_NAN = Fixedpoint_MAX;
static constexpr Fixedpoint Fixedpoint_DIV_ZERO = (Fixedpoint_ONE - 1); // max value when using div the demoniator may be and not considered zero
inline static constexpr Fixedpoint FixedMul(Fixedpoint a, Fixedpoint b) { return (a >> Fixedpoint_FRACHALF) * (b >> Fixedpoint_FRACHALF); }
inline static constexpr Fixedpoint FixedDiv2(Fixedpoint a, Fixedpoint b) { return (a / (b >> Fixedpoint_FRACHALF)) << Fixedpoint_FRACHALF; }
inline static constexpr Fixedpoint FixedDiv(Fixedpoint a, Fixedpoint b) { return (b >> Fixedpoint_FRACHALF) == 0 ? Fixedpoint_NAN : FixedDiv2(a, b); }
typedef Fixedpoint Xform[3][4];
/* Describes a single 2D point */
 struct Point { int X; int Y; } ;
/* Describes a single 3D point in homogeneous coordinates; the W
coordinate isn't present, though; assumed to be 1 and implied */
 struct Point3 { Fixedpoint X, Y, Z; } ;
 struct IntPoint3 { int X; int Y; int Z; } ;
/* Describes a series of points (used to store a list of vertices that
describe a polygon; each vertex is assumed to connect to the two
adjacent vertices; last vertex is assumed to connect to first) */
 struct PointListHeader { int Length; Point * PointPtr; } ;
/* Describes the beginning and ending X coordinates of a single
horizontal line */
 struct HLine { int XStart; int XEnd; } ;
/* Describes a Length-long series of horizontal lines, all assumed to
be on contiguous scan lines starting at YStart and proceeding
downward (used to describe a scan-converted polygon to the
low-level hardware-dependent drawing code). */
 struct HLineList { int Length; int YStart; HLine * HLinePtr; } ;
 struct Rect { int Left, Top, Right, Bottom; } ;
/* structure describing one face of an object (one polygon) */
 struct Face { int * VertNums; int NumVerts; int Color; }  ;
 struct RotateControl  { double RotateX, RotateY, RotateZ; } ;
 struct MoveControl {
	Fixedpoint MoveX, MoveY, MoveZ, MinX, MinY, MinZ,
		MaxX, MaxY, MaxZ;
} ;

/* fields common to every object */
struct Object {
	virtual void DrawFunc() = 0;	 /* draws object */
	virtual void RecalcFunc() = 0;	 /* prepares object for drawing */
	virtual void MoveFunc() = 0;	  /* moves object */
	virtual ~Object() {}
	//	void(*DrawFunc)(Object*);    
	//	void(*RecalcFunc)(Object*);   
	//	void(*MoveFunc)(Object*);    
		int RecalcXform;        /* 1 to indicate need to recalc */    
		Rect EraseRect;      /* rectangle to erase in backbuffer */
};
/* basic object */
/* structure describing a polygon-based object */
struct PObject : public Object {
	void DrawFunc() override final;
	void RecalcFunc()  override final;
	void MoveFunc()  override final;
	int RDelayCount, RDelayCountBase; /* controls rotation speed */
	int MDelayCount, MDelayCountBase; /* controls movement speed */
	Xform XformToWorld;        /* transform from object->world space */
	Xform XformToView;         /* transform from object->view space */
	RotateControl Rotate;      /* controls rotation change over time */
	MoveControl Move;          /* controls object movement over time */
	int NumVerts;              /* # vertices in VertexList */
	Point3 * VertexList;       /* untransformed vertices */
	Point3 * XformedVertexList;   /* transformed into view space */
	Point3 * ProjectedVertexList; /* projected into screen space */
	Point * ScreenVertexList;     /* converted to screen coordinates */
	int NumFaces;              /* # of faces in object */
	Face * FaceList;           /* pointer to face info */
} ;

void XformVec(Xform, Fixedpoint *, Fixedpoint *);
void ConcatXforms(Xform, Xform, Xform);
int FillConvexPolygon(PointListHeader *, int, int, int);
void FillRectangleX(int, int, int, int, unsigned int, int);
void XformAndProjectPObject(PObject *);
void DrawPObject(PObject *);
void AppendRotationX(Xform, double);
void AppendRotationY(Xform, double);
void AppendRotationZ(Xform, double);
void DrawPixel(int, int, int);
void InitializeFixedPoint(void);
void RotateAndMovePObject(PObject *);
void InitializeCubes(void);
//extern int DisplayedPage, NonDisplayedPage, 
extern int RecalcAllXforms;
extern int NumObjects;
extern Xform WorldViewXform;
extern Object *ObjectList[];
extern Point3 CubeVerts[];

