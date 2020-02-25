#ifndef PUBFUN_H
#define PUBFUN_H

#include "predefine.h"

class PubFun
{
public:
    PubFun();
private:
    static bool getSubFolders(QString sDir, vector<QString> &vectFolder);
public:
    static bool getFolderFile(QString sDir, QString sFilter, bool bSubDir, vector<QString> &vectFile);

    static QString str_replace(QString sOri, QString sOld, QString sNew);

    static bool str_trimLeft(QString &sOri, QString sTrim);

    static bool str_trimRight(QString &sOri, QString sTrim);

    static bool str_separateDirAndFileName(QString sFullName, QString &sDir, QString &sFileName);
};

#endif // PUBFUN_H
