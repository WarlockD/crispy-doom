#include "doom_common.hpp"
#include "doom_math.h"

namespace doom_cpp {
	// Patches.
	// A patch holds one or more columns.
	// Patches are used for sprites and all masked pictures,
	// and we compose textures from the TEXTURE1/2 lists
	// of patches.
	// On Windows, vsnprintf() is _vsnprintf().

	// Bounding box coordinate storage.
	template<typename T>
	struct _point_t {
		using value_type = T;
		using self_type = _point_t<T>;
		using refrence = T&;
		using pointer = T*;
		using const_refrence = const T&;
		using const_pointer = const T*;
		_point_t() : x{ static_cast<value_type>(0) }, y{ static_cast<value_type>(0) } {}
		template<typename TT>
		_point_t(const _point_t<TT>& copy) : x{ static_cast<value_type>(copy.x) }, y{ static_cast<value_type>(copy.y) } {}
		_point_t(value_type x, value_type y) : x{ x }, y{ y } {}
		template<typename TT>
		self_type& operator+=(const _point_t<TT>& r)  { x += r.x; y += r.y; return *this; }
		template<typename TT>
		self_type& operator-=(const _point_t<TT>& r)  { x -= r.x; y -= r.y; return *this; }
		template<typename TT>
		self_type& operator*=(const _point_t<TT>& r)  { x *= r.x; y *= r.y; return *this; }
		template<typename TT>
		self_type& operator/=(const _point_t<TT>& r)  { x /= r.x; y /= r.y; return *this; }
		value_type x;
		value_type y;
	};
	using fpoint_t = _point_t<fixed_t>;
	using ipoint_t = _point_t<int16_t>;

	template<typename T>
	struct _rect_t {
		using point_type = _point_t<T>;
		using value_type = typename point_type::value_type;
		using refrence = typename point_type::refrence;
		using pointer = typename point_type::pointer;
		using const_refrence = typename point_type::const_refrence;
		using const_pointer = typename point_type::const_pointer;
	
		_rect_t() : pos{}, size{} {}
		template<typename TT>
		_rect_t(const _point_t<TT>& pos, const _point_t<TT>& size) : pos{ pos }, size{ size } {}

		refrence width() { return size.x; }
		refrence height() { return size.y; }
		const_refrence width() const { return size.x; }
		const_refrence height() const { return size.y; }
		point_type top_left() const { return pos; }
		point_type top_right() const { return point_type(pos.x + size.x - static_cast<value_type>(1), pos.y); }
		point_type bottom_left() const { return point_type(pos.x, pos.y + size.y - static_cast<value_type>(1)); }
		point_type bottom_right() const { return point_type(pos.x + size.x - static_cast<value_type>(1), pos.y + size.y - static_cast<value_type>(1)); }

		point_type pos;
		point_type size;
	};

	using frect_t = _rect_t<fixed_t>;
	using irect_t = _rect_t<int16_t>;

	struct bbox_t {
		enum class BOX
		{
			TOP,
			BOTTOM,
			LEFT,
			RIGHT
		};	// bbox coordinates
		fixed_t& top() { return _box[static_cast<uint32_t>(BOX::TOP)]; }
		fixed_t& bottom() { return _box[static_cast<uint32_t>(BOX::BOTTOM)]; }
		fixed_t& left() { return _box[static_cast<uint32_t>(BOX::LEFT)]; }
		fixed_t& right() { return _box[static_cast<uint32_t>(BOX::RIGHT)]; }
		const fixed_t& top() const { return _box[static_cast<uint32_t>(BOX::TOP)]; }
		const fixed_t& bottom()const { return _box[static_cast<uint32_t>(BOX::BOTTOM)]; }
		const fixed_t& left() const { return _box[static_cast<uint32_t>(BOX::LEFT)]; }
		const fixed_t& right()const { return _box[static_cast<uint32_t>(BOX::RIGHT)]; }
		bbox_t() : _box{ fixed_t::min_value, fixed_t::max_value,fixed_t::min_value, fixed_t::max_value } {}
		bbox_t(fixed_t top, fixed_t bottom, fixed_t left, fixed_t right) : _box{ top, bottom,left, right } {}
		void clear() { *this = bbox_t(); }
		fixed_t& operator[](BOX b) { return _box[static_cast<uint32_t>(b)]; }
		const fixed_t& operator[](BOX b) const { return _box[static_cast<uint32_t>(b)]; }
		template<typename TT>
		bbox_t& operator+=(const _point_t<TT>& p) {
			if (p.x < left()) left() = p.x;
			else if (p.x > right()) right() = p.x;
			if (p.y < bottom()) bottom() = p.y;
			else if (p.y > top()) top() = p.y;
		}
		template<typename TT>
		bbox_t& operator+=(const _rect_t<TT>& p) {
			*this += p.topleft();
			*this += p.bottomright();
		}
	private:
		fixed_t _box[4];
	};

#ifdef _WIN32
#ifdef _MSC_VER /* not needed for Visual Studio 2008 */
#pragma pack(push,1)
	struct patch_t {
		ipoint_t size;		// bounding box size 
		ipoint_t offset;	// pixels to the left of origin 
			// pixels below the origin 
#if 0
		int16_t		width;		
		int16_t		height;
		int16_t		leftoffset;
		int16_t		topoffset;
#endif
		int32_t		columnofs[8];	// only [width] used
									// the [0] is &columnofs[width] 
	};

