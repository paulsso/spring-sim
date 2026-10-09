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

static void usage(char *prog) {
  fprintf(stderr, "usage: %s \n"
  "FLAG             |  TYPE                                         |   DEFAULT\n"
  "--mass, -m       |  double                                       |   1.0\n"
  "--damping, -c    |   ..                                          |   0.0\n"
  "--stiffness, -k  |   ..                                          |   1.0\n"
  "--integrator, -i |   string (euler, sympletic_euler, verlet, rk4) \n"
  "--x0             |   double\n"
  "--v0             |   ..\n"
  "--F0             |   ..\n"
  "--omega          |   ..\n"
  "--dt             |   ..\n"
  "--tend           |   ..\n"
  "--help, -h       |   NO_ARG\n", prog);
}

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

static const char *option_name(const int val) {
  for (const struct option *o = longopts; o->name != NULL; o++) {
    if (o->val == val) {
      return o->name;
    }
  }
  return "?";
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
        goto help;
      default:
        return EXIT_FAILURE;
    }
  }

  if (optind < argc) {
    fprintf(stderr, "unexpected argument '%s'\n", argv[optind]);
    return EXIT_FAILURE;
  }

goto special_cases;

resume:
  printf("t,x,v,E\n");
  while (s.t < t_end) {
    printf("%.6f,%.10f,%.10f,%10f\n", s.t, s.x, s.v, spring_energy(&s, &p));
    step(&s, &p, dt, spring_deriv);
  }

  return EXIT_SUCCESS;
  
  bad_value:
    fprintf(stderr, "[ERROR] invalid input '%s' to --%s (-%c) \n", optarg, option_name(opt), opt);
    return EXIT_FAILURE;

  help:
    usage(argv[0]);
    return EXIT_SUCCESS;

  special_cases:
    if (dt < 0) { 
      fprintf(stderr, "[ERROR] dt cannot be less than 0\n");
      return EXIT_FAILURE;
    }
    if (t_end < 1.0) {
      fprintf(stderr, "[ERROR] t_end should not be less than 1\n");
      return EXIT_FAILURE;
    }
    if (p.m <= 0) {
      fprintf(stderr, "[ERROR] mass has to be greater than 0\n");
      return EXIT_FAILURE;
    }
    goto resume;
}
