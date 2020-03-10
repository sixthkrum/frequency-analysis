#include <iostream>
#include <fstream>
#include <math.h>
#include <limits.h>
#include <cstring>
#include <map>
#include <iterator>

unsigned int cstring_to_int ( char* s , bool check_only )
{
  unsigned int temp = 0;
  unsigned int num = 0;
  unsigned int len = strlen(s);

  for ( int i = 0 ; s[i] != '\0' ; i++)
  {
    temp = int(s[i] - '0');
    if( temp < 0 || temp > 9 )
    {
      throw 0;
    }

    num = temp * pow ( 10 , len - i - 1 ) + num;
  }

  if ( check_only == 1 )
  {
    return 1;
  }

  return num;
}

inline unsigned int carry_over_bitmask ( int leftover_bits )
{
  unsigned int temp = 0;
  for ( int i = 0 ; i < leftover_bits ; i ++)
  {
    temp = pow ( 2 , i ) + temp;
  }

  return temp;
}

int main(int argc , char* argv[])
{
  switch (argc)
  {
    case 3:
    {
      std::fstream file;
      file.open( argv[1] , std::ios::in );
      if ( file.peek() == EOF )
      {
        std::cout << "file is empty or does not exist\n";
        return -1;
      }

      try
      {
        cstring_to_int ( argv[2] , 1 );
      }

      catch (...)
      {
        std::cout << "second argument is not an integer\n";

        return -1;
      }

    } break;

    default:
    {
      std::cout << "first argument: filename\n" << "second argument: no. of bits per symbol\n";

      return -1;
    } break;

  }

  //std::cout << "starting\n";

  unsigned int symbol_bits;
  symbol_bits = cstring_to_int ( argv[2] , 0 );

  std::map < unsigned int , unsigned int > symbol_map;

  int leftover_bits = symbol_bits % 8;
  unsigned int full_symbol_bytes = symbol_bits / 8;
  unsigned int partial_symbol = 0;
  unsigned int symbol = 0;
  unsigned int carry_over_bits = 0;
  bool early_eof = 0;
  bool mod8_flag = [ leftover_bits ]() -> bool {
    if( leftover_bits == 0)
    {
      return 1;
    }

    return 0;

  } ();

  std::fstream file;
  file.open( argv[1] , std::ios::in | std::ios::binary );

  if ( mod8_flag && symbol_bits >= 8)
  {
    while ( file.peek() != EOF)
    {
      symbol = 0;
      for ( int i = 0 ; i < full_symbol_bytes ; i ++ )
      {
        partial_symbol = 0;
        file.read( (char*)&partial_symbol , 1 );
        symbol = symbol +  ( partial_symbol << ( ( full_symbol_bytes - i - 1 ) * 8 ) );

        if( file.peek() == EOF )
        {
          symbol = symbol >> ( ( full_symbol_bytes - i - 1 ) * 8 );
          break;
        }
      }

      try
      {
        symbol_map [ symbol ] ++;
      }

      catch ( std::out_of_range )
      {
        symbol_map.insert ( std::pair < unsigned int , unsigned int > ( symbol , 1 ) );
      }
    }
  }

  else if ( !mod8_flag && symbol_bits > 8 )
  {
    while ( file.peek() != EOF)
    {
      symbol = 0;
      for ( int i = 0 ; i < full_symbol_bytes ; i ++ )
      {
        partial_symbol = 0;
        file.read( (char*)&partial_symbol , 1 );
        symbol = symbol +  ( partial_symbol << ( ( full_symbol_bytes - i - 1 ) * 8 ) );

        if( file.peek() == EOF )
        {
          symbol = symbol >> ( ( full_symbol_bytes - i - 1 ) * 8 );
          break;
        }
      }

      symbol = ( carry_over_bits << ( 8 * full_symbol_bytes ) ) + symbol;

      if ( leftover_bits > 0 && file.peek() != EOF )
      {
        partial_symbol = 0;
        file.read( (char*)&partial_symbol , 1 );
        symbol = ( symbol << leftover_bits ) + ( partial_symbol >> ( 8 - leftover_bits ) );
        carry_over_bits = partial_symbol & carry_over_bitmask (  8 - leftover_bits );
        leftover_bits = symbol_bits - ( 8 - leftover_bits ) - ( 8 * full_symbol_bytes );
      }

      else if ( leftover_bits <= 0 )
      {
        symbol =  ( symbol >> ( - leftover_bits ) );
        carry_over_bits = partial_symbol & carry_over_bitmask (  - leftover_bits );
        leftover_bits = symbol_bits + leftover_bits - ( 8 * full_symbol_bytes );
      }

      else
      {
        early_eof = 1;
      }

      try
      {
        symbol_map [ symbol ] ++;
      }

      catch ( std::out_of_range )
      {
        symbol_map.insert ( std::pair < unsigned int , unsigned int > ( symbol , 1 ) );
      }

    }

    if ( ! early_eof )
    {
      symbol = carry_over_bits;

      try
      {
        symbol_map [ symbol ] ++;
      }

      catch ( std::out_of_range )
      {
        symbol_map.insert ( std::pair < unsigned int , unsigned int > ( symbol , 1 ) );
      }

    }

  }

  else
  {
    leftover_bits = 8;
    file.read( (char*)&partial_symbol , 1 );
    while ( ! early_eof )
    {
      if ( int(leftover_bits) - int(symbol_bits) >= 0 )
      {
        symbol = partial_symbol >> ( leftover_bits - symbol_bits );
        partial_symbol = partial_symbol & carry_over_bitmask ( leftover_bits - symbol_bits );
        leftover_bits = leftover_bits - symbol_bits;
        if ( leftover_bits == 0)
        {
          leftover_bits = 8;
          partial_symbol = 0;
          if ( file.peek() != EOF )
          {
            file.read( (char*)&partial_symbol , 1 );
          }

          else
          {
            early_eof = 1;
          }

        }

      }

      else
      {
        carry_over_bits = partial_symbol;
        if( file.peek() != EOF)
        {
          symbol = ( carry_over_bits << ( - ( leftover_bits - symbol_bits ) ) );
          partial_symbol = 0;
          file.read( (char*)&partial_symbol , 1 );
          leftover_bits = 8 + ( leftover_bits - symbol_bits );
          symbol = symbol + ( partial_symbol >> ( leftover_bits ) );
          partial_symbol = partial_symbol & carry_over_bitmask ( leftover_bits );
        }

        else
        {
          symbol = carry_over_bits;
          early_eof = 1;
        }

      }

      try
      {
        symbol_map [ symbol ] ++;
      }

      catch ( std::out_of_range )
      {
        symbol_map.insert ( std::pair < unsigned int , unsigned int > ( symbol , 1 ) );
      }

    }

  }

  std::map < unsigned int , unsigned int > :: iterator p;

  for ( p = symbol_map.begin() ; p != symbol_map.end() ; p ++ )
  {
    std::cout << p -> first << '\t' << p -> second << '\n';
  }

  file.close();

  return 1;
}
