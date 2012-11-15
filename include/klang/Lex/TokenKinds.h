#ifndef KLANG_TOKENKINDS_H
#define KLANG_TOKENKINDS_H

namespace klang {
  namespace tok {

    enum TokenKind {
      tok_eof = -1,

      // commands
      tok_def = -2,
      tok_extern = -3,

      // primary
      tok_identifier = -4,
      tok_number = -5
    };

  }//namespace tok
}//namespace klang

#endif //#ifndef KLANG_TOKENKINDS_H

