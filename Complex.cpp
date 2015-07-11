#include "Complex.h"
#include <math.h>

// Instantiates a new complex number based on the real and imaginary part
Complex::Complex(double real, double imag)
{
    re = real;
    im = imag;
}

// Empty constructor
Complex::Complex()
{
}

// Empty destructor
Complex::~Complex()
{
}

// Returns the absolute value of the complex number
double Complex::abs()
{
    return hypot(re, im);
}

// Returns the argument of the complex number
double Complex::angle()
{
    return atan2(im, re);
}

// Returns the complex conjugate of the complex number
Complex Complex::conj()
{
    Complex result;
    result.re = re;
    result.im = -im;

    return result;
}

// Returns the reciprocal of the complex number
Complex Complex::reciprocal()
{
    double scale = re * re + im * im;
    Complex result;
    result.re = re / scale;
    result.im = -re / scale;

    return result;
}

// Returns the exponential of the complex number (meaning e^<complex_number>)
Complex Complex::Exp()
{
    Complex result;
    result.re = exp(re) * cos(im);
    result.im = exp(re) * sin(im);

    return result;
}

// Returns the sin of the complex number
Complex Complex::Sin()
{
    Complex result;
    result.re = sin(re) * cosh(im);
    result.im = cos(re) * sinh(im);

    return result;
}

// Returns the cos of the complex number
Complex Complex::Cos()
{
    Complex result;
    result.re = cos(re) * cosh(im);
    result.im = -sin(re) * sinh(im);

    return result;
}

// Returns the tangent of the complex number as sin/cos
Complex Complex::Tan()
{
    Complex a = this->Sin();
    Complex b = this->Cos();

    return a / b;
}

// Summation of 2 complex numbers
Complex Complex::operator+ (const Complex &b)
{
    double real = this->re + b.re;
    double imag = this->im + b.im;

    return Complex(real, imag);
}

// Summation of a complex number and a double
Complex Complex::operator+ (const double& b)
{
    double real = this->re + b;
    double imag = this->im;

    return Complex(real, imag);
}

// Incrementing the complex number with another complex number
void Complex::operator+= (const Complex& a)
{
    this->re += a.re;
    this->im += a.im;
}

// Incrementing the complex number with a double
void Complex::operator+= (const double& a)
{
    this->re += a;
}

// Difference of 2 complex number
Complex Complex::operator- (const Complex& b)
{
    double real = this->re - b.re;
    double imag = this->im - b.im;

    return Complex(real, imag);
}

// Difference of a complex number and a double
Complex Complex::operator- (const double& b)
{
    double real = this->re - b;
    double imag = this->im;

    return Complex(real, imag);
}

// Decrementing the complex number with another complex number
void Complex::operator-= (const Complex& a)
{
	this->re -= a.re;
    this->im -= a.im;
}

// Decrementing the complex number with a double
void Complex::operator-= (const double& a)
{
    this->re -= a;
}

// Multiplication of 2 complex numbers
Complex Complex::operator* (const Complex& b)
{
    double real = this->re * b.re - this->im * b.im;
    double imag = this->re * b.im + this->im * b.re;

    return Complex(real, imag);
}

// Multiplication of a complex number and a double
Complex Complex::operator* (const double& b)
{
    double real = this->re * b;
    double imag = this->im * b;

    return Complex(real, imag);
}

// Multiplying the complex number with another complex number
void Complex::operator*= (const Complex& a)
{
    this->re = this->re * a.re - this->im * a.im;
    this->im = this->re * a.im + this->im * a.re;
}

// Multiplying the complex number with a double
void Complex::operator*= (const double& a)
{
    this->re *= a;
    this->im *= a;
}

// Division of 2 complex numbers as as * (1 / b)
Complex Complex::operator/ (Complex& b)
{
    return *this * b.reciprocal();
}

// Division of a complex number with a double
Complex Complex::operator/ (const double& b)
{
    double real = this->re / b;
    double imag = this->im / b;

    return Complex(real, imag);
}

// Dividing the complex number with another complex number
void Complex::operator/= (Complex& a)
{
    *this *= a.reciprocal();
}

// Dividing the complex number with a double
void Complex::operator/= (const double& a)
{
    this->re /= a;
    this->im /= a;
}

// Assigning the complex number to the value of another complex number
void Complex::operator= (const Complex& a)
{
	this->re = a.re;
	this->im = a.im;
}

// Assigning the complex number to the value of a double
void Complex::operator= (const double& a)
{
	this->re = a;
	this->im = 0;
}
