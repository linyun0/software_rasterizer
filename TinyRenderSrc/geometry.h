#pragma once
#include <cmath>
#include <cassert>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp> // 用于 glm::to_string

typedef unsigned char u08;
#define MY_PI 3.1415926
#define TWO_PI (2.0* MY_PI)

template<int n> struct vec {
    double data[n] = { 0 };
    double& operator[](const int i) { assert(i >= 0 && i < n); return data[i]; }
    double  operator[](const int i) const { assert(i >= 0 && i < n); return data[i]; }
};

template<int n> double operator*(const vec<n>& lhs, const vec<n>& rhs) {
    double ret = 0;                         // N.B. Do not ever, ever use such for loops! They are highly confusing.
    for (int i = n; i--; ret += lhs[i] * rhs[i]); // Here I used them as a tribute to old-school game programmers fighting for every CPU cycle.
    return ret;                             // Once upon a time reverse loops were faster than the normal ones, it is not the case anymore.
}

template<int n> vec<n> operator+(const vec<n>& lhs, const vec<n>& rhs) {
    vec<n> ret = lhs;
    for (int i = n; i--; ret[i] += rhs[i]);
    return ret;
}

template<int n> vec<n> operator-(const vec<n>& lhs, const vec<n>& rhs) {
    vec<n> ret = lhs;
    for (int i = n; i--; ret[i] -= rhs[i]);
    return ret;
}

template<int n> vec<n> operator*(const vec<n>& lhs, const double& rhs) {
    vec<n> ret = lhs;
    for (int i = n; i--; ret[i] *= rhs);
    return ret;
}

template<int n> vec<n> operator*(const double& lhs, const vec<n>& rhs) {
    return rhs * lhs;
}

template<int n> vec<n> operator/(const vec<n>& lhs, const double& rhs) {
    vec<n> ret = lhs;
    for (int i = n; i--; ret[i] /= rhs);
    return ret;
}

template<int n> std::ostream& operator<<(std::ostream& out, const vec<n>& v) {
    for (int i = 0; i < n; i++) out << v[i] << " ";
    return out;
}

template<> struct vec<2> {
    double x = 0, y = 0;
    double& operator[](const int i) { assert(i >= 0 && i < 2); return i ? y : x; }
    double  operator[](const int i) const { assert(i >= 0 && i < 2); return i ? y : x; }
};

template<> struct vec<3> {
    double x = 0, y = 0, z = 0;
    double& operator[](const int i) { assert(i >= 0 && i < 3); return i ? (1 == i ? y : z) : x; }
    double  operator[](const int i) const { assert(i >= 0 && i < 3); return i ? (1 == i ? y : z) : x; }
};

template<> struct vec<4> {
    double x = 0, y = 0, z = 0, w = 0;
    double& operator[](const int i) { assert(i >= 0 && i < 4); return i < 2 ? (i ? y : x) : (2 == i ? z : w); }
    double  operator[](const int i) const { assert(i >= 0 && i < 4); return i < 2 ? (i ? y : x) : (2 == i ? z : w); }
    vec<2> xy()  const { return { x, y }; }
    vec<3> xyz() const { return { x, y, z }; }
};

typedef vec<2> vec2;
typedef vec<3> vec3;
typedef vec<4> vec4;

template<int n> double norm(const vec<n>& v) {
    return std::sqrt(v * v);
}

template<int n> vec<n> normalized(const vec<n>& v) {
    return v / norm(v);
}

inline vec3 cross(const vec3& v1, const vec3& v2) {
    return { v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x };
}

template<int n> struct dt;

template<int nrows, int ncols> struct mat {
    vec<ncols> rows[nrows] = { {} };

    vec<ncols>& operator[] (const int idx) { assert(idx >= 0 && idx < nrows); return rows[idx]; }
    const vec<ncols>& operator[] (const int idx) const { assert(idx >= 0 && idx < nrows); return rows[idx]; }

    double det() const {
        return dt<ncols>::det(*this);
    }

    double cofactor(const int row, const int col) const {
        mat<nrows - 1, ncols - 1> submatrix;
        for (int i = nrows - 1; i--; )
            for (int j = ncols - 1; j--; submatrix[i][j] = rows[i + int(i >= row)][j + int(j >= col)]);
        return submatrix.det() * ((row + col) % 2 ? -1 : 1);
    }

    mat<nrows, ncols> invert_transpose() const {
        mat<nrows, ncols> adjugate_transpose; // transpose to ease determinant computation, check the last line
        for (int i = nrows; i--; )
            for (int j = ncols; j--; adjugate_transpose[i][j] = cofactor(i, j));
        return adjugate_transpose / (adjugate_transpose[0] * rows[0]);
    }

    mat<nrows, ncols> invert() const {
        return invert_transpose().transpose();
    }

    mat<ncols, nrows> transpose() const {
        mat<ncols, nrows> ret;
        for (int i = ncols; i--; )
            for (int j = nrows; j--; ret[i][j] = rows[j][i]);
        return ret;
    }
};

