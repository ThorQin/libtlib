/*
 * mkres.c
 *
 *  Created on: 2009-5-14
 *      Author: thor
 *       Email: thor.qin@gmail.com
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#define PATH_MAX 256
#pragma warning(disable : 4996)
#elif defined(__linux)
#include <dirent.h>
#endif
#include <errno.h>

void generate_code(FILE *fi, const char *res_name, int zero_end, int static_variable)
{
	unsigned char buffer[4096];
	size_t rd = 0;
	int total = 0;
	if (static_variable)
		printf("static const unsigned char %s[] =\n{\n\t", res_name);
	else
		printf("extern const unsigned char %s[] =\n{\n\t", res_name);
	while (!feof(fi))
	{
		size_t i;
		rd = fread(buffer, 1, 4096, fi);
		
		for (i = 0; i < rd; ++i)
		{
			if (total % 16 == 0 && total != 0)
				printf("\n\t");
			printf("%3d", buffer[i]);
			if (i < rd - 1 || !feof(fi))
				printf(",");
			++total;
		}
	}
	if (zero_end)
	{
		if (total % 16 == 0 && total != 0)
			printf(",\n\t  0");
		else
			printf(",  0");
	}
		
	printf("\n};\n");
	if (static_variable)
		printf("static const unsigned int %s_length = %d;\n", res_name, total);
	else
		printf("extern const unsigned int %s_length = %d;\n", res_name, total);
}

void show_error()
{
	fprintf(stderr, "Invalid parameters. \n"
			"Please try \"mkres --help\""
			" to get more informations.\n\n");
}

void show_usage()
{
	printf("Usage: mkres [options] <input file>\n" \
			"Make a resource file from input file.\n\n");
	printf("Options:\n" \
			"    -n, --name <NAME>   Generate resource name, default is 'data'.\n"
			"    -z, --zero          Resource with zero terminated, useful with string resource.\n"
			"    -s, --static        Generate static variable, default is extern.\n"
			"    -h, --help          Show this usage information and leave.\n\n");
}

int main(int argc, char *argv[])
{
//	char *output = NULL;
	char *input = NULL;
	char *name = NULL;

	int i = 1;
	int zero_end = 0;
	int static_variable = 0;
	char res_name[61] =
	{ 0	};
	FILE *fi = 0;

	for (; i < argc; ++i)
	{
		if (*(argv[i]) != '-')
		{
			if (i != argc - 1)
			{
				show_error();
				return -1;
			}
			input = argv[i];
		}
		else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
		{
			show_usage();
			return 0;
		}
		else if (strcmp(argv[i], "--zero") == 0 || strcmp(argv[i], "-z") == 0)
		{
			zero_end = 1;
		}
		else if (strcmp(argv[i], "--static") == 0 || strcmp(argv[i], "-s") == 0)
		{
			static_variable = 1;
		}
		else if (strcmp(argv[i], "--name") == 0 || strcmp(argv[i], "-n") == 0)			
		{
			if (i >= argc - 2 || *(argv[i + 1]) == '-')
			{
				show_error();
				return -1;				
			}
			name = argv[i + 1];
			++i;
		}			
	}
	if (input == NULL)
	{
		show_error();
		return -1;			
	}
	if (strlen(input) > PATH_MAX - 5)
	{
		fprintf(stderr, "Generate failed: pathname too lang.\n");
		return -1;
	}

	if (name == NULL)
	{
		strcpy(res_name, "data");
	}
	else
	{
		if (strlen(name) > 60)
		{
			fprintf(stderr, "Generate failed: resource name too lang.\n");
			return -1;			
		}
		strcpy(res_name, name);
	}

	fi = fopen(input, "rb");
	if (fi == NULL)
	{
		fprintf(stderr, "Open input file failed: errno: %d.\n", errno);
		return -1;
	}

	generate_code(fi, res_name, zero_end, static_variable);

	fclose(fi);

	return 0;
}
