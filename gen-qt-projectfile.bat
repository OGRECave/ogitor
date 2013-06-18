@echo off

set OLD_DIR=%CD%

cd /d %1

echo TRANSLATIONS = \
for /r %1\qtOgitor\languages %%a in (ogitor*.ts) do echo     %%a \
echo.


echo FORMS = \
for /r %1\qtOgitor %%a in (*.ui) do echo     %%a \
echo.

echo SOURCES = \
for /r %1\qtOgitor\src %%a in (*.cpp) do echo     %%a \
for /r %1\qtOgitor\qtpropertybrowser-2.5\src %%a in (*.cpp) do echo     %%a \
echo.

echo HEADERS = \
for /r %1\qtOgitor\include %%a in (*.hxx) do echo     %%a \
for /r %1\qtOgitor\qtpropertybrowser-2.5\src %%a in (*.h) do echo     %%a \
echo.

cd /d %OLD_DIR%
