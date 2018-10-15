// UNTESTED!!

#ifndef __CG_H__
#define __CG_H__

#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <vector>

#ifndef NDEBUG
#define ASSERT(expr, msg) if (!(expr)) FAIL(msg)
#define FAIL(msg) { fputs(msg, stderr); abort(); }
#else
#define ASSERT(...)
#define FAIL(...)
#endif

// Float processing

#define EPS 1e-8

template <typename T>
inline bool eq(const T &a, const T &b) {
    return a - EPS < b && b < a + EPS;
}

template <typename T>
inline bool neq(const T &a, const T &b) {
    return b <= a - EPS || a + EPS <= b;
}

template <typename T>
inline bool eq2(const T &a, const T &b) {
    return fabs(a - b) < EPS;
}

template <typename T>
inline bool neq2(const T &a, const T &b) {
    return fabs(a - b) >= EPS;
}

template <typename T>
inline bool lt(const T &a, const T &b) {
    return a - b < -EPS;
}

template <typename T>
inline bool leqt(const T &a, const T &b) {
    return a - b < EPS;
}

template <typename T>
inline bool gt(const T &a, const T &b) {
    return b - a < -EPS;
}

template <typename T>
inline bool geqt(const T &a, const T &b) {
    return b - a < EPS;
}

// Elementary elements

struct vec2 {
    vec2() = default;
    vec2(double v) : x(v), y(v) {}
    vec2(double _x, double _y) : x(_x), y(_y) {}

    double x = 0.0, y = 0.0;

    double len() const {
        return hypot(x, y);
    }

    double arg() const {
        return atan2(y, x);
    }

    vec2 norm() const {
        return (*this) / len();
    }

    vec2 operator+(const vec2 &b) const {
        return {x + b.x, y + b.y};
    }

    vec2 &operator+=(const vec2 &b) {
        x += b.x;
        y += b.y;
        return *this;
    }

    vec2 operator-(const vec2 &b) const {
        return {x - b.x, y - b.y};
    }

    vec2 &operator-=(const vec2 &b) {
        x -= b.x;
        y -= b.y;
        return *this;
    }

    vec2 operator-() const {
        return {-x, -y};
    }

    friend vec2 operator*(const vec2 &, double);
    friend vec2 operator*(double, const vec2 &);

    vec2 &operator*=(double k) {
        x *= k;
        y *= k;
        return *this;
    }

    vec2 operator/(double k) const {
        ASSERT(neq(k, 0.0), "Divided by zero");
        return {x / k, y / k};
    }

    vec2 &operator/=(double k) {
        ASSERT(neq(k, 0.0), "Divided by zero");
        x /= k;
        y /= k;
        return *this;
    }

    bool operator==(const vec2 &b) const {
        return eq(x, b.x) && eq(y, b.y);
    }

    bool operator!=(const vec2 &b) const {
        return neq(x, b.x) || neq(y, b.y);
    }
};

inline vec2 operator*(const vec2 &a, double k) {
    return {a.x * k, a.y * k};
}

inline vec2 operator*(double k, const vec2 &a) {
    return {a.x * k, a.y * k};
}

struct mat2 {
    mat2() = default;
    mat2(double k) {
        // initialize kI
        m[0][0] = m[1][1] = k;
    }
    mat2(double *a) {
        memcpy(m, a, sizeof(m));
    }
    mat2(double m11, double m12, double m21, double m22) {
        m[0][0] = m11;
        m[0][1] = m12;
        m[1][0] = m21;
        m[1][1] = m22;
    }

    double m[2][2] = {{0.0}, {0.0}};

    double det() const {
        return m[0][0] * m[1][1] - m[0][1] * m[1][0];
    }

    mat2 adj() const {
        return {m[1][1], -m[0][1], -m[1][0], m[0][0]};
    }

    mat2 trans() const {
        return {m[0][0], m[1][0], m[0][1], m[1][1]};
    }

    mat2 inv() const {
        ASSERT(neq(det(), 0.0), "Singular matrix");
        return adj() / det();
    }

    double *operator[](const unsigned i) {
        return m[i];
    }

