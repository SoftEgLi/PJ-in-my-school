//Visitor
class Visitor{
    public: virtual void visit(Txt* txt)=0;
            virtual void visit(Definetxt* definetxt)=0;
            virtual void visit(Roottxt* roottxt)=0;
};
class Canvasvisitor:public Visitor{
    public: void visit(Txt* txt){};
            void visit(Definetxt* definetxt);
            void visit(Roottxt* roottxt);
            Canvasvisitor(){};
};
