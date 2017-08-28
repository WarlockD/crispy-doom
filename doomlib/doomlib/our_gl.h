#ifndef __OUR_GL_H__
#define __OUR_GL_H__
#include "tiny_common.h"
#include <list>

#include "tgaimage.h"
#include "model.h"

#include "geometry.h"

template<typename T>
struct IShader {
	using real_type = T;
	virtual ~IShader() {}
    virtual vec<4,real_type> vertex(int iface, int nthvert) = 0;
    virtual bool fragment(vec<3,T> bar, TGAColor &color) = 0;
};

template<typename T>
struct	vertex_t
{
	using real_type = T;
	vec<3, real_type> uvw;
	vec<3, real_type> xyz;
	vertex_t *next;
};

// ----------------------------------------------------------------------------
// The edge structure.  This is used to keep track of each left & right edge
// during scan conversion.  The algorithm does not pre-build an edge list
// prior to rendering, rather it renders edges as it builds them.  This
// structure is used only to keep the variables together.
// ----------------------------------------------------------------------------
template<typename T>
struct	edge_t
{
	using real_type = T;
	vec<2, real_type> uv;
	vec<2, real_type> wx;
	float	u, du;
	float	v, dv;
	float	w, dw;
	float	x, dx;
	int	height;
} ;


template<typename T>
class TinyRender {
	using real_type = T;
	using Vec2f = vec<2, real_type>;
	using Vec3f = vec<3, real_type>;
	using Vec4f = vec<3, real_type>;
	using Matrix = mat<4, 4, real_type>;

public:
	const Matrix& ModelView() const { return _ModelView; }
	const Matrix& Viewport() const { return _Viewport; }
	const Matrix& Projection() const { return _Projection; }
	Matrix& ModelView()  { return _ModelView; }
	Matrix& Viewport()  { return _Viewport; }
	Matrix& Projection()  { return _Projection; }
	void viewport(int x, int y, int w, int h) {
		_Viewport = Matrix::identity();
		_Viewport[0][3] = x + w / 2.f;
		_Viewport[1][3] = y + h / 2.f;
		_Viewport[2][3] = 1.f;
		_Viewport[0][0] = w / 2.f;
		_Viewport[1][1] = h / 2.f;
		_Viewport[2][2] = 0;
	}

	void projection(float coeff) {
		_Projection = Matrix::identity();
		_Projection[3][2] = coeff;
	}

	void lookat(Vec3f eye, Vec3f center, Vec3f up) {
		Vec3f z = (eye - center).normalize();
		Vec3f x = cross(up, z).normalize();
		Vec3f y = cross(z, x).normalize();
		Matrix Minv = Matrix::identity();
		Matrix Tr = Matrix::identity();
		for (int i = 0; i<3; i++) {
			Minv[0][i] = x[i];
			Minv[1][i] = y[i];
			Minv[2][i] = z[i];
			Tr[i][3] = -center[i];
		}
		_ModelView = Minv*Tr;
	}

	Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P) {
		Vec3f s[2];
		for (int i = 2; i--; ) {
			s[i][0] = C[i] - A[i];
			s[i][1] = B[i] - A[i];
			s[i][2] = A[i] - P[i];
		}
		Vec3f u = cross(s[0], s[1]);
		if (std::abs(u[2])>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
			return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
		return Vec3f(-1, 1, 1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
	}
	// IMAGE_T has width(), height(), and set(x,y,c)
	template<typename IMAGE_T>
	void triangle(mat<4, 3, real_type> &clipc, IShader<real_type> &shader, IMAGE_T &image, real_type *zbuffer) {
		const size_t width = image.get_width();
		const size_t height = image.get_height();
		mat<3, 4, float> pts = (_Viewport*clipc).transpose(); // transposed to ease access to each of the points
		mat<3, 2, float> pts2;
		for (int i = 0; i < 3; i++) pts2[i] = proj<2>(pts[i] / pts[i][3]);

		Vec2f bboxmin(std::numeric_limits<real_type>::max(), std::numeric_limits<real_type>::max());
		Vec2f bboxmax(-std::numeric_limits<real_type>::max(), -std::numeric_limits<real_type>::max());
		Vec2f clamp(width - 1, height - 1);
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 2; j++) {
				bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts2[i][j]));
				bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts2[i][j]));
			}
		}
		Vec2i P;
		TGAColor color;
		for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
			for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
				Vec3f bc_screen = barycentric(pts2[0], pts2[1], pts2[2], P);
				Vec3f bc_clip = Vec3f(bc_screen.x / pts[0][3], bc_screen.y / pts[1][3], bc_screen.z / pts[2][3]);
				bc_clip = bc_clip / (bc_clip.x + bc_clip.y + bc_clip.z);
				real_type frag_depth = clipc[2] * bc_clip;
				if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z<0 || zbuffer[P.x + P.y*width]>frag_depth) continue;
				bool discard = shader.fragment(bc_clip, color);
				if (!discard) {
					zbuffer[P.x + P.y* width] = frag_depth;
					image.set(P.x, P.y, color);
				}
			}
		}

	};
