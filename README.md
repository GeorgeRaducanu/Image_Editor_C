## Copyright Raducanu George-Cristian 2022-2023 321CA
------------------------------
# Image editor

The program contains several functionalities for images in 
*NetPBM* format, both for ascii and binary respresention of the files.

The functionalities offered are:
>**LOAD file**- loading operation for an image<br/>
>
>**SAVE file [ascii]** - save operation where the ascii
parameter is optional<br/>
>
>**SELECT x1 y1 x2 y2** - select pixels in the given region<br/>
>
>**SELECT ALL** - select the entire image<br/>
>
>**ROTATE angle** - rotate the image with the provided angle<br/>
>
>**EQUALIZE** - equalize the entire image<br/>
>
>**CROP** - crop the current selected range<br/>
>
>**APPLY param** - apply a filter/kernel convolution from the
>following:<br/>
>>**SHARPEN**<br/>
>>**BLUR**<br/>
>>**GAUSSIAN_BLUR**<br/>
>>**EDGE**<br/>
>
>**HISTOGRAM X Y** - represents using the "*" character the histogram of the
>image, where X is the max number of "*", and Y is the number of
>bins of the histogram<br/>
>**EXIT** - end command

----
## Ideas used and strategy
For a quick implementation with no errors there is only one structure
used in the entire program. The structure is called image.
 
 It contains the sizes and characteristics of the current image.T
 
 There are three channels for each color *R G B*.
  In the case of a grayscale image the picture is
   stored on the first channel corespondind to the red color.

----

 ## Difficulties encountered and solutions
 For the case of multiple APPLY commands succesevely, the precision
  is decreased if the values are maintained in the [0, 255] range.
Thus in order to overcome this problem I multiplied by a large 
constant the values of the pixels for colour images.

In this way precision is not lost because there are much more digits to keep
track of the modified changes as we cannot keap the images in float form.

At each SAVE command the values are restored in the apropiate range, and after
printing are again restored in the enlarged domain, because further processing
with the current image may continue.

The constant is defines as a macro and is a power of two in order to increase 
the speed of the program.

-----

## Further ideas
The domain of larger kernel is a very interesting one, such as 5x5 or 7x7
and may provide interesting applications.

-----

## A more deeper dive into the implementation

The program contains functions for the operations described above
and also some helper ones such as *clamp_func* and *rotate_once*.

The *clamp_func* function is used in the convolution with the kernel
and allows for a bounded control over the pixels.

The *rotate_once* function rotates the selected part of the image or
the full image by 90 degres (pi/2) to the right. Using this function,
the *rotate_func* is much more simple to implement as it's called a
number of 1, 2 or 3 times depending on the modulus of the angle.

The *apply_kernel* function is responsible with the convolution
operation. It initializes the values corresponding to the recquired
kernel and then by allocating a helper matrix and copying the original
values it proceeds with the operations. 

**Note**: The operation affects the pixels on the edge of the selection
only if the selection is smaller than the entire image i.e. it
has existing neighbours (doesn't)  matter if the neighbours are not
in the selection, as long as they exist.

For the calculation only one double is used in order to consume as
 few memory as possible.
Also the helper matrix is of type int and it is recycled for each
channel in order to be as memory friendly as possible and take as 
least time as possible with memory alocation.

---
## Observations:
  The program has been tested intesly with the valgrind tool and it
   is completely memory-leek-proof.
  
  Compilation has been done with the flags -Wall, -Wextra -Wshadow 
  to avoid as many errors and mistakes as posiible.

---
## Critics and further improvements:

The main is bigger than recommended for a good coding style. 
A possible improvement is to make separate functions for parsing
the arguments received from stdin, and not doing it in the main function.

Also, some of the functions can be reduced in size by making separate
allocation and dealocation functions. Further improvements can be made,
but for a quick and correct implementation the choosen method is the 
one that can be seen in the code.
