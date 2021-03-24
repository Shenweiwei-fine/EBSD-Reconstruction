#ifndef MAT3_H
#define MAT3_H
#include <QtMath>

template <class T>
class Mat3
{
private:

    T a11,a12,a13;
    T a21,a22,a23;
    T a31,a32,a33;

public:

    Mat3(double e1=0, double e2=0, double e3=0) {
            //默认的话正好返回的是E.
        T c1,c2,c3,s1,s2,s3;
        c1=qCos(e1/180*3.14159265358979323846);
        c2=qCos(e2/180*3.14159265358979323846);
        c3=qCos(e3/180*3.14159265358979323846);
        s1=qSin(e1/180*3.14159265358979323846);
        s2=qSin(e2/180*3.14159265358979323846);
        s3=qSin(e3/180*3.14159265358979323846);
        a11=c1*c3-s1*c2*s3;
        a12=s1*c3+c1*c2*s3;
        a13=s2*s3;
        a21=-c1*s3-s1*c2*c3;
        a22=-s1*s3+c1*c2*c3;
        a23=s2*c3;
        a31=s1*s2;
        a32=-c1*s2;
        a33=c2;
    }

    Mat3(T a, T b, T c, T d, T e, T f, T g, T h ,T i){  //与matlab的下标顺序相似
        a11=a;  a12=d;  a13=g;
        a21=b;  a22=e;  a23=h;
        a31=c;  a32=f;  a33=i;
    }

    Mat3(double theta, double ux, double uy, double uz){
        double a=cos(theta/2);
        double b=sin(theta/2);
        double c=uy*b;
        double d=uz*b;
        b=ux*b;

        a11=1-2*c*c-2*d*d;
        a12=2*b*c-2*a*d;
        a13=2*a*c+2*b*d;
        a21=2*b*c+2*a*d;
        a22=1-2*b*b-2*d*d;
        a23=2*c*d-2*a*b;
        a31=2*b*d-2*a*c;
        a32=2*a*b+2*c*d;
        a33=1-2*b*b-2*c*c;

    }

    Mat3<T> operator= (const Mat3<T> m) {
        a11=m.a11;
        a12=m.a12;
        a13=m.a13;
        a21=m.a21;
        a22=m.a22;
        a23=m.a23;
        a31=m.a31;
        a32=m.a32;
        a33=m.a33;
        return *this;
    }

    Mat3<T> operator= (double x){
        a11=a12=a13=a22=a21=a23=a31=a32=a33=x;
        return *this;
    }

    const Mat3<T> operator* (const Mat3<T> m) {
        Mat3<T> tmp;
        tmp.a11=a11*m.a11+a12*m.a21+a13*m.a31;
        tmp.a12=a11*m.a12+a12*m.a22+a13*m.a32;
        tmp.a13=a11*m.a13+a12*m.a23+a13*m.a33;

        tmp.a21=a21*m.a11+a22*m.a21+a23*m.a31;
        tmp.a22=a21*m.a12+a22*m.a22+a23*m.a32;
        tmp.a23=a21*m.a13+a22*m.a23+a23*m.a33;

        tmp.a31=a31*m.a11+a32*m.a21+a33*m.a31;
        tmp.a32=a31*m.a12+a32*m.a22+a33*m.a32;
        tmp.a33=a31*m.a13+a32*m.a23+a33*m.a33;
        return tmp;
    }

    const Mat3<T> operator* (double n) {
        Mat3<T> tmp;
        tmp.a11=a11*n;
        tmp.a12=a12*n;
        tmp.a13=a13*n;
        tmp.a21=a21*n;
        tmp.a22=a22*n;
        tmp.a23=a23*n;
        tmp.a31=a31*n;
        tmp.a32=a32*n;
        tmp.a33=a33*n;
        return tmp;
    }

