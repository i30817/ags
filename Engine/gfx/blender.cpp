//=============================================================================
//
// Adventure Game Studio (AGS)
//
// Copyright (C) 1999-2011 Chris Jones and 2011-20xx others
// The full list of copyright holders can be found in the Copyright.txt
// file, which is part of this source code distribution.
//
// The AGS source code is provided under the Artistic License 2.0.
// A copy of this license can be found in the file License.txt and at
// http://www.opensource.org/licenses/artistic-license-2.0.php
//
//=============================================================================

#include "core/types.h"
#include "gfx/blender.h"
#include "util/wgt2allg.h"

extern "C" {
    unsigned long _blender_trans16(unsigned long x, unsigned long y, unsigned long n);
    unsigned long _blender_trans15(unsigned long x, unsigned long y, unsigned long n);
}

// the allegro "inline" ones are not actually inline, so #define
// over them to speed it up
#define getr32(xx) ((xx >> _rgb_r_shift_32) & 0xFF)
#define getg32(xx) ((xx >> _rgb_g_shift_32) & 0xFF)
#define getb32(xx) ((xx >> _rgb_b_shift_32) & 0xFF)
#define geta32(xx) ((xx >> _rgb_a_shift_32) & 0xFF)
#define makeacol32(r,g,b,a) ((r << _rgb_r_shift_32) | (g << _rgb_g_shift_32) | (b << _rgb_b_shift_32) | (a << _rgb_a_shift_32))

// Take hue and saturation of blend colour, luminance of image
unsigned long _myblender_color15_light(unsigned long x, unsigned long y, unsigned long n)
{
    float xh, xs, xv;
    float yh, ys, yv;
    int r, g, b;

    rgb_to_hsv(getr15(x), getg15(x), getb15(x), &xh, &xs, &xv);
    rgb_to_hsv(getr15(y), getg15(y), getb15(y), &yh, &ys, &yv);

    // adjust luminance
    yv -= (1.0 - ((float)n / 250.0));
    if (yv < 0.0) yv = 0.0;

    hsv_to_rgb(xh, xs, yv, &r, &g, &b);

    return makecol15(r, g, b);
}

// Take hue and saturation of blend colour, luminance of image
// n is the last parameter passed to draw_lit_sprite
unsigned long _myblender_color16_light(unsigned long x, unsigned long y, unsigned long n)
{
    float xh, xs, xv;
    float yh, ys, yv;
    int r, g, b;

    rgb_to_hsv(getr16(x), getg16(x), getb16(x), &xh, &xs, &xv);
    rgb_to_hsv(getr16(y), getg16(y), getb16(y), &yh, &ys, &yv);

    // adjust luminance
    yv -= (1.0 - ((float)n / 250.0));
    if (yv < 0.0) yv = 0.0;

    hsv_to_rgb(xh, xs, yv, &r, &g, &b);

    return makecol16(r, g, b);
}

// Take hue and saturation of blend colour, luminance of image
unsigned long _myblender_color32_light(unsigned long x, unsigned long y, unsigned long n)
{
    float xh, xs, xv;
    float yh, ys, yv;
    int r, g, b;

    rgb_to_hsv(getr32(x), getg32(x), getb32(x), &xh, &xs, &xv);
    rgb_to_hsv(getr32(y), getg32(y), getb32(y), &yh, &ys, &yv);

    // adjust luminance
    yv -= (1.0 - ((float)n / 250.0));
    if (yv < 0.0) yv = 0.0;

    hsv_to_rgb(xh, xs, yv, &r, &g, &b);

    return makeacol32(r, g, b, geta32(y));
}

// Take hue and saturation of blend colour, luminance of image
unsigned long _myblender_color15(unsigned long x, unsigned long y, unsigned long n)
{
    float xh, xs, xv;
    float yh, ys, yv;
    int r, g, b;

    rgb_to_hsv(getr15(x), getg15(x), getb15(x), &xh, &xs, &xv);
    rgb_to_hsv(getr15(y), getg15(y), getb15(y), &yh, &ys, &yv);

    hsv_to_rgb(xh, xs, yv, &r, &g, &b);

    return makecol15(r, g, b);
}

// Take hue and saturation of blend colour, luminance of image
unsigned long _myblender_color16(unsigned long x, unsigned long y, unsigned long n)
{
    float xh, xs, xv;
    float yh, ys, yv;
    int r, g, b;

    rgb_to_hsv(getr16(x), getg16(x), getb16(x), &xh, &xs, &xv);
    rgb_to_hsv(getr16(y), getg16(y), getb16(y), &yh, &ys, &yv);

    hsv_to_rgb(xh, xs, yv, &r, &g, &b);

    return makecol16(r, g, b);
}

// Take hue and saturation of blend colour, luminance of image
unsigned long _myblender_color32(unsigned long x, unsigned long y, unsigned long n)
{
    float xh, xs, xv;
    float yh, ys, yv;
    int r, g, b;

    rgb_to_hsv(getr32(x), getg32(x), getb32(x), &xh, &xs, &xv);
    rgb_to_hsv(getr32(y), getg32(y), getb32(y), &yh, &ys, &yv);

    hsv_to_rgb(xh, xs, yv, &r, &g, &b);

    return makeacol32(r, g, b, geta32(y));
}

