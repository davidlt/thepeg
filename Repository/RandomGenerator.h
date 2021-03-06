// -*- C++ -*-
//
// RandomGenerator.h is a part of ThePEG - Toolkit for HEP Event Generation
// Copyright (C) 1999-2011 Leif Lonnblad
//
// ThePEG is licenced under version 2 of the GPL, see COPYING for details.
// Please respect the MCnet academic guidelines, see GUIDELINES for details.
//
#ifndef ThePEG_RandomGenerator_H
#define ThePEG_RandomGenerator_H
// This is the declaration of the RandomGenerator class.

#include "ThePEG/Config/ThePEG.h"
#include "ThePEG/Interface/Interfaced.h"
#include "gsl/gsl_rng.h"

namespace ThePEG {

/**
 * RandomGenerator is an interface to the CLHEP::RandomEngine
 * classes. To avoid excessive virtual function calls, the
 * RandomGenerator caches random numbers generated by the engine which
 * are then retrieved by the non-virtual inlined rnd() method.
 *
 * Sub-classes of RandomGenerator should be used to
 * implement a particular random engine.
 *
 * RandomGenerator only provides a flat distribution between 0 and
 * 1. Any other distribution can be achieved using the CLHEP random
 * classes using the engine returned from the randomGenerator()
 * method.
 *
 * @see \ref RandomGeneratorInterfaces "The interfaces"
 * defined for RandomGenerator.
 * @see UseRandom
 */
class RandomGenerator: public Interfaced {

public:

  /** A vector of doubles. */
  typedef vector<double> RndVector;

  /** The size_type of RndVector. */
  typedef RndVector::size_type size_type;

public:

  /** @name Standard constructors and destructors. */
  //@{
  /**
   * Default constructor.
   */
  RandomGenerator();

  /**
   * Copy-constructor.
   */
  RandomGenerator(const RandomGenerator &);

  /**
   * Destructor.
   */
  virtual ~RandomGenerator();
  //@}

  /**
   * Reset the underlying random engine with the given \a seed. If the
   * \a seed is set to -1 a standard seed will be used.
   */
  virtual void setSeed(long seed) = 0;

  /** @name Functions to return random numbers. */
  //@{
  /**
   * Return a (possibly cached) flat random number in the interval
   * \f$]0,1[\f$.
   */
  double rnd() {
    if ( nextNumber == theNumbers.end() ) fill();
    return *nextNumber++;
  }

  /**
   * Return a flat random number in the interval
   * \f$]0,b[\f$.
   */
  template <typename Unit> Unit rnd(Unit b) { return b*rnd(); }

  /**
   * Return a flat random number in the interval
   * \f$]a,b[\f$.
   */
  template <typename Unit>
  Unit rnd(Unit a, Unit b) { return a + rnd(b - a); }

  /**
   * Return \a n flat random number in the interval
   * \f$]0,1[\f$.
   */
  RndVector rndvec(int n) {
    RndVector ret(n);
    for ( int i = 0; i < n; ++i ) ret[i] = rnd();
    return ret;
  }

  /**
   * Return a (possibly cached) flat random number in the interval
   * \f$]0,1[\f$.
   */
  double operator()() { return rnd(); }

  /**
   * Return a (possibly cached) flat integer random number in the
   * interval \f$[0,N[\f$.
   */
  long operator()(long N) { return long(rnd() * N); }

  /**
   * Return a true with probability \a p. Uses rnd(). Also uses
   * push_back(double).
   */
  bool rndbool(double p = 0.5);

  /**
   * Return a true with probability \a p1/(\a p1+\a p2). Uses
   * rnd(). Also uses push_back(double).
   */
  bool rndbool(double p1, double p2) { return rndbool(p1/(p1 + p2)); }

  /**
   * Return -1, 0, or 1 with relative probabilities \a p1, \a p2, \a
   * p3. Uses rnd(). Also uses push_back(double).
   */
  int rndsign(double p1, double p2, double p3);

  /**
   * Return an integer \f$i\f$ with probability p\f$i\f$/(\a p0+\a
   * p1). Uses rnd().
   */
  int rnd2(double p0, double p1) {
    return rndbool(p0, p1)? 0: 1;
  }

