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

	cmpl $0, %esi /*if the x coordinate is less than 0, jump to out_of_bounds*/
	jl out_of_bounds

	cmpl $0, %edx /*if the y coordinate is less than 0, jump to out_of_bounds*/
	jl out_of_bounds

	movl $1, %eax /*if coordinates are within the image move 1 to return register eax*/
	
	/*pop r12 and r13 to restore values*/
	popq %r13 
	popq %r12 

	addq $8, %rsp  /*restore stack pointer to original location*/

	ret /*return 1 for point being in bounds*/

out_of_bounds: 
	movl $0, %eax /*if coordinates are out of the image move 0 to return register eax*/
	
	/*pop r12 and r13 to restore values*/
	popq %r13
	popq %r12 

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

	pushq %r12 
	pushq %r13

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
	
	
	popq %r13
	popq %r12
	ret
/*
 * Calculate the square of an integer
 *
 * Parameters:
 *   %rdi     - integer to be squared
 */
	.globl square
square:
	subq $8, %rsp /*align stack pointer for procedure calls*/
	imul %edi, %edi /*square first argument*/
	movl %edi, %eax /*move squared argument to return register eax*/
	addq $8, %rsp /*restore stack pointer to original location*/
	ret


/*
 * Calculate the squared distance between two points
 *
 * Parameters:
 * %edi     - x1
 * %esi     - y1
 * %edx     - x2
 * %ecx     - y2
 */
	.globl square_dist
square_dist:
/* Register use:
 * %edi - x1, x1x2 difference, y1y2 difference
 * %esi - y1
 * %edx - x2
 * %ecx - y2
 * %r10 - x1x2 difference, x1x2 squared difference
 * %r11 - y1y2 difference
 * %eax - x1x2 squared difference, y1y2 squared difference, total squared difference
 */
	
	/* push r12 and r13 to preserve values*/
	push %r12
	push %r13
	subq $8, %rsp 

	movq %rsi, %r12 /*move y1 to r12*/
	movq %rcx, %r13 /* move y2 to r13*/

	subq %rdi, %rdx /*subtract x2-x1, store in rdx*/ 
	movq %rdx, %rdi /*move result of substraction to first argument register rdi*/ 
	call square /*call square with x2-x1*/ 

	subq %r12, %r13 /*subtract y2-y1, store in r13*/ 
	movq %rax, %r12 /*move squared result of substraction to r12*/ 
	movq %r13, %rdi /*move result of substraction to first argument register rdi*/ 
	call square /*call square with y2-y1*/ 

	addq %r12, %rax /*add results together and move to rax*/ 

	addq $8, %rsp /*restore stack pointer to original location*/ 
	/*pop callee-saved registers*/ 
	popq %r13
	popq %r12 
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
	/*Register usage
	 * %edi - image pointer, rect x, rect x + rect width, rect y, rect y + rect height, image pointer
	 * %esi - rect pointer, 0, x index
	 * %edx - color value, image width, image height, y index
	 * %ecx - color value
	 * %r12d - image pointer
	 * %r13d - rect pointer, color value
	 * %r14d - min x, x index
	 * %r15d - max x
	 * %ebx - min y
	 * %ebp - min y, y index
	 * %eax - min x, max x, min y, max y
	 */

	/* push r12, r13, r14, r15, ebx, and ebp to preserve values*/
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15
	pushq %rbx
	pushq %rbp

	subq $8, %rsp /*align stack pointer for procedure calls*/

	movq %rdi, %r12 /*move image pointer to r12*/
	movq %rsi, %r13 /*move rect pointer to r13*/
	push %rdx /*push color value so that it can be preserved for later*/

	movl RECT_X_OFFSET(%r13), %edi /*move rect x value to first argument register edi*/
	movq $0, %rsi /*move 0 to second argument register rsi*/
	movl IMAGE_WIDTH_OFFSET(%r12), %edx /*move image width to third argument register edx*/
	call clamp /*call clamp with rect x, 0, and image width as parameters*/
	movl %eax, %r14d /*store clamped minimum x value in r14d*/
	
	addq RECT_WIDTH_OFFSET(%r13), %rdi /*add rect width to rect x*/
	call clamp /*call clamp with rect x + rect width, 0, and image width as parameters*/
	movl %eax, %r15d /*store clamped maximum x value in r15d*/

	movl RECT_Y_OFFSET(%r13), %edi /*move rect y value to first argument register edi*/
	movl IMAGE_HEIGHT_OFFSET(%r12), %edx /*move image height to third argument register edx*/
	call clamp /*call clamp with rect y, 0, and image height as parameters*/
	movl %eax, %ebx /*store clamped minimum y value in ebx*/
	
	addq RECT_HEIGHT_OFFSET(%r13), %rdi /*add rect height to rect y*/
	call clamp /*call clamp with rect y + rect height, 0, and image height as parameters*/
	movl %eax, %ebp /*store clamped maximum y value in ebp*/

	pop %r13 /*pop color value into r13*/

