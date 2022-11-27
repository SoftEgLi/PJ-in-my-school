#include"Element.h"
#include"Canvasvisitor.h"
#include"Txt.h"
extern Canvas* canvas;


void Definetxt::accept(Visitor& visitor){
    visitor.visit(this);
}
void Txt::accept(Visitor& visitor){
    definetxt->accept(visitor);
    roottxt->accept(visitor);
    canvas->draw();
    canvas->printresults();
    canvas->showStatistic();
}

void Roottxt::accept(Visitor& visitor){
    visitor.visit(this);
}