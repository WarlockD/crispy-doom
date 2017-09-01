#pragma once

#include "geometry.h"
void DrawVerticalRun(char **ScreenPtr, int XAdvance, int RunLength, const color_t& Color);

template<typename COLOR_TYPE>
class render_texture {
public:
	using color_type = COLOR_TYPE;
	constexpr static size_t bytes_per_pixel = sizeof(color_type::value_type) * color_type::dim;
	constexpr static size_t bits_per_pixel = bytes_per_pixel / 8;
	constexpr static size_t default_stride(size_t width) { return width + bytes_per_pixel - 1 & ~(bytes_per_pixel - 1); }
	render_texture(void* buffer, size_t width, size_t height, size_t stride) : _buffer(buffer), _size(width, height, stride) {}
	render_texture(void* buffer, size_t width, size_t height) : _buffer(buffer), _size(width, height, default_stride(width)) {}
	render_texture() : _buffer(nullptr), _size(0, 0, 0) {}
	void setup(void* buffer, size_t width, size_t height, size_t stride) {
		*this = render_texture(buffer, width, height,stride);
	}
	void setup(void* buffer, size_t width, size_t height) {
		*this = render_texture(buffer, width, height);
	}
#if 0
	void setup(color_type* s, size_t width, size_t height) {
		_texture = s;
		_size = vec<3, size_t>(width, height, stride);
	}
#endif
	size_t width() const { return _size.x; }
	size_t height() const { return _size.y; }
	size_t stride() const { return _size.z; }
	const color_t* data() const { return _texture; }
	color_t* data() { return _texture; }
	color_type& operator[](size_t i) const { return _texture[i]; }


	color_t* get_line(size_t y) { return static_cast<color_t*>(_buffer + y * stride()); }
	void set_pixel(size_t x, size_t y, const color_type& c) { get_line()[x] = c; }
	const color_type& get_pixel(size_t x, size_t y) const { return get_line()[x]; }
	template<size_t N, typename T>
	const color_type& get_pixel(const vec<N, T>& v) const { return get_pixel(size_t(v.x), size_t(v.y)); }
	template<size_t N, typename T>
	void set_pixel(const vec<N, T>& v, const color_type& c) const { set_pixel(size_t(v.x), size_t(v.y),c); }
private:
	void* _buffer;
	vec<3, size_t> _size;
};
#if 0
/* Structure describing one face of an object (one polygon) */
template<typename VT, typename CT>
struct Face {
	using render_texture_type = render_texture<CT>;
	using color_t = CT;
	using value_type = VT;
	using Vec2f = vec<2, value_type>;
	using Vec3f = vec<3, value_type>;
	using Matrix = mat<4, 4, value_type>;
	using point_t = vec<2, int16_t>;

	std::vector<Vec3f> verts;
	//int * VertNums;   
	/* pointer to list of indexes of this polygon's vertices
					  in the object's vertex list. The first two indexes
					  must select end and start points, respectively, of this
					  polygon's unit normal vector. Second point should also
					  be an active polygon vertex */
	//int NumVerts;     
	/* # of verts in face, not including the initial
					  vertex, which must be the end of a unit normal vector
					  that starts at the second index in VertNums */			  
	color_t ColorIndex;   /* direct palette index; used only for non-shaded faces */
	color_t FullColor; /* polygon's color */
	int ShadingType;  /* none, ambient, diffuse, texture mapped, etc. */
	render_texture_type * TexMap; /* pointer to bitmap for texture mapping, if any */
	std::vector<point_t> uvs;
	//vec<2,int16_t> 
	/* TexVerts; /* pointer to list of this polygon's vertices, in
					  TextureMap coordinates. Index n must map to index
					  n + 1 in VertNums, (the + 1 is to skip over the unit
					  normal endpoint in VertNums) */
} ;

#endif
template<typename VT, typename CT>
struct verts {
	using render_texture_type = render_texture<CT>;
	using color_t = CT;
	using value_type = VT;
	using Vec2f = vec<2, value_type>;
	using Vec3f = vec<3, value_type>;
	using Matrix = mat<4, 4, value_type>;
	using point_t = vec<2, int16_t>;
	/* Statics to save time that would otherwise pass them to subroutines. */
	int MaxVert, NumVerts, DestY;
	point_t * VertexPtr;
	point_t * TexVertsPtr;
	char * TexMapBits;
	int TexMapWidth;

};


	
/* Sets up an edge to be scanned; the edge starts at StartVert and proceeds
in direction Edge->Direction through the vertex list. Edge->Direction must
be set prior to call; -1 to scan a left edge (backward through the vertex
list), 1 to scan a right edge (forward through the vertex list).
Automatically skips over 0-height edges. Returns 1 for success, or 0 if
there are no more edges to scan. */