template<int nrows, int ncols> vec<ncols> operator*(const vec<nrows>& lhs, const mat<nrows, ncols>& rhs) {
    return (mat<1, nrows>{{lhs}}*rhs)[0];
}

template<int nrows, int ncols> vec<nrows> operator*(const mat<nrows, ncols>& lhs, const vec<ncols>& rhs) {
    vec<nrows> ret;
    for (int i = nrows; i--; ret[i] = lhs[i] * rhs);
    return ret;
}

template<int R1, int C1, int C2>mat<R1, C2> operator*(const mat<R1, C1>& lhs, const mat<C1, C2>& rhs) {
    mat<R1, C2> result;

    // 初始化结果矩阵为零
    for (int i = 0; i < R1; ++i) {
        for (int j = 0; j < C2; ++j) {
            result[i][j] = 0;
        }
    }

    // 执行矩阵乘法
    for (int i = 0; i < R1; ++i) {            // 遍历左矩阵的行
        for (int j = 0; j < C2; ++j) {        // 遍历右矩阵的列
            for (int k = 0; k < C1; ++k) {    // 遍历左矩阵的列/右矩阵的行
                result[i][j] += lhs[i][k] * rhs[k][j];
            }
        }
    }

    return result;
}

template<int nrows, int ncols>mat<nrows, ncols> operator*(const mat<nrows, ncols>& lhs, const double& val) {
    mat<nrows, ncols> result;
    for (int i = nrows; i--; result[i] = lhs[i] * val);
    return result;
}

template<int nrows, int ncols>mat<nrows, ncols> operator/(const mat<nrows, ncols>& lhs, const double& val) {
    mat<nrows, ncols> result;
    for (int i = nrows; i--; result[i] = lhs[i] / val);
    return result;
}

template<int nrows, int ncols>mat<nrows, ncols> operator/(const mat<nrows, ncols>& lhs, const vec<ncols>& val) {
    mat<nrows, ncols> result;
    for (int i = 0; i < nrows; ++i)
    {
        for (int j = 0; j < ncols; ++j) {

            result[i][j] = lhs[i][j] / val[j];

        }
    }
    return result;
}


template<int nrows, int ncols>mat<nrows, ncols> operator+(const mat<nrows, ncols>& lhs, const mat<nrows, ncols>& rhs) {
    mat<nrows, ncols> result;
    for (int i = nrows; i--; )
        for (int j = ncols; j--; result[i][j] = lhs[i][j] + rhs[i][j]);
    return result;
}

template<int nrows, int ncols>mat<nrows, ncols> operator-(const mat<nrows, ncols>& lhs, const mat<nrows, ncols>& rhs) {
    mat<nrows, ncols> result;
    for (int i = nrows; i--; )
        for (int j = ncols; j--; result[i][j] = lhs[i][j] - rhs[i][j]);
    return result;
}

template<int nrows, int ncols> std::ostream& operator<<(std::ostream& out, const mat<nrows, ncols>& m) {
    for (int i = 0; i < nrows; i++) out << m[i] << std::endl;
    return out;
}

template<int n> struct dt { // template metaprogramming to compute the determinant recursively
    static double det(const mat<n, n>& src) {
        double ret = 0;
        for (int i = n; i--; ret += src[0][i] * src.cofactor(0, i));
        return ret;
    }
};

template<> struct dt<1> {   // template specialization to stop the recursion
    static double det(const mat<1, 1>& src) {
        return src[0][0];
    }
};
#define MAX_BONE_INFLUENCE 4

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 mvPosition;
    glm::vec3 mvpPosition;
    glm::vec3 Normal;
    glm::vec3 mvNormal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};


class Mesh {
public:
    /*  网格数据  */
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<Texture> m_textures;
    /*  函数  */
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures):m_vertices(vertices),m_indices(indices),m_textures(textures) {};
    //void Draw(Shader &shader);
private:
    /*  渲染数据  */
   // unsigned int VAO, VBO, EBO;
    /*  函数  */
    //void setupMesh();
};