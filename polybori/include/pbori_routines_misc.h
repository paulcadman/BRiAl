// -*- c++ -*-
//*****************************************************************************
/** @file pbori_routines_misc.h
 *
 * @author Alexander Dreyer
 * @date 2006-08-23
 *
 * This file includes files, which defines miscellaneous function templates.
 *
 * @par Copyright:
 *   (c) 2006 by
 *   Dep. of Mathematics, Kaiserslautern University of Technology and @n
 *   Fraunhofer Institute for Industrial Mathematics (ITWM)
 *   D-67663 Kaiserslautern, Germany
 *
 * @internal 
 * @version \$Id$
 *
 * @par History:
 * @verbatim
 * $Log$
 * Revision 1.15  2006/11/30 19:42:44  dreyer
 * CHANGE: lead(bound) now uses cached and recursive variant
 *
 * Revision 1.14  2006/11/29 13:40:03  dreyer
 * CHANGE: leadexp() made recursive and cached
 *
 * Revision 1.13  2006/11/28 09:32:58  dreyer
 * CHANGE: lead() (for dlex, dp_asc) is recursive and cached now
 *
 * Revision 1.12  2006/11/27 16:25:14  dreyer
 * CHANGE: CDegreeCache, now inherited from standard cache; dlex-lead cached
 *
 * Revision 1.11  2006/11/22 15:46:22  dreyer
 * ADD: CacheManager replacing CCacheManagement for external use
 * CHANGE: CacheManager used, where necessary
 * CHANGE: multiplesOf, uses cached recursion
 *
 * Revision 1.10  2006/11/21 09:52:05  dreyer
 * CHANGE: some simple functions in BoolePolynomial inlined
 * ADD: caching of ternary operations
 * ADD: commandline switch PBORI_FAST_MULTIPLICATION (dense multiplication)
 *
 * Revision 1.9  2006/11/20 16:37:03  dreyer
 * FIX: broken assertion
 *
 * Revision 1.8  2006/11/20 16:18:07  dreyer
 * ADD: BooleSet new node-constructor, also in dd_multiply_recursively
 *
 * Revision 1.7  2006/11/20 14:56:46  dreyer
 * CHANGE CCacheType names, operator*=, CDDInterface node Constructor
 *
 * Revision 1.6  2006/10/24 14:21:56  dreyer
 * ADD: variable_name functional
 *
 * Revision 1.5  2006/10/23 16:05:55  dreyer
 * ADD: BoolePolyRing::set/get(Ring)VariableName()
 *
 * Revision 1.4  2006/10/04 15:46:49  dreyer
 * ADD: divisorsOf(exp_type), orderedExpBegin/End;
 * CHANGE: Polynomials printing respects ordering
 *
 * Revision 1.3  2006/09/21 16:09:59  dreyer
 * ADD: caching mechanism for BoolePolynomial::deg()
 *
 * Revision 1.2  2006/09/14 10:57:26  dreyer
 * ADD: usedVariablesExp()
 *
 * Revision 1.1  2006/08/24 14:52:15  dreyer
 * + Initial Version
 *
 * Revision 1.1  2006/08/23 14:24:54  dreyer
 * ADD: BooleSet::usedVariables and infrastructure
 *
 * @endverbatim
**/
//*****************************************************************************

// include basic definitions
#include "pbori_defs.h"

// temprarily
#include "CIdxVariable.h"

// temprarily
#include "CacheManager.h"

#include "CDDOperations.h"

BEGIN_NAMESPACE_PBORI

template<class Iterator>
typename Iterator::value_type
index_vector_hash(Iterator start, Iterator finish){

  typedef typename Iterator::value_type value_type;

  value_type vars = 0;
  value_type sum = 0;
 
  while (start != finish){
    vars++;
    sum += ((*start)+1) * ((*start)+1);
    ++start;
  }
  return sum * vars;
}

/// Function templates for determining the degree of a decision diagram
/// with the help of cache (e. g. CDegreeCache)
template <class DegreeCacher, class NaviType>
typename NaviType::size_type
dd_cached_degree(const DegreeCacher& cache, NaviType navi) {

  typedef typename NaviType::size_type size_type;

  if (navi.isConstant()) // No need for caching of constant nodes' degrees
    return 0;
 
  // Look whether result was cached before
  typename DegreeCacher::node_type result = cache.find(navi);
  if (result.isValid())
    return *result;

  // Get degree of then branch (contains at least one valid path)...
  size_type deg = dd_cached_degree(cache, navi.thenBranch()) + 1;
 
  // ... combine with degree of else branch
  deg = std::max(deg,  dd_cached_degree(cache, navi.elseBranch()) );

  // Write result to cache
  cache.insert(navi, deg);
 
  return deg;
}

