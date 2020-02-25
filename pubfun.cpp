#include "pubfun.h"

PubFun::PubFun()
{

}

bool PubFun::getSubFolders(QString sDir, vector<QString> &vectFolder)
{
    QDir dir(sDir);
    if (!dir.exists())
        return false;

    vectFolder.push_back(sDir);

    dir.setFilter(QDir::Dirs);
    dir.setSorting(QDir::DirsFirst);

    QFileInfoList list = dir.entryInfoList();

    int i = 0;
    bool bIsDir;

    while(i<list.size())
    {
        QFileInfo fileInfo = list.at(i);
        if(fileInfo.fileName()=="."|fileInfo.fileName()=="..")
        {
            i++;
            continue;
        }

        bIsDir = fileInfo.isDir();
        if (bIsDir)
        {
            //vectDir.push_back(fileInfo.filePath());

            getSubFolders(fileInfo.filePath(), vectFolder);
        }
        else
        {

        }

        i++;
    }

    return true;
}

bool PubFun::getFolderFile(QString sDir, QString sFilter, bool bSubDir, vector<QString> &vectFile)
{
    vectFile.clear();

    //get all folders
    vector<QString> vectDir;

    if(bSubDir == true)
        getSubFolders(sDir, vectDir);
    else
        vectDir.push_back(sDir);

    //get files in these folders
    size_t n = vectDir.size();
    for(size_t d=0; d<n; d++)
    {
        QDir dir(vectDir[d]);
        if (!dir.exists())
            return false;

        dir.setFilter(QDir::Files);

        QStringList filterList;
        if(sFilter.isEmpty() == false)
            filterList << sFilter;
        else
            filterList << "*.*";

        QFileInfoList list = dir.entryInfoList(filterList);

        int i = 0;
        while(i<list.size())
        {
            QFileInfo fileInfo = list.at(i);
            if(fileInfo.fileName()=="."|fileInfo.fileName()=="..")
                continue;

            if (!fileInfo.isDir())
            {
                i++;
                QString sName = PubFun::str_replace(fileInfo.filePath(), "\\", "/");
                vectFile.push_back(sName);
            }

        };
    }

    return vectFile.empty()==false;
}

QString PubFun::str_replace(QString sOri, QString sOld, QString sNew)
{
    int oldLen = sOld.length();
    int newLen = sNew.length();

    int i = 0;
    while(1)
    {
        int pos = sOri.indexOf(sOld, i);
        if(pos < 0)
            break;
        sOri.replace(pos, oldLen, sNew);
        i+=newLen;
    }

    return sOri;
}

bool PubFun::str_trimLeft(QString &sOri, QString sTrim)
{
    bool bRet = false;
    while(1)
    {
        int nTrim = sTrim.length();
        int nAll = sOri.length();

        if(nTrim == 0 || nAll == 0)
            break;

        int pos = sOri.indexOf(sTrim);
        if(pos != 0)
            break;

        sOri = sOri.mid(nTrim, nAll-nTrim);
        bRet = true;
    }

    return bRet;
}

bool PubFun::str_trimRight(QString &sOri, QString sTrim)
{
    bool bRet = false;
    while(1)
    {
        int nTrim = sTrim.length();
        int nAll = sOri.length();

        if(nTrim == 0 || nAll == 0)
            break;

        int pos = sOri.lastIndexOf(sTrim);
        if(pos != nAll-nTrim)
            break;

        sOri = sOri.mid(0, nAll-nTrim);
        bRet = true;
    }

    return bRet;
}

bool PubFun::str_separateDirAndFileName(QString sFullName, QString &sDir, QString &sFileName)
{
    int nAll = sFullName.length();
    if(nAll <= 0)
        return false;

    str_replace(sFullName, "\\", "/");

    int pos = sFullName.lastIndexOf("/");
    if(pos < 0)
        return false;

    sDir = sFullName.mid(0,pos);
    sFileName = sFullName.mid(pos+1, nAll-pos-1);

    return true;
}
