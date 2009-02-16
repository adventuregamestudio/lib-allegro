/*         ______   ___    ___ 
 *        /\  _  \ /\_ \  /\_ \ 
 *        \ \ \L\ \\//\ \ \//\ \      __     __   _ __   ___ 
 *         \ \  __ \ \ \ \  \ \ \   /'__`\ /'_ `\/\`'__\/ __`\
 *          \ \ \/\ \ \_\ \_ \_\ \_/\  __//\ \L\ \ \ \//\ \L\ \
 *           \ \_\ \_\/\____\/\____\ \____\ \____ \ \_\\ \____/
 *            \/_/\/_/\/____/\/____/\/____/\/___L\ \/_/ \/___/
 *                                           /\____/
 *                                           \_/__/
 *
 *      OpenGL drawing routines
 *
 *      By Elias Pschernig.
 */

#include "allegro5/allegro5.h"
#include "allegro5/a5_opengl.h"
#include "allegro5/internal/aintern_display.h"
#include "allegro5/internal/aintern_opengl.h"



static void set_opengl_blending(ALLEGRO_COLOR *color)
{
   const int blend_modes[4] = {
      GL_ZERO, GL_ONE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA
   };
   ALLEGRO_COLOR *bc;
   int src_color, dst_color, src_alpha, dst_alpha;
   float r, g, b, a;

   al_unmap_rgba_f(*color, &r, &g, &b, &a);

   al_get_separate_blender(&src_color, &dst_color, &src_alpha,
      &dst_alpha, NULL);
   glEnable(GL_BLEND);
   glBlendFuncSeparate(blend_modes[src_color], blend_modes[dst_color],
      blend_modes[src_alpha], blend_modes[dst_alpha]);
   bc = _al_get_blend_color();
   glColor4f(r * bc->r, g * bc->g, b * bc->b, a * bc->a);
}



/* Dummy implementation of clear. */
static void ogl_clear(ALLEGRO_DISPLAY *d, ALLEGRO_COLOR *color)
{
   ALLEGRO_DISPLAY *ogl_disp = (void *)d;
   ALLEGRO_BITMAP *target = al_get_target_bitmap();
   ALLEGRO_BITMAP_OGL *ogl_target = (void *)target;
   unsigned char r, g, b, a;

   if ((!ogl_target->is_backbuffer && ogl_disp->ogl_extras->opengl_target != ogl_target)
     || target->locked) {
      _al_clear_memory(color);
      return;
   }

   al_unmap_rgba(*color, &r, &g, &b, &a);

   glClearColor(r / 255.0, g / 255.0, b / 255.0, a / 255.0);
   glClear(GL_COLOR_BUFFER_BIT);
}



static void ogl_draw_pixel(ALLEGRO_DISPLAY *d, float x, float y,
   ALLEGRO_COLOR *color)
{
   ALLEGRO_DISPLAY *ogl_disp = (void *)d;
   ALLEGRO_BITMAP *target = al_get_target_bitmap();
   ALLEGRO_BITMAP_OGL *ogl_target = (void *)target;

   if ((!ogl_target->is_backbuffer && ogl_disp->ogl_extras->opengl_target != ogl_target)
      || target->locked)  {
      _al_draw_pixel_memory(target, x, y, color);
      return;
   }

   /* For sub bitmaps. */
   if (target->parent) {
      x += target->xofs;
      y += target->yofs;
   }

   set_opengl_blending(color);
   glBegin(GL_POINTS);
   glVertex2d(x, y);
   glEnd();
}



/* Add drawing commands to the vtable. */
void _al_ogl_add_drawing_functions(ALLEGRO_DISPLAY_INTERFACE *vt)
{
   vt->clear = ogl_clear;
   vt->draw_pixel = ogl_draw_pixel;
}

/* vim: set sts=3 sw=3 et: */