// trans24 blender, but preserve alpha channel from image
unsigned long _myblender_alpha_trans24(unsigned long x, unsigned long y, unsigned long n)
{
    unsigned long res, g, alph;

    if (n)
        n++;

    alph = y & 0xff000000;
    y &= 0x00ffffff;

    res = ((x & 0xFF00FF) - (y & 0xFF00FF)) * n / 256 + y;
    y &= 0xFF00;
    x &= 0xFF00;
    g = (x - y) * n / 256 + y;

    res &= 0xFF00FF;
    g &= 0xFF00;

    return res | g | alph;
}

void set_my_trans_blender(int r, int g, int b, int a)
{
    // use standard allegro 15 and 16 bit blenders, but customize
    // the 32-bit one to preserve the alpha channel
    set_blender_mode(_blender_trans15, _blender_trans16, _myblender_alpha_trans24, r, g, b, a);
}

// plain copy source to destination
// assign new alpha value as a summ of alphas.
unsigned long _additive_alpha_copysrc_blender(unsigned long x, unsigned long y, unsigned long n)
{
    unsigned long newAlpha = ((x & 0xff000000) >> 24) + ((y & 0xff000000) >> 24);

    if (newAlpha > 0xff) newAlpha = 0xff;

    return (newAlpha << 24) | (x & 0x00ffffff);
}

FORCEINLINE unsigned long argb2argb_blend_core(unsigned long src_col, unsigned long dst_col, unsigned long src_alpha)
{
    unsigned long dst_g, dst_alpha;
    src_alpha++;
    dst_alpha = geta32(dst_col);
    if (dst_alpha)
        dst_alpha++;

    // dst_g now contains the green hue from destination color
    dst_g   = (dst_col & 0x00FF00) * dst_alpha / 256;
    // dst_col now contains the red & blue hues from destination color
    dst_col = (dst_col & 0xFF00FF) * dst_alpha / 256;

    // res_g now contains the green hue of the pre-final color
    dst_g   = (((src_col & 0x00FF00) - (dst_g   & 0x00FF00)) * src_alpha / 256 + dst_g)   & 0x00FF00;
    // res_rb now contains the red & blue hues of the pre-final color
    dst_col = (((src_col & 0xFF00FF) - (dst_col & 0xFF00FF)) * src_alpha / 256 + dst_col) & 0xFF00FF;

    // dst_alpha now contains the final alpha
    // we assume that final alpha will never be zero
    dst_alpha  = 256 - (256 - src_alpha) * (256 - dst_alpha) / 256;
    // src_alpha is now the final alpha factor made for being multiplied by,
    // instead of divided by: this makes it possible to use it in faster
    // calculation below
    src_alpha  = /* 256 * 256 == */ 0x10000 / dst_alpha;

    // setting up final color hues
    dst_g   = (dst_g   * src_alpha / 256) & 0x00FF00;
    dst_col = (dst_col * src_alpha / 256) & 0xFF00FF;
    return dst_col | dst_g | (--dst_alpha << 24);
}

// blend source to destination with respect to source and destination alphas;
// assign new alpha value as a multiplication of translucenses.
// combined_alpha = front.alpha + back.alpha * (1 - front.alpha);
// combined_rgb = (front.rgb * front.alpha + back.rgb * (1 - front.alpha) * back.alpha) / combined_alpha;
unsigned long _argb2argb_blender(unsigned long src_col, unsigned long dst_col, unsigned long src_alpha)
{
    if (src_alpha > 0)
        src_alpha = geta32(src_col) * ((src_alpha & 0xFF) + 1) / 256;
    else
        src_alpha = geta32(src_col);
    if (src_alpha == 0)
        return dst_col;
    return argb2argb_blend_core(src_col, dst_col, src_alpha);
}

unsigned long _rgb2argb_blender(unsigned long src_col, unsigned long dst_col, unsigned long src_alpha)
{
    if (src_alpha == 0 || src_alpha == 0xFF)
        return src_col | 0xFF000000;
    return argb2argb_blend_core(src_col | 0xFF000000, dst_col, src_alpha);
}

unsigned long _argb2rgb_blender(unsigned long src_col, unsigned long dst_col, unsigned long src_alpha)
{
   unsigned long res, g;

   if (src_alpha > 0)
        src_alpha = geta32(src_col) * ((src_alpha & 0xFF) + 1) / 256;
    else
        src_alpha = geta32(src_col);
   if (src_alpha)
      src_alpha++;

   res = ((src_col & 0xFF00FF) - (dst_col & 0xFF00FF)) * src_alpha / 256 + dst_col;
   dst_col &= 0xFF00;
   src_col &= 0xFF00;
   g = (src_col - dst_col) * src_alpha / 256 + dst_col;

   res &= 0xFF00FF;
   g &= 0xFF00;

   return res | g;
}

void set_additive_alpha_blender()
{
    set_blender_mode(nullptr, nullptr, _additive_alpha_copysrc_blender, 0, 0, 0, 0);
}

void set_argb2argb_blender(int alpha)
{
    set_blender_mode(nullptr, nullptr, _argb2argb_blender, 0, 0, 0, alpha);
}

// sets the alpha channel to opaque. used when drawing a non-alpha sprite onto an alpha-sprite
unsigned long _opaque_alpha_blender(unsigned long x, unsigned long y, unsigned long n)
{
    return x | 0xff000000;
}

void set_opaque_alpha_blender()
{
    set_blender_mode(nullptr, nullptr, _opaque_alpha_blender, 0, 0, 0, 0);
}