//template<typename T, typename IT = vector<T>::const_iiterator>
template<typename REAL_TYPE = float, typename INT_TYPE = int>
struct vec_traits {
	static_assert(std::is_signed<INT_TYPE>::value, "Must be a signed value");
	using real_type = REAL_TYPE;
	using int_type = INT_TYPE;
	using size_type = std::make_unsigned_t<int_type>;
	using difference_type = std::ptrdiff_t;
	using Vec2f = vec<2, real_type>;
	using Vec3f = vec<3, real_type>;
	using Vec2i = vec<2, int_type>;
	using Vec3i = vec<3, int_type>;
	using Matrix = mat<4, 4, real_type>;
};


template<typename COLOR_TYPE = color<4,uint8_t>, typename REAL_TYPE = float,typename INT_TYPE = int>
struct vertex {
	using traits = vec_traits<REAL_TYPE, INT_TYPE>;
	using Color = COLOR_TYPE;
	using Vec2f = typename traits::Vec2f;
	using Vec2i = typename traits::Vec2i;
	Vec2f xy;
	Vec2f uv;
	Color color;
};
// default vertex
using Vertex = vertex<color<4, uint8_t>, float, int>;

template<typename VT = Vertex, typename IT = std::vector<Vertex>::iterator>
class Edge {
public:
	using iterator = IT;
	using Vertex = VT;
	using traits = typename Vertex::traits;
	using size_type = typename traits::size_type;
	using difference_type = typename traits::difference_type;
	using real_type = typename traits::real_type;
	using int_type = typename traits::int_type;
	using Vec2f = typename traits::Vec2f;
	using Vec2i = typename traits::Vec2i;

	iterator current_end;/* vertex # of end of current edge */
	int_type remaining_scans;    /* height left to scan out in dest */
	Vec2f source;
	Vec2f source_step;
	//int Direction;         
	/* through edge list; 1 for a right edge (forward through vertex list), -1 for a left edge (backward through vertex list) */
	int_type DestX;             /* current X location in dest for this edge */
	int_type DestXIntStep;      /* whole part of dest X step per scan-line Y step */
	int_type DestXDirection;    /* -1 or 1 to indicate way X steps (left/right) */
	int_type DestXErrTerm;      /* current error term for dest X stepping */
	int_type DestXAdjUp;        /* amount to add to error term per scan line move */
	int_type DestXAdjDown;      /* amount to subtract from error term when the
									error term turns over */
									/* Steps an edge one scan line in the destination, and the corresponding
									distance in the source. If an edge runs out, starts a new edge if there
									is one. Returns 1 for success, or 0 if there are no more edges to scan. */

