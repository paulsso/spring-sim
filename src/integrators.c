#include <math.h>
#include "spring.h"

void spring_deriv(const SpringState *s, const SpringParams *p, double *dxdt, double *dvdt)
{
  double F_t = p->F0 * cos( p->omega * s->t );
  *dxdt = s->v;
  *dvdt = (F_t - p->c * s->v - p->k * s->x) / p->m;
}

double spring_energy(const SpringState *s, const SpringParams *p)
{
  return (double)( 0.5 * p->m * pow(s->v, 2.0) + 0.5 * p->k * pow(s->x, 2.0) );
}

void step_euler(SpringState *s, const SpringParams *p, double dt, DerivFn f)
{
  double dxdt, dvdt;
  f(s, p, &dxdt, &dvdt);
  s->x += dt*dxdt;
  s->v += dt*dvdt;
  s->t += dt;
}

void step_sympletctic_euler(SpringState *s, const SpringParams *p, double dt, DerivFn f)
{
  double dxdt, dvdt;
  f(s, p, &dxdt, &dvdt);
  s->v += dt*dvdt;
  s->x += dt*s->v;
  s->t += dt;
}

void step_verlet(SpringState *s, const SpringParams *p, double dt, DerivFn f)
{
  /* TODO: velocity Verlet, Think about how damping fits a scheme that assumes acceleration depends on x only */
  (void)p; (void)f;
  s->t += dt;
}

void step_rk4(SpringState *s, const SpringParams *p, double dt, DerivFn f)
{ 
  /* TODO: four stages k1..k4 using temporary SpringState copies */
  (void)p; (void)f;
  s->t += dt;
}
