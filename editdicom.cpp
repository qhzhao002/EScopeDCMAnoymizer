#include "editdicom.h"
#include "pubfun.h"

#include <QSettings>
#include "mainwindow.h"

//extern MainWindow* g_pMainWnd;

bool g_bInitCode = false;

EditDicom::EditDicom()
    :m_bEditDcmSub(false)
{
    if(g_bInitCode == false)
    {
//        DJDecoderRegistration::registerCodecs();
//        DJEncoderRegistration::registerCodecs();
        g_bInitCode = true;
    }
}

QString EditDicom::getNewFullName(QString sOldFullName, QString sOldTopDir, QString sNewTopDir)
{
    QString sNewFullName = PubFun::str_replace(sOldFullName, sOldTopDir, sNewTopDir);

    QString sNewDir, sFileName;
    PubFun::str_separateDirAndFileName(sNewFullName, sNewDir, sFileName);

    QDir d(sNewDir);
    if(d.exists() == false)
    {
        if(d.mkpath(sNewDir) == false)
        {
            return "";
        }
    }

    return sNewFullName;
}

void EditDicom::getChildTag(DcmObject *pEle, vector<DcmObject *> &v)
{
    if(EVR_SQ == pEle->getVR())
    {
        v.push_back(pEle);

        DcmObject * pChildObj = pEle->nextInContainer(nullptr);
        while(pChildObj)
        {
            getChildTag(pChildObj, v);
            pChildObj = pEle->nextInContainer(pChildObj);
        }
    }
    else if(pEle->getTag() == DCM_Item)
    {
        v.push_back(pEle);

        DcmObject * pChildObj = pEle->nextInContainer(nullptr);
        while(pChildObj)
        {
            getChildTag(pChildObj, v);
            pChildObj = pEle->nextInContainer(pChildObj);
        }
    }
    else
    {
        v.push_back(pEle);
    }
}

void EditDicom::getAllTag(DcmDataset *pDs, vector<DcmObject *> &v)
{
    if(pDs == nullptr)
        return;

    DcmObject * pObj = pDs->nextInContainer(nullptr);
    while(pObj)
    {
        getChildTag(pObj,v);
        pObj = pDs->nextInContainer(pObj);
    }
}

int EditDicom::editTag(vector<DcmObject *> &v, TAGVALUE &tag, QString *sErrorMsg)
{
    int nRet = 0;

    unsigned long nTag = strtoul(tag.sTag.toStdString().c_str(), nullptr, 16);
    DcmTagKey key(nTag>>16, (nTag<<16) >> 16);

    int n = v.size();
    for(int i=0;i<n;i++)
    {
        DcmElement* pEle = (DcmElement*)v[i];
        if(pEle->getTag().getXTag() == key)
        {
            //if(pEle->putOFStringArray(t.sValue.toStdString().c_str()).bad())
            OFCondition of;
            of = pEle->putString(tag.sValue.toStdString().c_str());

            if(of.bad())
            {
                nRet = -1;

                if(sErrorMsg)
                {
                    OFString temp_str;
                    DimseCondition::dump(temp_str, of);

                    *sErrorMsg = temp_str.c_str();
                }
            }
        }
    }

    return nRet;
}
/*
int EditDicom::getChildESTag(DcmDataset *pDs, EDCMOBJ *pESObj)
{
    if(!pESObj)
        return -1;
    if(!pESObj->pDcmObj)
        return -2;

    if(EVR_SQ == pESObj->pDcmObj->getVR())
    {
        DcmObject * pDcmChildObj = pESObj->pDcmObj->nextInContainer(nullptr);
        while(pDcmChildObj)
        {
            EDCMOBJ* pChildESObj = new EDCMOBJ;
            pChildESObj->pDs = pDs;
            pChildESObj->pDcmObj = pDcmChildObj;
            getChildESTag(pDs, pChildESObj);
            pESObj->child.push_back(pChildESObj);

            pDcmChildObj = pESObj->pDcmObj->nextInContainer(pDcmChildObj);
        }
    }
    else if(pESObj->pDcmObj->getTag() == DCM_Item)
    {
        DcmObject * pDcmChildObj = pESObj->pDcmObj->nextInContainer(nullptr);
        while(pDcmChildObj)
        {
            EDCMOBJ* pChildESObj = new EDCMOBJ;
            pChildESObj->pDs = pDs;
            pChildESObj->pDcmObj = pDcmChildObj;
            getChildESTag(pDs, pChildESObj);
            pESObj->child.push_back(pChildESObj);

            pDcmChildObj = pESObj->pDcmObj->nextInContainer(pDcmChildObj);
        }
    }
    else
    {
        //no child element
    }

    return 0;
}

int EditDicom::deleteDcmObj(DcmDataset *pDs, DcmObject *pDcmObj)
{
    if(!pDs || !pDcmObj)
        return -1;

    EDCMOBJ *pESObj = new EDCMOBJ;
    pESObj->pDs = pDs;
    pESObj->pDcmObj = pDcmObj;
    getChildESTag(pDs, pESObj);
    delete pESObj;

    DcmElement* p = pDs->remove(pDcmObj);
    qDebug() << "remove = " << (long)p;

    return 0;
}
*/
int EditDicom::deleteTag(DcmDataset *pDs, vector<DcmObject *> &v, TAGVALUE &tag, QString *sErrorMsg)
{
    int nRet = 0;

    unsigned long nTag = strtoul(tag.sTag.toStdString().c_str(), nullptr, 16);
    DcmTagKey key(nTag>>16, (nTag<<16) >> 16);

    int n = v.size();
    for(int i=0;i<n;i++)
    {
        DcmElement* pEle = (DcmElement*)v[i];
        if(pEle->getTag().getXTag() == key)
        {
            //deleteDcmObj(pDs, pEle);
            pDs->remove(pEle);
        }
    }

    return nRet;
}