	// posts are runs of non masked source pixels
	struct post_t
	{
		uint8_t		topdelta;	// -1 is the last post in a column
		uint8_t		length; 	// length data bytes follows
	};
#pragma pack(pop)
#endif
#endif

	// need to build this as a template that has a list of 
	// static functions used to copy data from one buffer to another screen
	// buffer
	template<size_t _WIDTH, size_t _HEIGHT>
	class Video {
		static constexpr bool RANGECHECK = false;

	public:
		//
		// VIDEO
		//
		static constexpr size_t SCREENWIDTH = _WIDTH;
		static constexpr size_t SCREENHEIGHT = _HEIGHT;
		static constexpr size_t CENTERY = (SCREENHEIGHT / 2);

		// haleyjd 08/28/10: implemented for Strife support
		// haleyjd 08/28/10: Patch clipping callback, implemented to support Choco
		// Strife.
		typedef bool(*vpatchclipfunc_t)(patch_t *, int, int);
		constexpr size_t width() const { return _WIDTH; }
		constexpr size_t height() const { return _HEIGHT; }



		void SetPatchClipCallback(vpatchclipfunc_t func) { patchclip_callback = func; }


		// Allocates buffer screens, call before R_Init.
		void Init(void) {}



		void CopyRect(const uint8_t *source, const irect_t& src, const ipoint_t& pos) {
			if (RANGECHECK && (src.pos.x < 0
				|| src.pos.x + src.size.width > SCREENWIDTH || src.pos.x < 0
				|| src.pos.y + src.size.height > SCREENHEIGHT || src.pos.y < 0
				|| pos.x + src.size.width > SCREENWIDTH || pos.x < 0
				|| pos.y + src.size.height > SCREENHEIGHT || pos.y < 0))
			{
				I_Error("CopyRect Range Check");
			}
			MarkRect(rect_t(dest,src.size));

			const uint8_t* src = source + SCREENWIDTH * src.pos.y + src.pos.x;
			uint8_t* dest = dest_screen + SCREENWIDTH * pos.y + pos.x;
			// something better than this?
			for (auto height = src.height(); height>0; height--)
			{
				std::memcpy(dest, src, src.size.x);
				src += SCREENWIDTH;
				dest += SCREENWIDTH;
			}
		}

		void DrawPatch(const ipoint_t& pos, const patch_t& patch) {
			ipoint_t p = pos - patch.offset;

			// haleyjd 08/28/10: Strife needs silent error checking here.
			if (patchclip_callback && !patchclip_callback(patch, x, y)) return;

			if (RANGECHECK && {x < 0
				|| x + patch.size.x > SCREENWIDTH
				|| y < 0
				|| y + patch.size.y > SCREENHEIGHT)}
			{
				I_Error("Bad V_DrawPatch");
			}

			MarkRect(irect_t(p, patch.size));
			auto w = patch.size.x;
			uint8_t *desttop = dest_screen + p.y * SCREENWIDTH + p.x;

			for (decltype(w) col = 0; col < w; x++, col++, desttop++)
			{
				column_t *column = (column_t *)((uint8_t *)patch + patch.columnofs[col]);

				// step through the posts in a column
				while (column->topdelta != 0xff)
				{
					const uint8_t *source = (uint8_t *)column + 3;
					uint8_t * dest = desttop + column->topdelta*SCREENWIDTH;
					int ccount = column->length;
					for (int count = column->length; count; count--) {
						*dest = *source++;
						dest += SCREENWIDTH;
					}
					column = (column_t *)((uint8_t *)column + column->length + 4);
				}
			}
		}
		void DrawPatchFlipped(const ipoint_t& pos, patch_t *patch) {
			ipoint_t p = pos - patch.offset;

			// haleyjd 08/28/10: Strife needs silent error checking here.
			if (patchclip_callback && !patchclip_callback(patch, x, y)) return;

			if (RANGECHECK && {x < 0
				|| x + patch.size.x > SCREENWIDTH
				|| y < 0
				|| y + patch.size.y > SCREENHEIGHT)}
			{
				I_Error("Bad V_DrawPatch");
			}

			MarkRect(irect_t(p, patch.size));
			auto w = patch.size.x;
			uint8_t *desttop = dest_screen + p.y * SCREENWIDTH + p.x;

			for (decltype(w) col = 0; col < w; x++, col++, desttop++)
			{
				column_t *column = (column_t *)((byte *)patch + LONG(patch->columnofs[w - 1 - col]));

				// step through the posts in a column
				while (column->topdelta != 0xff)
				{
					const uint8_t *source = (uint8_t *)column + 3;
					uint8_t * dest = desttop + column->topdelta*SCREENWIDTH;
					int ccount = column->length;
					for (int count = column->length; count; count--) {
						*dest = *source++;
						dest += SCREENWIDTH;
					}
					column = (column_t *)((uint8_t *)column + column->length + 4);
				}
			}
		}
		void DrawTLPatch(const ipoint_t& pos, patch_t *patch) { assert(0); }
		void DrawAltTLPatch(const ipoint_t& pos, patch_t * patch) { assert(0); }
		void DrawShadowedPatch(const ipoint_t& pos, patch_t *patch) { assert(0); }
		void DrawXlaPatch(const ipoint_t& pos, patch_t * patch) { assert(0); }    // villsa [STRIFE]
		void DrawPatchDirect(const ipoint_t& pos, patch_t *patch) { assert(0); }