	const iterator _begin;
	const iterator _end;
	const iterator _max;
	// template helper to create this monstrosity
	Edge(iterator begin_, iterator end_, iterator max_) : _begin(begin_), _end(end_), _max(max_) {}
	bool setup(iterator start) {
		for (;;) {
			/* Done if this edge starts at the bottom vertex */
			if (start == _max) return false;
			/* Advance to the next vertex, wrapping if we run off the start or end of the vertex list */
			iterator next = start;
			if (++next == _end) next = _begin;

			/* Calculate the variables for this edge and done if this is not a zero-height edge */
			// this is tricky with floats, find a fudge?
			remaining_scans = int_type(next->xy.y - start->xy.y);
			if (remaining_scans != 0) { // need to check this on floats
				real_type DestYHeight = real_type(remaining_scans);
				current_end = next;
				source = _begin->xy; // should convert bytitself
				source_step = (next->uv - source) / DestYHeight;
				/* Set up Bresenham-style variables for dest X stepping */
				DestX = int_type(start->xy.x);
				int_type  DestXWidth = int_type(next->xy.x - start->xy.x);
				if (DestXWidth < 0) {
					/* Set up for drawing right to left */
					DestXDirection = -1;
					DestXWidth = -DestXWidth;
					DestXErrTerm = 1 - remaining_scans;
					DestXIntStep = -(DestXWidth / remaining_scans);
				}
				else {
					/* Set up for drawing left to right */
					DestXDirection = 1;
					DestXErrTerm = 0;
					DestXIntStep = DestXWidth / remaining_scans;
				}
				DestXAdjUp = DestXWidth % remaining_scans; //  % remaining_scans;
				DestXAdjDown = remaining_scans;
				return true;  /* success */
			}
			start = next;
		}
		return false;
	}
	/* Steps an edge one scan line in the destination, and the corresponding
	distance in the source. If an edge runs out, starts a new edge if there
	is one. Returns true for success, or false if there are no more edges to scan. */
	bool step()
	{
		/* Count off the scan line we stepped last time; if this edge is
		finished, try to start another one */
		/* Set up the next edge; done if there is no next edge */
		if (--remaining_scans == 0) return setup(current_end);
		/* Step the current source edge */
		source += source_step;
		/* Step dest X with Bresenham-style variables, to get precise dest pixel
		placement and avoid gaps */
		DestX += DestXIntStep;  /* whole pixel step */
		/* Do error term stuff for fractional pixel X step handling */
		if ((DestXErrTerm += DestXAdjUp) > 0) {
			DestX += DestXDirection;
			DestXErrTerm -= DestXAdjDown;
		}
		return true;
	}
};

template<typename IT, typename VEC_VALUE_TYPE = IT::value_type>
inline static Edge<VEC_VALUE_TYPE, IT> CreateEdge(IT begin_, IT end_, IT max_) {
	return Edge<VEC_VALUE_TYPE, IT>(begin_, end_, max_);
}

/* Draws a texture-mapped polygon, given a list of destination polygon
vertices, a list of corresponding source texture polygon vertices, and a
pointer to the source texture's descriptor. */

	
template<typename ARRAY_TYPE, typename COLOR_TYPE = color<4,uint8_t>, typename VEC_TRAITS = vec_traits<>, typename VERTEX_TYPE = ARRAY_TYPE::value_type>
inline static void DrawTexturedPolygon(render_texture<COLOR_TYPE>& target, const render_texture<COLOR_TYPE>& texture, const ARRAY_TYPE& polygons) {
	using Vertex = VERTEX_TYPE;
	using real_type = typename Vertex::traits::real_type;
	using int_type = typename Vertex::traits::int_type;
	if (polygons.size() < 3) return;
	auto min_max_vert = std::minmax_element(polygons.begin(), polygons.end(), [](const T &a, const T &b) { return a.xy.y < b.xy.y; });
	if (min_max_vert.first->xy.y >= min_max_vert.second->xy.y) return; // fail
}

struct HLine{ int XStart; int XEnd; } ;
struct HLineList { int Length; int YStart; HLine * HLinePtr; };
template<typename INT_TYPE, typename COLOR_TYPE>
struct Face  { INT_TYPE * VertNums; int NumVerts; COLOR_TYPE Color; };
template<typename REAL_TYPE>
struct RotateControl  { REAL_TYPE RotateX, RotateY, RotateZ; };
template<typename VEC_TYPE>
struct MoveControl {
	using Vec3f = VEC_TYPE;
	Vec3f move;
	Vec3f min;
	Vec3f max;
} ;
template<typename REAL_TYPE=float>
class XForm {
public:
	using self_type = XForm<REAL_TYPE>;
	using real_type = REAL_TYPE;
	using Matrix = mat<3, 4, real_type>;
	using Vec3f = vec<3, real_type>;
	XForm() = default;

