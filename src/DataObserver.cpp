#include"Element.h"
#include"DataObserver.h"
extern Canvas* canvas;
void Order::update(Line* element){
        // ordercount++;
} 
void Order::update(Text* element){
        // ordercount++;
}
void Order::update(Rectangle* element){
        // ordercount++;
}
void Order::show(){
        cout<<"总执行指令数量:"<<ordercount<<"条"<<endl;
}
void Linelength::update(Line* element){
        double length=sqrt(pow(element->y_to-element->y_from,2.0)+pow(element->x_to-element->x_from,2.0));
        lengthcount+=length;
}
void Linelength::update(Rectangle* element){
        double length=element->width*2+element->height*2;
        lengthcount+=length;
}
void Linelength::show(){
        cout<<"输出直线的总长度："<<lengthcount<<endl;
}
void StringList::update(Text* element){
        save.push_back(element->str);
}
void StringList::show(){
        cout<<"所有输出字符串:";if(save.size()==0)cout<<endl;
        for(auto iter=save.begin();iter<save.end();iter++){
                if(iter==this->save.end()-1)cout<<*iter<<endl;
                else cout<<*iter<<",";
        }
}
Important::Important(){
    outfile.open("Important.txt");
    outfile<<"此文本文件存储重要命令在画布上的位置(不考虑截断)"<<endl;
    outfile<<"对于调用View的命令(!View),存储的重要命令为View中color值为显示器像素最大值的元素"<<endl;
}
void Important::update(Line* element){
        if(element->colorvalue>=canvas->maxcolor){
                vector<string> s_temp;
                s_temp.push_back(to_string(element->x_from));
                s_temp.push_back(to_string(element->y_from));
                s_temp.push_back(to_string(element->x_to));
                s_temp.push_back(to_string(element->y_to));
                for(int i=0;i<4;i++){
                        if(i==0)outfile<<"line(";
                        outfile<<s_temp[i];
                        if(i<3)outfile<<",";
                        else outfile<<")"<<endl;
                }
        }
}
void Important::update(Rectangle* element){
        if(element->colorvalue>=canvas->maxcolor){
                vector<string> s_temp;
                s_temp.push_back(to_string(element->x_from));
                s_temp.push_back(to_string(element->y_from));
                s_temp.push_back(to_string(element->width));
                s_temp.push_back(to_string(element->height));
                for(int i=0;i<4;i++){
                        if(i==0)outfile<<"Rect(";
                        outfile<<s_temp[i];
                        if(i<3)outfile<<",";
                        else outfile<<")"<<endl;
                }
        }
}
void Important::update(Text* element){
        if(element->colorvalue>=canvas->maxcolor){
                string s1=to_string(element->x_from);
                string s2=to_string(element->y_from);
                outfile<<"Text("<<s1<<","<<s2<<","<<"\""<<element->str<<"\")"<<endl;   
        }
}