#ifndef PREDEFINE_H
#define PREDEFINE_H

//*******************************************dcmtk includes******************************************//
#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#define INCLUDE_CSTDLIB
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
#define INCLUDE_CCTYPE
#define INCLUDE_CSIGNAL
#include "dcmtk/ofstd/ofstdinc.h"

//#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/ofstd/ofdatime.h"
#include "dcmtk/dcmnet/dicom.h"         /* for DICOM_APPLICATION_ACCEPTOR */
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmnet/dcmtrans.h"      /* for dcmSocketSend/ReceiveTimeout */
#include "dcmtk/dcmnet/dcasccfg.h"      /* for class DcmAssociationConfiguration */
#include "dcmtk/dcmnet/dcasccff.h"      /* for class DcmAssociationConfigurationFile */
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmdata/dcuid.h"        /* for dcmtk version name */
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcostrmz.h"     /* for dcmZlibCompressionLevel */

//for decode, encode
#include <dcmtk/dcmjpeg/djdecode.h>
#include "dcmtk/dcmjpeg/djencode.h"
#include "dcmtk/dcmjpeg/djrplol.h"
#include "dcmtk/dcmdata/dcrledrg.h" //for DcmRLEDecoderRegistration


//*******************************************std includes******************************************//
#include <QString>
#include <QtDebug>
#include <QDir>


#include <vector>
using namespace std;

//*******************************************other******************************************/

typedef struct _TAGVALUE
{
    QString sTag;
    QString sValue;
    bool bDeleteThisTag;
    _TAGVALUE()
        :bDeleteThisTag(false)
    {}

}TAGVALUE;

#endif // PREDEFINE_H
