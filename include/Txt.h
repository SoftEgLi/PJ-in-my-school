/*      Visitor设计模式，文本文件是Subject，以此来实现文本文件的分区域处理
        accept函数中调用visit函数，visit函数实现对Canvas的操作
*/
class File{
    public:virtual void accept(Visitor& visitor)=0;
};
class Txt:public File{                                  //原文本文件
    private:vector<string>txt;
            Definetxt* definetxt;
            Roottxt* roottxt;
    public: Txt(string s);
            void accept(Visitor& visitor);
                friend class Canvasvisitor;
};

class Definetxt:public File{                            //预定义文本
    private:vector<string>txt;
    public: Definetxt(vector<string>& save);
            void accept(Visitor& visitor);
             friend class Canvasvisitor;
};
class Roottxt:public File{                              //指令文本
        private:
                vector<string>txt;
        public:
                Roottxt(vector<string>& save);
                void accept(Visitor& visitor);
                friend class Canvasvisitor;
};
