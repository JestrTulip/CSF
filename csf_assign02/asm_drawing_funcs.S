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
	/*Resgister usage
	 * %rdi - image pointer
	 * %esi - x coordinate
	 * %edx - y coordinate
	 * %r12d - image width
	 * %r13d - image height
	 * %eax - 0 or 1
	 */
	subq $8, %rsp /*align stack pointer for procedure calls*/

	/*push r12 and r13 to preserve values*/
	pushq %r12
	pushq %r13

	movl IMAGE_WIDTH_OFFSET(%rdi), %r12d /*store image width in r10*/ 
	movl IMAGE_HEIGHT_OFFSET(%rdi), %r13d /*store image height r11*/
	
	cmpl %r12d, %esi /*if the x coordinate >= image width jump to out_of_bounds*/
	jge out_of_bounds

	cmpl %r13d, %edx /*if the y coordinate >= image height jump to out_of_bounds*/
	jge out_of_bounds

	movl $1, %eax /*if coordinates are within the image move 1 to return register eax*/
	
	/*pop r12 and r13 to restore values*/
	popq %r12 
	popq %r13 

	addq $8, %rsp  /*restore stack pointer to original location*/

	ret /*return 1 for point being in bounds*/

out_of_bounds: 
	movl $0, %eax /*if coordinates are out of the image move 0 to return register eax*/
	
	/*pop r12 and r13 to restore values*/
	popq %r12 
	popq %r13 

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
	imul %edx, %eax /*multiply image width by x coordinate*/
	addl %esi, %eax /*add y coordinate to the product of the image width and x coordinate*/
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

	movl %edi, %eax /*if value within min-max move the value to the return register eax*/
	addq $8, %rsp /*restore stack pointer to original location*/
	ret /*return the value*/

	clamp_to_max:
	movl %edx, %eax /*move max to return register eax as value must be clamped to max*/
	addq $8, %rsp /*restore stack pointer to original location*/
	ret /*return clamped maximum value*/

	clamp_to_min:
	movl %esi, %eax /*move min to return register eax as value must be clamped to min*/
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
	subq $8, %rsp /*align stack pointer for procedure calls*/
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
	/*Resgister usage
	 * %edi - fg
	 * %esi - bg
	 * %edx - alpha
	 * %r12d - fg, alpha * fg
	 * %r13d - bg
	 * %r14d - 255, 255 - alpha, (255 - alpha) * bg
	 * %r15d - 255
	 * %eax - (alpha * fg) + (255 - alpha) * bg, ((alpha * fg) + (255 - alpha) * bg)/255
	 */

	subq $8, %rsp /*align stack pointer for procedure calls*/

	/*push r12, r13, r14, r15, rdx to preserve values*/
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15
	pushq %rdx

	movl %edi, %r12d /*move foreground component to r12d*/
	movl %esi, %r13d /*move foreground component to r12d*/

	imul %edx, %r12d /*multiply alpha and foreground component and put it in r12d*/
	movl $255, %r14d /*move 255 into r14d*/
	subl %edx, %r14d /*subtract alpha from 255 and put it in r14d*/
	imul %r13d, %r14d /*multiply (255-alpha) and background component and put it in r14d*/
	addl %r14d, %r12d /*add (alpha*fg) and (255-alpha)*bg and put it in r12d*/
	movl %r12d, %eax /*move (alpha*fg) + (255-alpha)*bg into eax*/
	movl $0, %edx /*move 0 into edx in preperation for division*/
	movl $255, %r15d /*move 255 in r15d*/
	idiv %r15d /*divide (alpha * fg) + (255-alpha)*bg by 255 and put it in eax*/

	/*pop r12, r13, r14, r15, rdx to restore values*/
	popq %rdx
	popq %r15
	popq %r14
	popq %r13
	popq %r12

	addq $8, %rsp /*restore stack pointer to original location*/
	ret /*return blended color value in return register eax*/

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
	 * %edi - fg color, bg color
	 * %esi - bg color
	 * %edx - alpha
	 * %r12d - 0, red, redgreen, redgreenblue, redgreenbluealpha
	 * %r13d - fg red, fg green, fg blue
	 * %r14d - bg red, bg green, bg blue
	 * %r15d - fg color
	 * %ebp - bg color  
	 * %eax - alpha, fg red, bg red, blended red, fg green, bg green, blended green, fg blue, bg blue, blended blue, blended color
	 */
	
 

	subq $8, %rsp /*align stack pointer for procedure calls*/
	
	/*push r12, r13, r14, r15, rbp to preserve values*/
	pushq %r14
	pushq %r13
	pushq %r12
	pushq %r15
	pushq %rbp

	movl %edi, %r15d /*move fg color to r15d*/
	movl %esi, %ebp /*move bg color to ebp*/

	movl $0, %r12d /*move 0 to r12d*/

	call get_a /*call get_a with fg color as paremter*/
	movl %eax, %edx /*move alpha to edx*/

	call get_r /*call get_r with fg color as parameter*/
	movl %eax, %r13d /*move fg red to r13d*/
	movl %esi, %edi /*move bg color to first argument register edi*/
	call get_r /*call get_r with bg color as parameter*/
	movl %eax, %r14d /*move bg red to r14d*/

	movl %r13d, %edi /*move fg red to first argument register edi*/
	movl %r14d, %esi /*move bg red to second argument register esi*/

	call blend_components /*call blend components with fg red and bg red as parameters*/
	movb %al, %r12b /*move blended red color r12b*/

	movl %r15d, %edi /*move fg color to first argument register edi*/
	movl %ebp, %esi /*move bg color to second argument register esi*/

	call get_g /*call get_g with fg color as parameter*/
	movl %eax, %r13d /*move fg green to r13d*/
	movl %esi, %edi /*move bg color to first argument register edi*/
	call get_g /*call get_g with bg color as parameter*/
	movl %eax, %r14d /*move bg green to r14d*/

	movl %r13d, %edi /*move fg green to first argument register edi*/
	movl %r14d, %esi /*move bg green to second argument register esi*/

	call blend_components /*call blend components with fg green and bg green as parameters*/
	shl $8, %r12 /*shift blended red left 8 bits*/
	movb %al, %r12b /*move blended green color r12b, r12d now contains redgreen*/

	movl %r15d, %edi /*move fg color to first argument register edi*/
	movl %ebp, %esi /*move bg color to second argument register esi*/

	call get_b /*call get_b with fg color as parameter*/
	movl %eax, %r13d /*move fg blue to r13d*/
	movl %esi, %edi /*move bg color to first argument register edi*/
	call get_b /*call get_b with bg color as parameter*/
	movl %eax, %r14d  /*move bg blue to r14d*/

	movl %r13d, %edi /*move fg blue to first argument register edi*/
	movl %r14d, %esi /*move bg blue to second argument register esi*/

	call blend_components /*call blend components with fg blue and bg blue as parameters*/
	shl $8, %r12 /*shift blended redgreen left 8 bits*/
	movb %al, %r12b /*move blended blue color r12b, r12 now contains redbluegreen*/

	shl $8, %r12 /*shift blended redgreen left 8 bits*/
	movb $255, %r12b /*move 255 (alpha value) to r12b, r12 now contains redbluegreenalpha*/

	movl %r12d, %eax /*move r12 to return register eax*/

	/*pop r12, r13, r14, r15, rbp to restore values*/
	popq %rbp
	popq %r15
	popq %r12
	popq %r13
	popq %r14

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
	/*Resgister usage
	 * %rdi - image pointer, foreground color
	 * %esi - pixel index, background color
	 * %edx - foreground color
	 * %r12 - image pointer, image data array
	 * %r13d - zero, pixel index
	 * %eax - blended color
	 */
	subq $8, %rsp /*align stack pointer for procedure calls*/

	movq %rdi, %r12 /*move image pointer to r12*/

	movq $0, %r13  /*move 0 to r13 to ensure all 64 bits are 0*/
	movl %esi, %r13d /*move pixel index to r13*/


	movq IMAGE_DATA_OFFSET(%r12), %r12 /*replace r12 with pointer to image data*/
	movl (%r12, %r13, 4), %esi /*move bacground color to second argument register esi*/
	movl %edx, %edi /*move foreground color to first argument register edi*/
	call blend_colors /*call blend color with fg and bg colors as parameters*/

	movl %eax, (%r12, %r13, 4) /*move blended color to pixel location*/

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
	/*Resgister usage
	 * %rdi - image pointer, 
	 * %esi - x coordinate, index
	 * %edx - y coordinate, color
	 * %ecx - color
	 * %eax - pixel index
	 */
	subq $8, %rsp /*align stack pointer for procedure calls*/

	call in_bounds /*call in_bound with image pointer, x coord, and y coord as parameters*/
	cmp $0, %eax /*compare in_bounds return value with zero*/
	je not_valid_pixel /*if in_bounds return equal to zero jump to not_valid pixel*/

	call compute_index /*call compute index with image pointer, x coord, and y coord as parameters*/

	movl %eax, %esi /*move index to 2nd argument register esi*/
	movl %ecx, %edx /*move color value to 3rd argument register edi*/
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
