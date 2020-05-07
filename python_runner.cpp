#include "python_runner.hpp"

#include <python2.7/Python.h>
#include <QDebug>
#include <QDir>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

namespace
{
#if defined(WINDOWS) || defined(__MINGW32__) || defined(__MINGW64__)
#include <windows.h>
#include <string>
std::string getexepath()
{
    char result[MAX_PATH];
    return std::string(result, GetModuleFileName(NULL, result, MAX_PATH));
}
#else
#include <limits.h>
#include <unistd.h>
#include <string>
std::string getexepath()
{
    char    result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    return std::string(result, (count > 0) ? count : 0);
}
#endif

static PyThreadState* _save;
}  // namespace

PythonRunner::PythonRunner()
{
    auto fname = getexepath();
    assert(!fname.empty());

    auto argv = (char*)fname.c_str();

    setvbuf(stdout, NULL, _IOLBF, 0);  // non buffered stdout

    assert(!Py_IsInitialized());

#if defined(WINDOWS) || defined(__MINGW32__) || defined(__MINGW64__)

    // TODO to the same for linux binaries?
    // required path to standard library so the user doesn't need his own Python installation
    auto currDir = QDir(fname.c_str());
    currDir.cdUp();
    currDir.cd("python");
    qDebug() << "PythonRunner::SetPythonHome " << currDir.path().toStdString().c_str();
    Py_SetPythonHome((char*)currDir.path().toStdString().c_str());
#endif

    Py_SetProgramName((char*)argv);
    Py_Initialize();

#if defined(WINDOWS) || defined(__MINGW32__) || defined(__MINGW64__)
    // TODO, use syspath? append instead modifying?
    // TODO is it really doing anything after py_initialize?
    // qDebug() << "PythonRunner::SetPath " << currDir.path().toStdString().c_str();
    auto namevalue = std::string{"PYTHONPATH="} + currDir.path().toStdString();
    qDebug() << "PythonRunner::set env " << namevalue.c_str();
    // PySys_SetPath((char*)currDir.path().toStdString().c_str());
    _putenv(namevalue.c_str());
#endif

    if (!PyEval_ThreadsInitialized())
    {
        qDebug() << "PythonRunner::PythonRunner PyEval_InitThreads ";
        PyEval_InitThreads();
    }

    assert(Py_IsInitialized());

    PyRun_SimpleString("import sys; sys.path.append('scripts/')");

    // PySys_SetArgv(1, &argv);
    _save = PyEval_SaveThread();
}
PythonRunner::~PythonRunner()
{
    PyEval_RestoreThread(_save);
    Py_Finalize();
}

void PythonRunner::run(const QString& script)
{
    qDebug() << "PythonRunner::run " << script;
    assert(!script.isEmpty());
    auto str = script.toStdString();

    PyEval_RestoreThread(_save);

    auto ret = PyRun_SimpleString(str.c_str());

    _save = PyEval_SaveThread();
    if (ret)
    {
        throw std::runtime_error(std::string{"PythonRunner::run failed on "} + str);
    }
}

QString PythonRunner::eval(const QString& script)
{
    // qDebug() << "PythonRunner::eval " << script;
    assert(!script.isEmpty());
    auto str = script.toStdString();

    PyEval_RestoreThread(_save);

    static PyObject* main             = PyImport_AddModule("__main__");
    static PyObject* globalDictionary = PyModule_GetDict(main);
    static PyObject* localDictionary  = PyDict_New();

    static PyObject* obj;
    if (obj)
    {
        // Py_DECREF(obj);
    }
    obj = PyRun_String(str.c_str(), Py_eval_input, globalDictionary, localDictionary);

    QString data;
    if (!obj)
    {
        _save = PyEval_SaveThread();
        throw std::runtime_error(std::string{"PythonRunner::eval failed on "} + str);
    }
    else
    {
        const char* s = PyString_AsString(obj);
        if (s)
        {
            data = s;
        }
    }

    // FIXME crashes on windows
    // Py_CLEAR(compiled);  // or Py_DECREF

    _save = PyEval_SaveThread();

    return data;
}

void PythonRunner::import(const QString& fname)
{
    qDebug() << "PythonRunner::import " << fname;
    assert(!fname.isEmpty());
    auto str    = fname.toStdString();
    auto extPos = str.find(".py");
    if (extPos != std::string::npos)
    {
        str = str.substr(0, extPos);
    }
    auto cmd = std::string{"import "} + str + ";";

    PyEval_RestoreThread(_save);
    auto ret = PyRun_SimpleString(cmd.c_str());
    _save    = PyEval_SaveThread();

    if (ret)
    {
        throw std::runtime_error(std::string{"PythonRunner::import failed on "} + str);
    }
}
