//-------------------------------------------------
//
/**  \class L1MuDTERS
 *
 *   Extrapolation Result Selector (Quality Sorter Unit):
 *                
 *   selects the 2 best (highest target quality) 
 *   out of the 12 (6) extrapolations
 *   performed per start track segment
 *
 *
 *
 *   N. Neumeister            CERN EP
 */
//
//--------------------------------------------------
#ifndef L1MUDT_ERS_H
#define L1MUDT_ERS_H

//---------------
// C++ Headers --
//---------------

#include <utility>

//----------------------
// Base Class Headers --
//----------------------

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------

class L1MuDTTrackSegPhi;
class L1MuDTSEU;

//              ---------------------
//              -- Class Interface --
//              ---------------------

class L1MuDTERS {
public:
  /// constructor
  L1MuDTERS(const L1MuDTSEU&);

  /// destructor
  ~L1MuDTERS();

  /// run L1MuDTERS
  void run();

  /// reset ERS
  void reset();

  /// return extrapolation quality
  inline unsigned int quality(int id) const { return m_quality[id]; }

  /// return extrapolation address; (address = 15 indicates negative ext. result)
  inline unsigned short int address(int id) const { return m_address[id]; }

  /// return pointer to start and target track segment
  std::pair<const L1MuDTTrackSegPhi*, const L1MuDTTrackSegPhi*> ts(int id) const;

private:
  const L1MuDTSEU& m_seu;

  unsigned short int m_quality[2];  //@@ 1 bit
  unsigned short int m_address[2];  //@@ 4 bits

  const L1MuDTTrackSegPhi* m_start[2];
  const L1MuDTTrackSegPhi* m_target[2];
};

#endif
