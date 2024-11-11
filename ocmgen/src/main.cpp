#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#ifndef WIN32
  #include <unistd.h>
#endif
#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "ocmgen/getopt.h"
#include "ocmgen/ocmgen.h"
#include "ocmgen/tokenize.h"

void setup_python_options(getopt_t *gopt);
int emit_python(ocmgen_t *ocm);

void setup_cpp_options(getopt_t *gopt);
int emit_cpp(ocmgen_t *ocm);

int main(int argc, char *argv[]) {
  getopt_t *gopt = getopt_create();

  // clang-format off
    getopt_add_bool(gopt, 'h', "help",     0, "Show this help");
    getopt_add_bool(gopt, 't', "tokenize", 0, "Show tokenization");
    getopt_add_bool(gopt, 'd', "debug",    0, "Show parsed file");
    getopt_add_bool(gopt,   0, "lazy",     0, "Generate output file only if .ocm is newer");
    getopt_add_bool(gopt,   0, "use-quotes-for-includes", 0,
        "Use quotes instead of angular brackets for including header files");
    getopt_add_string(gopt, 0, "package-prefix", "",
        "Add this package name as a prefix to the declared package");
    getopt_add_bool(gopt,   0, "version",  0, "Show version information and exit");


    getopt_add_spacer(gopt, "**** C++ options ****");
    getopt_add_bool(gopt, 'x', "cpp",      0, "Emit C++ code");
    setup_cpp_options(gopt);


    getopt_add_spacer(gopt, "**** Python options ****");
    getopt_add_bool(gopt, 'p', "python",   0, "Emit Python code");
    setup_python_options(gopt);

  // clang-format on

  if (!getopt_parse(gopt, argc, argv, 1) || getopt_get_bool(gopt, "help")) {
    printf("Usage: %s [options] <input files>\n\n", argv[0]);
    getopt_do_usage(gopt);
    return 0;
  }

  ocmgen_t *ocm = ocmgen_create();
  ocm->gopt = gopt;

  for (unsigned int i = 0; i < g_ptr_array_size(gopt->extraargs); i++) {
    char *path = (char *)g_ptr_array_index(gopt->extraargs, i);

    int res = ocmgen_handle_file(ocm, path);
    if (res) return res;
  }

  // // If "--version" was specified, then show version information and exit.
  // if (getopt_get_bool(gopt, "version")) {
  //     printf("ocm-gen %d.%d.%d\n", OCM_MAJOR_VERSION, OCM_MINOR_VERSION, OCM_MICRO_VERSION);
  //     return 0;
  // }

  // If "-t" or "--tokenize" was specified, then show tokenization
  // information and exit.
  if (getopt_get_bool(gopt, "tokenize")) {
    return 0;
  }

  int did_something = 0;
  int res = 0;
  if (getopt_get_bool(gopt, "debug")) {
    did_something = 1;
    ocmgen_dump(ocm);
  }

  if (getopt_get_bool(gopt, "cpp")) {
    did_something = 1;
    if (emit_cpp(ocm)) {
      printf("An error occurred while emitting C++ code.\n");
      res = -1;
    }
  }

  if (getopt_get_bool(gopt, "python")) {
    did_something = 1;
    if (emit_python(ocm)) {
      printf("An error occurred while emitting Python code.\n");
      res = -1;
    }
  }

  if (did_something == 0) {
    printf("No actions specified. Try --help.\n");
    res = -1;
  }

  return res;
}
