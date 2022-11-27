class Device{                       //Device类
        public: virtual void setPixel(Element* element,int x,int y,int value)=0;                
                
};
class Device_2:public Device{                   //Device的setPixel的功能是画点，以及防止越界（View,Canvas）
        public:  
                void setPixel(Element* element,int x,int y,int value);
                    
};

class Device_256:public Device{
        public: 
                void setPixel(Element* element,int x,int y,int value);
                
};

class Coord{
        public:virtual int convert(int y)=0;
                string type;
                Coord(string s):type(s){};
                virtual int x_max_limit(ViewDef* insert,ViewDef* inserted,int x_position)=0; 
                virtual int y_max_limit(ViewDef* insert,ViewDef* inserted,int y_position)=0;
                virtual int x_min_limit(ViewDef* insert,ViewDef* inserted,int x_position)=0;
                virtual int y_min_limit(ViewDef* insert,ViewDef* inserted,int y_position)=0;
};
class Screen:public Coord{
        public:int convert(int y){return y;}
                Screen():Coord("Screen"){};
                int x_max_limit(ViewDef* insert,ViewDef* inserted,int x_position);//用于嵌套View
                int y_max_limit(ViewDef* insert,ViewDef* inserted,int y_position);
                int x_min_limit(ViewDef* insert,ViewDef* inserted,int x_position);
                int y_min_limit(ViewDef* insert,ViewDef* inserted,int y_position);
};

class World:public Coord{
        public: int height;
                int convert(int y){return (height-1-y);}
                World(int h):Coord("World"),height(h){};
                int x_max_limit(ViewDef* insert,ViewDef* inserted,int x_position);
                int y_max_limit(ViewDef* insert,ViewDef* inserted,int y_position);
                int x_min_limit(ViewDef* insert,ViewDef* inserted,int x_position);
                int y_min_limit(ViewDef* insert,ViewDef* inserted,int y_position);
};
