
#ifndef TS_BIT_H
#define TS_BIT_H


#include "types.h"


namespace ts
{
  const int MaxBitFlag = 32;

  class Bit
  {
  public:
    static BitVector flagToBit( int flag ) 
    { 
      return (1 << flag); 
    }

    static bool has( BitVector bitvector, int flag )
    {
      return ( bitvector != 0 && ( bitvector & flagToBit( flag ) ) );
    }

    static void set( BitVector *bitvector, int flag )
    {
      (*bitvector) |= flagToBit( flag );
    }

    static void remove( BitVector *bitvector, int flag )
    {
      (*bitvector) &= ~( flagToBit( flag ) );
    }
  
  private:
    Bit() {}
    ~Bit() {}

  };

} // namespace ts

#endif // TS_BIT_H