	/* Concatenate a rotation by Angle around the X axis to transformation in
	XformToChange, placing the result back into XformToChange. */
	void AppendRotationX(real_type Angle)
	{
		real_type Temp10, Temp11, Temp12, Temp20, Temp21, Temp22;
		real_type CosTemp = std::cos(Angle);
		real_type SinTemp = std::sin(Angle);
		_data[0][1] = 0;
		/* Calculate the new values of the six affected matrix entries */
		Temp10 = (CosTemp * _data[1][0]) + (-SinTemp* _data[2][0]);
		Temp11 = (CosTemp* _data[1][1]) + (-SinTemp* _data[2][1]);
		Temp12 = (CosTemp* _data[1][2]) + (-SinTemp* _data[2][2]);
		Temp20 = (SinTemp* _data[1][0]) + (CosTemp* _data[2][0]);
		Temp21 = (SinTemp* _data[1][1]) + (CosTemp* _data[2][1]);
		Temp22 = (SinTemp* _data[1][2]) + (CosTemp* _data[2][2]);
		/* Put the results back into XformToChange */
		_data[1][0] = Temp10; _data[1][1] = Temp11;
		_data[1][2] = Temp12; _data[2][0] = Temp20;
		_data[2][1] = Temp21; _data[2][2] = Temp22;
	}
	void AppendRotationY(real_type Angle)
	{
		real_type Temp10, Temp11, Temp12, Temp20, Temp21, Temp22;
		real_type CosTemp = std::cos(Angle);
		real_type SinTemp = std::sin(Angle);
		/* Calculate the new values of the six affected matrix entries */
		/* Calculate the new values of the six affected matrix entries */
		Temp00 = (CosTemp* _data[0][0]) + (SinTemp* _data[2][0]);
		Temp01 = (CosTemp* _data[0][1]) + (SinTemp* _data[2][1]);
		Temp02 = (CosTemp* _data[0][2]) + (SinTemp* _data[2][2]);
		Temp20 = (-SinTemp* _data[0][0]) + (CosTemp* _data[2][0]);
		Temp21 = (-SinTemp* _data[0][1]) + (CosTemp* _data[2][1]);
		Temp22 = (-SinTemp* _data[0][2]) + (CosTemp* _data[2][2]);
		/* Put the results back into XformToChange */
		_data[1][0] = Temp10; _data[1][1] = Temp11;
		_data[1][2] = Temp12; _data[2][0] = Temp20;
		_data[2][1] = Temp21; _data[2][2] = Temp22;
	}
	void AppendRotationZ(real_type Angle)
	{
		real_type Temp10, Temp11, Temp12, Temp20, Temp21, Temp22;
		real_type CosTemp = std::cos(Angle);
		real_type SinTemp = std::sin(Angle);
		/* Calculate the new values of the six affected matrix entries */
		/* Calculate the new values of the six affected matrix entries */
		/* Calculate the new values of the six affected matrix entries */
		Temp00 = (CosTemp* _data[0][0]) + (-SinTemp* _data[1][0]);
		Temp01 = (CosTemp* _data[0][1]) + (-SinTemp* _data[1][1]);
		Temp02 = (CosTemp* _data[0][2]) + (-SinTemp* _data[1][2]);
		Temp10 = (SinTemp* _data[0][0]) + (CosTemp* _data[1][0]);
		Temp11 = (SinTemp* _data[0][1]) + (CosTemp* _data[1][1]);
		Temp12 = (SinTemp* _data[0][2]) + (CosTemp* _data[1][2]);
		/* Put the results back into XformToChange */
		_data[1][0] = Temp10; _data[1][1] = Temp11;
		_data[1][2] = Temp12; _data[2][0] = Temp20;
		_data[2][1] = Temp21; _data[2][2] = Temp22;
	}
	/* Matrix multiplies Xform by SourceVec, and stores the result in DestVec.
	Multiplies a 4x4 matrix times a 4x1 matrix; the result is a 4x1 matrix. Cheats
	by assuming the W coord is 1 and bottom row of matrix is 0 0 0 1, and doesn't
	bother to set the W coordinate of the destination. */
	vec<3,real_type> Vec(const vec<3, real_type> & SourceVec) const{
		vec<3, real_type>  DestVec;
		for (size_t i = 0; i<3; i++)
			DestVec[i] = (_data[i][0]* SourceVec[0]) +
			(_data[i][1]* SourceVec[1]) +
			(_data[i][2]* SourceVec[2]) +
			_data[i][3];   /* no need to multiply by W = 1 */
		return DestVec;
	}


