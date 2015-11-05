#ifndef TONIC_H
#define TONIC_H

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

#define ARRAY_LENGTH(a) (sizeof(a) / sizeof(a[0]))
#define stubbed() (fprintf(stderr, "%s:%d stubbed!\n", __FILE__, __LINE__))
// apparently strdup is posix, not iso
char *strdup(const char *input);
double step(double current, double towards, double increment);

#define CROAK(m, ...) {fprintf(stderr, m __VA_ARGS__); exit(1);}

#define POSITIVE(n) (n > 0)
#define NEGATIVE(n) (n < 0)
#endif
