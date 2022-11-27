//用来统计数据的Observer
//Element中Txt是subject
//由于只写关于Txt的update代码过于密集
//所以也引入对于其它和统计信息相关的Element的update函数
class Observer{
    public:     
        virtual void update(Line* element)=0;
        virtual void update(Text* element)=0;
        virtual void update(Rectangle* element)=0;
        virtual void show()=0;
};
class Order:public Observer{            //统计指令数目     
    public: 
        int ordercount=0;
        void update(Line* element);
        void update(Text* element);
        void update(Rectangle* element);
        void show();
};

class Linelength:public Observer{           //统计直线长度
    private:
        double lengthcount=0.0;
    public: 
        void update(Line* element);
        void update(Text* element){};
        void update(Rectangle* element);
        void show();
};

class StringList:public Observer{               //统计字符串
    private:
        vector<string>save; 
    public:         
        void update(Line* element){};
        void update(Text* element);
        void update(Rectangle* element){};
        void show();
};

class Important:public Observer{
        private:
                ofstream outfile;
        public:
                Important();
                void update(Line* element);
                void update(Text* element);
                void update(Rectangle* element);
                void show(){};
};