.Lx_rectloop:
	cmp %r14d, %r15d /*compare x value index to max x*/
	jle .Lexit_x_rectloop /* exit for loop if max x is less than or equal to the x value index*/
	
	push %rbx /*push starting y index value so it can be restored for every iteration of the loop*/

.Ly_rectloop:
	cmp %ebx, %ebp /*compare y value index to max y*/
	jle .Lexit_y_rectloop /* exit for loop if max y is less than or equal to the x value index*/

	movq %r12, %rdi /*move image pointer to first argument register rdi*/
	movl %r14d, %esi /*move x index value to second argument register esi*/
	movl %ebx, %edx /*move y index value to third argument register edx*/
	movl %r13d, %ecx /*move color value to fourth argument register ecx*/
	call draw_pixel /*call draw pixel with image pointer, x index, y, index, and color as parameters*/
	inc %ebx /*increment y index by one*/
	jmp .Ly_rectloop /*begin next iteration of y loop*/

.Lexit_y_rectloop:
	inc %r14d /*increment x index by one as end of column has been reached */
	pop %rbx /*restore y index to min y value*/
	jmp .Lx_rectloop /*begin next iteration of x loop*/

.Lexit_x_rectloop:
	addq $8, %rsp  /*restore stack pointer to original location*/

	/* pop r12, r13, r14, r15, ebx, and ebp to restore values*/
	popq %rbp
	popq %rbx
	popq %r15
	popq %r14
	popq %r13
	popq %r12
	
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
	/*Register usage
	 * %edi - image pointer, radius, x index, image pointer
	 * %esi - x coord, y index, x index
	 * %edx - y coord, x coord, y index
	 * %ecx - radius, radius squared, y coord, color value
	 * %r8 - color value
	 * %r12d - image pointer
	 * %r13d - x coord
	 * %r14d - y coord
	 * %r15d - radius
	 * %ebx - x index
	 * %ebp - y index
	 * %eax - radius squared, squared dist
	 */

	/* push r12, r13, r14, r15, ebx, and ebp to preserve values*/
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15
	pushq %rbx
	pushq %rbp
	subq $8, %rsp /*align stack pointer for procedure calls*/

	movq %rdi, %r12 /*move image pointer to r12*/
	movl %esi, %r13d /*move x coordinate to r13d*/
	movl %edx, %r14d /*move y coordinate to r14d*/
	movl %ecx, %r15d /*move radius to r15d*/
	pushq %r8 /*push color value so that it can be preserved for later*/

	movl %r15d, %edi /*move radius to first argument register edi*/
	call square /*call square with radius as parameter*/
	movl %eax, %r15d /*move radius squared to r15d*/
	
	movl $0, %ebx /*move 0 to x index*/
	movl $0, %ebp /*move 0 to y index*/

.Lx_circloop:
	cmp %ebx, IMAGE_WIDTH_OFFSET(%r12) /*compare x index to image width*/
	jle .Lexit_x_circloop /*if image width less than or equal to x index exit x for loop*/
.Ly_circloop:
	cmp %ebp, IMAGE_HEIGHT_OFFSET(%r12) /*compare y index to image height*/
	jle .Lexit_y_circloop /*if image height less than or equal to y index exit y for loop*/

	movl %ebx, %edi /*move x index to first argument register edi*/
	movl %ebp, %esi /*move y index to second argument register esi*/
	movl %r13d, %edx /*move x coord to third argument register edx*/
	movl %r14d, %ecx /*move y coord to fourth argument register ecx*/
	call square_dist /*call square_dist with x index, y index, x coord, and y coord as parameters*/

	cmp %eax, %r15d /*compare squared distance to radius squared*/
	jl .Lout_of_circ /*if radius squared distance less than radius squared point is not in circle and exit if statement*/

	movq %r12, %rdi /*move image pointer to first argument register edi*/
	movl %ebx, %esi /*move x index to second argument register esi*/
	movl %ebp, %edx /*move y index into third argument register edx*/
	pop %rcx /*pop color value into ecx*/
	push %rcx /*push color value back into stack*/
	call draw_pixel /*call draw pixel with image pointer, x index, y index, and color as parameters*/

	inc %ebp /*increment y index*/
	jmp .Ly_circloop /*being next iteration of y loop*/

