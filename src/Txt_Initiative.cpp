#include"Element.h"
#include"Txt.h"
Txt::Txt(string s){
    string temp;
    ifstream infile;
    infile.open(s.c_str());
    while(1){
        getline(infile,temp);
        if(infile.fail())break;
        txt.push_back(temp);
    }
    definetxt=new Definetxt(txt);
    roottxt=new Roottxt(txt);
}
Definetxt::Definetxt(vector<string>& save){
    int* pos(int n,string s);
    string subcopy(string s,char start,char del,char end);
    bool flag=0;
    for(int i=0;i<save.size();i++){
        string temp=save[i];
        if(temp[0]=='#'){           //储存预定义文本
            flag=1;
            txt.push_back(temp);
            continue;
        }
        if(temp[0]=='}'){
            txt.push_back(temp);    
            flag=0;
            continue;
        }
        if(flag){
            txt.push_back(temp);
            continue;
        }
    }
}
Roottxt::Roottxt(vector<string>& save){
        for(int i=0;i<save.size();i++){
                if(save[i][0]!='#'&&save[i][0]!='}')
                txt.push_back(save[i]);
        }
}
