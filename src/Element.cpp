#include"Element.h"
#include"DataObserver.h"
#include"Device.h"
extern Order oc;
ViewDef* Canvas::findViewDef(string name){
        for(int i=0;i<viewdefs.size();i++){
                if(name==viewdefs[i]->name)return viewdefs[i];
        }
}
Canvas::Canvas(string s1,int w,int h,string s2):width(w),height(h){
        board.resize(h);
        for(int i=0;i<h;i++)board[i].resize(w);
        if(s1=="Monochrome"){
                resolution = new Device_2;
                maxcolor=1;
        }
        else if(s1=="Grayscale"){
                resolution=new Device_256; 
                maxcolor=255;
        }
        if(s2=="Screen_Coord")coord=new Screen;
        else if(s2=="World_Coord")coord=new World(h);
}
void Canvas::showStatistic(){
        for (auto iter=observers.begin();iter<observers.end(); iter++)
        {      
               (*iter)->show();
               
        }
        
}
void Canvas::addobserver(Observer* observer){
        observers.push_back(observer);
}
extern int colorcount;
void Canvas::clear(){
        for(int i=0;i<height;i++){
                for(int j=0;j<width;j++)
                board[i][j]=0;
        }
        observers.pop_back();
        observers.pop_back();
        oc.ordercount=colorcount;;
        observers.push_back(new Linelength);
        observers.push_back(new StringList);
}
void Canvas::printresults(){
        for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
                if(board[i][j])cout<<board[i][j];
                else cout<<' ';
        }cout<<endl;
        }
}
void Canvas::draw(){
    for(auto iter=elements.begin();iter<elements.end();iter++){
        (*iter)->draw(0,0,this);
        if((*iter)->type=="Line"||(*iter)->type=="Text"||(*iter)->type=="Rect"){oc.ordercount++;
        }
        //关于View的ordercount增量已经在View的draw函数中加上去了
    }
    
}
void Line::draw(int x,int y,Canvas* canvas){
        int x1=x_from+x;
        int y1=y_from+y;
        int x2=x_to+x;
        int y2=y_to+y;
        if(x1==x2){					//斜率不存在
		for(int i=min(y1,y2);i<=max(y1,y2);i++){
			canvas->resolution->setPixel(this,x1,canvas->coord->convert(i),colorvalue);
		}
	}
	else if(y1==y2){
		for(int i=min(x1,x2);i<=max(x1,x2);i++){
			canvas->resolution->setPixel(this,i,canvas->coord->convert(y1),colorvalue);
		}
	}
	else {						//斜率存在
		int distance=abs((y2-y1)/(x2-x1));				//比如斜率为1.5，则distance=1;
		float k=(y2-y1)*1.0/(x2-x1);				//计算斜率，用来判断斜率正负
		int sum=2*abs(y2-y1)-(2*distance+1)*abs(x2-x1);//Bresenham直线算法
		int x_temp,left;
		int y_temp=min(y1,y2);
		if(k>0){				//斜率大于0
			left=1;				//flag==1则从左往右画，否则从右往左画
			x_temp=min(x1,x2);
		}
		else {
		x_temp=max(x1,x2);
		}
		canvas->resolution->setPixel(this,x_temp,canvas->coord->convert(y_temp),colorvalue);
		for(int i=min(x1,x2);i<=max(x1,x2);i++){	//i用来控制循环的次数，即画点的个数
			if(sum>=0){										
				sum+=2*(abs(y2-y1)-(distance+1)*abs(x2-x1));//Bresenham直线算法
				y_temp+=distance+1;						
			}
			else{
				sum+=2*(abs(y2-y1)-distance*abs(x2-x1));
				y_temp+=distance;
			}
			if(left){
				x_temp++;						//从左向右
			}
			else {
				x_temp--;						//从右向左
			}
			canvas->resolution->setPixel(this,x_temp,canvas->coord->convert(y_temp),colorvalue);	
                        
			}
	}
        x_from+=x;
        x_to+=x;
        y_from+=y;
        y_to+=y;
        for(auto iter=canvas->observers.begin();iter<canvas->observers.end();iter++){   //统计数据
                (*iter)->update(this);
        }
}
void Text::draw(int x,int y,Canvas* canvas){
        string hex_to_binary(int pos,int i);
        int x_start=x_from+x;
        int y_start=y_from+y;
        int x1=x_start;
        int y1=canvas->coord->convert(y_start);
        for(int i=0;i<str.length();i++){
                int pos=str[i]-32;
                string s;
                if(x1+8*(i-1)>=canvas->width)break;
                for(int j=0;j<=7;j++){
                        s=hex_to_binary(pos,7-j);
                        if((y1-j)<0)break;
                        for(int k=0;k<=7;k++){
                                if(x1+i*8+k>=canvas->width)break;
                                if(s[k]=='1')canvas->resolution->setPixel(this,x1+i*8+k,y1-j,colorvalue);
                        }
                }
        }
        x_from+=x;
        y_from+=y;
        for(auto iter=canvas->observers.begin();iter<canvas->observers.end();iter++){
                (*iter)->update(this);
        }
}

