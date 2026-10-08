#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "spring.h"

enum {
  OPT_X0 = 256,
  OPT_V0,
  OPT_F0,
  OPT_OMEGA,
  OPT_DT,
  OPT_TEND
};

static const struct option longopts[] = {
    /* name,                has_arg,            flag, val */
    {"mass",                required_argument,  NULL, 'm'},
    {"damping",             required_argument,  NULL, 'c'},
    {"stiffness",           required_argument,  NULL, 'k'},
    {"integrator",          required_argument,  NULL, 'i'},
    {"x0",                  required_argument,  NULL, OPT_X0},
    {"v0",                  required_argument,  NULL, OPT_V0},
    {"F0",                  required_argument,  NULL, OPT_F0},
    {"omega",               required_argument,  NULL, OPT_OMEGA},
    {"dt",                  required_argument,  NULL, OPT_DT},
    {"tend",                required_argument,  NULL, OPT_TEND},
    {"help",                no_argument,        NULL, 'h'},
    {NULL, 0, NULL, 0}
};

static void usage(const char *prog) {
  // TODO: List the options and defaults; print to stderr
  fprintf(stderr, "usage: %s [options]\n");
}

// TODO: strtod plus checks for end pointer, trailing junk and errno.
static int parse_double(const char *str, double *out);

// TODO: Search a {name, StepFn} table, return NULL if not found
static StepFn lookup_integrator(const char *name);
int main(int argc, char *argv[])
{ 
  SpringParams p = { .m = 1.0, .c = 0.0, .k = 1.0, .F0 = 0.0, .omega = 0.0 };
  SpringState  s = { .t = 0.0, .x = 1.0, .v = 0.0 };

  const double dt     = 0.01;
  const double t_end  = 20.0;
  StepFn step = step_euler;

  int opt;
  while ((opt = getopt_long(argc, argv, "m:c:k:i:h", longopts, NULL)) != -1) {
    switch (opt) {
      case 'm':
        if (parse_double(optarg, &p.m) != 0) goto bad_value;
        break;
      case 'c':
        if (parse_double(optarg, &p.c) != 0) goto bad_value;
        break;
      case 'k':
        if (parse_double(optarg, &p.k) != 0) goto bad_value;
        break;
      case OPT_X0:
        if (parse_double(optarg, &s.x) != 0) goto bad_value;
        break;
      case OPT_V0:
        if (parse_double(optarg, &s.v) != 0) goto bad_value;
        break;
      case OPT_F0:
        if (parse_double(optarg, &p.F0) != 0) goto bad_value;
        break;
      case OPT_OMEGA:
        if (parse_double(optarg, &p.omega) != 0) goto bad_value;
        break;
      case OPT_DT:
        if (parse_double(optarg, &dt) != 0) goto bad_value;
        break;
      case OPT_TEND:
        if (parse_double(optarg, &t_end) != 0) goto bad_value;
        break;
      case 'h':
        break;
      default:
        return EXIT_FAILURE;
    }
  }

  if (optind < argc) {
    fprintf(stderr, "unexpedted argument '%s'\n", argv[optind]);
    return EXIT_FAILURE;
  }
  
  // TODO: Validate input values

  printf("t,x,v,E\n");
  while (s.t < t_end) {
    printf("%.6f,%.10f,%.10f,%10f\n", s.t, s.x, s.v, spring_energy(&s, &p));
    step(&s, &p, dt, spring_deriv);
  }

  return EXIT_SUCCESS;
}
