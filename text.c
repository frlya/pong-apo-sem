#include "text.h"

void renderText(int state){
    //WIP
}

void drawText(int x, int y, char *text){
    //WIP
}


/*
*----------------------------------------------------
* Code below is taken from 8.lecture 
*----------------------------------------------------
*/

int char_width(int ch) {
  int width;
  if (!fdes->width) {
    width = fdes->maxwidth;
  } else {
    width = fdes->width[ch-fdes->firstchar];
  }
  return width;
}

/*
*   Adjusts scale of character.
*   Service function
*/
void draw_pixel_big(int x, int y, unsigned short color, unsigned short **fb, int scale) {
  int i,j;
  for (i=0; i<scale; i++) {
    for (j=0; j<scale; j++) {
      draw_pixel(x+i, y+j, color, fb);
    }
  }
}


/*
 *  Draws character on the screen.
 *
 *  int x, y             - coordinates of left upper corner of char box.
 *  char ch              - character or its code from font table
 *  unsigned short color - color of the char      
 */
void draw_char(int x, int y, char ch, unsigned short color, unsigned short **fb, int scale) {
  int w = char_width(ch);
  const font_bits_t *ptr;
  if ((ch >= fdes->firstchar) && (ch-fdes->firstchar < fdes->size)) {
    if (fdes->offset) {
      ptr = &fdes->bits[fdes->offset[ch-fdes->firstchar]];
    } else {
      int bw = (fdes->maxwidth+15)/16;
      ptr = &fdes->bits[(ch-fdes->firstchar)*bw*fdes->height];
    }
    int i, j;
    for (i=0; i<fdes->height; i++) {
      font_bits_t val = *ptr;
      for (j=0; j<w; j++) {
        if ((val&0x8000)!=0) {
          draw_pixel_big(x+scale*j, y+scale*i, color, fb, scale);
        }
        val<<=1;
      }
      ptr++;
    }
  }
}