	/* Matrix multiplies SourceXform1 by SourceXform2 and stores result in
	DestXform. Multiplies a 4x4 matrix times a 4x4 matrix; result is a 4x4 matrix.
	Cheats by assuming bottom row of each matrix is 0 0 0 1, and doesn't bother
	to set the bottom row of the destination. */
	self_type Concat(const self_type& other) const {
		self_type DestXform;
		int i, j;

		for (i = 0; i<3; i++) {
			for (j = 0; j<4; j++)
				DestXform[i][j] =
				(SourceXform1[i][0]* SourceXform2[0][j]) +
				(SourceXform1[i][1]* SourceXform2[1][j]) +
				(SourceXform1[i][2]* SourceXform2[2][j]) +
				SourceXform1[i][3];
		}
		return DestXform;
	}
private:
	real_type _data[2][4];
};

struct Rect  { int Left, Top, Right, Bottom; } ;
struct Object {
	virtual void DrawFunc() = 0;   /* draws object */ 
	virtual void RecalcFunc() = 0;   /* prepares object for drawing */  
	virtual void MoveFunc() = 0; /* moves object */     
	int RecalcXform;        /* 1 to indicate need to recalc */    \
	Rect EraseRect[2];      /* rectangle to erase in each page */
	virtual ~Object() {}
};
template<typename COLOR_TYPE = color<4,uint8_t>, typename REAL_TYPE = float, typename INT_TYPE = int>
struct PObject  : public Object{
	using Color = COLOR_TYPE;
	using traits = vec_traits<REAL_TYPE, INT_TYPE>;
	using int_type = typename traits::int_type;
	using real_type = typename traits::real_type;
	using Xform = XForm<REAL_TYPE>;
	using Point3 = typename traits::Vec3f;
	using Point = typename traits::Vec2f;
	int_type RDelayCount, RDelayCountBase; /* controls rotation speed */
	int_type MDelayCount, MDelayCountBase; /* controls movement speed */
	Xform XformToWorld;        /* transform from object->world space */
	Xform XformToView;         /* transform from object->view space */
	RotateControl<real_type> Rotate;      /* controls rotation change over time */
	MoveControl<Point3> Move;          /* controls object movement over time */
	int_type NumVerts;              /* # vertices in VertexList */
	Point3 * VertexList;       /* untransformed vertices */
	Point3 * XformedVertexList;   /* transformed into view space */
	Point3 * ProjectedVertexList; /* projected into screen space */
	Point * ScreenVertexList;     /* converted to screen coordinates */
	int_type NumFaces;              /* # of faces in object */
	Face<int_type,Color> * FaceList;           /* pointer to face info */
} ;

/* Draws a texture-mapped polygon, given a list of destination polygon
vertices, a list of corresponding source texture polygon vertices, and a
pointer to the source texture's descriptor. */


template<typename VT, typename CT>
class PolygonRender {
public:
	using render_texture_type = render_texture<CT>;
	using color_t = CT;
	using value_type = VT;
	using Vec2f = vec<2, value_type>;
	using Vec3f = vec<3, value_type>;
	using Matrix = mat<4, 4, value_type>;
	using point_t = vec<2, int16_t>;



	PolygonRender(std::vector<point_t>& polygon, std::vector<point_t>& texture_uv, render_texture_type& texture, render_texture_type& screen)
		: _polygon(polygon), _texture_uv(texture_uv), _texture(texture), _screen(screen) {}

	void draw(render_texture_type& screen) {
		/* Nothing to draw if less than 3 vertices */
		if (_polygon.size() < 3) return;
		auto min_max_vert = std::minmax_element(_polygon.begin(), minmax_element.end(), [](const point_t &a, const point_t &b) { return a.y < b.y; });
		if (min_max_vert.first->y >= min_max_vert.second->y) return; // Reject flat (0-pixel-high) polygons 

	}
private:
	std::vector<point_t> _polygon;
	std::vector<point_t> _texture_uv;
	render_texture_type& _texture;
	render_texture_type& _screen;

};

