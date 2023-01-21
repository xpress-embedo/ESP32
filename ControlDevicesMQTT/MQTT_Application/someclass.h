#ifndef SOMECLASS_H
#define SOMECLASS_H

#include <QObject>

class SomeClass : public QObject
{
  Q_OBJECT
public:
  explicit SomeClass(QObject *parent = nullptr);

signals:

public slots:
  void callMe();

};

#endif // SOMECLASS_H
