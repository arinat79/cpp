#include <cmath>
#include <iomanip>
#include <iostream>


using namespace std;

namespace numbers {

    class complex {
        double re, im;
    public:
        complex(double c = 0, double d = 0) : re(c), im(d) {};
        explicit complex(const char *s)
        {
            char c;
            sscanf(s, "%c%lf%c%lf%c", &c, &re, &c, &im, &c);
        }
        double get_re() const {
            return re;
        }
        double get_im() const {
            return im;
        }
        double abs2() const {
            return re * re + im * im;
        }
        double abs() const {
            return sqrt(re * re + im * im);
        }

        void to_string(char *buf, size_t size) const
        {
            snprintf(buf, size, "(%.10g,%.10g)", re, im);
        }
        friend complex operator +(const complex &a, const complex &b);

        friend complex operator -(const complex &b, const complex &a);

        friend complex operator *(const complex &b, const complex &a);

        friend complex operator /(const complex &b, const complex &a);

        friend complex operator ~(const complex &a);
        friend complex operator -(const complex &a);
    };


    complex operator ~(const complex &a)
    {
        return complex(a.re, -a.im);
    }
    complex operator -(const complex &a)
    {
        return complex(-a.re, -a.im);
    }

    complex operator +(const complex &a, const complex &b)
    {
        return complex(b.re + a.re, b.im + a.im);
    }

    complex operator -(const complex &b, const complex &a) {
        return complex(b.re - a.re, b.im - a.im);
    }

    complex operator *(const complex &b, const complex &a) {
        return complex(b.re * a.re - b.im * a.im, b.im  * a.re + a.im * b.re);
    }

    complex operator /(const complex &b, const complex &a) {
        double ch1 = b.re * a.re + b.im * a.im;
        double zn = a.im * a.im + a.re * a.re;
        double ch2 = a.re * b.im - b.re * a.im;
        return complex(ch1 / zn, ch2 / zn);
    }

    class complex_stack {
    private:
        complex *buffer;
        int arrsize;
        int realsize;

    public:
        complex_stack() : buffer(NULL), arrsize(0), realsize(0) { };

        complex_stack(const complex_stack &s)
        {
            buffer = new complex[s.realsize];
            arrsize = s.arrsize;
            realsize = s.realsize;
            for (int i = 0; i < arrsize; i++) {
                buffer[i] = s.buffer[i];
            }
        }

        ~complex_stack() { delete[] buffer; };

        void Grow()
        {
            int newsize = arrsize * 2 + 1;
            complex *newbuffer = new complex[newsize];
            for (int i = 0; i < arrsize; i++)
                newbuffer[i] = buffer[i];
            delete[] buffer;
            buffer = newbuffer;
            realsize = newsize;
        }

        complex operator [](int index) const
        {
            return buffer[index];
        };

        void Add(const complex &element)
        {
            if (realsize == arrsize)
            {
                Grow();
            }
            arrsize++;
            buffer[arrsize - 1] = element;
        }

        size_t size() const
        {
            return arrsize;
        }

        complex operator+() const
        {
            return buffer[arrsize - 1];
        }

        complex_stack operator~() const
        {
            complex_stack st = *this;
            st.arrsize -= 1;
            return st;
        }

        complex_stack &operator = (const complex_stack &a)
        {
            if (this == &a) {
                return *this;
            }
            delete[]buffer;
            realsize = a.realsize;
            arrsize = a.arrsize;
            buffer = new complex[realsize];
            for (int i = 0; i < arrsize; i++) {
                buffer[i] = a.buffer[i];
            }
            return *this;
        }

        friend complex_stack operator<<(const complex_stack &s, const complex &a);

    };

    complex_stack operator<<(const complex_stack &s, const complex &a)
    {

        complex_stack st = s;
        st.Add(a);
        return st;
    }

    complex eval(char **args, const complex &z)
    {
        complex_stack s;
        complex tmp, argl, argr;
        int i = 0;
        while (args[i] != NULL)
        {
            switch (args[i][0]) {
            case '(':
                tmp = complex(args[i]);
                s = s << tmp;
                break;

            case 'z':
                s = s << z;
                break;

            case '+':
                argr = +s;
                s = ~s;
                argl = +s;
                s = ~s;
                tmp = argl + argr;
                s = s << tmp;
                break;

            case '-':
                argr = +s;
                s = ~s;
                argl = +s;
                s = ~s;
                tmp = argl - argr;
                s = s << tmp;
                break;

            case '*':
                argr = +s;
                s = ~s;
                argl = +s;
                s = ~s;
                tmp = argl * argr;
                s = s << tmp;
                break;

            case '/':
                argr = +s;
                s = ~s;
                argl = +s;
                s = ~s;
                tmp = argl / argr;
                s = s << tmp;
                break;

            case '!':
                tmp = +s;
                s = s << tmp;
                break;

            case ';':
                s = ~s;
                break;

            case '~':
                tmp = +s;
                s = ~s;
                tmp = ~tmp;
                s = s << tmp;
                break;

            case '#':
                tmp = +s;
                s = ~s;
                tmp = -tmp;
                s = s << tmp;
                break;
            }
            i++;
        }
        tmp = +s;
        return tmp;
    }
}



int main(int argc, char **argv)
{
    double pi = acos(-1);
    numbers::complex c(argv[1]);
    double r;
    sscanf(argv[2], "%lf", &r);
    double cx = c.get_re(), cy = c.get_im();
    int n;
    sscanf(argv[3], "%d", &n);

    numbers::complex res;

    double fi = pi / n, delta = 2 * pi / n;

    for (int i = 1; i <= n; i++) {
        numbers::complex tmp(cos(fi), sin(fi));
        numbers::complex arg(r * cos(fi) + cx, r * sin(fi) + cy);
        numbers::complex func = eval(argv + 4, arg);
        numbers::complex f = tmp * func;

        res = res + f;

        fi += delta;
    }
    numbers::complex x(0, 2 * pi*r / n);
    res = res * x;
    char s[50];
    res.to_string(s, 50);
    std::cout << s << std::endl;
    return 0;
}