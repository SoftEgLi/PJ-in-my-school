#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include"classlist.h"
#include<cmath>
using namespace std;
//Device类中存放基本的成员变量和单个点的函数。Widget类继承Device类
//Widget类中存放printstr，line等函数，Canvas类，View类继承Widget类
//对于坐标系和分辨率的变化，我的做法是输出的方式不变，改变存储的位置（比如屏幕坐标系，就从数组的0行开始存储
//世界坐标系，就从数组的最后一行倒着存储）
//Device中存放draw_point，print_point来合并对点的处理方式，cross_board函数用来判断是否越界
extern Canvas* canvas;

class Canvas{
        public: const int width;
                const int height;
                int colorvalue;
                int maxcolor;
                vector<vector<char>> board;
                vector<ViewDef*> viewdefs;
                vector<Element*>elements;
                vector<Observer*> observers;
                Device* resolution;
                Coord* coord;
                Canvas(string s1,int w,int h,string s2);
                void setcolor(int color){colorvalue=color;};
                void addobserver(Observer* observer);
                void showStatistic();
                void printresults();
                void draw();
                void clear();
                ViewDef* findViewDef(string name);
};

class Element{
        public:virtual void draw(int x,int y,Canvas* canvas)=0;
                string type;
                int x_min,y_min;
                int x_max,y_max;
                Element(string s1,int x_min=0,int y_min=0,int x_max=99,int y_max=59):
                type(s1),x_min(x_min),y_min(y_min),x_max(x_max),y_max(y_max){};//限制元素的范围，默认为整个画布
};
class Line:public Element{
        public: int x_from,y_from;
                int x_to,y_to;
                int colorvalue;
                void draw(int x,int y,Canvas* canvas);
                Line(int x1,int y1,int x2,int y2,int color,int x_min=0,int y_min=0,int x_max=99,int y_max=59)
                :Element("Line",x_min,y_min,x_max,y_max),colorvalue(color),
                x_from(x1),y_from(y1),x_to(x2),y_to(y2){};
};

class Text:public Element{
        public: int x_from,y_from;
                int colorvalue;
                string str;
                void draw(int x,int y,Canvas* canvas);
                Text(int x,int y,string s,int color,int x_min=0,int y_min=0,int x_max=99,int y_max=59):
                Element("Text",x_min,y_min,x_max,y_max),colorvalue(color),
                x_from(x),y_from(y),str(s){};
};
class Rectangle:public Element{
    public: int x_from,y_from;
            int width,height;
            int colorvalue;
            void draw(int x,int y,Canvas* canvas);
            Rectangle(int x1,int y1,int width,int height,int color,int x_min=0,int y_min=0,int x_max=99,int y_max=59):
            Element("Rect",x_min,y_min,x_max,y_max),colorvalue(color),
            x_from(x1),y_from(y1),width(width),height(height){};
};

class ViewDef{                                                  //宏定义的View
        public: vector<string>string_order;
                string name;
                int width;
                int height;
                void addinstrument(string s){string_order.push_back(s);}
                ViewDef(string name,int width,int height):name(name),width(width),height(height){};
};
class View:public Element{                                      //调用的View
        public: int x_start,y_start;                            //保存showViewat的坐标
                int colorvalue;
                ViewDef* viewdef;
                string name;
                int width;
                int height;
                vector<Element*>elements;
                void draw(int x,int y,Canvas* canvas);
                void setcolor(int color){colorvalue=color;}
                View(ViewDef* viewdef,int x_start,int y_start);
};
     