void Rectangle::draw(int x,int y,Canvas* canvas){
    int x_start=x_from+x;
    int y_start=y_from+y;
    int x1=x_start;
    int y1=canvas->coord->convert(y_start);

    for(int i=x1;i<x1+width;i++){
        canvas->resolution->setPixel(this,i,y1,colorvalue);
        canvas->resolution->setPixel(this,i,y1-height+1,colorvalue);
    }
    for(int i=y1;i>y1-height;i--){
        canvas->resolution->setPixel(this,x1,i,colorvalue);
        canvas->resolution->setPixel(this,x1+width-1,i,colorvalue);
    }
        x_from+=x;
        y_from+=y;
    for(auto iter=canvas->observers.begin();iter<canvas->observers.end();iter++){
                (*iter)->update(this);
        }
}
extern Canvas* canvas;
View::View(ViewDef* viewdef,int x_start,int y_start):Element("View"),x_start(x_start),y_start(y_start),viewdef(viewdef)
{       string subcopy(string s,char start,char del,char end);
        int* pos(int n,string s);
        int* view_color_value;
        for(int i=0;i<viewdef->string_order.size();i++){
                if(viewdef->string_order[i][0]=='c'){
                        string value=subcopy(viewdef->string_order[i],'(',' ','\0');
                        colorvalue=pos(1,value)[0];
                        continue;
                }
                if(viewdef->string_order[i][0]=='l'){
                        string position=subcopy(viewdef->string_order[i],'(',' ','\0');
                        int* pos2=pos(4,position);
                        if(canvas->coord->type=="World")                
                        elements.push_back(new Line(pos2[0],pos2[1],pos2[2],pos2[3],colorvalue,
                        x_start,y_start,x_start+viewdef->width-1,y_start+viewdef->height-1));
                        else if(canvas->coord->type=="Screen")
                        elements.push_back(new Line(pos2[0],pos2[1],pos2[2],pos2[3],colorvalue,
                        x_start,y_start+1-viewdef->height,x_start+viewdef->width-1,y_start));
                        continue;
                }
                if(viewdef->string_order[i][0]=='t'){
                        string position=subcopy(viewdef->string_order[i],'(',' ','\"');
                        string str=subcopy(viewdef->string_order[i],'\"','\"',')');
                        int* pos2=pos(2,position);
                        if(canvas->coord->type=="World")
                        elements.push_back(new Text(pos2[0],pos2[1],str,colorvalue,
                        x_start,y_start,x_start+viewdef->width-1,y_start+viewdef->height-1));
                        else if(canvas->coord->type=="Screen") 
                        elements.push_back(new Text(pos2[0],pos2[1],str,colorvalue,
                        x_start,y_start+1-viewdef->height,x_start+viewdef->width-1,y_start));
                        continue;
                }
                if(viewdef->string_order[i][0]=='r'){
                        string position=subcopy(viewdef->string_order[i],'(',' ','\0');
                        int* pos2=pos(4,position);
                        if(canvas->coord->type=="World")
                        elements.push_back(new Rectangle(pos2[0],pos2[1],pos2[2],pos2[3],colorvalue,
                        x_start,y_start,x_start+viewdef->width-1,y_start+viewdef->height-1));
                        else if(canvas->coord->type=="Screen") 
                        elements.push_back(new Rectangle(pos2[0],pos2[1],pos2[2],pos2[3],colorvalue,
                        x_start,y_start+1-viewdef->height,x_start+viewdef->width-1,y_start));
                        continue;
                }
                if(viewdef->string_order[i][0]==' '){
                        string limit_string=subcopy(viewdef->string_order[i],':',':','\0');
                        int *limit=pos(4,limit_string);                    //格式为" Line(x1,y1,x2,y2):(x_min,y_min,x_max,y_max)"
                        if(viewdef->string_order[i][1]=='c'){
                                string value=subcopy(viewdef->string_order[i],'(',' ','\0');
                                view_color_value=pos(1,value);
                                continue;
                        }
                        if(viewdef->string_order[i][1]=='l'){
                                string position=subcopy(viewdef->string_order[i],'(',' ','\0');
                                int *pos2=pos(4,position);
                                if(canvas->coord->type=="World")                
                                        elements.push_back(new Line(pos2[0],pos2[1],pos2[2],pos2[3],*view_color_value,
                                        x_start+limit[0],y_start+limit[1],
                                        x_start+limit[2],y_start+limit[3]));
                                else if(canvas->coord->type=="Screen")
                                        elements.push_back(new Line(pos2[0],pos2[1],pos2[2],pos2[3],*view_color_value,
                                        x_start+limit[0],y_start+limit[1]+1-viewdef->height,
                                        x_start+limit[2],y_start+limit[3]+1-viewdef->height));
                                continue;
                        }
                        if(viewdef->string_order[i][1]=='t'){
                        string position=subcopy(viewdef->string_order[i],'(',' ','\"');
                        string str=subcopy(viewdef->string_order[i],'\"','\"',')"');
                        int* pos2=pos(2,position);
                        if(canvas->coord->type=="World") 
                                elements.push_back(new Text(pos2[0],pos2[1],str,*view_color_value,
                                x_start+limit[0],y_start+limit[1],
                                x_start+limit[2],y_start+limit[3]));
                        else if(canvas->coord->type=="Screen")
                                elements.push_back(new Text(pos2[0],pos2[1],str,*view_color_value,
                                x_start+limit[0],y_start+limit[1]+1-viewdef->height,
                                x_start+limit[2],y_start+limit[3]+1-viewdef->height));
                        continue;
                        }
                        if(viewdef->string_order[i][1]=='r'){
                        string position=subcopy(viewdef->string_order[i],'(',' ','\0');
                        int* pos2=pos(4,position);
                        if(canvas->coord->type=="World") 
                                elements.push_back(new Rectangle(pos2[0],pos2[1],pos2[2],pos2[3],*view_color_value,
                                x_start+limit[0],y_start+limit[1],
                                x_start+limit[2],y_start+limit[3]));
                        else if(canvas->coord->type=="Screen")
                                elements.push_back(new Rectangle(pos2[0],pos2[1],pos2[2],pos2[3],*view_color_value,
                                x_start+limit[0],y_start+limit[1]+1-viewdef->height,
                                x_start+limit[2],y_start+limit[3]+1-viewdef->height));
                        continue;
                        }
                }
        }
        }
extern Order oc;                                    //ShowViewat根据坐标系不同有两类，                                          
void View::draw(int x,int y,Canvas* canvas){        //可以写到Coord类中，但这样会导致函数过多，有些乱，                                    
        int orderc=viewdef->string_order.size();    //而且会导致类的依赖关系比较复杂，所以我直接分类讨论了
        oc.ordercount+=orderc;
    for(auto iter=elements.begin();iter<elements.end();iter++){
        if(canvas->coord->type=="World")(*iter)->draw(x_start,y_start,canvas);     
        else if(canvas->coord->type=="Screen")(*iter)->draw(x_start,y_start+1-viewdef->height,canvas);  
    }  
}
