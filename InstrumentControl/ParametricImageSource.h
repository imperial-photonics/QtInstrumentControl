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

   virtual void SetParameter(const QString& parameter, ParameterType type, QVariant value) {};
   virtual QVariant GetParameter(const QString& parameter, ParameterType type) { return QVariant(); };
   virtual QVariant GetParameterLimit(const QString& parameter, ParameterType type, Limit limit) { return 0; };
   virtual QVariant GetParameterMinIncrement(const QString& parameter, ParameterType type) { return QVariant(); }; // returns QVariant() if no min increment
   virtual EnumerationList GetEnumerationList(const QString& parameter) { return EnumerationList(); };
   virtual bool IsParameterWritable(const QString& parameter) { return true; };
   virtual bool IsParameterReadOnly(const QString& parameter) { return false; };

   QMutex* control_mutex;

signals:
   void ControlLockUpdated(bool locked);
};