.Lout_of_circ:
	inc %ebp /*increment y index*/
	jmp .Ly_circloop /*being next iteration of y loop*/

.Lexit_y_circloop:
	inc %ebx /*increment x index*/
	movl $0, %ebp /*restore y index to 0*/
	jmp .Lx_circloop /*being next iteration of x loop*/

.Lexit_x_circloop:
	popq %rcx 
	addq $8, %rsp  /*restore stack pointer to original location*/

	/* pop r12, r13, r14, r15, ebx, and ebp to restore values*/
	popq %rbp
	popq %rbx
	popq %r15
	popq %r14
	popq %r13
	popq %r12

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
	/*Register usage
	 * %rdi - image pointer, tilemap pointer, x coord, x coord + tile width, y coord, y coord + tile height, tilemap pointer, image pointer
	 * %esi - x coord, tile x, tile x + tile width, 0, bg x, fg x
	 * %edx - y coord, tile y, tile y + tile height, image width, bg y, fg y
	 * %rcx - tilemap pointer
	 * %r8 - tile pointer
	 * %r12d - image pointer
	 * %r13d - x coord, x index
	 * %r14d - y coord, y index
	 * %r15d - tilemap pointer
	 * %ebx - tile pointer
	 * %ebp - stack index
	 * %eax - in_bounds return value, min x, max x, min y, max y, bg index, fg index
	 */

	/*Stack usage
	 * %rsp + 0 - max x
	 * %rsp + 4 - max y
	 * %rsp + 8 - min y
	 * %rsp + 12 - min x
	 */

	/* push r12, r13, r14, r15, ebx, and ebp to preserve values*/
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15
	pushq %rbx
	pushq %rbp

	subq $40, %rsp /*align stack pointer for procedure calls and make space to store values in stack*/

	movq %rdi, %r12 /*move image pointer to r12*/
	movl %esi, %r13d /*move x coord to r13d*/
	movl %edx, %r14d /*move y coord to r14d*/
	movq %rcx, %r15 /*move tilemap pointer to r15*/
	movq %r8, %rbx /*move tile pointer to rbx*/



	movq %r15, %rdi /*move tilemap pointer to first argument register rdi*/
	movl RECT_X_OFFSET(%rbx), %esi /*move tile x to second argument register esi*/
	movl RECT_Y_OFFSET(%rbx), %edx /*move tile y to third argument register edx*/
	call in_bounds /*call in_bounds with tilemap pointer, tile x, and tile y as parameters*/
	movl %eax, %ebp /*move in_bounds result to ebp*/

	addl RECT_HEIGHT_OFFSET(%rbx), %edx /*add tile height to tile y*/
	subl $1, %edx /*subtract one from tile y + tile height*/
	call in_bounds /*call in_bounds with tilemap pointer, tile x, and tile y + tile height - 1 as parameters*/
	and %eax, %ebp /*and in_bounds result to previous in_bound result*/

	movl RECT_X_OFFSET(%rbx), %esi /*move tile x to second argument register esi*/
	addl RECT_WIDTH_OFFSET(%rbx), %esi /*add tile width to tile x*/
	movl RECT_Y_OFFSET(%rbx), %edx /*move tile y to third argument register edx*/ 
	subl $1, %esi /*subtract one from tile x + tile width*/
	call in_bounds /*call in_bounds with tilemap pointer, tile x, and tile y + tile height - 1 as parameters*/
	and %eax, %ebp /*and in_bounds result to previous in_bound results*/

	movl RECT_X_OFFSET(%rbx), %esi
	movl RECT_Y_OFFSET(%rbx), %edx
	addl RECT_WIDTH_OFFSET(%rbx), %esi /*add tile width to tile x*/
	addl RECT_HEIGHT_OFFSET(%rbx), %edx /*add tile height to tile y*/ 
	subl $1, %edx /*subtract one from tile y + tile height*/
	subl $1, %esi /*subtract one from tile x + tile width*/
	call in_bounds /*call in_bounds with tilemap pointer, tile x + tile width - 1, and tile y + tile height - 1 as parameters*/
	and %eax, %ebp /*and in_bounds result to previous in_bound results*/

	cmp $0, %ebp /*compare in_boud results logical &ed together to 0*/
	je .Ltile_out_of_bounds /*if any of the in_bounds returned zero than the &ed value is zero, so tile is not completely within tilemap and jump to tile_out_of_bounds*/



	movl %r13d, %edi /*move x coord to first argument register edi*/
	movq $0, %rsi /*move 0 to second argument register rsi*/
	movl IMAGE_WIDTH_OFFSET(%r12), %edx /*move image width to third argument register edx*/
	call clamp /*call clamp with x coord, 0, and image width as parameters*/
	movl $3, %ebp /*move 3 to stack index*/
	movl %eax, (%rsp, %rbp, 4) /*store clamped minimum x value in stack*/
	movl %eax, %r13d /*move minimum x value to r13*/

	addl RECT_WIDTH_OFFSET(%rbx), %edi /*add tile width to x coord*/
	call clamp /*call clamp with x coord + tile width, 0, and image width as parameters*/
	movl $0, %ebp /*move 0 to stack index*/
	movl %eax, (%rsp, %rbp, 4) /*store clamped maximum x value in stack*/

	movl %r14d, %edi /*move y coord to first argument register edi*/
	movl IMAGE_HEIGHT_OFFSET(%r12), %edx /*move image height to third argument register edx*/
	call clamp /*call clamp with y coord, 0, and image height as parameters*/
	movl $1, %ebp /*move 1 to stack index*/
	movl %eax, (%rsp, %rbp, 4) /*store clamped minimum y value in stack*/
	movl %eax, %r14d /*move minimum y value to r14*/
	
	addl RECT_HEIGHT_OFFSET(%rbx), %edi /*add tile height to y coord*/
	call clamp /*call clamp with y coord + tile height, 0, and image width as parameters*/
	movl $2, %ebp /*move 3 to stack index*/
	movl %eax, (%rsp, %rbp, 4) /*store clamped maximum y value in stack*/
	