private:
	Matrix _ModelView;
	Matrix _Viewport;
	Matrix _Projection;

};

#include <unordered_set>

template<typename T>
class TinyTest {
	using real_type = T;
	using Vec2f = vec<2, real_type>;
	using Vec3f = vec<3, real_type>;
	using Vec4f = vec<4, real_type>;
	using Matrix = mat<4, 4, real_type>;
	std::list<Model> _models;

	std::vector<real_type> zbuffer;
	using renderer_t = TinyRender<T>;

	renderer_t renderer;
	static Vec3f       light_dir;
	static Vec3f       eye;
	static Vec3f    center;
	static Vec3f        up;

	struct GouraudShader : public IShader<real_type> {
		Vec3f varying_intensity; // written by vertex shader, read by fragment shader
		mat<4, 3, real_type> varying_tri; // triangle coordinates (clip coordinates), written by VS, read by FS
		Model* model;
		renderer_t &renderer;
		GouraudShader(renderer_t& r) : renderer(r) {}
		virtual Vec4f vertex(int iface, int nthvert) {
			varying_intensity[nthvert] = std::max(0.f, model->normal(iface, nthvert)*light_dir); // get diffuse lighting intensity
			Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert)); // read the vertex from .obj file
			varying_tri.set_col(nthvert, gl_Vertex);
			return renderer.Viewport()*renderer.Projection()*renderer.ModelView()*gl_Vertex; // transform it to screen coordinates
		}

		virtual bool fragment(Vec3f bar, TGAColor &color) {
			float intensity = varying_intensity*bar;   // interpolate intensity for the current pixel
			color = TGAColor(255, 255, 255)*intensity; // well duh
			return false;                              // no, we do not discard this pixel
		}
	};
	struct TextureShader : public IShader<real_type> {
		Vec3f          varying_intensity; // written by vertex shader, read by fragment shader
		mat<2, 3, real_type> varying_uv;        // same as above
		mat<4, 3, real_type> varying_tri; // triangle coordinates (clip coordinates), written by VS, read by FS
		Model* model;
		renderer_t &renderer;
		TextureShader(renderer_t& r) : renderer(r) {}
		std::unordered_set<uint32_t> debug;
		virtual Vec4f vertex(int iface, int nthvert) {
			varying_uv.set_col(nthvert, model->uv(iface, nthvert));
			varying_intensity[nthvert] = std::max(0.f, model->normal(iface, nthvert)*light_dir); // get diffuse lighting intensity
			Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert)); // read the vertex from .obj file
			varying_tri.set_col(nthvert, gl_Vertex);
			return renderer.Viewport()*renderer.Projection()*renderer.ModelView()*gl_Vertex; // transform it to screen coordinates
		}

		virtual bool fragment(Vec3f bar, TGAColor &color) {
			float intensity = varying_intensity*bar;   // interpolate intensity for the current pixel
			Vec2f uv = varying_uv*bar;                 // interpolate uv for the current pixel

			color = model->diffuse(uv)*intensity;      // well duh

			debug.emplace(color.num);
			return false;                              // no, we do not discard this pixel
			
		}
	};
	struct Shader : public IShader<real_type> {
		mat<2, 3, real_type> varying_uv;  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
		mat<4, 3, real_type> varying_tri; // triangle coordinates (clip coordinates), written by VS, read by FS
		mat<3, 3, real_type> varying_nrm; // normal per vertex to be interpolated by FS
		mat<3, 3, real_type> ndc_tri;     // triangle in normalized device coordinates

		Model* model;
		TinyRender<T> &renderer;
		Shader(renderer_t& r) : renderer(r) {}
		virtual Vec4f vertex(int iface, int nthvert) {
			varying_uv.set_col(nthvert, model->uv(iface, nthvert));
			varying_nrm.set_col(nthvert, proj<3>((renderer.Projection()*renderer.ModelView()).invert_transpose()*embed<4>(model->normal(iface, nthvert), 0.f)));
			Vec4f gl_Vertex = renderer.Projection()*renderer.ModelView()*embed<4>(model->vert(iface, nthvert));
			varying_tri.set_col(nthvert, gl_Vertex);
			ndc_tri.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));
			return gl_Vertex;
		}

		virtual bool fragment(Vec3f bar, TGAColor &color) {
			Vec3f bn = (varying_nrm*bar).normalize();
			Vec2f uv = varying_uv*bar;

			mat<3, 3, real_type> A;
			A[0] = ndc_tri.col(1) - ndc_tri.col(0);
			A[1] = ndc_tri.col(2) - ndc_tri.col(0);
			A[2] = bn;

			mat<3, 3, real_type> AI = A.invert();

			Vec3f i = AI * Vec3f(varying_uv[0][1] - varying_uv[0][0], varying_uv[0][2] - varying_uv[0][0], 0);
			Vec3f j = AI * Vec3f(varying_uv[1][1] - varying_uv[1][0], varying_uv[1][2] - varying_uv[1][0], 0);

			mat<3, 3, real_type> B;
			B.set_col(0, i.normalize());
			B.set_col(1, j.normalize());
			B.set_col(2, bn);

			Vec3f n = (B*model->normal(uv)).normalize();

			float diff = std::max(real_type{}, n*light_dir);
			color = model->diffuse(uv)*diff;

			return false;
		}
	};

	TextureShader _shader;

	size_t _width;
	size_t _height;