OFCondition EditDicom::saveDcmFile(DcmFileFormat &fm, QString sFileName)
{
    DcmDataset * pDataset = fm.getDataset();
    E_TransferSyntax xfer = pDataset->getOriginalXfer();
    const char* transferSyntax = NULL;
    fm.getMetaInfo()->findAndGetString(DCM_TransferSyntaxUID, transferSyntax);
    string losslessTransUID = "1.2.840.10008.1.2.4.70";
    string lossTransUID = "1.2.840.10008.1.2.4.51";
    string losslessP14 = "1.2.840.10008.1.2.4.57";
    string lossyP1 = "1.2.840.10008.1.2.4.50";
    string lossyRLE = "1.2.840.10008.1.2.5";
    if (transferSyntax == losslessTransUID || transferSyntax == lossTransUID || transferSyntax == losslessP14 || transferSyntax == lossyP1)
    {
        DJDecoderRegistration::registerCodecs();
        pDataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);
    }
    else if (transferSyntax == lossyRLE)
    {
        DcmRLEDecoderRegistration::registerCodecs();
        pDataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);
    }
    else
    {
        pDataset->chooseRepresentation(xfer, NULL);
    }

    //save

    return fm.saveFile(sFileName.toStdString().c_str(), EXS_LittleEndianExplicit);
}

int EditDicom::editDicomInfo(DcmDataset* pDs, vector<TAGVALUE> &tagList, QString *sErrorMsg)
{
    int nRet = 0;

    //get all tags
    vector<DcmObject*> v;
    getAllTag(pDs, v);

    int n = tagList.size();
    //delete
    for(size_t i=0; i<n; i++)
    {
        TAGVALUE& t = tagList[i];
        if(t.bDeleteThisTag == true)
        {
            deleteTag(pDs, v, t, sErrorMsg);
        }
    }

    //edit
    for(int i=0; i<n; i++)
    {
        TAGVALUE& t = tagList[i];
        if(t.bDeleteThisTag == true)
            continue;

        int r = editTag(v, t, sErrorMsg);
        if(r)
            nRet = r;
    }

    return nRet;
}

