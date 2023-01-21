#include "someclass.h"
#include "QDebug"

SomeClass::SomeClass(QObject *parent)
  : QObject{parent}
{

}

void SomeClass::callMe()
{
  qDebug() << "I am being called!";
}
