/*
 * Assembly-language implementations of the drawing functions
 */

/* Offsets of struct Image fields */
#define IMAGE_WIDTH_OFFSET   0
#define IMAGE_HEIGHT_OFFSET  4
#define IMAGE_DATA_OFFSET    8

/* Offsets of struct Rect fields */
#define RECT_X_OFFSET        0
#define RECT_Y_OFFSET        4
#define RECT_WIDTH_OFFSET    8
#define RECT_HEIGHT_OFFSET   12

	.section .text

/***********************************************************************
   Helper functions
   You should declare these as .globl so that
   they can be called from the unit test code.
 ***********************************************************************/

/*
 * Check if coordinates are within bounds of an image
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %esi     - x coordinate (pixel column)
 *   %edx     - y coordinate (pixel row)
 */
	.globl in_bounds
in_bounds: 
	subq $8, %rsp /*align stack pointer for procedure calls*/

	movl IMAGE_WIDTH_OFFSET(%rdi), %r12d /*store image width in r10*/ 
	movl IMAGE_HEIGHT_OFFSET(%rdi), %r13d /*store image height r11*/
	
	cmpl %r12d, %esi /*if the x coordinate > image width jump to out_of_bounds*/
	jge out_of_bounds

	cmpl %r13d, %edx /*if the y coordinate > image height jump to out_of_bounds*/
	jge out_of_bounds

	movl $1, %eax /*if coordinates are within the image move 1 to return register eax*/
	addq $8, %rsp  /*restore stack pointer to original location*/

	ret /*return 1 for point being in bounds*/

out_of_bounds: 
	movl $0, %eax /*if coordinates are out of the image move 0 to return register eax*/
	addq $8, %rsp /*restore stack pointer to original location*/
	ret /*return 0 for point being out of bounds*/

/*
 * Convert coordinates of an image to the index where the data for those coordinates are
 * located
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %esi     - x coordinate (pixel column)
 *   %edx     - y coordinate (pixel row)
 */
	.globl compute_index
compute_index:
	subq $8, %rsp /*align stack pointer for procedure calls*/
	movl IMAGE_WIDTH_OFFSET(%rdi), %eax /*move pointer to the image width value*/
	imul (%esi), %eax /*multiply image width by x coordinate*/
	addl (%edx), %eax /*add y coordinate to the product of the image width and x coordinate*/
	addq $8, %rsp  /*restore stack pointer to original location*/
	ret


/*
 * Restrict a given value so it is always within a determined range
 *
 * Parameters:
 *   %edi     - value to be clamped
 *   %esi     - minimum value
 *   %edx     - maximum value
 */
	.globl clamp
clamp:
	subq $8, %rsp /*align stack pointer for procedure calls*/

	cmp %edx, %edi /*if value greater than max jump to clamp_to_max*/
	jg clamp_to_max

	cmp %esi, %edi /*if value less than than jump to clamp_to_min*/
	jl clamp_to_min

	movl (%edi), %eax /*if value within min-max move the value to the return register eax*/
	addq $8, %rsp /*restore stack pointer to original location*/
	ret /*return the value*/

	clamp_to_max:
	movl (%edx), %eax /*move max to return register eax as value must be clamped to max*/
	addq $8, %rsp /*restore stack pointer to original location*/
	ret /*return clamped maximum value*/

	clamp_to_min:
	movl (%esi), %eax /*move min to return register eax as value must be clamped to min*/
	addq $8, %rsp /*restore stack pointer to original location*/
	ret /*return clamped minimum value*/

/*
 * Get the 8 bit red component of a color stored in a 32 bit integer
 *
 * Parameters:
 *   %edi     - color
 */
	.globl get_r
get_r:
	subq $8, %rsp /*align stack pointer for procedure calls*/
	movl %edi, %eax /*move 8 bit red component to the return register eax*/
	shr $24, %eax /*shift color right 24 bits (final 8 bits are red component)*/
	and $255, %eax /*XOR shifted color with 255 (only red component is kept in edi)*/
	addq $8, %rsp /*restore stack pointer to original location*/
	ret

/*
 * Get the 8 bit green component of a color stored in a 32 bit integer
 *
 * Parameters:
 *   %edi     - color
 */
	.globl get_g
get_g:
	subq $8, %rsp /*align stack pointer for procedure calls*/
	movl %edi, %eax /*move 8 bit green component to the return register eax*/
	shr $16, %eax /*shift color right 16 bits (final 8 bits are green component)*/
	and $255, %eax /*XOR shifted color with 255 (only green component is kept in edi)*/
	addq $8, %rsp /*restore stack pointer to original location*/
	ret


/*
 * Get the 8 bit blue component of a color stored in a 32 bit integer
 *
 * Parameters:
 *   %edi     - color
 */
	.globl get_b
