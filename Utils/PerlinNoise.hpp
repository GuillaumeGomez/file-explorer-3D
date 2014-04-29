#ifndef PERLINNOISE_HPP
#define PERLINNOISE_HPP

#include <vector>

class PerlinNoise
{
public:
  PerlinNoise();
  PerlinNoise(unsigned int seed); // Generate a new permutation vector based on the value of seed
  double noise(double x, double y, double z); // Get a noise value, for 2D images z can have any value

private:
  std::vector<int> p;
  double fade(double t);
  double lerp(double t, double a, double b);
  double grad(int hash, double x, double y, double z);
};

#endif // PERLINNOISE_HPP