  /**
   * Return an integer \f$i\f$ with probability p\f$i\f$/(\a p0+\a
   * p1+\a p2). Uses rnd().
   */
  int rnd3(double p0, double p1, double p2) {
    return 1 + rndsign(p0, p1, p2);
  }

  /**
   * Return an integer/ \f$i\f$ with probability p\f$i\f$(\a p0+\a
   * p1+\a p2+\a p3). Uses rnd().
   */
  int rnd4(double p0, double p1, double p2, double p3);

  /**
   * Return a number between zero and infinity, distributed according
   * to \f$e^-x\f$.
   */
  double rndExp() {
    return -log(rnd());
  }

  /**
   * Return a number between zero and infinity, distributed according
   * to \f$e^-{x/\mu}\f$ where \f$\mu\f$ is the \a mean value.
   */
  template <typename Unit>
  Unit rndExp(Unit mean) { return mean*rndExp(); }

  /**
   * Return a number distributed according to a Gaussian distribution
   * with zero mean and unit variance.
   */
  double rndGauss() { 
    if ( gaussSaved ) {
      gaussSaved = false;
      return savedGauss;
    }
    double r = sqrt(-2.0*log(rnd()));
    double phi = rnd()*2.0*Constants::pi;
    savedGauss = r*cos(phi);
    gaussSaved = true;
    return r*sin(phi);
  }

  /**
   * Return a number distributed according to a Gaussian distribution
   * with a given standard deviation, \a sigma, and a given \a mean.
   */
  template <typename Unit>
  Unit rndGauss(Unit sigma, Unit mean = Unit()) {
    return mean + sigma*rndGauss();
  }

  /**
   * Return a positive number distributed according to a
   * non-relativistic Breit-Wigner with a given width, \a gamma, and a
   * given \a mean.
   */
  template <typename Unit>
  Unit rndBW(Unit mean, Unit gamma) {
    if ( gamma <= Unit() ) return mean;
    return mean + 0.5*gamma*tan(rnd(atan(-2.0*mean/gamma), Constants::pi/2));
  }

  /**
   * Return a positive number distributed according to a
   * non-relativistic Breit-Wigner with a given width, \a gamma, and a
   * given \a mean. The distribution is cut-off so that the number is
   * between \a mean - \a cut and \a mean + \a cut
   */
  template <typename Unit>
  Unit rndBW(Unit mean, Unit gamma, Unit cut) {
    if ( gamma <= Unit() || cut <= Unit() ) return mean;
    return mean + 0.5*gamma*tan(rnd(atan(-2.0*min(mean,cut)/gamma),
				    atan(2.0*cut/gamma)));
  }

  /**
   * Return a positive number distributed according to a relativistic
   * Breit-Wigner with a given width, \a gamma, and a given \a mean.
   */
  template <typename Unit>
  Unit rndRelBW(Unit mean, Unit gamma) {
    if ( gamma <= Unit() ) return mean;
    return sqrt(sqr(mean) + mean*gamma*tan(rnd(atan(-mean/gamma),
					       Constants::pi/2)));
  }

  /**
   * Return a positive number distributed according to a relativistic
   * Breit-Wigner with a given width, \a gamma, and a given \a
   * mean. The distribution is cut-off so that the number is between
   * \a mean - \a cut and \a mean + \a cut
   */
  template <typename Unit>
  Unit rndRelBW(Unit mean, Unit gamma, Unit cut) {
    if ( gamma <= Unit() || cut <= Unit() ) return mean;
    double minarg = cut > mean? -mean/gamma:
      (sqr(mean - cut) - sqr(mean))/(gamma*mean);
    double maxarg = (sqr(mean + cut) - sqr(mean))/(mean*gamma);
    return sqrt(sqr(mean) + mean*gamma*tan(rnd(atan(minarg), atan(maxarg))));
  }

  /**
   * Return a non-negative number generated according to a Poissonian
   * distribution with a given \a mean. Warning: the method
   * implemented is very slow for large mean and large return
   * values. For this reason the maximum return value is given by \a
   * nmax.
   */
  long rndPoisson(double mean);
  //@}

