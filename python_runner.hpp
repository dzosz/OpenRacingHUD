#ifndef PYTHON_RUNNER_H
#define PYTHON_RUNNER_H

#include <QObject>

class PythonRunner : public QObject
{
    Q_OBJECT
  public:
    PythonRunner();
    ~PythonRunner();

    Q_INVOKABLE void run(const QString& script);
    Q_INVOKABLE QString eval(const QString& script);
    Q_INVOKABLE void    import(const QString& fname);
};

#endif  // PYTHON_RUNNER_H