/*
int EditDicom::editDicomInfo(DcmDataset* pDs, vector<TAGVALUE> &tagList, QString *sErrorMsg)
{
    int nRet = 0;

    size_t n = tagList.size();
    for(size_t i=0; i<n; i++)
    {
        TAGVALUE& t = tagList[i];

        unsigned long nTag = strtoul(t.sTag.toStdString().c_str(), nullptr, 16);
        DcmTagKey key(nTag>>16, (nTag<<16) >> 16);
        DcmElement* pEle = nullptr;
        if(pDs->findAndGetElement(key, pEle,m_bEditDcmSub).bad())
            continue;
        if(pEle == nullptr)
            continue;

        //if(pEle->putOFStringArray(t.sValue.toStdString().c_str()).bad())
        OFCondition of;
        of = pEle->putString(t.sValue.toStdString().c_str());

        if(of.bad())
        {
            nRet = -1;

            if(sErrorMsg)
            {
                OFString temp_str;
                DimseCondition::dump(temp_str, of);

                *sErrorMsg = temp_str.c_str();
            }
        }
    }

    return nRet;
}
*/
bool EditDicom::setTagList(QString sIniName, QString sGroupName)
{
    m_tagList.clear();

    if(QFile::exists(sIniName) == false)
    {
        {//patient name
            TAGVALUE t;
            t.sTag = "0x00100010";
            t.sValue = "";
            m_tagList.push_back(t);
        }

        {//patient ID
            TAGVALUE t;
            t.sTag = "0x00100020";
            t.sValue = "";
            m_tagList.push_back(t);
        }

        {//other Patient IDs
            TAGVALUE t;
            t.sTag = "0x00101000";
            t.sValue = "";
            m_tagList.push_back(t);
        }
        {//Other Patient Names
            TAGVALUE t;
            t.sTag = "0x00101001";
            t.sValue = "";
            m_tagList.push_back(t);
        }

        {//delete Icon Image Sequence
            TAGVALUE t;
            t.sTag = "0x00880200";
            t.sValue = "";
            t.bDeleteThisTag = true;
            m_tagList.push_back(t);
        }

        return true;
    }
    else
    {
        QSettings st(sIniName, QSettings::IniFormat);
        st.setIniCodec("UTF-8");
        st.beginGroup(sGroupName);

        QStringList keys = st.childKeys();
        int n = keys.size();
        for(int i=0;i<n;i++)
        {
            TAGVALUE t;
            t.sTag = keys[i];
            t.sValue = st.value(keys[i]).toString();

            t.sValue = PubFun::str_replace(t.sValue, "/", "\\");

            if(t.sValue.toLower() == "@del")
                t.bDeleteThisTag = true;

            m_tagList.push_back(t);

            qDebug() << t.sTag << " = " << t.sValue;
        }

        st.endGroup();

        return m_tagList.empty()==false;
    }


}

bool EditDicom::setSourceDir(QString sDir, QString sFilter, bool bSubDir)
{
    m_SrcDir = PubFun::str_replace(sDir, "\\", "/");
    return PubFun::getFolderFile(sDir,  sFilter,  bSubDir, m_vectFile);
}

bool EditDicom::editToNewDir(QString sNewDir)
{
    emit endEdit(false);

    sNewDir = PubFun::str_replace(sNewDir, "\\", "/");
    int nError = 0;
    int nWarn = 0;
    QString sError;

    size_t n = m_vectFile.size();
    for(size_t i=0; i<n; i++)
    {
        QString sOldFullName = m_vectFile[i];
        QString sNewFullName = getNewFullName(sOldFullName, m_SrcDir, sNewDir);

        bool bError = false;
        bool bWarn = false;
        OFCondition cond;

        do
        {
            //edit
            DcmFileFormat dcm;
            cond = dcm.loadFile((const char*)sOldFullName.toLocal8Bit() );
            //dcm.loadAllDataIntoMemory();
            if(cond.bad())
            {
                bError = true;

                if(sError.isEmpty() == true)
                {
                    OFString temp_str;
                    DimseCondition::dump(temp_str, cond);
                    sError = QString("Load file failed : %1 (%2)").arg(sOldFullName).arg(temp_str.c_str());
                }

                break;
            }

            DcmDataset* pDs = dcm.getDataset();
            if(pDs == nullptr)
            {
                bError = true;

                if(sError.isEmpty() == true)
                    sError = "Get dicom dataset failed!";

                break;
            }

            {
                QString str;
                int r = editDicomInfo(pDs, m_tagList, &str);
                if( r== -1)
                {
                    bError = true;

                    if(sError.isEmpty() == true)
                        sError = QString("Edit file failed: %1 (%2)").arg(sOldFullName).arg(str);

                    break;
                }
            }


            //E_TransferSyntax xfer = pDs->getOriginalXfer();
            //dcm.chooseRepresentation(xfer, nullptr);
            //cond = dcm.saveFile(sNewFullName.toStdString().c_str(), xfer);

            //always save it as little endia
            cond = saveDcmFile(dcm,sNewFullName);

            if(cond.bad() == true)
            {
                bError = true;
                if(sError.isEmpty() == true)
                {
                    OFString temp_str;
                    DimseCondition::dump(temp_str, cond);

                    sError = QString("Error in saving file: %1 (%2)").arg(sOldFullName).arg(temp_str.c_str());
                }

                break;
            }

        }while(false);

        if(bError)
            nError++;
        else if(bWarn)
            nWarn++;

        //g_pMainWnd->setProcessInfo(i+1, n, nError, nWarn);
        emit setProcessInfo(i+1, n, nError, nWarn);
    }

    //g_pMainWnd->setStatusBarMsg(sError);
    emit setStatusBarMsg(sError);
    emit endEdit(true);

    return true;
}

