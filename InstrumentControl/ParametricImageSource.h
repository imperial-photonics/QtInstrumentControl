#pragma once

#include "ImageSource.h"
#include <QVariant>
#include <QMutex>

enum ParameterType { Integer, Float, Boolean, Text, Enumeration };
enum Limit { Min, Max };

typedef QList<QPair<QString, int>> EnumerationList;

class ParametricImageSource : public ImageSource
{
   Q_OBJECT

public:

   ParametricImageSource(QObject* parent = 0) :
      ImageSource(parent) {}

   virtual void setParameter(const QString& parameter, ParameterType type, QVariant value) {};
   virtual QVariant getParameter(const QString& parameter, ParameterType type) { return QVariant(); };
   virtual QVariant getParameterLimit(const QString& parameter, ParameterType type, Limit limit) { return 0; };
   virtual QVariant getParameterMinIncrement(const QString& parameter, ParameterType type) { return QVariant(); }; // returns QVariant() if no min increment
   virtual EnumerationList getEnumerationList(const QString& parameter) { return EnumerationList(); };
   virtual bool isParameterWritable(const QString& parameter) { return true; };
   virtual bool isParameterReadOnly(const QString& parameter) { return false; };

   QMutex* control_mutex;

signals:
   void controlLockUpdated(bool locked);
};