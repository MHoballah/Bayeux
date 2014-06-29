//! \file datatools/i_clear.h
#ifndef DATATOOLS_I_CLEAR_H
#define DATATOOLS_I_CLEAR_H

namespace datatools {

//! \brief A pure abstract class (interface) for inherited clearable classes.
class i_clear {
 public:
  /**
   * A pure virtual member.
   * Clear (reset) the object's internals (typically empty a container).
   */
  virtual void clear() = 0;
};

} // end of namespace datatools

#endif // DATATOOLS_I_CLEAR_H