  /** @name Access the cached random numbers from the underlying engine. */
  //@{
  /**
   * Give back a partly unused random number. This is typically used
   * when generating integral random numbers. In eg. rndbool(double p)
   * a random number <code>r</code> is drawn and if it is less than
   * <code>p</code> true is returned, but <code>r</code> is still a
   * good random number in the interval <code>]0,p[</code>. Hence
   * <code>r/p</code> is still a good random number in the interval
   * <code>]0,1[</code> and this is then pushed back into the cache
   * and is used by the next call to rnd(). Note that the resulting
   * random number is of lesser quality, and successive calls to
   * push_back() should be avoided. To ensure a highest quality random
   * number random number in the next call to rnd(), pop_back() should
   * be used.
   */
  void push_back(double r) {  
    if ( r > 0.0 && r < 1.0 && nextNumber != theNumbers.begin() )
      *--nextNumber = r;
  }

  /**
   * Discard the next random number in the cache.
   */
  void pop_back() {
    if ( nextNumber != theNumbers.end() ) ++nextNumber;
  }

  /**
   * Discard all random numbers in the cache. Typically used after the
   * internal random engine has been reinitialized for some reason.
   */
  void flush() { nextNumber = theNumbers.end(); }

  /**
   * Generate n random numbers between 0 and 1 and put them in the
   * output iterator.
   */
  template <typename OutputIterator>
  void rnd(OutputIterator o, size_type n) {
    while ( n-- ) *o++ = rnd();
  }
  //@}

protected:

  /**
   * Initializes this random generator. This should be done first of
   * all before the initialization of any other object associated with
   * an event generator.
   */
  virtual void doinit();

public:


  /** @name Functions used by the persistent I/O system. */
  //@{
  /**
   * Function used to write out object persistently.
   * @param os the persistent output stream written to.
   */
  void persistentOutput(PersistentOStream & os) const;

  /**
   * Function used to read in object persistently.
   * @param is the persistent input stream read from.
   * @param version the version number of the object when written.
   */
  void persistentInput(PersistentIStream & is, int version);
  //@}

  /**
   * Standard Init function used to initialize the interface.
   */
  static void Init();

  /**
   * Return a gsl_rng interface to this random generator.
   */
  gsl_rng * getGslInterface() { return gsl; }

protected:

  /**
   * Utility function for the interface.
   */
  void setSize(size_type newSize);

  /**
   * Fill the cache with random numbers.
   */
  virtual void fill() = 0;

  /**
   * A vector of cached numbers.
   */
  RndVector theNumbers;

  /**
   * Iterator pointing to the next number to be extracted
   */
  RndVector::iterator nextNumber;

  /**
   * The size of the cache.
   */
  size_type theSize;

  /**
   * The seed to initialize the random generator with.
   */
  long theSeed;

  /**
   * A saved Gaussian random number.
   */
  mutable double savedGauss;

  /**
   * Indicate the precense of a saved Gaussian random number.
   */
  mutable bool gaussSaved;

  /**
   * A pinter to a gsl_rng interface to this generator.
   */
  gsl_rng * gsl;

private:

  /**
   * Describe a concrete class with persistent data. Note that the
   * class should in principle be abstract.
   */
  static ClassDescription<RandomGenerator> initRandomGenerator;

  /**
   *  Private and non-existent assignment operator.
   */
  RandomGenerator & operator=(const RandomGenerator &);

};

/** @cond TRAITSPECIALIZATIONS */

/** This template specialization informs ThePEG about the base classes
 *  of RandomGenerator. */
template <>
struct BaseClassTrait<RandomGenerator,1>: public ClassTraitsType {
  /** Typedef of the first base class of RandomGenerator. */
  typedef Interfaced NthBase;
};

/** This template specialization informs ThePEG about the name of the
 *  RandomGenerator class. */
template <>
struct ClassTraits<RandomGenerator>:
    public ClassTraitsBase<RandomGenerator> {
  /** Return a platform-independent class name */
  static string className() { return "ThePEG::RandomGenerator";
  }
  /** This class should in principle be abstract, therefore the
      create() method will throw a std::logic_error if called. */
  static TPtr create() {
    throw std::logic_error("Tried to instantiate abstract class"
			   "'Pythis7::RandomGenerator'");
  }
};

/** @endcond */

}

#endif /* ThePEG_RandomGenerator_H */
