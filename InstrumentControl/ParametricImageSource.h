#pragma once

#include "ImageSource.h"
#include <QVariant>
#include <QMutex>

enum ParameterType { Integer, Float, Boolean, Text, Enumeration };
enum Limit { Min, Max };

typedef QList<QPair<QString, int>> EnumerationList;

class ParametericImageSource : public ImageSource
{
   Q_OBJECT

public:
   virtual void SetParameter(const QString& parameter, ParameterType type, QVariant value) = 0;
   virtual QVariant GetParameter(const QString& parameter, ParameterType type) = 0;
   virtual QVariant GetParameterLimit(const QString& parameter, ParameterType type, Limit limit) = 0;
   virtual QVariant GetParameterMinIncrement(const QString& parameter, ParameterType type) { return QVariant(); }; // returns QVariant() if no min increment
   virtual EnumerationList GetEnumerationList(const QString& parameter) = 0;
   virtual bool IsParameterWritable(const QString& parameter) { return true; };
   virtual bool IsParameterReadOnly(const QString& parameter) { return false; };

   QMutex* control_mutex;

signals:
   void ControlLockUpdated(bool locked);
};