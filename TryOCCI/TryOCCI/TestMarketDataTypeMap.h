#ifndef TESTMARKETDATATYPEMAP_ORACLE
# define TESTMARKETDATATYPEMAP_ORACLE

#ifndef OCCI_ORACLE
# include <occi.h>
#endif

#ifndef TESTMARKETDATATYPE_ORACLE
# include "TestMarketDataType.h"
#endif

void TestMarketDataTypeMap(oracle::occi::Environment* envOCCI_);

#endif
