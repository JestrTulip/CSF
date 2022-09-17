// C implementations of drawing functions (and helper functions)

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "drawing_funcs.h"

////////////////////////////////////////////////////////////////////////
// Helper functions
////////////////////////////////////////////////////////////////////////

// TODO: implement helper functions
int32_t in_bounds(struct Image *img, int32_t x, int32_t y){
  if((x < 0 || x >= img->width - 1) || (y < 0 || y >= img->height - 1)){
    return 0;
  }
  return 1;
}


uint32_t compute_index(struct Image *img, int32_t x, int32_t y){
  return x + (y * img->width);
}

int32_t clamp(int32_t val, int32_t min, int32_t max){
  int32_t min_diff = abs(min - val);
  int32_t max_diff = abs(max - val); 

  if (min_diff < max_diff) { 
    val = min; 
    return val; 
  } else { 
    val = max; 
  }
  return val; 
} 

uint8_t get_r(uint32_t color){
  uint32_t r = (color >> 24) & 0xFF; 
  return r;
}
uint8_t get_g(uint32_t color){
  uint32_t r = (color >> 16) & 0xFF; 
  return r;
}
uint8_t get_b(uint32_t color){
  uint8_t r = (color >> 8) & 0xFF; 
  return r;
}
uint8_t get_a(uint32_t color){
  uint8_t r = (color >> 0) & 0xFF; 
  return r;
}
uint8_t blend_components(uint32_t fg, uint32_t bg, uint8_t alpha){
  return (alpha * fg + (255 - alpha) * bg) / 255;
}
uint32_t blend_colors(uint32_t fg, uint32_t bg){
  uint32_t red = blend_components(get_r(fg), get_r(bg), get_a(fg))
  uint32_t red = blend_components(get_r(fg), get_r(bg), get_a(fg))
  uint32_t red = blend_components(get_r(fg), get_r(bg), get_a(fg))
  return (((((alpha << 8) | blue) << 8) | green) << 8) | red;
}
void set_pixel(struct Image *img, uint32_t index, uint32_t color){
  blend_colors(color, img[index]);
}
int64_t square(int64_t x){
  return x * x;
}
int64_t square_dist(int64_t x1, int64_t y1, int64_t x2, int64_t y2){
  int64_t x_diff;
  int64_t y_diff;
  if(x2 > x1){
    x_diff = x2 - x1;
  } else {
    x_diff = x1 - x2;
  }
  if(y2 > y1){
    y_diff = y2 - y1;
  } else {
    y_diff = y1 - y2;
  }
  return square(x_diff) + square(y_diff);
}
////////////////////////////////////////////////////////////////////////
// API functions
////////////////////////////////////////////////////////////////////////

//
// Draw a pixel.
//
// Parameters:
//   img   - pointer to struct Image
//   x     - x coordinate (pixel column)
//   y     - y coordinate (pixel row)
//   color - uint32_t color value
//
void draw_pixel(struct Image *img, int32_t x, int32_t y, uint32_t color) {
  
  if(!in_bounds(img, x, y)){ return;}

  set_pixel(img, compute_index(img, x, y), color);
}

//
// Draw a rectangle.
// The rectangle has rect->x,rect->y as its upper left corner,
// is rect->width pixels wide, and rect->height pixels high.
//
// Parameters:
//   img     - pointer to struct Image
//   rect    - pointer to struct Rect
//   color   - uint32_t color value
//
void draw_rect(struct Image *img,
               const struct Rect *rect,
               uint32_t color) {
  for(int i = 0; i < img->width; i++){
    for(int j = 0; j < img->height; j++){
      draw_pixel(img, clamp(i, rect->x, rect->x + rect->width), clamp(j, rect->y, rect->y + rect->width), color);
    }
  }
}

//
// Draw a circle.
// The circle has x,y as its center and has r as its radius.
//
// Parameters:
//   img     - pointer to struct Image
//   x       - x coordinate of circle's center
//   y       - y coordinate of circle's center
//   r       - radius of circle
//   color   - uint32_t color value
//
void draw_circle(struct Image *img,
                 int32_t x, int32_t y, int32_t r,
                 uint32_t color) {
  for(int i = 0; i < img->width; i++){
    for(int j = 0; j < img->height; j++){
      draw_pixel(img, clamp(square_dist(i, x), 0, square(r)), clamp(square_dist(j, y), 0, square(r)), color);
    }
  }

//
// Draw a tile by copying all pixels in the region
// enclosed by the tile parameter in the tilemap image
// to the specified x/y coordinates of the destination image.
// No blending of the tile pixel colors with the background
// colors should be done.
//
// Parameters:
//   img     - pointer to Image (dest image)
//   x       - x coordinate of location where tile should be copied
//   y       - y coordinate of location where tile should be copied
//   tilemap - pointer to Image (the tilemap)
//   tile    - pointer to Rect (the tile)
//
void draw_tile(struct Image *img,
               int32_t x, int32_t y,
               struct Image *tilemap,
               const struct Rect *tile) {
 if(!in_bounds(tilemap, tile->x, tile->y) || 
    !in_bounds(tilemap, tile->x, tile->y + tile->height) || 
    !in_bounds(tilemap, tile->x + tile->width, tile->y) || 
    !in_bounds(tilemap, tile->x + tile->width, tile->y + tile->height)){
    return;
  }
  for(int i = 0; i < tile->width; ++i){
    for(int j = 0; j < tile->height; ++j){
      if(in_bounds(img, i, j)){
        color = tilemap[compute_index(tilemap, i + tile->x, j + tile->y)]
        img[compute_index(img, i + x, j + y)] = color;
      }
  }
}

//
// Draw a sprite by copying all pixels in the region
// enclosed by the sprite parameter in the spritemap image
// to the specified x/y coordinates of the destination image.
// The alpha values of the sprite pixels should be used to
// blend the sprite pixel colors with the background
// pixel colors.
//
// Parameters:
//   img       - pointer to Image (dest image)
//   x         - x coordinate of location where sprite should be copied
//   y         - y coordinate of location where sprite should be copied
//   spritemap - pointer to Image (the spritemap)
//   sprite    - pointer to Rect (the sprite)
//
void draw_sprite(struct Image *img,
                 int32_t x, int32_t y,
                 struct Image *spritemap,
                 const struct Rect *sprite) {
  if(!in_bounds(spritemap, sprite->x, sprite->y) || 
    !in_bounds(spritemap, sprite->x, sprite->y + sprite->height) || 
    !in_bounds(spritemap, sprite->x + sprite->width, sprite->y) || 
    !in_bounds(spritemap, sprite->x + sprite->width, sprite->y + sprite->height)){
    return;
  }
  for(int i = 0; i < sprite->width; ++i){
    for(int j = 0; j < sprite->height; ++j){
      draw_pixel(img, i + x, j + y, spritemap[compute_index(spritemap, i + sprite->x, j + sprite->y)]);
    }
  }
}