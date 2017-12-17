/*******************************************************************
 * (c) Copyright 2016-2017 Microsemi SoC Products Group. All rights reserved.
 * Simple ray tracer written in 200 lines of C++
 * version 1.0 - 01/12/2017 by anton.krug@microsemi.com
 *******************************************************************/

#include "riscv_hal.h"
#include <float.h>
#include <cmath>

#include "raytracer.h"

struct Shade {
  float value;

  Shade(): value(0.0f) {
  }

  Shade(float valueInit): value(valueInit) {
  }

  Shade operator *(float scalar) {
    return Shade(this->value * scalar);
  }

  Shade operator +(Shade secondShade) {
    return Shade(this->value + secondShade.value);
  }

  Shade operator ~() {
    this->value = fminf(1.0f, fmaxf(0.0f, this->value));
    return *this;
  }

  operator int() {
    return (int) ((~*this).value * 20);
  }
};

class Vector3 {
  float x, y, z;

public:
  Vector3(): x(0.0f), y(0.0f), z(0.0f) {
  }

  Vector3(float xInit, float yInit, float zInit): x(xInit), y(yInit), z(zInit) {
  }

  Vector3 operator +(Vector3 secondVector) {
    return Vector3(x + secondVector.x, y + secondVector.y, z + secondVector.z);
  }

  Vector3 operator -(Vector3 secondVector) {
    return Vector3(x - secondVector.x, y - secondVector.y, z - secondVector.z);
  }

  Vector3 operator *(float scalar) {
    return Vector3(x * scalar, y * scalar, z * scalar);
  }

  Vector3 operator /(float scalar) {
    return Vector3(x / scalar, y / scalar, z / scalar);
  }

  float operator%(Vector3 secondVector) {
    return (x * secondVector.x + y * secondVector.y + z * secondVector.z);
  }

  Vector3 operator~() {
    const float magnitude = sqrtf(x * x + y * y + z * z);
    return Vector3(x / magnitude, y / magnitude, z / magnitude);
  }
};

struct Light: public Vector3 {
  Shade shade;
  Light(Vector3 source, Shade shadeInit): Vector3(source), shade(shadeInit) {
  }
};

struct Ray {
  Vector3 source;
  Vector3 direction;

  Ray(Vector3 sourceInit, Vector3 directionInit): source(sourceInit), direction(directionInit) {
  }
};

class Sphere {
  Vector3 center;
  float   radius;

public:
  Sphere(Vector3 centerInit, float radiusInit): center(centerInit), radius(radiusInit) {
  }

  Vector3 operator ^ (Vector3 pointOnSurface) {
    return ~(pointOnSurface - this->center);
  }

  bool detectHit(Ray ray, Vector3 &hitPoint) {
    Vector3 inRef   = ray.source - this->center;
    float   dotDir  = ray.direction % ray.direction;
    float   temp1   = ray.direction % inRef;
    float   temp2   = (inRef % inRef) - this->radius * this->radius;
    float   tempAll = temp1 * temp1 - dotDir * temp2;

    if (tempAll < 0.0f) return false; // The ray didn't hit the sphere at all

    const float distance = fminf( (-temp1 + sqrtf(tempAll)) / dotDir,
                                  (-temp1 - sqrtf(tempAll)) / dotDir );

    hitPoint = ray.source + ray.direction * distance;
    return true;
  }
};

Shade calculateShadeOfTheRay(Ray ray, Light light) {
  Sphere  sphere(Vector3(0.0f, 0.0f, HEIGHT), HEIGHT/2.0f);
  Shade   ambient = 0.1f;
  Shade   shadeOfTheRay;
  Vector3 hitPoint;

  if (sphere.detectHit(ray, hitPoint)) {
    Vector3 hitNormal    = sphere ^ hitPoint;
    Vector3 hitReflected = ray.direction - (hitNormal * 2.0f *(ray.direction % hitNormal));
    Vector3 hitLight     = ~(light - hitPoint);
    float   diffuse      = fmaxf(0.0f, hitLight % hitNormal);
    float   specular     = fmaxf(0.0f, hitLight % hitReflected);

    shadeOfTheRay = light.shade * powf(specular, SMOOTHNESS) + light.shade * diffuse + ambient;
  }
  return shadeOfTheRay;
}

uint32_t raytracer() {
  uint32_t sum = 0;
  for (float zoom = 12.0f; zoom <= 32.0f; zoom+=10.0f) {
	for (float lightRotate = 0.0f; lightRotate < 2.0f * M_PI_F; lightRotate += M_PI_F / 11.0f) {
	  Light light(Vector3(2.0f * WIDTH  *  cosf(lightRotate),
						  3.0f * HEIGHT * (sinf(lightRotate)-0.5f), -100.0f), Shade(0.7f));

	  for (int y = 2; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
		  Ray rayForThisPixel( Vector3(0.0f,        0.0f,          0.0f),
							  ~Vector3(x - WIDTH/2, y - HEIGHT /2, zoom));
		  sum += calculateShadeOfTheRay(rayForThisPixel, light);
		}
	  }
	}
  }
  return sum;
}