/// Function templates for determining the degree of a decision diagram
/// with the help of cache (e. g. CDegreeCache)
/// Variant *with* given upper bound
/// Assumming that the bound is valid!
template <class DegreeCacher, class NaviType, class SizeType>
typename NaviType::size_type
dd_cached_degree(const DegreeCacher& cache, NaviType navi, SizeType bound) {

  typedef typename NaviType::size_type size_type;

  // No need for caching of constant nodes' degrees
  if (navi.isConstant() || bound == 0)
    return 0;
 
  // Look whether result was cached before
  typename DegreeCacher::node_type result = cache.find(navi);
  if (result.isValid())
    return *result;

  // Get degree of then branch (contains at least one valid path)...
  size_type deg = dd_cached_degree(cache, navi.thenBranch(), bound - 1) + 1;

  // ... combine with degree of else branch
  if (bound > deg)              // if deg <= bound, we are already finished
    deg = std::max(deg,  dd_cached_degree(cache, navi.elseBranch(), bound) );

  // Write result to cache
  cache.insert(navi, deg);
 
  return deg;
}

template <class Iterator, class NameGenerator, 
          class Separator, class EmptySetType, 
          class OStreamType>
void 
dd_print_term(Iterator start, Iterator finish, const NameGenerator& get_name,
              const Separator& sep, const EmptySetType& emptyset, 
              OStreamType& os){

  if (start != finish){
    os << get_name(*start);
    ++start;
  }
  else
    os << emptyset();

  while (start != finish){
    os << sep() << get_name(*start);
    ++start;
  }
}

template <class TermType, class NameGenerator,
          class Separator, class EmptySetType,
          class OStreamType>
void 
dd_print_term(const TermType& term, const NameGenerator& get_name,
              const Separator& sep, const EmptySetType& emptyset, 
              OStreamType& os){
  dd_print_term(term.begin(), term.end(), get_name, sep, emptyset, os);
}


template <class Iterator, class NameGenerator,
          class Separator, class InnerSeparator, 
          class EmptySetType, class OStreamType>
void 
dd_print_terms(Iterator start, Iterator finish, const NameGenerator& get_name,
               const Separator& sep, const InnerSeparator& innersep,
               const EmptySetType& emptyset, OStreamType& os) {

  if (start != finish){
    dd_print_term(*start, get_name, innersep, emptyset, os);
    ++start;
  }

  while (start != finish){
    os << sep(); 
    dd_print_term(*start, get_name, innersep, emptyset, os);
    ++start;
  }

}


template <class PolynomialType>
PolynomialType 
dd_multiply_recursively(PolynomialType a, PolynomialType b){

  assert( a.diagram().manager().getManager() == 
          b.diagram().manager().getManager() );

  if (a.isZero() || b.isZero()) return 0;
  if (a.isOne()) return b;
  if (b.isOne()) return a;
  if (a == b) return a;

  // Get cache management types
  typedef CommutativeCacheManager<CCacheTypes::multiply_recursive>
    cache_mgr_type;

  // Extract subtypes
  typedef typename PolynomialType::dd_type dd_type;
  typedef typename PolynomialType::set_type set_type;
  typedef typename PolynomialType::idx_type idx_type;
  typedef typename PolynomialType::navigator navigator;

  // Instantiate cache manager
  cache_mgr_type cache_mgr;

  // Get navigators (nodes) from polynomial
  navigator firstNavi(a.navigation()), secondNavi(b.navigation());

  // Look up, whether operation was already used
  navigator cached = cache_mgr.find(firstNavi, secondNavi);

  // Instantiate result
  PolynomialType result;

  if (cached.isValid()) {       // Cache lookup sucessful
    result = (set_type) cached;
  }
  else {                        // Cache lookup not sucessful
    // Get top variable's index
    idx_type index = std::min(*firstNavi, *secondNavi);

    // Get then- and else-branches wrt. current indexed variable
    PolynomialType as0 = a.diagram().subset0(index);
    PolynomialType as1 = a.diagram().subset1(index);
    PolynomialType bs0 = b.diagram().subset0(index);
    PolynomialType bs1 = b.diagram().subset1(index);

#ifdef PBORI_FAST_MULTIPLICATION
    if (*firstNavi == *secondNavi) {
      PolynomialType res00 = dd_multiply_recursively(as0, bs0);
      PolynomialType res11 = dd_multiply_recursively(as1 + as0, 
                                                     bs0 + bs1) - res00;
      result = dd_type(index, res11, res00);
    } else
#endif
      {
        if (as0 == as1){
          PolynomialType zero(0);
          bs1=zero.diagram();
        } 
        else {
          if (bs0 == bs1){
            PolynomialType zero(0);
            as1 = zero.diagram();
          }
        }
        // Do recursion
        result = dd_type(index,  
                         dd_multiply_recursively(as0, bs1) 
                         + dd_multiply_recursively(as1, bs1)
                         + dd_multiply_recursively(as1, bs0),
                         dd_multiply_recursively(as0, bs0) );

      }
    // Insert in cache
    cache_mgr.insert(firstNavi, secondNavi, result.navigation());
  }


  return result;
}


template <class CacheType, class DegCacheMgr, class NaviType, 
          class TermType, class BinComp>
