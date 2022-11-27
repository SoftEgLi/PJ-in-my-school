#include"Element.h"
#include"Device.h"
extern Canvas* canvas;
void Device_2::setPixel(Element* element,int x,int y_pos,int value){ 
        int y=canvas->coord->convert(y_pos);
        if(x<element->x_min||x>element->x_max||y<element->y_min||y>element->y_max
        ||x>canvas->width-1||x<0||y>canvas->height-1||y<0)
                return;
        if(value==0)canvas->board[y][x]=0+'0';
        else canvas->board[y][x]=1+'0';
}
void Device_256::setPixel(Element* element,int x,int y_pos,int value){ 
        int y=canvas->coord->convert(y_pos);
        if(x<element->x_min||x>element->x_max||y<element->y_min||y>element->y_max
        ||x>canvas->width-1||x<0||y>canvas->height-1||y<0){
                return; 
        }
        canvas->board[y_pos][x]=(value/25.6+'0');
}
int Screen::x_max_limit(ViewDef* insert,ViewDef* inserted,int x_position){
        if(insert->width>=(inserted->width-x_position))return inserted->width-1;
        else return x_position+insert->width-1;
}
int Screen::y_max_limit(ViewDef* insert,ViewDef* inserted,int y_position){
        if(y_position>=inserted->height-1)return inserted->height-1;
        else return y_position;
}
int Screen::x_min_limit(ViewDef* insert,ViewDef* inserted,int x_position){
        return x_position;              //默认x坐标不会为负数,不考虑嵌套View的x坐标为负数的情况
}
int Screen::y_min_limit(ViewDef* insert,ViewDef* inserted,int y_position){
        if(insert->height-1>=y_position)return 0;
        else return y_position+1-insert->height;
}
int World::x_max_limit(ViewDef* insert,ViewDef* inserted,int x_position){
       if(insert->width>=(inserted->width-x_position))return inserted->width-1;
        else return x_position+insert->width-1;
}
int World::y_max_limit(ViewDef* insert,ViewDef* inserted,int y_position){
        if(insert->height-1+y_position>=inserted->height-1)return inserted->height-1;
        else return y_position+insert->height-1;
}
int World::x_min_limit(ViewDef* insert,ViewDef* inserted,int x_position){
        return x_position;
}
int World::y_min_limit(ViewDef* insert,ViewDef* inserted,int y_position){
        return y_position;
}