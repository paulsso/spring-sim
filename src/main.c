#include <stdio.h>
#include <stdlib.h>
#include "spring.h"

int main(int argc, char **argv)
{
  /* TODO: Parse params, dt, t_end and integrator choice from argv */

  (void)argc; (void)argv;

  SpringParams p = { .m = 1.0, .c = 0.0, .k = 1.0, .FO = 0.0, .omega = 0.0 };
  SpringState  s = { .t = 0.0, .x = 1.0, .v = 0.0 };

  const double dt     = 0.01;
  const double t_end  = 20.0;
  StepFn step = step_euler;

  printf("t,x,v,E\n");
  while (s.t < t_end) {
    printf("%.6f,%.10f,%.10f,%10f\n", s.t, s.x, s.v, spring_energy(&s, &p));
    step(&s, &p, dt, spring_deriv);
  }

  return EXIT_SUCCESS;
}
