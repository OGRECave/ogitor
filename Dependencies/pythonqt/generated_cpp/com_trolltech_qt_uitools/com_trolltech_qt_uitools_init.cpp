#include <PythonQt.h>
#include <PythonQtSystem.h>
#include "com_trolltech_qt_uitools0.h"

PYTHONQT_EXPORT void PythonQt_init_QtUiTools(PyObject* module);

void PythonQt_init_QtUiTools(PyObject* module) {
PythonQt::priv()->registerClass(&QUiLoader::staticMetaObject, "QtUiTools", PythonQtCreateObject<PythonQtWrapper_QUiLoader>, PythonQtSetInstanceWrapperOnShell<PythonQtShell_QUiLoader>, module, 0);

}
