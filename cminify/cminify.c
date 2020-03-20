#!/usr/local/bin/tcc  -run


/**   cminify.c   **/


// USAGE:   $  cminify  source_and_destination_filename
//    or    $  cminify  source_filename   destination_filename

#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <unistd.h>


int
main( int argc, char **argv ) {


  if( 1 == argc ) {
    fprintf( stderr, "\n\
\
 USAGE:  cminify.c  source_and_destination_filename\n\
    or   cminify.c  source_filename  destination_filename\n\
\
\n" );
    return -1;
  }

// Open, check length, malloc & read into buffer :
 FILE * f1 = fopen( argv[ 1 ], "rb" );
  if( !f1 ) { fprintf( stderr, "\n! ERROR: fopen() !\n" ); return -1; }
 int len = 0, charc;
  while( EOF != ( charc = fgetc( f1 ))) len++;  // fprintf( stderr, "\n len = %d .\n", len );
  rewind( f1 );
 char * text = calloc( 1, len + 4 );  // sense in '+4' :  prev -> spot -> next .
  if( !text ) { fprintf( stderr, "\n! ERROR: calloc() !\n" ); return -1; }
 char * spot = text;
  while( EOF != ( charc = fgetc( f1 ))) *spot++ = 0xFF & charc;
  fclose( f1 );
 char * end = spot;  // text + len;
 char * out = spot = text;

// cheol :  \r\n => \n
// erase/change:  \r \e \b \a \v \f \t
  while( spot < end ) {
    switch( *spot ) {
      case '\r':  break;
      case '\e':  ;   // ESC
      case '\b':  ;   // backspace
      case '\a':  ;   // bell
      case '\v':  ;   // vertical tab
      case '\f':  ;   // form feed
      case '\t':  *out = ' ';   ++out; break;
      default:    *out = *spot; ++out;
    }
    ++spot;
  }
  *out = '\0'; end = out;

// line-folding:
// fold "\\\n" line-breaks :
 char * prev = out = spot = text; ++spot;
  while( prev != end ) {
    if( '\\' == *prev && '\n' == *spot ) { prev += 2; spot += 2; }
    else { *out = *prev; ++out; ++prev; ++spot; }
  }
  *out = '\0'; end = out;

// Remove all ( c- and cpp-style ) comments :
 int inccomment   = 0,
     incppcomment = 0,
     inquote      = 0;
  prev = out = spot = text; ++spot;
  while( prev != end ) {
    if( !inquote && !inccomment && !incppcomment ) {
      if( '/' == *prev && '/' == *spot ) {
        *out = ' '; ++out;  // ???
        prev += 2; spot += 2;
        incppcomment = 1;
        continue;
      }
      if( '/' == *prev && '*' == *spot ) {
        prev += 2; spot += 2;
        inccomment = 1;
        continue;
      }
      // how     to get into inquote :   "
      // how NOT to get into inquote :   '\"'
      if( '\\' == *prev && '\"' == *spot ) {
        *out = *prev; ++out; *out = *spot; ++out;
        prev += 2; spot += 2;
        continue;
      }
      if( '\"' == *prev ) {
        *out = *prev; ++out;
        ++prev; ++spot;
        inquote = 1;
        continue;
      }
      // default :
      *out = *prev; ++out; ++prev; ++spot;
      continue;
    }
    if( inccomment ) {
      if( '*' == *prev && '/' == *spot ) {
        *out = ' '; ++out;  // ???
        prev += 2; spot += 2;
        inccomment = 0;
      }
      else { ++prev; ++spot; }
      continue;
    }
    if( incppcomment ) {
      if( '\n' == *prev ) { *out = *prev; ++out; ++prev; ++spot; incppcomment = 0; }
      else { ++prev; ++spot; }
      continue;
    }
    // else == if( inquote )
    if( '\\' == *prev ) {
      if( 'a'  == *spot || 'b'  == *spot || 'e'  == *spot || 'n'  == *spot ||
          'v'  == *spot || '\\' == *spot || '\'' == *spot || '\"' == *spot ||
          '\?' == *spot || 't'  == *spot || 'f'  == *spot || 'r'  == *spot ||
          'u'  == *spot || 'U'  == *spot || 'x'  == *spot || 'X'  == *spot  ) {
        *out = *prev; ++out; *out = *spot; ++out;
        prev += 2; spot += 2;
      }
      else { *out = *prev; ++out; ++prev; ++spot; }
      continue;
    }
    if( '\"' == *prev ) {
      *out = *prev; ++out;
      ++prev; ++spot;
      inquote = 0;
      continue;
    }
    *out = *prev; ++out; ++prev; ++spot;  // default action of inquote
  }  // while
  *out = '\0'; end = out;

// Leave only 1 space :
// (1st.)
  inquote = 0;
  prev = out = spot = text; ++spot;
  while( prev != end ) {
    if( !inquote ) {
      // how     to get into inquote :   "
      // how NOT to get into inquote :   '\"'
      if( '\\' == *prev && '\"' == *spot ) {
        *out = *prev; ++out; *out = *spot; ++out;
        prev += 2; spot += 2;
        continue;
      }
      if( '\"' == *prev ) {
        *out = *prev; ++out;
        ++prev; ++spot;
        inquote = 1;
        continue;
      }
      if( ' ' == *prev && ' ' == *spot ) { ++prev; ++spot; continue; }
      // default :
      *out = *prev; ++out; ++prev; ++spot;
      continue;
    }
    // else == if( inquote )
    if( '\\' == *prev ) {
      if( 'a'  == *spot || 'b'  == *spot || 'e'  == *spot || 'n'  == *spot ||
          'v'  == *spot || '\\' == *spot || '\'' == *spot || '\"' == *spot ||
          '\?' == *spot || 't'  == *spot || 'f'  == *spot || 'r'  == *spot ||
          'u'  == *spot || 'U'  == *spot || 'x'  == *spot || 'X'  == *spot  ) {
        *out = *prev; ++out; *out = *spot; ++out;
        prev += 2; spot += 2;
      }
      else { *out = *prev; ++out; ++prev; ++spot; }
      continue;
    }
    if( '\"' == *prev ) {
      *out = *prev; ++out;
      ++prev; ++spot;
      inquote = 0;
      continue;
    }
    *out = *prev; ++out; ++prev; ++spot;  // default action of inquote
  }  // while
  *out = '\0'; end = out;

// Interim transformation :  '\n #' => '\n#'
// This can NOT be in quote !  ( \n )
 char * next;
  prev = out = spot = text; ++spot;
  next = spot; ++next;
  // first line:
  if( ' ' == *prev && '#' == *spot ) { ++prev; ++spot; ++next; }
  // rest:
  while( prev != end ) {
    if( '\n' == *prev && ' ' == *spot && '#' == *next ) {
      *out = *prev; ++out;
      *out = *next; ++out;
      prev += 3; spot += 3; next += 3;
    }
    else { *out = *prev; ++out; ++prev; ++spot; ++next; }
  }  // while
  *out = '\0'; end = out;

// Remove unneeded \n :
// This can NOT be in quote !  ( \n )
// You must leave Macros and Directives !
 int isdirective = 0;
  prev = out = spot = text; ++spot;
  // first line:
  if( '#' == *prev ) { isdirective = 1; }
  // rest:
  while( prev != end ) {
    if( '\n' == *prev ) {
      if( '#' == *spot ) { *out = *prev; ++out; ++prev; ++spot; isdirective = 1; }
      else {
        if( isdirective) { *out = *prev; ++out; ++prev; ++spot; isdirective = 0; }
        else { *out = ' '; ++out; ++prev; ++spot; }
      }
    }
    else { *out = *prev; ++out; ++prev; ++spot; }
  }  // while
  *out = '\0'; end = out;

// ALL \n ARE MANDATORY !!! by now

// Leave only 1 space :
// (2nd.)
  inquote = 0;
  prev = out = spot = text; ++spot;
  while( prev != end ) {
    if( !inquote ) {
      // how     to get into inquote :   "
      // how NOT to get into inquote :   '\"'
      if( '\\' == *prev && '\"' == *spot ) {
        *out = *prev; ++out; *out = *spot; ++out;
        prev += 2; spot += 2;
        continue;
      }
      if( '\"' == *prev ) {
        *out = *prev; ++out;
        ++prev; ++spot;
        inquote = 1;
        continue;
      }
      if( ' ' == *prev && ' ' == *spot ) { ++prev; ++spot; continue; }
      // default :
      *out = *prev; ++out; ++prev; ++spot;
      continue;
    }
    // else == if( inquote )
    if( '\\' == *prev ) {
      if( 'a'  == *spot || 'b'  == *spot || 'e'  == *spot || 'n'  == *spot ||
          'v'  == *spot || '\\' == *spot || '\'' == *spot || '\"' == *spot ||
          '\?' == *spot || 't'  == *spot || 'f'  == *spot || 'r'  == *spot ||
          'u'  == *spot || 'U'  == *spot || 'x'  == *spot || 'X'  == *spot  ) {
        *out = *prev; ++out; *out = *spot; ++out;
        prev += 2; spot += 2;
      }
      else { *out = *prev; ++out; ++prev; ++spot; }
      continue;
    }
    if( '\"' == *prev ) {
      *out = *prev; ++out;
      ++prev; ++spot;
      inquote = 0;
      continue;
    }
    *out = *prev; ++out; ++prev; ++spot;  // default action of inquote
  }  // while
  *out = '\0'; end = out;

// Misc. space-cutting if !inquote :
// (1st.)
// '{ ' => '{'   []{}();,?!
// NO!:  ';;'  => ';'    ! for( ;; )
// NO!:  '; ;' => ';'    ! for( ; ; )
// OK.:  ' ;'  => ';'
// OK.:  ' ,'  => ','
// OK.:  '= '  => '='    // only this side, NOT ' =' !
  inquote = 0;
  prev = out = spot = text; ++spot;
  while( prev != end ) {
    if( !inquote ) {
      if( '\n' == *prev && '#' == *spot ) {  // isdirective
        while( prev != end ) {
          *out = *prev; ++out; ++prev; ++spot;
          if( '\n' == *prev ) break;
        }
        continue;
      }
      // how     to get into inquote :   "
      // how NOT to get into inquote :   '\"'
      if( '\\' == *prev && '\"' == *spot ) {
        *out = *prev; ++out; *out = *spot; ++out;
        prev += 2; spot += 2;
        continue;
      }
      if( '\"' == *prev ) {
        *out = *prev; ++out;
        ++prev; ++spot;
        inquote = 1;
        continue;
      }
      if( ' ' == *prev ) {
        if( '[' == *spot || ']' == *spot || '(' == *spot || ')'  == *spot ||
            '{' == *spot || '}' == *spot || ';' == *spot || ','  == *spot ||
                                            '!' == *spot || '\?' == *spot  ) {
          ++prev; ++spot; continue;
        }
      }
      if( '[' == *prev || ']' == *prev || '(' == *prev || ')'  == *prev ||
          '{' == *prev || '}' == *prev || ';' == *prev || ','  == *prev ||
                                          '!' == *prev || '\?' == *prev  ) {
        if( ' ' == *spot ) {
          *out = *prev; ++out;
          prev += 2; spot += 2;
          continue;
        }
      }
      if( '=' == *prev && ' ' == *spot ) {
        *out = *prev; ++out;
        prev += 2; spot += 2;
        continue;
      }
      // default :
      *out = *prev; ++out; ++prev; ++spot;
      continue;
    }
    // else == if( inquote )
    if( '\\' == *prev ) {
      if( 'a'  == *spot || 'b'  == *spot || 'e'  == *spot || 'n'  == *spot ||
          'v'  == *spot || '\\' == *spot || '\'' == *spot || '\"' == *spot ||
          '\?' == *spot || 't'  == *spot || 'f'  == *spot || 'r'  == *spot ||
          'u'  == *spot || 'U'  == *spot || 'x'  == *spot || 'X'  == *spot  ) {
        *out = *prev; ++out; *out = *spot; ++out;
        prev += 2; spot += 2;
      }
      else { *out = *prev; ++out; ++prev; ++spot; }
      continue;
    }
    if( '\"' == *prev ) {
      *out = *prev; ++out;
      ++prev; ++spot;
      inquote = 0;
      continue;
    }
    *out = *prev; ++out; ++prev; ++spot;  // default action of inquote
  }  // while
  *out = '\0'; end = out;

// Leave only 1 space :
// (3rd.)
  inquote = 0;
  prev = out = spot = text; ++spot;
  while( prev != end ) {
    if( !inquote ) {
      // how     to get into inquote :   "
      // how NOT to get into inquote :   '\"'
      if( '\\' == *prev && '\"' == *spot ) {
        *out = *prev; ++out; *out = *spot; ++out;
        prev += 2; spot += 2;
        continue;
      }
      if( '\"' == *prev ) {
        *out = *prev; ++out;
        ++prev; ++spot;
        inquote = 1;
        continue;
      }
      if( ' ' == *prev && ' ' == *spot ) { ++prev; ++spot; continue; }
      // default :
      *out = *prev; ++out; ++prev; ++spot;
      continue;
    }
    // else == if( inquote )
    if( '\\' == *prev ) {
      if( 'a'  == *spot || 'b'  == *spot || 'e'  == *spot || 'n'  == *spot ||
          'v'  == *spot || '\\' == *spot || '\'' == *spot || '\"' == *spot ||
          '\?' == *spot || 't'  == *spot || 'f'  == *spot || 'r'  == *spot ||
          'u'  == *spot || 'U'  == *spot || 'x'  == *spot || 'X'  == *spot  ) {
        *out = *prev; ++out; *out = *spot; ++out;
        prev += 2; spot += 2;
      }
      else { *out = *prev; ++out; ++prev; ++spot; }
      continue;
    }
    if( '\"' == *prev ) {
      *out = *prev; ++out;
      ++prev; ++spot;
      inquote = 0;
      continue;
    }
    *out = *prev; ++out; ++prev; ++spot;  // default action of inquote
  }  // while
  *out = '\0'; end = out;

// Transformation :  '\n ' => '\n' ; ' \n' => '\n'
// This can NOT be in quote !  ( \n )
  prev = out = spot = text; ++spot;
  // first line:
  if( ' ' == *prev ) { ++prev; ++spot; }
  // rest:
  while( prev != end ) {
    if( ' '  == *prev && '\n' == *spot ) { ++prev; ++spot; continue; }
    if( '\n' == *prev && ' '  == *spot ) {
      *out = *prev; ++out;
      prev += 2; spot += 2;
      continue;
    }
    *out = *prev; ++out; ++prev; ++spot;
  }  // while
  *out = '\0'; end = out;

// Misc. space-cutting if !inquote :
// (2nd.)
// '{ ' => '{'   []{}();,?!
// NO!:  ';;'  => ';'    ! for( ;; )
// NO!:  '; ;' => ';'    ! for( ; ; )
// OK.:  ' ;'  => ';'
// OK.:  ' ,'  => ','
// OK.:  '= '  => '='    // only this side, NOT ' =' !
  inquote = 0;
  prev = out = spot = text; ++spot;
  while( prev != end ) {
    if( !inquote ) {
      if( '\n' == *prev && '#' == *spot ) {  // isdirective
        while( prev != end ) {
          *out = *prev; ++out; ++prev; ++spot;
          if( '\n' == *prev ) break;
        }
        continue;
      }
      // how     to get into inquote :   "
      // how NOT to get into inquote :   '\"'
      if( '\\' == *prev && '\"' == *spot ) {
        *out = *prev; ++out; *out = *spot; ++out;
        prev += 2; spot += 2;
        continue;
      }
      if( '\"' == *prev ) {
        *out = *prev; ++out;
        ++prev; ++spot;
        inquote = 1;
        continue;
      }
      if( ' ' == *prev ) {
        if( '[' == *spot || ']' == *spot || '(' == *spot || ')'  == *spot ||
            '{' == *spot || '}' == *spot || ';' == *spot || ','  == *spot ||
                                            '!' == *spot || '\?' == *spot  ) {
          ++prev; ++spot; continue;
        }
      }
      if( '[' == *prev || ']' == *prev || '(' == *prev || ')'  == *prev ||
          '{' == *prev || '}' == *prev || ';' == *prev || ','  == *prev ||
                                          '!' == *prev || '\?' == *prev  ) {
        if( ' ' == *spot ) {
          *out = *prev; ++out;
          prev += 2; spot += 2;
          continue;
        }
      }
      if( '=' == *prev && ' ' == *spot ) {
        *out = *prev; ++out;
        prev += 2; spot += 2;
        continue;
      }
      // default :
      *out = *prev; ++out; ++prev; ++spot;
      continue;
    }
    // else == if( inquote )
    if( '\\' == *prev ) {
      if( 'a'  == *spot || 'b'  == *spot || 'e'  == *spot || 'n'  == *spot ||
          'v'  == *spot || '\\' == *spot || '\'' == *spot || '\"' == *spot ||
          '\?' == *spot || 't'  == *spot || 'f'  == *spot || 'r'  == *spot ||
          'u'  == *spot || 'U'  == *spot || 'x'  == *spot || 'X'  == *spot  ) {
        *out = *prev; ++out; *out = *spot; ++out;
        prev += 2; spot += 2;
      }
      else { *out = *prev; ++out; ++prev; ++spot; }
      continue;
    }
    if( '\"' == *prev ) {
      *out = *prev; ++out;
      ++prev; ++spot;
      inquote = 0;
      continue;
    }
    *out = *prev; ++out; ++prev; ++spot;  // default action of inquote
  }  // while
  *out = '\0'; end = out;

// Leave only 1 space :
// (4th.)
  inquote = 0;
  prev = out = spot = text; ++spot;
  while( prev != end ) {
    if( !inquote ) {
      // how     to get into inquote :   "
      // how NOT to get into inquote :   '\"'
      if( '\\' == *prev && '\"' == *spot ) {
        *out = *prev; ++out; *out = *spot; ++out;
        prev += 2; spot += 2;
        continue;
      }
      if( '\"' == *prev ) {
        *out = *prev; ++out;
        ++prev; ++spot;
        inquote = 1;
        continue;
      }
      if( ' ' == *prev && ' ' == *spot ) { ++prev; ++spot; continue; }
      // default :
      *out = *prev; ++out; ++prev; ++spot;
      continue;
    }
    // else == if( inquote )
    if( '\\' == *prev ) {
      if( 'a'  == *spot || 'b'  == *spot || 'e'  == *spot || 'n'  == *spot ||
          'v'  == *spot || '\\' == *spot || '\'' == *spot || '\"' == *spot ||
          '\?' == *spot || 't'  == *spot || 'f'  == *spot || 'r'  == *spot ||
          'u'  == *spot || 'U'  == *spot || 'x'  == *spot || 'X'  == *spot  ) {
        *out = *prev; ++out; *out = *spot; ++out;
        prev += 2; spot += 2;
      }
      else { *out = *prev; ++out; ++prev; ++spot; }
      continue;
    }
    if( '\"' == *prev ) {
      *out = *prev; ++out;
      ++prev; ++spot;
      inquote = 0;
      continue;
    }
    *out = *prev; ++out; ++prev; ++spot;  // default action of inquote
  }  // while
  *out = '\0'; end = out;

// Join quotes :
// "..."  "...." => "......"
  inquote = 0;
  prev = out = spot = text; ++spot;
  // next = spot; ++next;
  while( prev != end ) {
    if( !inquote ) {
      // how     to get into inquote :   "
      // how NOT to get into inquote :   '\"'
      if( '\\' == *prev && '\"' == *spot ) {
        *out = *prev; ++out; *out = *spot; ++out;
        prev += 2; spot += 2;
        continue;
      }
      if( '\"' == *prev ) {
        *out = *prev; ++out;
        ++prev; ++spot;
        inquote = 1;
        continue;
      }
      // default :
      *out = *prev; ++out; ++prev; ++spot;
      continue;
    }
    // else == if( inquote )
    if( '\\' == *prev ) {
      if( 'a'  == *spot || 'b'  == *spot || 'e'  == *spot || 'n'  == *spot ||
          'v'  == *spot || '\\' == *spot || '\'' == *spot || '\"' == *spot ||
          '\?' == *spot || 't'  == *spot || 'f'  == *spot || 'r'  == *spot ||
          'u'  == *spot || 'U'  == *spot || 'x'  == *spot || 'X'  == *spot  ) {
        *out = *prev; ++out; *out = *spot; ++out;
        prev += 2; spot += 2;
      }
      else { *out = *prev; ++out; ++prev; ++spot; }
      continue;
    }
    if( '\"' == *prev ) {
      next = spot; ++next;
      if( ' ' == *spot && '\"' == *next ) {
        prev += 3; spot += 3;
      }
      else {
        *out = *prev; ++out;
        ++prev; ++spot;
        inquote = 0;
      }
      continue;
    }
    *out = *prev; ++out; ++prev; ++spot;  // default action of inquote
  }  // while
  *out = '\0'; end = out;

// Remove unneeded \n in \n\n :
// This can NOT be in quote !  ( \n )
  prev = out = spot = text; ++spot;
  // for beginning of file:
  while( '\n' == *prev ) { ++prev; ++spot; }
  // rest:
  while( prev != end ) {
    if( '\n' == *prev && '\n' == *spot ) { ++prev; ++spot; }
    else { *out = *prev; ++out; ++prev; ++spot; }
  }  // while
  *out = '\0'; end = out;

// END OF MINIFYING .

// Write buffer back into file :
  f1 = fopen( 2 < argc ? argv[ 2 ] : argv[ 1 ], "wb" );
  if( !f1 ) { fprintf( stderr, "\n! ERROR: fopen() !\n" ); return -1; }
  spot = text;
  while( spot < end ) { fputc( *spot, f1 ); ++spot; }
  fclose( f1 );
  free( text );

 return 0;
}