template<typename T>
class FastTextureAdvance {
public:
	using render_texture_type = render_texture<T>;
	using color_type = typename render_texture_type::color_type;
	// http ://www.jagregory.com/abrash-black-book/#fast-texture-mapping-an-implementation
	inline void advance(color_type*& src, color_type*& dst,vec<2, int16_t>& advance, vec<2, int16_t>& uv, vec<2, int16_t>& sum) {
		dst[_scanoffset] = = *src;
		uv += advance;
		if(-)
		*dst* = *_src; // set the pixel
	}
	FastTextureAdvance(color_t* src, color_t*dest) :_src(src), _dest(dest) {}
private:
	uint16_t _scanoffset;
	render_texture_type& _src;
	render_texture_type& _dest;
};
#if 0
template<typename CT,typename VT>
void DrawHorizontalLineList(screen<CT>& screen, const vec<3,VT>& line_list, const CT& Color) {
	for (const auto& line : line_list) {
		if (line.z > 0) { // z is length
			const size_t line_end = std::min(line.x + line.z, screen.width());
			for (size_t x = size_t(line.x); x < line_end; x++)
				screen.set_pixel(x, line.y, color);
		}
	}
}
/* Draws a horizontal run of pixels, then advances the bitmap pointer to
the first pixel of the next run. */
static inline void DrawHorizontalRun(color_t** ScreenPtr, int stride, int XAdvance, int RunLength, const color_t& Color)
{
	color_t* WorkingScreenPtr = *ScreenPtr;
	for (int i = 0; i<RunLength; i++)
	{
		*WorkingScreenPtr = Color;
		WorkingScreenPtr += XAdvance;
	}
	/* Advance to the next scan line */
	WorkingScreenPtr += stride;
	*ScreenPtr = WorkingScreenPtr;
}
/* Draws a vertical run of pixels, then advances the bitmap pointer to
the first pixel of the next run. */
void DrawVerticalRun(color_t** ScreenPtr, int stride, int XAdvance, int RunLength, const color_t& Color)
{
	color_t* WorkingScreenPtr = *ScreenPtr;
	for (int i = 0; i<RunLength; i++)
	{
		*WorkingScreenPtr = Color;
		WorkingScreenPtr += stride;
	}
	/* Advance to the next column */
	WorkingScreenPtr += XAdvance;
	*ScreenPtr = WorkingScreenPtr;
}

