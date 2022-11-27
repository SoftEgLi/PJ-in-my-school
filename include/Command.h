class Invoker{
    public:void invoke(string order);
            Invoker(){};
};
class CommandExecuter{
    public:vector<Command*> commands;
            vector<Command*> undocommands;
    void execute(Command* command);
};
class Command{
    public: string type;
            virtual void execute()=0;
            virtual void undo()=0;
            virtual void redo()=0;
            Command(string s):type(s){};
};
class Colorcommand:public Command{
    public: int colorvalue;
            void execute();
            void undo();
            void redo();
            Colorcommand(int value):Command("Color"),colorvalue(value){};
};
class Linecommand:public Command{
    public: int x_from;
            int y_from;
            int x_to;
            int y_to;
            int colorvalue;
            void execute();
            void undo();
            void redo();
            Linecommand(int x_from,int y_from,int x_to,int y_to,int colorvalue):Command("Line"),
            x_from(x_from),y_from(y_from),x_to(x_to),y_to(y_to),colorvalue(colorvalue){};
};
class Textcommand:public Command{
    public: int x_from;
            int y_from;
            string str;
            int colorvalue;
            void execute();
            void undo();
            void redo();
            Textcommand(int x_from,int y_from,string str,int colorvalue):Command("Text"),
            x_from(x_from),y_from(y_from),str(str),colorvalue(colorvalue){};
};
class Rectcommand:public Command{
    public: int x_from;
            int y_from;
            int width;
            int height;
            int colorvalue;
            void execute();
            void undo();
            void redo();
            Rectcommand(int x_from,int y_from,int width,int height,int colorvalue):Command("Rect"),
            x_from(x_from),y_from(y_from),width(width),height(height),colorvalue(colorvalue){};
};
class Viewcommand:public Command{
    public: int x_start;
            int y_start;
            string name;
            void execute();
            void undo();
            void redo();
            Viewcommand(string name,int x_start,int y_start):Command("View"),
            name(name),x_start(x_start),y_start(y_start){};
};