.Lx_tileloop:
	cmp %r13d, 0(%rsp) /*compare x index to max x*/
	jle .Lexit_x_tileloop /*if max x less than or equal to x index then exit x loop*/
.Ly_tileloop:
	cmp %r14d, 8(%rsp) /*compare y index to max y*/
	jle .Lexit_y_tileloop  /*if max y less than or equal to y index then exit y loop*/

	movq %r15, %rdi /*move tilemap pointer to first argument register rdi*/
	movl RECT_X_OFFSET(%rbx), %esi /*move tile x to second argument register esi*/
	addl %r13d, %esi /*add x index to tile x*/
	movl $3, %ebp /*move 3 to stack index*/
	subl (%rsp, %rbp, 4), %esi /*subtract min x from tile x + x index*/
	movl RECT_Y_OFFSET(%rbx), %edx /*move tile y to third argument register edi*/
	addl %r14d, %edx /*add y index to tile y*/
	subl 4(%rsp), %edx /*subtract min y from tile y + y index*/
	movq $0, %rax /*move 0 to rax to ensure entire register is empty*/
	call compute_index /*call compute index with tilemap pointer, tile x + x index - min x, and tile y + y index - min y*/
	movl %eax, %ebp /*move background index to ebp*/

	movq %r12, %rdi /*move image pointer to first argument register rdi*/
	movl %r13d, %esi /*move x index to second argument register esi*/
	movl %r14d, %edx /*move y index to third argument register edx*/
	movl $0, %eax /*move 0 to rax to ensure entire register is empty*/
	call compute_index /*call computer index with image pointer, x index, and y index as parameters*/

	push %r12 /*push image pointer to preserve value*/
	push %r15 /*push tilemap pointer to preserve value*/

	movq IMAGE_DATA_OFFSET(%r12), %r12 /*replace r12 with image data*/
	movq IMAGE_DATA_OFFSET(%r15), %r15 /*replace r15 with tilemap data*/

	movl (%r15, %rbp, 4), %r10d /*move foreground pixel color to temp register r10d*/
	movl %r10d, (%r12, %rax, 4) /*replace background pixel color with foreground pixel color*/
	


	pop %r15 /*pop tilemap pointer to restore value*/
	pop %r12 /*pop image pointer to restore value*/

	inc %r14d /*increment y index by one*/
	jmp .Ly_tileloop /*begin next iteration of y loop*/


.Lexit_y_tileloop:
	inc %r13d /*increment x index by one*/
	movl 4(%rsp), %r14d /*reset y index to y_min*/
	jmp .Lx_tileloop /*begin next iteration of x loop*/