    const double *operator[](const unsigned i) const {
        return m[i];
    }

    mat2 operator+(const mat2 &b) const {
        return {
            m[0][0] + b.m[0][0], m[0][1] + b.m[0][1],
            m[1][0] + b.m[1][0], m[1][1] + b.m[1][1]
        };
    }

    mat2 &operator+=(const mat2 &b) {
        m[0][0] += b.m[0][0];
        m[0][1] += b.m[0][1];
        m[1][0] += b.m[1][0];
        m[1][1] += b.m[1][1];
        return *this;
    }

    mat2 operator-(const mat2 &b) const {
        return  {
            m[0][0] - b.m[0][0], m[0][1] - b.m[0][1],
            m[1][0] - b.m[1][0], m[1][1] - b.m[1][1]
        };
    }

    mat2 operator-() const {
        return {
            -m[0][0], -m[0][1],
            -m[1][0], -m[1][1]
        };
    }

    mat2 &operator-=(const mat2 &b) {
        m[0][0] -= b.m[0][0];
        m[0][1] -= b.m[0][1];
        m[1][0] -= b.m[1][0];
        m[1][1] -= b.m[1][1];
        return *this;
    }

    mat2 operator*(const mat2 &b) const {
        return {
            m[0][0] * b.m[0][0] + m[0][1] * b.m[1][0],
            m[0][0] * b.m[0][1] + m[0][1] * b.m[1][1],
            m[1][0] * b.m[0][0] + m[1][1] * b.m[1][0],
            m[1][0] * b.m[0][1] + m[1][1] * b.m[1][1]
        };
    }

    friend mat2 operator*(const mat2 &, double);
    friend mat2 operator*(double, const mat2 &);
    friend vec2 operator*(const mat2 &, const vec2 &);
    friend vec2 operator*(const vec2 &, const mat2 &);

    mat2 &operator*=(const mat2 &b) {
        memcpy(m, ((*this) * b).m, sizeof(m));
        return *this;
    }

    mat2 &operator*=(double k) {
        m[0][0] *= k;
        m[0][1] *= k;
        m[1][0] *= k;
        m[1][1] *= k;
        return *this;
    }

    mat2 operator/(double k) const {
        ASSERT(neq(k, 0.0), "Divided by zero");
        return {m[0][0] / k, m[0][1] / k, m[1][0] / k, m[1][1] / k};
    }

    mat2 &operator/=(double k) {
        ASSERT(neq(k, 0.0), "Divided by zero");
        m[0][0] /= k;
        m[0][1] /= k;
        m[1][0] /= k;
        m[1][1] /= k;
        return *this;
    }
};

inline mat2 operator*(const mat2 &a, double k) {
    return {a.m[0][0] * k, a.m[0][1] * k, a.m[1][0] * k, a.m[1][1] * k};
}

inline mat2 operator*(double k, const mat2 &a) {
    return {a.m[0][0] * k, a.m[0][1] * k, a.m[1][0] * k, a.m[1][1] * k};
}

inline vec2 operator*(const mat2 &a, const vec2 &b) {
    return {
        a.m[0][0] * b.x + a.m[0][1] * b.y,
        a.m[1][0] * b.x + a.m[1][1] * b.y
    };
}

inline vec2 operator*(const vec2 &a, const mat2 &b) {
    return {
        a.x * b.m[0][0] + a.y * b.m[1][0],
        a.x * b.m[0][1] + a.y * b.m[1][1]
    };
}

struct Line {
    // Line p + vt, t in R
    Line() = default;
    Line(const vec2 &_p, const vec2 &_v) : p(_p), v(_v) {}

    vec2 p, v;

    bool valid() const {
        return v != vec2(0.0);
    }
};

inline Line line(const vec2 &A, const vec2 &B) {
    return {A, B - A};
}

// Utilities

inline double dot(const vec2 &a, const vec2 &b) {
    return a.x * b.x + a.y * b.y;
}

inline double cross(const vec2 &a, const vec2 &b) {
    return a.x * b.y - a.y * b.x;
}

inline double shadow(const vec2 &a, const vec2 &b) {
    // b shot on a
    ASSERT(a != vec2(0.0), "'a' is a zero vector");
    return cross(a, b) / a.len();
}

