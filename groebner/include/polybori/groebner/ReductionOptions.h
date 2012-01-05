// -*- c++ -*-
//*****************************************************************************
/** @file ReductionOptions.h 
 *
 * @author Alexander Dreyer
 * @date 2012-01-05
 *
 * This file includes the definition of the class @c ReductionOptions.
 *
 * @par Copyright:
 *   (c) 2012 by The PolyBoRi Team
 *
**/
//*****************************************************************************

#ifndef polybori_groebner_ReductionOptions_h_
#define polybori_groebner_ReductionOptions_h_

// include basic definitions
#include "groebner_defs.h"

BEGIN_NAMESPACE_PBORIGB

/** @class ReductionOptions
 * @brief This class defines option values for the ReductionStrategy.
 *
 **/

class ReductionOptions {
public:
  ReductionOptions():
    reducibleUntil(-1),
    optLL(false), optBrutalReductions(true),
    optRedTail(true), optRedTailDegGrowth(true) {}

  bool optBrutalReductions;
  bool optLL;
  bool optRedTailDegGrowth;
  bool optRedTail;
  idx_type reducibleUntil;
};

END_NAMESPACE_PBORIGB

#endif /* polybori_groebner_ReductionOptions_h_ */