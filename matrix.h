#ifndef MATRIX_H
#define MATRIX_H
#include <QList>
#include <QDebug>

template  <class T>
class Matrix
{
public:

    int n,w;        //n是行数
    QList<QList<T>> mat;

public:

    Matrix(int nn=0 , int ww=-2){
        n=nn;
        w=ww;
        if (ww<0)  w=n;

        T tmp=0;
        QList<T> tmpList;
        for (int ii=0; ii<w ; ii++)
        {
            tmpList.append(tmp);
        }
        for (int ii=0; ii<n ; ii++)
        {
            mat.append(tmpList);
        }
    }

    Matrix<T> operator= (const Matrix<T> m){
        mat.clear();
        for(int ii=0; ii<m.n ; ii++)
        {
            mat.append(m.mat[ii]);
        }
        n=m.n;
        w=m.w;
        return *this;
    }

    const Matrix<T> operator* (double x){
        Matrix<T> tmp(n,w);
        for(int ii=0; ii<tmp.n; ii++)
        {
            for(int jj=0; jj<tmp.w; jj++)
            {
                tmp.mat[ii][jj]=mat[ii][jj]*x;
            }
        }
        return tmp;
    }

    const Matrix<T> operator* (int x){
        Matrix<T> tmp(n,w);
        for(int ii=0; ii<tmp.n; ii++)
        {
            for(int jj=0; jj<tmp.w; jj++)
            {
                tmp.mat[ii][jj]=mat[ii][jj]*x;
            }
        }
        return tmp;
    }

    const Matrix<T> operator* (const Matrix<T> m) {
        if (w!=m.n)
        {
            qDebug()<<"没法乘！！";
            return m;
        }

        Matrix<T> tmp(n,m.w);
        T t;
        for(int ii=0; ii<tmp.w; ii++)
        {
            for(int jj=0; jj<tmp.n; jj++)
            {
                t=0;
                for(int kk=0 ; kk<w; kk++)
                    t=t+mat[ii][kk]*m.mat[kk][jj];
                tmp.mat[ii][jj]=t;
            }
        }

        return  tmp;
    }

    const Matrix<T> operator^(const Matrix<T> m){
        //这一函数是按位乘法
        if( n!=m.n || w!=m.w){
            qDebug()<<"没法按位乘！！";
            return m;
        }
        Matrix<T> tmp(n,w);
        for(int ii=0; ii<tmp.n; ii++)
        {
            for(int jj=0; jj<tmp.w; jj++)
            {
                tmp.mat[ii][jj]=mat[ii][jj]*m.mat[ii][jj];
            }
        }
        return tmp;
    }

    const Matrix<T> operator+(const Matrix<T> m){
        if( n!=m.n || w!=m.w){
            qDebug()<<"没法相加！！";
            return this;
        }
        Matrix<T> tmp(n,w);
        for(int ii=0; ii<tmp.n; ii++)
        {
            for(int jj=0; jj<tmp.w; jj++)
            {
                tmp.mat[ii][jj]=mat[ii][jj]+m.mat[ii][jj];
            }
        }
        return tmp;
    }

    QList<T>& operator[] (int i){
        if (i>=n||i<0)   return mat[0];
        else    return mat[i];
    }

    T& at(int i , int j)
    {
        return mat[i][j];
    }





};

#endif // MATRIX_H
