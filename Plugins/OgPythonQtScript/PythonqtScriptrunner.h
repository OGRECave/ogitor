#ifndef PYTHONQTSCRIPTRUNNER_H
#define PYTHONQTSCRIPTRUNNER_H

#include "PythonQt.h"

namespace Ogitors
{
    class OgPythonQtScriptRunner: public QObject
    {
        Q_OBJECT
    public:
        OgPythonQtScriptRunner();
        virtual ~OgPythonQtScriptRunner();

        // return Interpreter Init Message
        const std::string getInitMessage();

    public slots:
        //! output redirection
        void stdOut(const QString& s);
        //! output redirection
        void stdErr(const QString& s);

    protected:
        //! flush output that was not yet printed
        void flushStdOut();

    private:
        PythonQtObjectPtr mMainModule;
        QString _stdOut;
        QString _stdErr;

        bool _hadError;
    };
}
#endif // PYTHONQTSCRIPTRUNNER_H