bool EditDicom::editToOldDir()
{
    emit endEdit(false);

    int nError = 0;
    int nWarn = 0;
    QString sError;

    size_t n = m_vectFile.size();
    for(size_t i=0; i<n; i++)
    {
        QString sFullName = m_vectFile[i];
        QString sTmpFullName = sFullName + ".edit.temp";

        bool bError = false;
        bool bWarn = false;

        //edit
        do
        {
            {
                OFCondition cond;

                DcmFileFormat dcm;
                cond = dcm.loadFile(sFullName.toStdString().c_str() );
                //dcm.loadAllDataIntoMemory();
                if(cond.bad())
                {
                    bError = true;
                    if(sError.isEmpty() == true)
                    {
                        OFString temp_str;
                        DimseCondition::dump(temp_str, cond);
                        sError = QString("Load file failed : %1 (%2)").arg(sFullName).arg(temp_str.c_str());
                    }

                    break;
                }

                DcmDataset* pDs = dcm.getDataset();
                if(pDs == nullptr)
                {
                    bError = true;

                    if(sError.isEmpty() == true)
                        sError = "Get dicom dataset failed!";

                    break;
                }

                QString str;
                int r = editDicomInfo(pDs, m_tagList, &str);
                if( r== -1)
                {
                    bError = true;

                    if(sError.isEmpty() == true)
                        sError = QString("Edit file failed: %1 (%2)").arg(sFullName).arg(str);

                    break;
                }

                //E_TransferSyntax xfer = pDs->getOriginalXfer();
                //dcm.chooseRepresentation(xfer, nullptr);
                //cond = dcm.saveFile(sTmpFullName.toStdString().c_str(), xfer);
                cond = saveDcmFile(dcm, sTmpFullName);

                if(cond.bad() == true)
                {
                    bError = true;
                    if(sError.isEmpty() == true)
                    {
                        OFString temp_str;
                        DimseCondition::dump(temp_str, cond);

                        sError = QString("Saving file failed: %1 (%2)").arg(sTmpFullName).arg(temp_str.c_str());
                    }

                    break;
                }
            }

            //delete old
            QString get = sFullName.toUtf8();
            if(QFile::remove(get) == true)
            {
                //rename temp name to old name
                bool x = QFile::rename(sTmpFullName, sFullName);
                if(x == false)
                {
                    bError = true;
                    if(sError.isEmpty() == true)
                        sError = QString("Rename file failed: (%1)->(%s) ").arg(sTmpFullName).arg(sFullName);

                    break;
                }
            }
            else
            {
                bError = true;
                if(sError.isEmpty() == true)
                    sError = QString("Delete old file failed : %1").arg(sFullName);

                break;
            }

        }while (false);

        if(bError)
            nError++;
        else if(bWarn)
            nWarn++;

        //g_pMainWnd->setProcessInfo(i+1, n, nError, nWarn);
        emit setProcessInfo(i+1, n, nError, nWarn);
    }

    //g_pMainWnd->setStatusBarMsg(sError);
    emit setStatusBarMsg(sError);
    emit endEdit(true);

    return true;
}
