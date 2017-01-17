/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Feb 10, 2015
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2015 Richard Bateman and the FireBreath Dev Team
\**********************************************************/

#include "APITypes.h"
#include "Deferred.h"

using namespace FB;




namespace FB {
    template class Promise<FB::variant>;
    template class Promise<FB::VariantList>;
    template class Deferred<FB::variant>;
    template class Deferred<FB::VariantList>;
}