inline vec2 shdvec(const vec2 &a, const vec2 &b) {
    ASSERT(a != vec2(0.0), "'a' is a zero vector");
    return cross(a, b) / cross(a, a) * a;
}

inline vec2 mirror(const vec2 &a, const vec2 &b) {
    vec2 s = shdvec(a, b);
    return 2 * s - b;
}

inline mat2 rot(double t) {
    // A*x: rotate x counter-clockwise
    // x*A: rotate x clockwise
    return {
        cos(t), -sin(t),
        sin(t), cos(t)
    };
}

inline mat2 rot2(double t) {
    // A*x: rotate x clockwise
    // x*A: rotate x counter-clockwise
    return {
        cos(t), sin(t),
        -sin(t), cos(t)
    };
}

inline vec2 rot(const vec2 &a, double t) {
    // counter-clockwise
    return rot(t) * a;
}

inline vec2 rot2(const vec2 &a, double t) {
    // clockwise
    return rot2(t) * a;
}

inline double dist(const vec2 &A, const vec2 &B) {
    return (A - B).len();
}

inline double dist(const vec2 &P, const Line &l) {
    ASSERT(l.valid(), "Invalid line");
    vec2 u = P - l.p;
    return (u - shdvec(u, l.v)).len();
}

inline double dist(const Line &p, const Line &q) {
    ASSERT(eq(cross(p.v, q.v), 0.0), "'p' intersects with 'q'");
    return dist(p.p, q);
}

inline double tris(const vec2 &a, const vec2 &b) {
    return cross(a, b) * 0.5;
}

inline vec2 intrp(const vec2 &A, const vec2 &B, double k) {
    // Return C, AC / AB = k
    // OK for negative k
    return (1 - k) * A + k * B;
}

inline vec2 intrp(const vec2 &A, const vec2 &B, double a, double b) {
    // Return C, AC / CB = a / b
    // OK for negative a, b
    return (b * A + a * B) / (a + b);
}

inline vec2 lli(const Line &p, const Line &q) {
    // Line-Line intersection
    ASSERT(neq(cross(p.v, q.v), 0.0), "Parallel lines")
    vec2 d = q.p - p.p;
    double a = dot(d + q.v, d);
    double b = dot(d - p.v, d + q.v - p.v);
    return p.p + a / (a + b) * p.v;
}

inline bool onseg(const vec2 &A, const vec2 &B, const vec2 &C) {
    // Test if C is on AB
    vec2 dA = C - A, dB = C - B;
    return eq(cross(dA, dB), 0.0) && leqt(dot(dA, dB), 0.0);
}

inline bool tstsegi(
    const vec2 &A, const vec2 &B, const vec2 &C, const vec2 &D) {
    vec2 u = B - A, v = D - C;
    return onseg(A, B, C) || onseg(A, B, D) ||
           onseg(C, D, A) || onseg(C, D, B) ||
           (lt(cross(C - A, u) * cross(D - A, u), 0.0) &&
            lt(cross(A - C, v) * cross(B - C, v), 0.0));
}

inline bool tstsegi2(
    const vec2 &A, const vec2 &B, const vec2 &C, const vec2 &D) {
    if (eq(cross(A - B, C - D), 0.0))
        return onseg(A, B, C) || onseg(A, B, D) || onseg(C, D, A);  // trick
    vec2 p = lli(line(A, B), line(C, D));
    return onseg(A, B, p) && onseg(C, D, p);
}

template <typename T>
inline T center3(const T &a, const T &b, const T &c) {
    return (a + b + c) / 3.0;
}

template <typename T>
inline T center(const vector<T> &P) {
    return accumulate(P.begin(), P.end()) / P.size();
}

template <typename T>
inline T center(const vector<T> &P, const vector<double> &w) {
    ASSERT(P.size() == w.size(), "Different length with 'P' and 'w'");
    T c = T(0.0);
    double W = 0.0;
    for (size_t i = 0; i < P.size(); i++) {
        c += P[i] * w[i];
        W += w[i];
    }
    return c / W;
}

#endif // __CG_H__
