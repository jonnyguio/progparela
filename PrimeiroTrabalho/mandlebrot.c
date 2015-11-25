/* Sequential Mandlebrot program */


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

#define		X_RESN	800       /* x resolution */
#define		Y_RESN	800       /* y resolution */

typedef struct complextype
	{
        float real, imag;
	} Compl;


int main (int argc, char *argv[])
{
	Window
		win;                            /* initialization for a window */
	unsigned int
		width, height,                  /* window size */
        x, y,                           /* window position */
        border_width,                   /*border width in pixels */
        display_width, display_height,  /* size of screen */
    	screen;                         /* which screen */
	int
		myId, numProcessors;
	char
		*window_name = "Mandelbrot Set", *display_name = NULL,
		str[100];
	GC
		gc;
	unsigned long
		valuemask = 0;
		XGCValues	values;
	Display		*display;
	XSizeHints	size_hints;
	Pixmap		bitmap;
	XPoint		points[800];
	FILE
		*fp, *fopen ();


	XSetWindowAttributes attr[1];

   /* Mandlebrot variables */
    int i, j, k;
    Compl	z, c;
    float	lengthsq, temp;


	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcessors);
	MPI_Comm_rank(MPI_COMM_WORLD, &myId);


	if (myId == 0)
	{	/* connect to Xserver */

		if (  (display = XOpenDisplay (display_name)) == NULL ) {
		   	fprintf (stderr, "drawon: cannot connect to X server %s\n",
					XDisplayName (display_name) );
			exit (-1);
		}

		/* get screen size */

		screen = DefaultScreen (display);
		display_width = DisplayWidth (display, screen);
		display_height = DisplayHeight (display, screen);

		/* set window size */

		width = X_RESN;
		height = Y_RESN;

		/* set window position */

		x = 100;
		y = 100;

	        /* create opaque window */

		border_width = 4;
		win = XCreateSimpleWindow (display, RootWindow (display, screen),
					x, y, width, height, border_width,
					BlackPixel (display, screen), WhitePixel (display, screen));

		size_hints.flags = USPosition|USSize;
		size_hints.x = x;
		size_hints.y = y;
		size_hints.width = width;
		size_hints.height = height;
		size_hints.min_width = 300;
		size_hints.min_height = 300;

		XSetNormalHints (display, win, &size_hints);
		XStoreName(display, win, window_name);

	        /* create graphics context */

		gc = XCreateGC (display, win, valuemask, &values);

		XSetBackground (display, gc, WhitePixel (display, screen));
		XSetForeground (display, gc, BlackPixel (display, screen));
		XSetLineAttributes (display, gc, 1, LineSolid, CapRound, JoinRound);

		attr[0].backing_store = Always;
		attr[0].backing_planes = 1;
		attr[0].backing_pixel = BlackPixel(display, screen);

		XChangeWindowAttributes(display, win, CWBackingStore | CWBackingPlanes | CWBackingPixel, attr);

		XMapWindow (display, win);
		XSync(display, 0);
	}

        /* Calculate and draw points */

	printf("MyId: %d - Display: %p - Window: %p\n", myId, display, &win);

	if (myId == 0) {

	}
    for (i = myId * (X_RESN / numProcessors); i < (myId + 1) * (X_RESN / numProcessors); i ++)
	{
    	for (j = 0; j < Y_RESN; j++) {

          	z.real = z.imag = 0.0;
          	c.real = ((float) j - 400.0)/200.0;               /* scale factors for 800 x 800 window */
	  		c.imag = ((float) i - 400.0)/200.0;
          	k = 0;

          	do  {                                             /* iterate for pixel color */
	            temp = z.real*z.real - z.imag*z.imag + c.real;
	            z.imag = 2.0*z.real*z.imag + c.imag;
	            z.real = temp;
	            lengthsq = z.real*z.real+z.imag*z.imag;
	            k++;
          	} while (lengthsq < 4.0 && k < 100);

	        if (k == 100) 
			{
				MPI_Bcast(&i, 1, MPI_INT, myId, MPI_COMM_WORLD);
				MPI_Bcast(&j, 1, MPI_INT, myId, MPI_COMM_WORLD);
				//XDrawPoint (display, win, gc, j, i);
			}
        }
	}

	XFlush (display);
	sleep (30);

	MPI_Finalize();
	/* Program Finished */

}
