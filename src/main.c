#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <errno.h>
#include <string.h>
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

static int parse_double(const char *str, double *out) {
  char *end;
  errno = 0;
  double val = strtod(str, &end);
  if (*end != '\0' || end == str || errno == ERANGE
    || isnan(val)) {
    return 1;
  }
  *out = val;
  return 0;
}

static StepFn lookup_integrator(const char *name, StepFn *fun) {
  StepFn ret = NULL;
  if (strcmp(name,"rk4") == 0) {
    ret = step_rk4;
  } else if (strcmp(name,"euler") == 0) {
    ret = step_euler;
  } else if (strcmp(name,"sympletic_euler") == 0) { 
    ret = step_sympletic_euler;
  } else if (strcmp(name,"verlet") == 0) {
    ret = step_verlet;
  }
  if (ret != NULL) *fun = ret;
  return ret;
}

int main(int argc, char *argv[])
{ 
  SpringParams p = { .m = 1.0, .c = 0.0, .k = 1.0, .F0 = 0.0, .omega = 0.0 };
  SpringState  s = { .t = 0.0, .x = 1.0, .v = 0.0 };

  double dt     = 0.01;
  double t_end  = 20.0;
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
      case 'i':
        if(lookup_integrator(optarg, &step) == NULL) goto bad_value;
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

  printf("t,x,v,E\n");
  while (s.t < t_end) {
    printf("%.6f,%.10f,%.10f,%10f\n", s.t, s.x, s.v, spring_energy(&s, &p));
    step(&s, &p, dt, spring_deriv);
  }

  return EXIT_SUCCESS;
  
  bad_value:
    fprintf(stderr, "[ERROR] \n");
    return EXIT_FAILURE;

}
