#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <ecl/ecl.h>

static const int WIDTH  = 10;
static const int HEIGHT = 10;
static FILE *global_output;

static void draw_line (FILE* output, double x1, double y1, double x2, double y2)
{
	fprintf (output, "plot [0:1] %f + %f * t, %f + %f * t notitle\n", x1, x2 - x1, y1, y2 - y1);
	fflush (output);
}

static cl_object start_gnuplot()
{
	FILE *output;
	int pipes[2];
	pid_t pid;

	pipe(pipes);
	pid = fork();

	if (!pid)
	{
		// close the write end, make stdin the new read end
		dup2(pipes[0], STDIN_FILENO);
		close(pipes[1]);

		char *args[] = { NULL };
		if (execvp("gnuplot", args) == -1)
		{
			fprintf(stdout, "exec command failed\n");
			return ECL_NIL;
		}
	}

	// close the read end and open the write end
	close(pipes[0]);
	output = fdopen(pipes[1], "w");

	fprintf(output, "set multiplot\n");
	fprintf(output, "set parametric\n");
	fprintf(output, "set xrange [-%d:%d]\n", WIDTH, WIDTH);
	fprintf(output, "set yrange [-%d:%d]\n", HEIGHT, HEIGHT);
	fprintf(output, "set size ratio -1\n");
	fprintf(output, "unset xtics\n");
	fprintf(output, "unset ytics\n");
	fflush(output);

	global_output = output;
	return ECL_NIL;
}

static cl_object stop_gnuplot()
{
	fprintf(stdout, "shutting down gnuplot\n");

	fprintf(global_output, "exit\n");
	fflush(global_output);
	fclose(global_output);

	return ECL_NIL;
}



static double x, y;
static double direction;
static int pendown;

static cl_object turtle_reset()
{
	x = y = 0.0;
	direction = 0.0;
	pendown = 1;

	fprintf(global_output, "clear\n");
	fflush(global_output);

	return ECL_NIL;
}

static cl_object turtle_pendown()
{
	cl_object result = ecl_make_integer(pendown);
	pendown = 1;
	return result;
}

static cl_object turtle_penup()
{
	cl_object result = ecl_make_integer(pendown);
	pendown = 0;
	return result;
}

static cl_object turtle_turn(cl_object degrees)
{
	const double value = ecl_to_double(degrees);
	direction += M_PI / 180.0 * value;
	return ecl_make_double_float(direction * 180.0 / M_PI);
}

static cl_object turtle_move(cl_object length)
{
	const double value = ecl_to_double(length);
	double newX, newY;

	newX = x + value * cos(direction);
	newY = y + value * sin(direction);

	if (pendown)
		draw_line(global_output, x, y, newX, newY);

	x = newX;
	y = newY;

	return cl_list(2, ecl_make_double_float(x), ecl_make_double_float(y));
}

static cl_object test_c()
{
	printf("this is a c function call\n");
	return ECL_NIL;
}

static void register_functions()
{
	cl_def_c_function(c_string_to_object("test-c"), (cl_objectfn_fixed)test_c, 0);
	cl_def_c_function(c_string_to_object("start-gnuplot"), (cl_objectfn_fixed)start_gnuplot, 0);
	cl_def_c_function(c_string_to_object("stop-gnuplot"), (cl_objectfn_fixed)stop_gnuplot,  0);
	cl_def_c_function(c_string_to_object("turtle-reset"), (cl_objectfn_fixed)turtle_reset, 0);
	cl_def_c_function(c_string_to_object("turtle-penup"), (cl_objectfn_fixed)turtle_penup, 0);
	cl_def_c_function(c_string_to_object("turtle-pendown"), (cl_objectfn_fixed)turtle_pendown, 0);
	cl_def_c_function(c_string_to_object("turtle-turn"), (cl_objectfn_fixed)turtle_turn, 1);
	cl_def_c_function(c_string_to_object("turtle-move"),  (cl_objectfn_fixed)turtle_move, 1);
}

#if 0
int main(int argc, char *argv[])
{
	cl_boot(argc, argv);
	register_functions();

	cl_object exit_obj = c_string_to_object(":EXIT");
	cl_object result = Cnil;

	cl_safe_eval(c_string_to_object("(load \"autoload.lsp\")"), Cnil, Cnil);

	while (cl_equal(exit_obj, result) == Cnil)
	{
		fprintf(stdout, "\n> ");
		fflush(stdout);

		cl_object form = cl_safe_eval(c_string_to_object("(read)"), Cnil, Cnil);
		result = cl_safe_eval(form, Cnil, Cnil);
		cl_print(1, result);
	}
	putchar('\n');

	cl_shutdown();

	return EXIT_SUCCESS;
}
#endif

int main(int argc, char *argv[])
{
	cl_boot(argc, argv);
	register_functions();

	cl_object exit_obj = c_string_to_object(":EXIT");
	cl_object result = Cnil;

	cl_safe_eval(c_string_to_object("(load \"autoload2.lisp\")"), Cnil, Cnil);

	cl_shutdown();

	return EXIT_SUCCESS;
}

