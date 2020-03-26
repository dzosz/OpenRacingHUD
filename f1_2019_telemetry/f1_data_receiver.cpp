#include "f1_data_receiver.hpp"

#include <python2.7/Python.h>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

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

static std::string libraryName          = "f1_2019_telemetry_reader";
static std::string receiverStartFunc    = "start";
static std::string receiverStopFunc     = "stop";
static std::string receiverName         = "DataReceiver";
static std::string parserName           = "F12019Parser";
static std::string receiverCallbackFunc = "register";

static PyObject*      receiver;
static PyThreadState* _save;

std::function<void(double, double, double, double)> wheelCallback;

extern "C" PyObject* printOnMessage(PyObject*, PyObject* args)
{
    PyObject* dict;
    auto      unpacked = PyArg_UnpackTuple(args, "mydict", 1, 1, &dict);
    assert(unpacked);
    assert(dict);

    auto motion = PyDict_GetItemString(dict, "Motion");
    assert(motion);
    auto wheelSlipTuple = PyObject_GetAttrString(motion, "wheelSlip");
    assert(wheelSlipTuple);

    PyObject* fl;
    PyObject* fr;
    PyObject* rl;
    PyObject* rr;
    unpacked = PyArg_UnpackTuple(wheelSlipTuple, "mywheels", 1, 4, &rl, &rr, &fl, &fr);
    assert(unpacked);

    assert(wheelCallback);
    wheelCallback(PyFloat_AsDouble(rl), PyFloat_AsDouble(rr), PyFloat_AsDouble(fl),
                  PyFloat_AsDouble(fr));

    // auto res = PyRun_String("print helo", 0, dict, dict);
    // assert(res);
    Py_RETURN_NONE;  // has to return some value!
}

void setDefaultCallback(PyObject* inst)
{
    static PyMethodDef methd          = {"printOnMessage", printOnMessage, METH_VARARGS,
                                "printOnMessage doc"};
    PyObject*          name           = PyString_FromString(methd.ml_name);
    PyObject*          pythonCallback = PyCFunction_NewEx(&methd, NULL, name);
    // auto*       args           = PyTuple_Pack(1, pythonCallback);

    PyObject* callbackName = PyString_FromString(receiverCallbackFunc.c_str());

    auto called = PyObject_CallMethodObjArgs(inst, callbackName, pythonCallback, NULL);
    assert(called);

    Py_DECREF(callbackName);
    Py_DECREF(name);
    // PyTuple_PackPy_DECREF(args);
}

F1DataReceiver::F1DataReceiver()
{
    auto fname = getexepath();
    auto argv  = (char*)fname.c_str();
    setvbuf(stdout, NULL, _IOLBF, 0);  // non buffered stdout
    Py_Initialize();
    PyEval_InitThreads();
    Py_SetProgramName((char*)argv);

    PySys_SetArgv(1, &argv);

    auto pModule = PyImport_ImportModule(libraryName.c_str());
    assert(pModule);

    auto receiverCls = PyObject_GetAttrString(pModule, receiverName.c_str());
    assert(receiverCls);

    auto parserCls = PyObject_GetAttrString(pModule, parserName.c_str());
    assert(parserCls);

    auto parser = PyObject_CallObject(parserCls, NULL);
    assert(parser);

    auto* args = PyTuple_Pack(1, parser);
    assert(args);

    receiver = PyObject_CallObject(receiverCls, args);
    assert(receiver);

    Py_DECREF(args);
}
F1DataReceiver::~F1DataReceiver()
{
    this->stop();
}
void F1DataReceiver::start()
{
    setDefaultCallback(receiver);
    auto called = PyObject_CallMethod(receiver, (char*)receiverStartFunc.c_str(), NULL);
    assert(called);

    _save = PyEval_SaveThread();
}

void F1DataReceiver::setWheelCallback(std::function<void(double, double, double, double)> cb)
{
    wheelCallback = cb;
}

void F1DataReceiver::stop()
{
    PyEval_RestoreThread(_save);
    auto called = PyObject_CallMethod(receiver, (char*)receiverStopFunc.c_str(), NULL);
    assert(!called);
}