    const Mat3<T> operator* (int n) {
        Mat3<T> tmp;
        tmp.a11=a11*n;
        tmp.a12=a12*n;
        tmp.a13=a13*n;
        tmp.a21=a21*n;
        tmp.a22=a22*n;
        tmp.a23=a23*n;
        tmp.a31=a31*n;
        tmp.a32=a32*n;
        tmp.a33=a33*n;
        return tmp;
    }

    friend Mat3<T> operator*(double n, const Mat3<T> m2);
    friend Mat3<T> operator*(int n, const Mat3<T> m2);

    const Mat3<T> operator+ (Mat3<T> m) {
        Mat3<T> tmp;
        tmp.a11=a11+m.a11;
        tmp.a12=a12+m.a12;
        tmp.a13=a13+m.a13;
        tmp.a21=a21+m.a21;
        tmp.a22=a22+m.a22;
        tmp.a23=a23+m.a23;
        tmp.a31=a31+m.a31;
        tmp.a32=a32+m.a32;
        tmp.a33=a33+m.a33;
        return tmp;
    }

    const Mat3<T> dot(Mat3<T> m1, Mat3<T> m2=0, Mat3<T> m3=0, Mat3<T> m4=0, Mat3<T> m5=0, Mat3<T> m6=0, Mat3<T> m7=0, Mat3<T> m8=0, Mat3<T> m9=0){
        //注：this是工具人不参与运算
        Mat3<T> tmp1;
        tmp1=m1*m2;
        tmp1=tmp1*m3;
        tmp1=tmp1*m4;
        tmp1=tmp1*m5;
        tmp1=tmp1*m6;
        tmp1=tmp1*m7;
        tmp1=tmp1*m8;
        tmp1=tmp1*m9;
        return tmp1;
    }

    T det(){
        return a11*a22*a33+a12*a23*a31+a13*a21*a32-a11*a23*a32-a13*a22*a31-a12*a21*a33;
    }

    const T trace(){
        return a11+a22+a33;
    }

    Mat3<T> transpose(){
        Mat3<T> tmp;
        tmp.a11=a11;
        tmp.a12=a21;
        tmp.a13=a31;
        tmp.a21=a12;
        tmp.a22=a22;
        tmp.a23=a32;
        tmp.a31=a13;
        tmp.a32=a23;
        tmp.a33=a33;
        return tmp;
    }

    Mat3<T> reverse(){
        Mat3<T> tmp;
        tmp.a11=a22*a33-a23*a32;
        tmp.a12=-a21*a33+a23*a31;
        tmp.a13=a21*a32-a22*a31;
        tmp.a21=-a12*a33+a13*a32;
        tmp.a22=a11*a33-a13*a31;
        tmp.a23=-a11*a32+a12*a31;
        tmp.a31=a12*a23-a13*a22;
        tmp.a32=-a11*a23+a13*a21;
        tmp.a33=a11*a22-a12*a21;
        T dt_1=1/this->det();
        tmp=tmp*dt_1;
        return tmp;
    }

    T operator[] (int i){       //跟matlab下标顺序一样.
        switch (i) {
        case 1: return a11;
        case 2: return a21;
        case 3: return a31;
        case 4: return a12;
        case 5: return a22;
        case 6: return a32;
        case 7: return a13;
        case 8: return a23;
        case 9: return a33;
        case 11: return a11;
        case 12: return a12;
        case 13: return a13;
        case 21: return a21;
        case 22: return a22;
        case 23: return a23;
        case 31: return a31;
        case 32: return a32;
        case 33: return a33;
        default: return 0;
        }
    }

    T at(int i){
        return *this[i];
    }
    //这个类可能还需要一些功能：求逆，归一化，···


};

template <class T>
Mat3<T> operator*(double n, const Mat3<T> m2)
{
   Mat3<T> tmp = m2*n;
   return tmp;
}

template <class T>
Mat3<T> operator*(int n, const Mat3<T> m2)
{
   Mat3<T> tmp = m2*n;
   return tmp;
}


#endif // MAT3_H
