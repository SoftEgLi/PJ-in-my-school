#include"Element.h"
#include"Txt.h"
#include"Canvasvisitor.h"
#include"Device.h"
#include"DataObserver.h"
Canvas* canvas;
extern Order oc;
/*      处理文本中的预定义文本，最终在Canvas中添加Viewdef，Viewdef存储的是string，但格式经过规范化
        对于View中调用View的指令，以被调用View的text指令为例，存储格式为“ text(int x1,int x2,string s):(x_min,y_min,x_max,y_max)”
        其中，开头带空格是为了区分调用指令和普通指令，冒号后面的内容是用来实现View中的截断操作。
        View中其他指令则不需要冒号后面的内容，
 */
void Canvasvisitor::visit(Definetxt* definetxt){                     //预定义
    int* pos(int n,string s);
    string subcopy(string s,char start,char del,char end);
    bool flag;
    for(int i=0;i<definetxt->txt.size();i++){
        if(definetxt->txt[i][0]=='#'){                 //新建ViewDef
            string name=subcopy(definetxt->txt[i],'#','#','(');
            string size=subcopy(definetxt->txt[i],'(',' ','{');
            int *pos2=pos(2,size);
            flag=1;
            canvas->viewdefs.push_back(new ViewDef(name,pos2[0],pos2[1]));
            delete []pos2;
            continue;
        }
        if(definetxt->txt[i][0]=='}'){
            flag=0;
            continue;
        }
        if(flag){                           //ViewDef增加指令
            if(definetxt->txt[i][1]=='c'){
                string order=subcopy(definetxt->txt[i],' ',' ','\0');
                canvas->viewdefs[canvas->viewdefs.size()-1]->addinstrument(order);
                continue;
            }
            if(definetxt->txt[i][1]=='l'){
                string order=subcopy(definetxt->txt[i],' ',' ','\0');
                canvas->viewdefs[canvas->viewdefs.size()-1]->addinstrument(order);
                continue;
            }
            if(definetxt->txt[i][1]=='t'){
                string order=subcopy(definetxt->txt[i],' ',' ','\0');
                canvas->viewdefs[canvas->viewdefs.size()-1]->addinstrument(order);
                continue;
            }
            if(definetxt->txt[i][1]=='r'){
                string order=subcopy(definetxt->txt[i],' ',' ','\0');
                canvas->viewdefs[canvas->viewdefs.size()-1]->addinstrument(order);
                continue;
            } 
            if(definetxt->txt[i][1]=='!'){              //处理嵌套的View  
                string name=subcopy(definetxt->txt[i],'!','!','(');
                string temp=subcopy(definetxt->txt[i],'(',' ','\0');
                int *offset=pos(2,temp);
                int j;
                for(j=0;j<canvas->viewdefs.size();j++){
                    if(name==canvas->viewdefs[j]->name){                 
                        for(int k=0;k<canvas->viewdefs[j]->string_order.size();k++){
                            if(canvas->viewdefs[j]->string_order[k][0]=='c'){
                               string new_order=" ";        //空一格
                               new_order+=canvas->viewdefs[j]->string_order[k];
                               canvas->viewdefs[canvas->viewdefs.size()-1]->addinstrument(new_order);
                               continue;
                           }
                           if(canvas->viewdefs[j]->string_order[k][0]=='l'){        
                                string old_order=subcopy(canvas->viewdefs[j]->string_order[k],'(',' ','\0');
                                int *pos3=pos(4,old_order);
                                pos3[0]+=offset[0]; 
                                pos3[2]+=offset[0];              
                                if(canvas->coord->type=="Screen"){
                                    pos3[1]+=offset[1]+1-canvas->viewdefs[j]->height;
                                    pos3[3]+=offset[1]+1-canvas->viewdefs[j]->height;
                                    }
                                else {
                                    pos3[1]+=offset[1];
                                    pos3[3]+=offset[1];
                                    }                           //根据嵌套View的位置修改参数，比如line中的x_from,y_from,x_to,y_to均加上x_offset,y_offset
                                string x_from=to_string(pos3[0]);
                                string y_from=to_string(pos3[1]);
                                string x_to=to_string(pos3[2]);
                                string y_to=to_string(pos3[3]);
                                string new_order=" line(";
                                new_order+=x_from;
                                new_order+=",";
                                new_order+=y_from;
                                new_order+=",";
                                new_order+=x_to;
                                new_order+=",";
                                new_order+=y_to;
                                new_order+=")";
                                int x_min,y_min;
                               int x_max,y_max;
                               x_min=canvas->coord->x_min_limit(canvas->viewdefs[j],canvas->viewdefs.back(),offset[0]);
                               y_min=canvas->coord->y_min_limit(canvas->viewdefs[j],canvas->viewdefs.back(),offset[1]);
                               x_max=canvas->coord->x_max_limit(canvas->viewdefs[j],canvas->viewdefs.back(),offset[0]);
                               y_max=canvas->coord->y_max_limit(canvas->viewdefs[j],canvas->viewdefs.back(),offset[1]);
                                new_order+=":(";
                                new_order+=to_string(x_min);
                                new_order+=",";
                                new_order+=to_string(y_min);
                                new_order+=",";
                                new_order+=to_string(x_max);
                                new_order+=",";
                                new_order+=to_string(y_max);
                                new_order+=")";
                                canvas->viewdefs[canvas->viewdefs.size()-1]->addinstrument(new_order);
                                continue;
                           }
                           if(canvas->viewdefs[j]->string_order[k][0]=='t'){         //嵌套View的text命令
                                string old_position=subcopy(canvas->viewdefs[j]->string_order[k],'(',' ','\"');
                                string old_text=subcopy(canvas->viewdefs[j]->string_order[k],'\"','\"',')');
                                int *pos3=pos(2,old_position);
                                pos3[0]+=offset[0];
                                if(canvas->coord->type=="Screen")pos3[1]+=offset[1]+1-canvas->viewdefs[j]->height;
                                else pos3[1]+=offset[1];
                                string x_from=to_string(pos3[0]);
                                string y_from=to_string(pos3[1]);
                                string new_order=" text(";
                                new_order+=x_from;
                                new_order+=",";
                                new_order+=y_from;
                                new_order+=",\"";
                                new_order+=old_text;
                                new_order+="\")";
                                int width_temp,height_temp;
                                    int x_min,y_min;
                                int x_max,y_max;
                                x_min=canvas->coord->x_min_limit(canvas->viewdefs[j],canvas->viewdefs.back(),offset[0]);
                                y_min=canvas->coord->y_min_limit(canvas->viewdefs[j],canvas->viewdefs.back(),offset[1]);
                                x_max=canvas->coord->x_max_limit(canvas->viewdefs[j],canvas->viewdefs.back(),offset[0]);
                                y_max=canvas->coord->y_max_limit(canvas->viewdefs[j],canvas->viewdefs.back(),offset[1]);
                                    new_order+=":(";
                                    new_order+=to_string(x_min);
                                    new_order+=",";
                                    new_order+=to_string(y_min);
                                    new_order+=",";
                                    new_order+=to_string(x_max);
                                    new_order+=",";
                                    new_order+=to_string(y_max);
                                    new_order+=")";
                                canvas->viewdefs[canvas->viewdefs.size()-1]->addinstrument(new_order);
                                continue;
                            }
                            if(canvas->viewdefs[j]->string_order[k][0]=='r'){         //嵌套View的rec命令
                                string old_position=subcopy(canvas->viewdefs[j]->string_order[k],'(',' ','\"');
                                int *pos3=pos(4,old_position);
                                pos3[0]+=offset[0];
                                if(canvas->coord->type=="Screen")pos3[1]+=offset[1]+1-canvas->viewdefs[j]->height;
                                else pos3[1]+=offset[1];
                                string x_from=to_string(pos3[0]);
                                string y_from=to_string(pos3[1]);
                                string wid=to_string(pos3[2]);
                                string hei=to_string(pos3[3]);
                                string new_order=" rect(";
                                new_order+=x_from;
                                new_order+=",";
                                new_order+=y_from;
                                new_order+=",";
                                new_order+=wid;
                                new_order+=",";
                                new_order+=hei;
                                new_order+=")";
                                int x_min,y_min;
                                int x_max,y_max;
                                x_min=canvas->coord->x_min_limit(canvas->viewdefs[j],canvas->viewdefs.back(),offset[0]);
                                y_min=canvas->coord->y_min_limit(canvas->viewdefs[j],canvas->viewdefs.back(),offset[1]);
                                x_max=canvas->coord->x_max_limit(canvas->viewdefs[j],canvas->viewdefs.back(),offset[0]);
                                y_max=canvas->coord->y_max_limit(canvas->viewdefs[j],canvas->viewdefs.back(),offset[1]);
                                new_order+=":(";
                                new_order+=to_string(x_min);
                                new_order+=",";
                                new_order+=to_string(y_min);
                                new_order+=",";
                                new_order+=to_string(x_max);
                                new_order+=",";
                                new_order+=to_string(y_max);
                                new_order+=")";
                                canvas->viewdefs[canvas->viewdefs.size()-1]->addinstrument(new_order);
                                continue;
                           }
                        }
                    }
                }
                continue;
            }
        }
        string new_order=subcopy(definetxt->txt[i],' ',' ','\0');
        canvas->viewdefs[canvas->viewdefs.size()-1]->addinstrument(new_order);//除嵌套View之外的指令
    }
}
/*      处理文本中的操作指令文本，最终在Canvas中添加Element 
*/
void Canvasvisitor::visit(Roottxt* roottxt){              
    int* pos(int n,string s);
    string subcopy(string s,char start,char del,char end);
    for(int i=0;i<roottxt->txt.size();i++){
        if(roottxt->txt[i][0]=='!'){
            string name=subcopy(roottxt->txt[i],'!','!','(');
            string temp=subcopy(roottxt->txt[i],'(',' ','\0');
            int *pos2=pos(2,temp);
            ViewDef* v1=canvas->findViewDef(name);
            canvas->elements.push_back(new View(v1,pos2[0],pos2[1]));
            delete []pos2;
            continue;
        }
        if(roottxt->txt[i][0]=='c'){
            oc.ordercount++;
            string temp=subcopy(roottxt->txt[i],'(',' ','\0');
            int *pos2=pos(1,temp);
            canvas->colorvalue=pos2[0];
            delete []pos2;
            continue;
        }
        if(roottxt->txt[i][0]=='l'){
            string temp=subcopy(roottxt->txt[i],'(',' ','\0');
            int *pos2=pos(4,temp);
            canvas->elements.push_back(new Line(pos2[0],pos2[1],pos2[2],pos2[3],canvas->colorvalue));
            delete []pos2;
            continue;
        }
        if(roottxt->txt[i][0]=='t'){
            string temp=subcopy(roottxt->txt[i],'(',' ','\"');
            int *pos2=pos(2,temp);
            string str=subcopy(roottxt->txt[i],'\"','\"',')');
            canvas->elements.push_back(new Text(pos2[0],pos2[1],str,canvas->colorvalue));
            delete []pos2;
            continue;
        }
        if(roottxt->txt[i][0]=='r'){
            string temp=subcopy(roottxt->txt[i],'(',' ','\"');
            int *pos2=pos(4,temp);
            canvas->elements.push_back(new Rectangle(pos2[0],pos2[1],pos2[2],pos2[3],canvas->colorvalue));
            delete []pos2;
            continue;
        }
    }
}
