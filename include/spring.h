#ifndef SPRING_H
#define SPRING_H

typedef struct {
 double m;
 double c;
 double k;
 double FO;
 double omega;
} SpringParams;

typedef struct {
  double t;
  double x;
  double v;

} SpringState;

/* Right-hand side of the ODE: writes dx/dt & dv/dt for state s */
typedef void (*DerivFn)(const SpringState *s, const SpringParams *p,
    double *dxdt, double *dvdt);

/* Advances s by one step dt in place */
typedef void (*StepFn)(SpringState *s, const SpringParams *p, double dt, DerivFn f);

void spring_deriv(const SpringState *s, const SpringParams *p, 
    double *dxdt, double *dvdt);
double spring_energy(const SpringState *s, const SpringParams *p);

void step_euler(SpringState *s, const SpringParams *p, double dt, DerivFn f);
void step_sympletic_euler(SpringState *s, const SpringParams *p, double dt, DerivFn f);
void step_verlet(SpringState *s, const SpringParams *p, double dt, DerivFn f);
void step_rk4(SpringState *s, const SpringParams *p, double dt, DerivFn f);

#endif /* SPRING_H */
