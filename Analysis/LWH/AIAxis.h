// -*- C++ -*-
//
// AIAxis.h is a part of ThePEG - Toolkit for HEP Event Generation
// Copyright (C) 1999-2011 Leif Lonnblad
//
// ThePEG is licenced under version 2 of the GPL, see COPYING for details.
// Please respect the MCnet academic guidelines, see GUIDELINES for details.
//
#ifndef LWH_AIAxis_H
#define LWH_AIAxis_H

#ifndef LWH_USING_AIDA

/** @cond DONT_DOCUMENT_STRIPPED_DOWN_AIDA_INTERFACES */

namespace AIDA {

class IAxis {

public:

  virtual ~IAxis() {}

  virtual bool isFixedBinning() const = 0;
  virtual double lowerEdge() const = 0;
  virtual double upperEdge() const = 0;
  virtual int bins() const = 0;
  virtual double binLowerEdge(int index) const = 0;
  virtual double binUpperEdge(int index) const = 0;
  virtual double binWidth(int) const = 0;
  virtual int coordToIndex(double coord) const = 0;

  enum { UNDERFLOW_BIN = -2, OVERFLOW_BIN = -1 };

};

}

/** @endcond */

#else
#include "AIDA/IAxis.h"
#endif

#endif /* LWH_AIAxis_H */
