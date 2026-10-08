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

void step_sympletic_euler(SpringState *s, const SpringParams *p, double dt, DerivFn f)
{
  double dxdt, dvdt;
  f(s, p, &dxdt, &dvdt);
  s->v += dt*dvdt;
  s->x += dt*s->v;
  s->t += dt;
}

void step_verlet(SpringState *s, const SpringParams *p, double dt, DerivFn f)
{
  double dxdt, dvdt, dvdt_next;
  f(s, p, &dxdt, &dvdt);
  s->x += dt*dxdt + ( dvdt * pow(dt,2) ) / 2;
  
  s->t += dt;
  f(s, p, &dxdt, &dvdt_next);
  s->v += 0.5*dt*(dvdt + dvdt_next);
}

void step_rk4(SpringState *s, const SpringParams *p, double dt, DerivFn f)
{ 
  double dxdt, dvdt, k1x, k2x, k3x, k4x, k1v, k2v, k3v, k4v;
  SpringState y0 = *s;
  f(&y0, p, &dxdt, &dvdt);
  k1x = dxdt;
  k1v = dvdt;

  y0.t += dt/2;
  y0.x += k1x*(dt/2);
  y0.v += k1v*(dt/2);
  f(&y0, p, &dxdt, &dvdt);
  k2x = dxdt;
  k2v = dvdt;

  y0.x += -k1x*(dt/2) + k2x*(dt/2);
  y0.v += -k1v*(dt/2) + k2v*(dt/2);
  f(&y0, p, &dxdt, &dvdt);
  k3x = dxdt;
  k3v = dvdt;

  y0.t += dt/2;
  y0.x += -k2x*(dt/2) + k3x*dt;
  y0.v += -k2v*(dt/2) + k3v*dt;
  f(&y0, p, &dxdt, &dvdt);
  k4x = dxdt;
  k4v = dvdt;

  s->x += (dt / 6)*(k1x + 2*k2x + 2*k3x + k4x);
  s->v += (dt / 6)*(k1v + 2*k2v + 2*k3v + k4v);
  s->t += dt;
}
