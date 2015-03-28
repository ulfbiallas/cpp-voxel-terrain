#ifndef DATATYPES_H
#define DATATYPES_H

#include <cmath>
#include <vector>



struct Vec3f {

	float x,y,z;

	Vec3f() {
		x = y = z = 0;
	}

	Vec3f(float x_, float y_, float z_) {
		x = x_;
		y = y_;
		z = z_;
	}

	Vec3f add(Vec3f v_) {
		return Vec3f(x+v_.x, y+v_.y, z+v_.z);
	}

	Vec3f sub(Vec3f v_) {
		return Vec3f(x-v_.x, y-v_.y, z-v_.z);
	}

	Vec3f mult(float s_) {
		return Vec3f(x*s_, y*s_, z*s_);
	}

	float dot(Vec3f v_) {
		return x*v_.x + y*v_.y + z*v_.z;
	}

	Vec3f crossproduct(Vec3f v_) {
		return Vec3f(y*v_.z - z*v_.y, z*v_.x - x*v_.z, x*v_.y - y*v_.x);
	}

	float norm() {
		return sqrt(x*x + y*y + z*z);
	}

	float norm2() {
		return x*x + y*y + z*z;
	}

	Vec3f normalize() {
		float length = norm();
		return Vec3f(x / length, y / length, z / length);
	}
};



struct TRIANGLE {

	Vec3f p[3], n[3];

	TRIANGLE() {
		p[0] = Vec3f();
		p[1] = Vec3f();
		p[2] = Vec3f();
		n[0] = Vec3f();
		n[1] = Vec3f();
		n[2] = Vec3f();
	};
};



struct GRIDCELL {
	Vec3f p[8];
	Vec3f n[8];
	float val[8];
};



#endif