get_b:
	subq $8, %rsp /*align stack pointer for procedure calls*/
	movl %edi, %eax /*move 8 bit blue component to the return register eax*/
	shr $8, %eax /*shift color right 8 bits (final 8 bits are blue component)*/
	and $255, %eax /*XOR shifted color with 255 (only blue component is kept in edi)*/
	addq $8, %rsp /*restore stack pointer to original location*/
	ret

/*
 * Get the 8 bit alpha component of a color stored in a 32 bit integer
 *
 * Parameters:
 *   %edi     - color
 */
	.globl get_a
get_a:
	subq $8, %rsp
	movl %edi, %eax /*move 8 bit alpha component to the return register eax*/
	and $255, %eax /*XOR shifted color with 255 (only alpha component is kept in edi)*/
	addq $8, %rsp /*restore stack pointer to original location*/
	ret


/*
 * Calculate the blended component of the foreground and background based on a certain alpha 
 * value and predetermined equation: (alpha * fg + (255 - alpha) * bg) / 255;
 *
 * Parameters:
 *   %edi     - foreground component
 *   %esi     - background component
 *   %edx     - alpha value
 */
	.globl blend_components
blend_components:
	
	subq $8, %rsp /*align stack pointer for procedure calls*/

	/*push r12, r13, r14, and r15 registers to preserve values*/

	imul %edx, %edi /*multiply alpha and foreground component and put it in rdi*/
	movl $255, %r10d /*move 255 into r10*/
	subl %edx, %r10d /*subtract alpha from 255 and put it in r10*/
	imul %esi, %r10d /*multiply (255-alpha) and background component and put it in esi*/
	addl %r10d, %edi /*add (alpha * fg) and (255-alpha)*bg and put it in esi*/
	movl %r12d, %eax /*put (alpha * fg) + (255-alpha)*bg into edx*/
	
	pushq %rdx /*push rdx registers to preserve value*/
	movl $0, %edx /*move 0 into edx representing first 4 bytes of dividend*/
	movl $255, %r11d /*move 255 into r11 representing divisor*/
	idiv %r15d /*divide (alpha * fg) + (255-alpha)*bg by 255 and put it in eax*/
	
	/*pop rdx registers to preserve value*/
	popq %rdx

	addq $8, %rsp /*restore stack pointer to original location*/
	ret /*return lended color value in eax*/

/*
 * Calculate the blended color of the foreground and background based on a predetermined
 * equation
 *
 * Parameters:
 *   %edi     - foreground color
 *   %esi     - background color
 */
	.globl blend_colors
blend_colors:
	/*Resgister usage
	 * %edi - fg color, bg color, fg red, fg green, fg blue
	 * %esi - bg color, bg red, bg green, bg blue
	 * %edx - fg alpha
	 * %r12d - fg blue
	 * %r13d - fg green
	 * %r14d - fg red
	 * %r15d - bg blue
	 * %ebx - bg green
	 * %ebp - blended blue, blended green, blended red
	 * %eax - fg alpha, fg blue, fg green, fg red, bg blue, bg green
	 *        blended red, blended green, blended blue
	 		  blended alpha, blended bluealpha, blended greenbluealpha, blended redgreenbluealpha
	 */
	subq $8, %rsp /*align stack pointer for procedure calls*/
	call get_a /*get alpha value passing foreground color in %edi as an argument*/
	movl %eax, %edx /* move alpha value to edx*/
	call get_b /*get blue component passing foreground color in %edi as an argument*/
	movl %eax, %r12d /* move foreground blue value to r12d*/
	call get_g /*get green component passing foreground color in %edi as an argument*/
	movl %eax, %r13d /* move foreground green value to r13d*/
	call get_r /*get red component passing foreground color in %edi as an argument*/
	movl %eax, %r14d /* move foreground red value to r14d*/

	movl %esi, %edi /* move background color to girst argument register edi*/
	call get_b /*get blue component passing background color in %edi as an argument*/
	movl %eax, %r15d /* move foreground blue value to r15d*/
	call get_g /*get green component passing background color in %edi as an argument*/
	movl %eax, %ebx /* move foreground green value to ebx*/
	call get_r /*get red component passing background color in %edi as an argument*/
	
	movl %eax, %esi /* move background red value to second argument register esi*/
	movl %r14d, %edi /* move foreground red value to first argument register edi*/
	call blend_components /* get blended red color passing fg red, bg red, and fg alpha as parameters*/
	pushq %rax /* push blended red color to stack*/

	movl %ebx, %esi /* move background green value to second argument register esi*/
	movl %r13d, %edi /* move foreground green value to first argument register edi*/
	call blend_components /* get blended green color passing fg green, bg green, and fg alpha as parameters*/
	pushq %rax /* push blended green color to stack*/

	movl %ebx, %esi /* move background blue value to second argument register esi*/
	movl %r13d, %edi /* move foreground blue value to first argument register edi*/
	call blend_components /* get blended blue color passing fg blue, bg blue, and fg alpha as parameters*/
	movl %eax, %ebp /* move blended blue color to stack*/

	movl $255, %eax /* move blended alpha value into eax*/
	shl $8, %ebp /* shift blended blue left 8 bits*/
	or %ebp, %eax /* or blended alpha and blue together*/

	popq %rbp /* pop blended green color to rbp*/
	shl $16, %ebp /* shift blended bluealpha left 8 bits*/
	or %ebp, %eax /* or blended bluealpha and green together*/

	popq %rbp /* push blended red color to rbp*/
	shl $24, %ebp /* shift blended greenbluealpha left 8 bits*/
	or %ebp, %eax /* or blended greenbluealpha and red together*/

	addq $8, %rsp /*restore stack pointer to original location*/
	ret /*return gully blended color in return register eax*/