/* Draws a line between the specified endpoints in color Color. */
void LineDraw(int XStart, int YStart, int XEnd, int YEnd, int Color)
{
	int Temp, AdjUp, AdjDown, ErrorTerm, XAdvance, XDelta, YDelta;
	int WholeStep, InitialPixelCount, FinalPixelCount, i, RunLength;
	char far *ScreenPtr;

	/* We'll always draw top to bottom, to reduce the number of cases we have to
	handle, and to make lines between the same endpoints draw the same pixels */
	if (YStart > YEnd) {
		Temp = YStart;
		YStart = YEnd;
		YEnd = Temp;
		Temp = XStart;
		XStart = XEnd;
		XEnd = Temp;
	}
	/* Point to the bitmap address first pixel to draw */
	ScreenPtr = MK_FP(SCREEN_SEGMENT, YStart * SCREEN_WIDTH + XStart);

	/* Figure out whether we're going left or right, and how far we're
	going horizontally */
	if ((XDelta = XEnd - XStart) < 0)
	{
		XAdvance = -1;
		XDelta = -XDelta;
	}
	else
	{
		XAdvance = 1;
	}
	/* Figure out how far we're going vertically */
	YDelta = YEnd - YStart;

	/* Special-case horizontal, vertical, and diagonal lines, for speed
	and to avoid nasty boundary conditions and division by 0 */
	if (XDelta == 0)
	{
		/* Vertical line */
		for (i = 0; i <= YDelta; i++)
		{
			*ScreenPtr = Color;
			ScreenPtr += SCREEN_WIDTH;
		}
		return;
	}
	if (YDelta == 0)
	{
		/* Horizontal line */
		for (i = 0; i <= XDelta; i++)
		{
			*ScreenPtr = Color;
			ScreenPtr += XAdvance;
		}
		return;
	}
	if (XDelta == YDelta)
	{
		/* Diagonal line */
		for (i = 0; i <= XDelta; i++)
		{
			*ScreenPtr = Color;
			ScreenPtr += XAdvance + SCREEN_WIDTH;
		}
		return;
	}

	/* Determine whether the line is X or Y major, and handle accordingly */
	if (XDelta >= YDelta)
	{
		/* X major line */
		/* Minimum # of pixels in a run in this line */
		WholeStep = XDelta / YDelta;

		/* Error term adjust each time Y steps by 1; used to tell when one
		extra pixel should be drawn as part of a run, to account for
		fractional steps along the X axis per 1-pixel steps along Y */
		AdjUp = (XDelta % YDelta) * 2;

		/* Error term adjust when the error term turns over, used to factor
		out the X step made at that time */
		AdjDown = YDelta * 2;

		/* Initial error term; reflects an initial step of 0.5 along the Y
		axis */
		ErrorTerm = (XDelta % YDelta) - (YDelta * 2);

		/* The initial and last runs are partial, because Y advances only 0.5
		for these runs, rather than 1. Divide one full run, plus the
		initial pixel, between the initial and last runs */
		InitialPixelCount = (WholeStep / 2) + 1;
		FinalPixelCount = InitialPixelCount;

		/* If the basic run length is even and there's no fractional
		advance, we have one pixel that could go to either the initial
		or last partial run, which we'll arbitrarily allocate to the
		last run */
		if ((AdjUp == 0) && ((WholeStep & 0x01) == 0))
		{
			InitialPixelCount--;
		}
		/* If there're an odd number of pixels per run, we have 1 pixel that can't
		be allocated to either the initial or last partial run, so we'll add 0.5
		to error term so this pixel will be handled by the normal full-run loop */
		if ((WholeStep & 0x01) != 0)
		{
			ErrorTerm += YDelta;
		}
		/* Draw the first, partial run of pixels */
		DrawHorizontalRun(&ScreenPtr, XAdvance, InitialPixelCount, Color);
		/* Draw all full runs */
		for (i = 0; i<(YDelta - 1); i++)
		{
			RunLength = WholeStep;  /* run is at least this long */
									/* Advance the error term and add an extra pixel if the error
									term so indicates */
			if ((ErrorTerm += AdjUp) > 0)
			{
				RunLength++;
				ErrorTerm -= AdjDown;   /* reset the error term */
			}
			/* Draw this scan line's run */
			DrawHorizontalRun(&ScreenPtr, XAdvance, RunLength, Color);
		}
		/* Draw the final run of pixels */
		DrawHorizontalRun(&ScreenPtr, XAdvance, FinalPixelCount, Color);
		return;
	}
	else
	{
		/* Y major line */

		/* Minimum # of pixels in a run in this line */
		WholeStep = YDelta / XDelta;

		/* Error term adjust each time X steps by 1; used to tell when 1 extra
		pixel should be drawn as part of a run, to account for
		fractional steps along the Y axis per 1-pixel steps along X */
		AdjUp = (YDelta % XDelta) * 2;

		/* Error term adjust when the error term turns over, used to factor
		out the Y step made at that time */
		AdjDown = XDelta * 2;

		/* Initial error term; reflects initial step of 0.5 along the X axis */
		ErrorTerm = (YDelta % XDelta) - (XDelta * 2);

		/* The initial and last runs are partial, because X advances only 0.5
		for these runs, rather than 1. Divide one full run, plus the
		initial pixel, between the initial and last runs */
		InitialPixelCount = (WholeStep / 2) + 1;
		FinalPixelCount = InitialPixelCount;

		/* If the basic run length is even and there's no fractional advance, we
		have 1 pixel that could go to either the initial or last partial run,
		which we'll arbitrarily allocate to the last run */
		if ((AdjUp == 0) && ((WholeStep & 0x01) == 0))
		{
			InitialPixelCount--;
		}
		/* If there are an odd number of pixels per run, we have one pixel
		that can't be allocated to either the initial or last partial
		run, so we'll add 0.5 to the error term so this pixel will be
		handled by the normal full-run loop */
		if ((WholeStep & 0x01) != 0)
		{
			ErrorTerm += XDelta;
		}
		/* Draw the first, partial run of pixels */
		DrawVerticalRun(&ScreenPtr, XAdvance, InitialPixelCount, Color);

		/* Draw all full runs */
		for (i = 0; i<(XDelta - 1); i++)
		{
			RunLength = WholeStep;  /* run is at least this long */
									/* Advance the error term and add an extra pixel if the error
									term so indicates */
			if ((ErrorTerm += AdjUp) > 0)
			{
				RunLength++;
				ErrorTerm -= AdjDown;   /* reset the error term */
			}
			/* Draw this scan line's run */
			DrawVerticalRun(&ScreenPtr, XAdvance, RunLength, Color);
		}
		/* Draw the final run of pixels */
		DrawVerticalRun(&ScreenPtr, XAdvance, FinalPixelCount, Color);
		return;
	}
}
#endif