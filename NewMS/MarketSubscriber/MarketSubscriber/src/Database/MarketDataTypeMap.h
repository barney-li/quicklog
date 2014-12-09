#ifndef MARKETDATATYPEMAP_ORACLE
# define MARKETDATATYPEMAP_ORACLE

#ifndef OCCI_ORACLE
# include <occi.h>
#endif

#ifndef MARKETDATATYPE_ORACLE
# include "MarketDataType.h"
#endif

void MarketDataTypeMap(oracle::occi::Environment* envOCCI_);

#endif
