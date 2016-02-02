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
		*fp, *fopen (),
		*results;


	XSetWindowAttributes attr[1];

   /* Mandlebrot variables */
    int
		i, j, k,
		l, isCalcs, jsCalcs, *isToPrint, *jsToPrint,
		counter1, counter2;
    Compl	z, c;
    float	lengthsq, temp;


	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcessors);
	MPI_Comm_rank(MPI_COMM_WORLD, &myId);

	isToPrint = (int *) malloc(sizeof(int) * numProcessors);
	jsToPrint = (int *) malloc(sizeof(int) * numProcessors);

	if (myId == 0)
	{
		/* connect to Xserver */

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
		results = fopen("results.txt", "w+");
	}

	/*
	Força os processos esperarem o processo 0 criar a janela corretamente
	Não é exatamente necessário, mas em situações específicas, ocorria um problema de impressão dos pontos
	*/
	MPI_Barrier(MPI_COMM_WORLD);

        /* Calculate and draw points */

	counter1 = 0; counter2 = 0;
	/*
	Quebra pra cada processo de maneira por balanceamento normal, sem intercalação
	Pode gerar problemas se X_RESN/numProcessors não for inteiro
	*/
    for (i = myId * (X_RESN / numProcessors); i < (myId + 1) * (X_RESN / numProcessors); i++)
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
			/* Garante que existe um valor para isCalcs e jsCalcs */
			isCalcs = -1;
			jsCalcs = -1;
	        if (k == 100)
			{
				/* Se o ponto entrou na condição, altera o isCalcs e jsCalcs */
				counter1++;
				isCalcs = i;
				jsCalcs = j;

				//XDrawPoint (display, win, gc, jsCalcs, isCalcs);
			}
			/* Envia de volta para o process 0 (responsável pela impressão) os pontos da iteração */
			MPI_Gather(&isCalcs, 1, MPI_INT, isToPrint, 1, MPI_INT, 0, MPI_COMM_WORLD);
			MPI_Gather(&jsCalcs, 1, MPI_INT, jsToPrint, 1, MPI_INT, 0, MPI_COMM_WORLD);
			if (myId == 0)
			{
				counter2++;
				for (l = 0; l < numProcessors; l++) {

					if (*(jsToPrint + l) != -1 && *(isToPrint + l) != -1)
						XDrawPoint (display, win, gc, *(jsToPrint + l), *(isToPrint + l));
				}
			}
        }
	}

	if (myId == 0) {
		XFlush (display);
		fclose(results);
	}
	sleep (10);

	MPI_Finalize();
	/* Program Finished */

}
