#include<iostream>
#include<string>
#include"asciis.h"
using namespace std;
int* pos(int n,string s){           //n为数目，含有双括号的
    int* pos2=new int[n];
    int sum=0;
    int count=-1;
    for(int i=1;s[i];i++){
        if(s[i]==','||s[i]==')'){
            count++;
            pos2[count]=sum;
            sum=0;
            continue;
        }
        sum*=10;
        sum+=s[i]-'0';
    }
    return pos2;
}
string subcopy(string s,char start,char del,char end){     //复制字符串，delete是删除字符，end是终止字符
    string temp;
    bool flag=0;
    for(int i=0;s[i]!=end;i++){
        if(s[i]==start)flag=1;
        if(flag){
            if(s[i]==del)continue;
            temp+=s[i];
        }
    }
    return temp;
}
string hex_to_binary(int pos,int i){          //将nAsciiDot中的十六进制数转换为2进制格式的字符串（8位）
    int temp=nAsciiDot[pos*8+i];
    string s;
    int div=128;//2的7次方
    for(int j=7;j>=0;j--){                  
        s+=temp/div+'0';
        temp%=div;
        div/=2;
    }
    return s;
}