		// Draw a linear block of pixels into the view buffer.

		void DrawBlock(const irect_t& rect, const uint8_t *src) {
			if (RANGECHECK && {x < 0
				|| x + width >SCREENWIDTH
				|| y < 0
				|| y + height > SCREENHEIGHT)}
			{
				I_Error("Bad V_DrawBlock");
			}
			MarkRect(rect);
			uint8_t *dest = dest_screen + rect.pos.y * SCREENWIDTH + rect.pos.x;
			for (auto height = rect.height(); height >= 0; height--) {
				memcpy(dest, src, width);
				src += rect.width();
				dest += SCREENWIDTH;
			}
		}

		void MarkRect(const irect_t& rect) {
			// If we are temporarily using an alternate screen, do not 
			// affect the update box.

			if (dest_screen == _videoBuffer) dirtybox += rect;
		}

		void DrawFilledBox(const irect_t& rect, int c) {

			uint8_t * buf = dest_screen + SCREENWIDTH * rect.pos.y + rect.pos.x;

			for (int y1 = 0; y1 < rect.height(); ++y1)
			{
				uint8_t * buf1 = buf;
				for (int x1 = 0; x1 < rect.width(); ++x1) *buf1++ = c;
				buf += SCREENWIDTH;
			}
		}
		void DrawHorizLine(const ipoint_t& pos, int16_t h,  int c) {
			uint8_t * buf = dest_screen + SCREENWIDTH * rect.pos.y + rect.pos.x;
			while(h--) *buf++ = c;
		}
		void DrawVertLine(const ipoint_t& pos, int16_t w, int c) {
			uint8_t * buf = dest_screen + SCREENWIDTH * pos.y + pos.x;
			while (w--) { *buf = c; buf += SCREENWIDTH; }
		}
		void DrawBox(const irect_t& rect, int c) {
			V_DrawHorizLine(rect.top_left(),rect.width(), c);
			V_DrawHorizLine(rect.bottom_left(), rect.width(), c);
			V_DrawVertLine(rect.top_left(), rect.height(), c);
			V_DrawVertLine(rect.top_right(), rect.height(), c);
		}

		// Draw a raw screen lump

		void DrawRawScreen(uint8_t *raw) { std::memcpy(dest_screen, raw, SCREENWIDTH * SCREENHEIGHT); }

		// Temporarily switch to using a different buffer to draw graphics, etc.

		void UseBuffer(uint8_t *buffer) { dest_screen = buffer; }

		// Return to using the normal screen buffer to draw graphics.

		void RestoreBuffer() { dest_screen = _videoBuffer; }

		// Save a screenshot of the current screen to a file, named in the 
		// format described in the string passed to the function, eg.
		// "DOOM%02i.pcx"

		//void ScreenShot(char *format);

		// Load the lookup table for translucency calculations from the TINTTAB
		// lump.

		void LoadTintTable(uint8_t * b) { tinttable = b; }

		// villsa [STRIFE]
		// Load the lookup table for translucency calculations from the XLATAB
		// lump.

		void LoadXlaTable(uint8_t * b) { xlatab = b; }

		//void DrawMouseSpeedBox(int speed);

		Video() : dest_screen{ _videoBuffer }, tinttable{ nullptr }, xlatab{ nullptr }, patchclip_callback{ nullptr }{}
	private:
		uint8_t _videoBuffer[SCREENWIDTH*SCREENHEIGHT];
		// Blending table used for fuzzpatch, etc.
		// Only used in Heretic/Hexen

		uint8_t *tinttable;

		// villsa [STRIFE] Blending table used for Strife
		uint8_t *xlatab;

		bbox_t dirtybox;
		// The screen buffer that the v_video.c code draws to.

		uint8_t *dest_screen;


		// haleyjd 08/28/10: clipping callback function for patches.
		// This is needed for Chocolate Strife, which clips patches to the screen.
		vpatchclipfunc_t patchclip_callback;

	};
}