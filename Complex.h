#ifndef COMPLEX_H
#define COMPLEX_H



class Complex
{
    public:
		// Real part of the complex number
        double re;

        // Imaginary part of the complex number
        double im;

        // Constructors and destructor
        Complex();
        Complex(double re, double im);
        virtual ~Complex();

        // Various operations applied to the complex numbers
        double abs();
        double angle();
        Complex conj();
        Complex reciprocal();
        Complex Exp();
        Complex Sin();
        Complex Cos();
        Complex Tan();

        // BASIC OPERATORS

        // Summation
        Complex operator+(const Complex& b);
        Complex operator+(const double& b);

        // Incrementing
        void operator+=(const Complex& a);
        void operator+=(const double& a);

        // Difference
        Complex operator-(const Complex& b);
        Complex operator-(const double& b);

        // Decrementing
        void operator-=(const Complex& a);
        void operator-=(const double& a);

        // Multiplication
        Complex operator*(const Complex& b);
        Complex operator*(const double& b);

        // Multiplication of THIS number with another
        void operator*=(const Complex& a);
        void operator*=(const double& a);

        // Division
        Complex operator/(Complex& b);
        Complex operator/(const double& b);

        // Division of THIS number with another
        void operator/=(Complex& a);
        void operator/=(const double& a);

        // Assignment
        void operator=(const Complex& a);
        void operator=(const double& a);


    protected:
    private:

};

#endif // COMPLEX_H