/*
 * Set pixel in image to new color
 *
 * Parameters:
 *   %rdi     - pointer to image
 *   %esi     - index of pixel to be changed
 *   %edx    - color for pixel to be changed to
 */
	.globl set_pixel
set_pixel:
	subq $8, %rsp /*align stack pointer for procedure calls*/

	addl IMAGE_DATA_OFFSET(%rdi), %esi /*move image pointer to index of pixel to be changed*/
	movl %edx, (%rdi) /*replace old color with the new color*/

	addq $8, %rsp /*restore stack pointer to original location*/
	ret



/***********************************************************************
   Public API functions
 ***********************************************************************/

/*
 * Draw a pixel.
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %esi     - x coordinate (pixel column)
 *   %edx     - y coordinate (pixel row)
 *   %ecx     - uint32_t color value
 */
	.globl draw_pixel
draw_pixel:
	subq $8, %rsp /*align stack pointer for procedure calls*/

	call in_bounds /*call in_bound with image pointer, x coord, and y coord as parameters*/
	cmp $0, %eax /*compare in_bounds return value with zero*/
	je not_valid_pixel /*if in_bounds return equal to zero jump to not_valid pixel*/

	call compute_index /*call compute index with image pointer, x coord, and y coord as parameters*/

	movl %eax, %esi /*move index to 2nd argument register esi*/
	movl %ecx, %edi /*move color value to 3rd argument register edi*/
	call set_pixel /*call set pixel with image pointer, index, and color value as parameters*/
	addq $8, %rsp /*restore stack pointer to original location*/
	ret

not_valid_pixel:
	addq $8, %rsp /*restore stack pointer to original location*/
	ret /*pixel is not in image so return with no changes made to image*/


/*
 * Draw a rectangle.
 * The rectangle has rect->x,rect->y as its upper left corner,
 * is rect->width pixels wide, and rect->height pixels high.
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %rsi     - pointer to struct Rect
 *   %edx     - uint32_t color value
 */
	.globl draw_rect
draw_rect:
	/* TODO: implement */
	ret

/*
 * Draw a circle.
 * The circle has x,y as its center and has r as its radius.
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %esi     - x coordinate of circle's center
 *   %edx     - y coordinate of circle's center
 *   %ecx     - radius of circle
 *   %r8d     - uint32_t color value
 */
	.globl draw_circle
draw_circle:
	/* TODO: implement */
	ret

/*
 * Draw a tile by copying all pixels in the region
 * enclosed by the tile parameter in the tilemap image
 * to the specified x/y coordinates of the destination image.
 * No blending of the tile pixel colors with the background
 * colors should be done.
 *
 * Parameters:
 *   %rdi - pointer to Image (dest image)
 *   %esi - x coordinate of location where tile should be copied
 *   %edx - y coordinate of location where tile should be copied
 *   %rcx - pointer to Image (the tilemap)
 *   %r8  - pointer to Rect (the tile)
 */
	.globl draw_tile
draw_tile:
	/* TODO: implement */
	ret

/*
 * Draw a sprite by copying all pixels in the region
 * enclosed by the sprite parameter in the spritemap image
 * to the specified x/y coordinates of the destination image.
 * The alpha values of the sprite pixels should be used to
 * blend the sprite pixel colors with the background
 * pixel colors.
 *
 * Parameters:
 *   %rdi - pointer to Image (dest image)
 *   %esi - x coordinate of location where sprite should be copied
 *   %edx - y coordinate of location where sprite should be copied
 *   %rcx - pointer to Image (the spritemap)
 *   %r8  - pointer to Rect (the sprite)
 */
	.globl draw_sprite
draw_sprite:
	/* TODO: implement */
	ret

/*
vim:ft=gas:
*/