.Lexit_x_tileloop:
	addq $40, %rsp /*restore stack pointer to original location*/
	/* pop r12, r13, r14, r15, rbx, and rbp to restore values*/
	popq %rbp
	popq %rbx
	popq %r15
	popq %r14
	popq %r13
	popq %r12 
	ret


.Ltile_out_of_bounds:
	addq $40, %rsp /*restore stack pointer to original location*/
	/* pop r12, r13, r14, r15, rbx, and rbp to restore values*/
	popq %rbp
	popq %rbx
	popq %r15
	popq %r14
	popq %r13
	popq %r12 
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
	/*Register usage
	 * %edi - image pointer, spritemap pointer, x coord, x coord + sprite width, y coord, y coord + sprite height, spritemap pointer, image pointer
	 * %esi - x coord, sprite x, sprite x + sprite width, 0, bg x, fg x, pixel index
	 * %edx - y coord, sprite y, sprite y + sprite height, image width, bg y, fg y, fg color
	 * %ecx - spritemap pointer
	 * %r8 - sprite pointer
	 * %r12d - image pointer
	 * %r13d - x coord, x index
	 * %r14d - y coord, y index
	 * %r15d - spritemap pointer
	 * %ebx - sprite pointer
	 * %ebp - stack index, fg index
	 * %eax - in_bounds return value, min x, max x, min y, max y, bg index, fg index
	 */

	/*Stack usage
	 * %rsp + 0 - max x
	 * %rsp + 4 - max y
	 * %rsp + 8 - min y
	 * %rsp + 12 - min x
	 */

	/* push r12, r13, r14, r15, ebx, and ebp to preserve values*/
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15
	pushq %rbx
	pushq %rbp

	subq $40, %rsp /*align stack pointer for procedure calls and make space to store values in stack*/

	movq %rdi, %r12 /*move image pointer to r12*/
	movl %esi, %r13d /*move x coord to r13d*/
	movl %edx, %r14d /*move y coord to r14d*/
	movq %rcx, %r15 /*move spritemap pointer to r15*/
	movq %r8, %rbx /*move sprite pointer to rbx*/



	movq %r15, %rdi /*move spritemap pointer to first argument register rdi*/
	movl RECT_X_OFFSET(%rbx), %esi /*move sprite x to second argument register esi*/
	movl RECT_Y_OFFSET(%rbx), %edx /*move sprite y to third argument register edx*/
	call in_bounds /*call in_bounds with spritemap pointer, sprite x, and sprite y as parameters*/
	movl %eax, %ebp /*move in_bounds result to ebp*/

	addl RECT_HEIGHT_OFFSET(%rbx), %edx /*add sprite height to tile y*/
	subl $1, %edx /*subtract one from sprite y + sprite height*/
	call in_bounds /*call in_bounds with spritemap pointer, sprite->x, and sprite->y + sprite->height - 1 as parameters*/
	and %eax, %ebp /*and in_bounds result to previous in_bound result*/

	movl RECT_Y_OFFSET(%rbx), %edx /*move sprite y to second argument register edx*/
	addl RECT_WIDTH_OFFSET(%rbx), %esi /*add sprite width to tile x*/
	subl $1, %edx /*subtract one from sprite x + sprite width*/
	call in_bounds /*call in_bounds with spritemap pointer, sprite x, and sprite y + sprite height - 1 as parameters*/
	and %eax, %ebp /*and in_bounds result ro previous in_bound results*/


	
	movl RECT_Y_OFFSET(%rbx), %edx
	addl RECT_HEIGHT_OFFSET(%rbx), %edx /*add tile height to tile y*/ 
	subl $1, %edx /*subtract one from tile y + tile height*/
	call in_bounds /*call in_bounds with spritemap pointer, sprite x + sprite width - 1, and sprite y + sprite height - 1 as parameters*/
	and %eax, %ebp /*and in_bounds result to previous in_bound results*/

	cmp $0, %ebp /*compare in_boud results logical &ed together to 0*/
	je .Ltile_out_of_bounds /*if any of the in_bounds returned zero than the &ed value is zero, so sprite is not completely within spritemap and jump to sprite_out_of_bounds*/



	movl %r13d, %edi /*move x coord to first argument register edi*/
	movq $0, %rsi /*move 0 to second argument register rsi*/
	movl IMAGE_WIDTH_OFFSET(%r12), %edx /*move image width to third argument register edx*/
	call clamp /*call clamp with x coord, 0, and image width as parameters*/
	movl $3, %ebp /*move 3 to stack index*/
	movl %eax, (%rsp, %rbp, 4) /*store clamped minimum x value in stack*/
	movl %eax, %r13d /*move minimum v value to r13*/

	addl RECT_WIDTH_OFFSET(%rbx), %edi /*add sprite width to x coord*/
	call clamp /*call clamp with x coord + sprite width, 0, and image width as parameters*/
	movl $0, %ebp /*move 0 to stack index*/
	movl %eax, (%rsp, %rbp, 4) /*store clamped maximum x value in stack*/

	movl %r14d, %edi /*move y coord to first argument register edi*/
	movl IMAGE_HEIGHT_OFFSET(%r12), %edx /*move image height to third argument register edx*/
	call clamp /*call clamp with y coord, 0, and image height as parameters*/
	movl $1, %ebp /*move 1 to stack index*/
	movl %eax, (%rsp, %rbp, 4) /*store clamped minimum y value in stack*/
	movl %eax, %r14d /*move minimum y value to r14*/
	
	addl RECT_HEIGHT_OFFSET(%rbx), %edi /*add sprite height to y coord*/
	call clamp /*call clamp with y coord + sprite height, 0, and image width as parameters*/
	movl $2, %ebp /*move 3 to stack index*/
	movl %eax, (%rsp, %rbp, 4) /*store clamped maximum y value in stack*/
	



