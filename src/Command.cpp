#include"Element.h"
#include"CanvasVisitor.h"
#include"Command.h"
#include"Txt.h"
#include"DataObserver.h"
extern Canvas* canvas;
extern CommandExecuter ce;
extern Order oc;
int colorcount;
void CommandExecuter::execute(Command* command){
    command->execute();
}

void Colorcommand::execute(){               //后续可以添加observer
    canvas->colorvalue=colorvalue;
    colorcount++;
    oc.ordercount++;
}
void Colorcommand::undo(){
    int count=0;
    int count2=0;
    colorcount--;
    oc.ordercount--;
    for(auto iter=ce.commands.begin();iter<ce.commands.end();iter++){
        if((*iter)->type=="Color")count++;
    }
    for(auto iter=ce.commands.begin();iter<ce.commands.end();iter++){
        if((*iter)->type=="Color")count2++;
        if(count2==count-1){
            (*iter)->execute();
            colorcount--;
            oc.ordercount--;
        }
    }
}
void Colorcommand::redo(){
    canvas->colorvalue=colorvalue;
    colorcount++;
    oc.ordercount++;
}

void Linecommand::execute(){
    canvas->elements.push_back(new Line(x_from,y_from,x_to,y_to,colorvalue));
    canvas->clear();
    canvas->draw();
    canvas->printresults();
}
void Linecommand::undo(){
    canvas->elements.pop_back();
    canvas->clear();
    canvas->draw();
    canvas->printresults();
}
void Linecommand::redo(){
    canvas->elements.push_back(new Line(x_from,y_from,x_to,y_to,colorvalue));
    canvas->clear();
    canvas->draw();
    canvas->printresults();
}

void Textcommand::execute(){
    canvas->elements.push_back(new Text(x_from,y_from,str,colorvalue));
    canvas->clear();
    canvas->draw();
    canvas->printresults();
}
void Textcommand::undo(){
    canvas->elements.pop_back();
    canvas->clear();
    canvas->draw();
    canvas->printresults();
}
void Textcommand::redo(){
    canvas->elements.push_back(new Text(x_from,y_from,str,colorvalue));
    canvas->clear();
    canvas->draw();
    canvas->printresults();
}

void Rectcommand::execute(){
    canvas->elements.push_back(new Rectangle(x_from,y_from,width,height,colorvalue));
    canvas->clear();
    canvas->draw();
    canvas->printresults();
}
void Rectcommand::undo(){
    canvas->elements.pop_back();
    canvas->clear();
    canvas->draw();
    canvas->printresults();
}
void Rectcommand::redo(){
    canvas->elements.push_back(new Rectangle(x_from,y_from,width,height,colorvalue));
    canvas->clear();
    canvas->draw();
    canvas->printresults();
}

void Viewcommand::execute(){
    canvas->elements.push_back(new View(canvas->findViewDef(name),x_start,y_start));
    canvas->clear();
    canvas->draw();
    canvas->printresults();
}
void Viewcommand::undo(){
    canvas->elements.pop_back();
    canvas->clear();
    canvas->draw();
    canvas->printresults();
}
void Viewcommand::redo(){
    canvas->elements.push_back(new View(canvas->findViewDef(name),x_start,y_start));
    canvas->clear();
    canvas->draw();
    canvas->printresults();
}

void Invoker::invoke(string s){
    int* pos(int n,string s);
    string subcopy(string s,char start,char del,char end);
    if(s[0]=='c'){
        string value=subcopy(s,'(',' ','\0');
        int *pos2=pos(1,value);
        ce.commands.push_back(new Colorcommand(pos2[0]));
        ce.commands[ce.commands.size()-1]->execute();
    }
    else if(s[0]=='l'){
        string position=subcopy(s,'(',' ','\0');
        int *pos2=pos(4,position);
        ce.commands.push_back(new Linecommand(pos2[0],pos2[1],pos2[2],pos2[3],canvas->colorvalue));
        ce.commands[ce.commands.size()-1]->execute();
    }
    else if(s[0]=='t'){
        string position=subcopy(s,'(',' ','\"');
        string str=subcopy(s,'\"','\"',')');
        int *pos2=pos(2,position);
        ce.commands.push_back(new Textcommand(pos2[0],pos2[1],str,canvas->colorvalue));
        ce.commands[ce.commands.size()-1]->execute();
    }
    else if(s[0]=='r'&&s[2]=='c'){
        string position=subcopy(s,'(',' ','\0');
        int *pos2=pos(4,position);
        ce.commands.push_back(new Rectcommand(pos2[0],pos2[1],pos2[2],pos2[3],canvas->colorvalue));
        ce.commands[ce.commands.size()-1]->execute();
    }
    else if(s[0]=='!'){
        string position=subcopy(s,'(',' ','\0');
        string name=subcopy(s,'!','!','(');
        int *pos2=pos(2,position);
        ce.commands.push_back(new Viewcommand(name,pos2[0],pos2[1]));
        ce.commands[ce.commands.size()-1]->execute();
    }
    else if(s[0]=='u'&&s[1]=='s'){
        string name=subcopy(s,'\"','\"',')');
        ifstream infile(name.c_str());
        string temp;
        vector<string>save;
        while(1){
            getline(infile,temp);
            if(infile.fail())break;
            save.push_back(temp);
        }
            Definetxt txt(save);
            Canvasvisitor visitor;
            visitor.visit(&txt);
    }
    else if(s[0]=='u'&&s[1]=='n'){
        ce.undocommands.push_back(ce.commands[ce.commands.size()-1]);
        ce.commands[ce.commands.size()-1]->undo();
        ce.commands.pop_back();
    }
    else if(s[0]=='r'&&s[2]=='d'){
        ce.commands.push_back(ce.undocommands[ce.undocommands.size()-1]);
        ce.undocommands[ce.undocommands.size()-1]->redo();
        ce.undocommands.pop_back();
    }
    else if(s[0]=='s'&&s[1]=='t'){
        canvas->showStatistic();
    }
    else if(s[0]=='s'&&s[1]=='h'){
        canvas->printresults();
    }
}