public:
	void rotate(real_type degrees) {
		renderer.Projection() * 
	}
	TinyTest() : _width(640), _height(480), _framebuffer(640, 480), _shader(renderer) {}
	TinyTest(size_t width, size_t height) : _width(width), _height(height),_shader(renderer) {
	}

	void load(const std::string& filename) {
		_models.emplace_back(filename.c_str());
	}
	void init() {
		zbuffer.resize(_width*_height);
		std::fill(zbuffer.begin(), zbuffer.end(), -std::numeric_limits<real_type>::max());
		renderer.lookat(eye, center, up);
		renderer.viewport(_width / 8, _height / 8, _width * 3 / 4, _height * 3 / 4);
		renderer.projection(real_type{ -1 } / (eye - center).norm());
		light_dir = proj<3>((renderer.Projection()*renderer.ModelView()*embed<4>(light_dir, real_type{}))).normalize();
	}
	
	template<typename IMAGE_TYPE>
	void render(IMAGE_TYPE& screen) {
		for (auto& model : _models) {
			_shader.model = &model;
			for (int i = 0; i<model.nfaces(); i++) {
				for (int j = 0; j<3; j++) {
					_shader.vertex(i, j);
				}
				renderer.triangle(_shader.varying_tri, _shader, screen, zbuffer.data());
			}
		}

	}
};
template <typename T> vec<3U, T> TinyTest<T>::light_dir(1, 1, 1);
template <typename T> vec<3U, T> TinyTest<T>::eye(1, 1, 3);
template <typename T> vec<3U, T> TinyTest<T>::center(0, 0, 0);
template <typename T> vec<3U, T> TinyTest<T>::up(0, 1, 0);

#endif //__OUR_GL_H__