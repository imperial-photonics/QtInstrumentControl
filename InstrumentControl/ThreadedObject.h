#pragma once

#include <QThread>

 
class ThreadedObject : public QObject
{
   Q_OBJECT
public:
   ThreadedObject(QObject* parent = nullptr, QThread* ex_thread = nullptr);

   void StartThread();

   QThread* GetThread();

   void ParentDestroyed();

   virtual ~ThreadedObject();

   virtual void Init() = 0;


protected:
   QThread* thread;
   bool private_thread;
};

