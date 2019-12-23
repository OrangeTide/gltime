#include <stdio.h>
#include <string.h>
#include <unistd.h>

static FILE *f;

static void
emit_color(unsigned long c)
{
	float r = ((c >> 16) & 255) / 255.;
	float g = ((c >> 8) & 255) / 255.;
	float b = (c & 255) / 255.;

	fprintf(stdout, "{ %0.3f, %0.3f, %0.3f },\n", r, g, b);
}

static int
openinput(const char *filename)
{
	if (f && f != stdin)
		fclose(f);

	if (!filename || (filename[0] == '-' && !filename[1]))
		f = stdin;
	else
		f = fopen(filename, "r");
	if (!f) {
		perror(filename);
		return -1;
	}

	return 0;
}

static int
process(const char *filename)
{
	char line[256];
	char *p;
	unsigned long out;

	if (openinput(filename))
		return -1;

	while (fgets(line, sizeof(line), f)) {
		/* detect missing newline */
		p = strchr(line, '\n');
		if (!p) {
			fprintf(stderr, "%s:line too long\n", filename);
			return -1;
		}
		*p = 0;

		/* strip comments */
		p = strchr(line, ';');
		if (p)
			*p = 0;

		if (sscanf(line, "%lx", &out)) {
			emit_color(out);
		}
	}


	fclose(f);
	f = NULL;
}

int
main(int argc, char **argv)
{
	optind = 1; // TODO: getopt()

	if (argc == 1) {
		fprintf(stderr, "usage: %s [files...]\n", argv[0]);
		return 1;
	}
	for (;optind < argc; optind++) {
		if (process(argv[optind]))
			return 1;
	}

	return 0;
}
