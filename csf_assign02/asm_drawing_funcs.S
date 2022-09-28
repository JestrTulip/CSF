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
	subq $8, %rsp 

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
	subq $8, %rsp
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
	subq $8, %rsp

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
	subq $8, %rsp
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
	subq $8, %rsp
	movl %edi, %eax /*move 8 bit red component to the return register eax*/
	shr $16, %eax /*shift color right 24 bits (final 8 bits are red component)*/
	and $255, %eax /*XOR shifted color with 255 (only red component is kept in edi)*/
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
	subq $8, %rsp
	movl %edi, %eax /*move 8 bit red component to the return register eax*/
	shr $8, %eax /*shift color right 24 bits (final 8 bits are red component)*/
	and $255, %eax /*XOR shifted color with 255 (only red component is kept in edi)*/
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
	movl %edi, %eax /*move 8 bit red component to the return register eax*/
	and $255, %eax /*XOR shifted color with 255 (only red component is kept in edi)*/
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
	
	subq $8, %rsp
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15
	movl %edi, %r12d
	movl %esi, %r13d
	imul %edx, %r12d /*multiply alpha and foreground component and put it in rdi*/
	movl $255, %r14d /*move 255 into r10*/
	subl %edx, %r14d /*subtract alpha from 255 and put it in r10*/
	imul %r13d, %r14d /*multiply (255-alpha) and background component and put it in esi*/
	addl %r14d, %r12d /*add (alpha * fg) and (255-alpha)*bg and put it in esi*/
	pushq %rdx
	movl %r12d, %eax /*put (alpha * fg) + (255-alpha)*bg into edx*/
	movl $0, %edx
	movl $255, %r15d
	idiv %r15d /*divide (alpha * fg) + (255-alpha)*bg by 255 and put it in eax*/
	popq %rdx
	popq %r15
	popq %r14
	popq %r13
	popq %r12
	addq $8, %rsp /*restore stack pointer to original location*/
	ret

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
	subq $8, %rsp /*move the stack pointer to  make 24 bits of space*/
	call get_a /*get alpha value passing foreground color in %edi as an argument*/
	movl %eax, %edx
	call get_b /*get blue component passing foreground color in %edi as an argument*/
	movl %eax, %r12d
	call get_g /*get green component passing foreground color in %edi as an argument*/
	movl %eax, %r13d
	call get_r /*get red component passing foreground color in %edi as an argument*/
	movl %eax, %r14d

	movl %esi, %edi
	call get_b /*get blue component passing foreground color in %edi as an argument*/
	movl %eax, %r15d
	call get_g /*get green component passing foreground color in %edi as an argument*/
	movl %eax, %ebx
	call get_r /*get red component passing foreground color in %edi as an argument*/
	
	movl %eax, %esi
	movl %r14d, %edi
	call blend_components
	pushq %rax

	movl %ebx, %esi
	movl %r13d, %edi
	call blend_components
	pushq %rax

	movl %ebx, %esi
	movl %r13d, %edi
	call blend_components
	movl %eax, %ebp

	movl $255, %eax
	shl $8, %ebp
	or %ebp, %eax

	popq %rbp
	shl $16, %ebp
	or %ebp, %eax

	popq %rbp
	shl $24, %ebp
	or %ebp, %eax

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
	/* TODO: implement */
	ret


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
