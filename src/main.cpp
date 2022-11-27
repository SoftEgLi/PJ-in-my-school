//PJ环节实现了所有的功能
//undo的性能方面，我的处理是在Canvas中删去需要undo的元素后，将Canvas重新画一遍
//统计信息的undo已经实现

//采用Composite+Strategy+Observer+Visitor+Command+的设计模式
//Composite设计模式与参考设计基本一致，相关代码存储在Element.h和Element.cpp中

//Strategy设计模式与参考设计大体一致，相关代码存储在Device.h和Device.cpp中

//Observer设计模式的代码存储在DataObserver.h和DataObserver.cpp中
//直线长度和字符串的统计是在每一个Element运行draw函数时进行的，只与Element的draw函数有依赖关系
//但指令数量的统计涉及到color指令，比较特殊，所以这部分既与Canvas,View的draw函数有关
//同时又与Command中Colorcommand的execute，undo,redo函数有依赖关系

//Visitor设计模式的代码存储在,CanvasVisitor.h,Txt.h,Txt._Function.cpp,Txt_Initiative.cpp中
//Visitor模式是我lab6所采取的一个模式，是为了拓展更复杂的文本指令所采取的设计模式。
//作为Subject的Txt被分为不同的区域，实现分区域处理。

//Command设计模式的代码存储在Command.h和Command.cpp中
//Command模式我用CommandExecuter来管理command，Invoker来进行交互，Canvas作为Receiver。
//Command的子类包括colorcommand，Linecommand，Textcommand，Rectcommand等等

#include"Element.h"
#include"Txt.h"
#include"CanvasVisitor.h"
#include"DataObserver.h"
#include"Command.h"
#include<Windows.h>
const int width=100;
const int height=60;
extern Canvas* canvas;
CommandExecuter ce;
Order oc;               //统计指令数量的observer设为全局变量，这主要是color的特殊性导致的
void Start_Work(string build,string txt){
    if(build=="draw-g256-cscreen")canvas=new Canvas("Grayscale",width,height,"Screen_Coord");
    else if(build=="draw-g256-cworld")canvas=new Canvas("Grayscale",width,height,"World_Coord");
    else if(build=="draw-g2-cworld")canvas=new Canvas("Monochrome",width,height,"World_Coord");
    else if(build=="draw-g2-cscreen")canvas=new Canvas("Monochrome",width,height,"Screen_Coord");
    canvas->addobserver(new Important);
    canvas->addobserver(&oc);
    canvas->addobserver(new Linelength);
    canvas->addobserver(new StringList);
    if(txt=="\0"){                              //交互模式
        while(1){
            Invoker invoker;
            string order;
            getline(cin,order);
            invoker.invoke(order);//注意，引号必须是英文形式
        }
    }
    else{                                       //脚本模式
        Txt t(txt.c_str());
        Canvasvisitor visitor;
        t.accept(visitor);
        while(1);                       //暂停            
    }
}
int main()         
{   string s;
    getline(cin,s);
    string build;                               //build中存储文件名之前的内容      
    string txt;                                 //txt中存储文本名（若为"\0",则进入交互模式）
    int count;
    for(int i=0;i<s.size();i++){               
        if(s[i]==' '){
            count++;
            continue;
        }
        if(count<5)build+=s[i];
        else txt+=s[i];
    }
    Start_Work(build,txt);    //注意，引号只支持英文形式
    
}