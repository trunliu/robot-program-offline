#ifndef CIRCULARRECTANGLE_H
#define CIRCULARRECTANGLE_H
//环形矩阵
class CircularRectangle
{
public:
    CircularRectangle();
private:
    double length,width,p1,p2;
    void setData(double l,double w,double p1,double p2);
    void createGodes();
};

#endif // CIRCULARRECTANGLE_H