TermType
dd_recursive_degree_lead(const CacheType& cache_mgr, const DegCacheMgr& deg_mgr,
                  NaviType navi, TermType init, BinComp comp) {

  if (navi.isConstant())
    return navi;//TermType(navi.terminalValue());

  NaviType cached = cache_mgr.find(navi);

  if (cached.isValid())
    return cached;

  NaviType thenNavi(navi.thenBranch()), elseNavi(navi.elseBranch());

  typedef typename NaviType::size_type size_type;
  size_type deg = (dd_cached_degree(deg_mgr, thenNavi) + 1);
  size_type deg_else = dd_cached_degree(deg_mgr, elseNavi);


  if ( comp(deg, deg_else) ) { // < for dlex, <= for dp_asc
    init = dd_recursive_degree_lead(cache_mgr, deg_mgr, elseNavi, 
                                    init, comp);
    deg = deg_else;
  }
  else {
    init = dd_recursive_degree_lead(cache_mgr, deg_mgr, thenNavi, 
                                    init, comp).change(*navi);
  }

  NaviType resultNavi(init.navigation());
  cache_mgr.insert(navi, resultNavi);
  deg_mgr.insert(resultNavi, deg);

  return init;
}

// with degree bound
template <class CacheType, class DegCacheMgr, class NaviType, 
          class TermType, class SizeType, class BinComp>
TermType
dd_recursive_degree_lead(const CacheType& cache_mgr, const DegCacheMgr& deg_mgr,
                         NaviType navi, TermType init, SizeType bound,
                         BinComp comp) {

  if ((bound == 0) || navi.isConstant())
    return navi;

  NaviType cached = cache_mgr.find(navi);

  if (cached.isValid())
    return cached;

  NaviType thenNavi(navi.thenBranch());

  typedef typename NaviType::size_type size_type;
  size_type deg = (dd_cached_degree(deg_mgr, thenNavi, bound - 1) + 1);

  if ( !comp(deg, bound) )
    init = dd_recursive_degree_lead(cache_mgr, deg_mgr, thenNavi, 
                                    init, bound - 1, comp).change(*navi);
  else {
    NaviType elseNavi(navi.elseBranch());
    size_type deg_else= dd_cached_degree(deg_mgr, elseNavi, bound);
    
    
    if ( comp(deg, deg_else) ) { // < for dlex, <= for dp_asc
      init = dd_recursive_degree_lead(cache_mgr, deg_mgr, elseNavi, 
                                      init, comp);
      deg = deg_else; 
    }
    else {
      init = dd_recursive_degree_lead(cache_mgr, deg_mgr, thenNavi, 
                                      init, comp).change(*navi);
    }
  }

  NaviType resultNavi(init.navigation());
  cache_mgr.insert(navi, resultNavi);
  deg_mgr.insert(resultNavi, deg);

  return init;
}



template <class CacheType, class DegCacheMgr, class NaviType, 
          class TermType, class BinComp>
TermType&
dd_recursive_degree_leadexp(const CacheType& cache_mgr, 
                            const DegCacheMgr& deg_mgr,
                            NaviType navi, TermType& result, BinComp comp) {

  if (navi.isConstant())
    return result;
 
  NaviType cached = cache_mgr.find(navi);

  if (cached.isValid())
    return result = result.multiplyFirst(cached);

  NaviType thenNavi(navi.thenBranch()), elseNavi(navi.elseBranch());

  typedef typename NaviType::size_type size_type;
  size_type deg_then = (dd_cached_degree(deg_mgr, thenNavi) + 1);
  size_type deg_else = dd_cached_degree(deg_mgr, elseNavi);


  if ( comp(deg_then, deg_else) ) { // < for dlex, <= for dp_asc
    navi = elseNavi;
  }
  else {
    result.push_back(*navi);
    navi = thenNavi;
  }

  return dd_recursive_degree_leadexp(cache_mgr, deg_mgr, navi, result, comp);
}

template <class CacheType, class DegCacheMgr, class NaviType, 
          class TermType, class SizeType, class BinComp>
TermType&
dd_recursive_degree_leadexp(const CacheType& cache_mgr, 
                            const DegCacheMgr& deg_mgr,
                            NaviType navi, TermType& result, SizeType bound,
                            BinComp comp) {

  if (navi.isConstant())
    return result;
 
  NaviType cached = cache_mgr.find(navi);

  if (cached.isValid())
    return result = result.multiplyFirst(cached);

  NaviType thenNavi(navi.thenBranch()), elseNavi(navi.elseBranch());

  typedef SizeType size_type;
  size_type deg_then = (dd_cached_degree(deg_mgr, thenNavi, bound - 1) + 1);

  if ( !comp(deg_then, bound) ) {
    result.push_back(*navi);
    navi = thenNavi;
    --bound;
  }
  else {

    size_type deg_else = dd_cached_degree(deg_mgr, elseNavi, bound);
    if ( comp(deg_then, deg_else) ) { // < for dlex, <= for dp_asc
      navi = elseNavi;
    }
    else {
      result.push_back(*navi);
      navi = thenNavi;
      --bound;
    }
  }

  return 
    dd_recursive_degree_leadexp(cache_mgr, deg_mgr, navi, result, bound, comp);
}


END_NAMESPACE_PBORI
