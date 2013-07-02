// -*- mode: c++ ; -*-
/* sensitive_hit_collection.h
 * Author (s) :   Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-05-26
 * Last modified: 2013-07-02
 *
 * License:
 *
 * Description:
 *
 *   GEANT4 sensitive hit collection
 *
 * History:
 *
 */

#ifndef MCTOOLS_G4_SENSITIVE_HIT_COLLECTION_H_
#define MCTOOLS_G4_SENSITIVE_HIT_COLLECTION_H_ 1

#include <vector>

#include <G4VHitsCollection.hh>

namespace mctools {

  namespace g4 {

    class sensitive_hit;

    class sensitive_hit_collection : public G4VHitsCollection
    {
    public:

      typedef std::vector<sensitive_hit*> hit_collection_type;

      sensitive_hit_collection ();

      sensitive_hit_collection (G4String a_detector_name, G4String a_collection_name);

      virtual ~sensitive_hit_collection ();

      G4int operator==(const sensitive_hit_collection & right) const;

      const hit_collection_type & get_hits () const;

      hit_collection_type & grab_hits ();

      // G4VHitsCollection Interface :

      virtual G4VHit * GetHit(size_t) const;

      virtual size_t GetSize() const;

    protected:

      hit_collection_type _hits;

    };

  } // end of namespace g4

} // end of namespace mctools

#endif // MCTOOLS_G4_SENSITIVE_HIT_COLLECTION_H_

// end of sensitive_hit_collection