.Lx_spriteloop:
	cmp %r13d, 0(%rsp) /*compare x index to max x*/
	jle .Lexit_x_spriteloop /*if max x less than or equal to x index then exit x loop*/
.Ly_spriteloop:
	cmp %r14d, 8(%rsp) /*compare y index to max y*/
	jle .Lexit_y_spriteloop  /*if max y less than or equal to y index then exit y loop*/

	movq %r15, %rdi /*move spritemap pointer to first argument register rdi*/
	movl RECT_X_OFFSET(%rbx), %esi /*move tile x to second argument register esi*/
	addl %r13d, %esi /*add x index to sprite x*/
	movl $3, %ebp /*move 3 to stack index*/
	subl (%rsp, %rbp, 4), %esi /*subtract min x from sprite x + x index*/
	movl RECT_Y_OFFSET(%rbx), %edx /*move sprite y to third argument register edi*/
	addl %r14d, %edx /*add y index to sprite y*/
	subl 4(%rsp), %edx /*subtract min y from sprite y + y index*/
	movq $0, %rax /*move 0 to rax to ensure entire register is empty*/
	call compute_index /*call compute index with spritemap pointer, sprite x + x index - min x, and sprite y + y index - min y*/
	movl %eax, %ebp /*move background index to ebp*/

	movq %r12, %rdi /*move image pointer to first argument register rdi*/
	movl %r13d, %esi /*move x index to second argument register esi*/
	movl %r14d, %edx /*move y index to third argument register edx*/
	call compute_index /*call computer index with image pointer, x index, and y index as parameters*/

	movq %r12, %rdi /*move image pointer to first argument register rdi*/

	movl %eax, %esi /*move image pixel index to second argument register esi*/

	push %r15 /*push spritemap pointer to preserve data*/
	movq IMAGE_DATA_OFFSET(%r15), %r15 /*replace r15 with spritemap data*/
	movl (%r15, %rbp, 4), %edx /*move sprite pixel color into third argument register edx*/
	pop %r15 /*restore spritemap pointer*/
	call set_pixel /*call set pixel with image pointer, image pixel index, and fg sprite color as parameters*/

	inc %r14d /*increment y index by one*/
	jmp .Ly_spriteloop /*begin next iteration of y loop*/


.Lexit_y_spriteloop:
	inc %r13d /*increment x index by one*/
	movl 4(%rsp), %r14d /*reset y index to y_min*/
	jmp .Lx_spriteloop /*begin next iteration of x loop*/

.Lexit_x_spriteloop:
	addq $40, %rsp /*restore stack pointer to original location*/
	/* pop r12, r13, r14, r15, rbx, and rbp to restore values*/
	popq %rbp
	popq %rbx
	popq %r15
	popq %r14
	popq %r13
	popq %r12 
	ret


.Lsprite_out_of_bounds:
	addq $40, %rsp /*restore stack pointer to original location*/
	/* pop r12, r13, r14, r15, rbx, and rbp to restore values*/
	popq %rbp
	popq %rbx
	popq %r15
	popq %r14
	popq %r13
	popq %r12 
	ret


/*
vim:ft=gas:
*/
