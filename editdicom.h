#ifndef EDITDICOM_H
#define EDITDICOM_H

#include "predefine.h"
#include <QFile>

typedef struct _EDCMOBJ
{
    DcmDataset *pDs;
    DcmObject* pDcmObj;
    vector<_EDCMOBJ*> child;

    _EDCMOBJ()
        :pDs(nullptr)
        ,pDcmObj(nullptr)
    {}

    ~_EDCMOBJ()
    {
        int n = child.size();
        for(int i=0;i<n;i++)
            delete child[i];

        DcmElement* p = pDs->remove(pDcmObj);
        qDebug() << "removeChild = " << (long)p;
    }
}EDCMOBJ;

class EditDicom : public QObject
{
    Q_OBJECT

public:
    EditDicom();

signals:
    void setProcessInfo(int nCur, int nAll, int nError, int nWarnning);
    void setStatusBarMsg(QString sMsg);
    void endEdit(bool b);
private:

    bool m_bEditDcmSub;

    vector<TAGVALUE> m_tagList;

    QString m_SrcDir;

    QString getNewFullName(QString sOldFullName, QString sOldTopDir,QString sNewTopDir);

    int editDicomInfo(DcmDataset* pDs, vector<TAGVALUE> &tagList, QString *sErrorMsg);

    void getChildTag(DcmObject *pEle, vector<DcmObject *> &v);

    void getAllTag(DcmDataset* pDs, vector<DcmObject *> &v);

    int editTag(vector<DcmObject *> &v, TAGVALUE &tag, QString *sErrorMsg);
/*
    int getChildESTag(DcmDataset* pDs, EDCMOBJ* pESObj);
    int deleteDcmObj(DcmDataset* pDs, DcmObject* pDcmObj);
*/
    int deleteTag(DcmDataset* pDs, vector<DcmObject *> &v, TAGVALUE &tag, QString *sErrorMsg);

public:
    static OFCondition saveDcmFile(DcmFileFormat &fm, QString sFileName);

public:

    vector <QString> m_vectFile;

    bool setTagList(QString sIniName, QString sGroupName);

    bool setSourceDir(QString sDir, QString sFilter, bool bSubDir);

    bool editToNewDir(QString sNewDir);

    bool editToOldDir();
};

#endif // EDITDICOM_H
