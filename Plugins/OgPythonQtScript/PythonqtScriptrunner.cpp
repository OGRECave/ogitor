#include "PythonqtScriptrunner.h"

#include "PythonQt_QtBindings.h"

using namespace Ogitors;

OgPythonQtScriptRunner::OgPythonQtScriptRunner()
{
    Py_NoSiteFlag = 1;
    Py_InitializeEx(0);
    
    PythonQt::init(PythonQt::IgnoreSiteModule | PythonQt::RedirectStdOut | PythonQt::PythonAlreadyInitialized);
    //PythonQt::init(PythonQt::IgnoreSiteModule | PythonQt::RedirectStdOut);
    PythonQt_init_QtBindings();
    mMainModule = PythonQt::self()->getMainModule();
    connect(PythonQt::self(), SIGNAL(pythonStdOut(const QString&)), this, SLOT(stdOut(const QString&)));
    connect(PythonQt::self(), SIGNAL(pythonStdErr(const QString&)), this, SLOT(stdErr(const QString&)));
    PyRun_SimpleString("print 'hello'");
}
//----------------------------------------------------------------------------
OgPythonQtScriptRunner::~OgPythonQtScriptRunner()
{
    PythonQt::cleanup();
}
const std::string OgPythonQtScriptRunner::getInitMessage()
{
    return std::string("PythonQt Interpreter version shit from QObject powered runner");
}
void OgPythonQtScriptRunner::stdOut(const QString& s)
{
    _stdOut += s;
    int idx;
    while ((idx = _stdOut.indexOf('\n'))!=-1) {
        //consoleMessage(_stdOut.left(idx));
        std::cout << _stdOut.left(idx).toLatin1().data() << std::endl;
        _stdOut = _stdOut.mid(idx+1);
    }
}

void OgPythonQtScriptRunner::stdErr(const QString& s)
{
    _hadError = true;
    _stdErr += s;
    int idx;
    while ((idx = _stdErr.indexOf('\n'))!=-1) {
        //consoleMessage(_stdErr.left(idx));
        std::cerr << _stdErr.left(idx).toLatin1().data() << std::endl;
        _stdErr = _stdErr.mid(idx+1);
    }
}

void OgPythonQtScriptRunner::flushStdOut()
{
    if (!_stdOut.isEmpty()) {
        stdOut("\n");
    }
    if (!_stdErr.isEmpty()) {
        stdErr("\n");
    }
}
