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

  std::cout << "starting\n";

  unsigned int symbol_bits;
  symbol_bits = cstring_to_int ( argv[2] , 0 );

  std::map < unsigned int , unsigned int > symbol_map;

  unsigned int leftover_bits = symbol_bits % 8;
  unsigned int full_symbol_bytes = symbol_bits / 8;
  unsigned int partial_symbol = 0;
  unsigned int symbol = 0;
  unsigned int carry_over_bits = 0;
  unsigned int carry_over_bitmask = [ leftover_bits ]() -> unsigned int {
    unsigned int temp = 0;
    for ( int i = 0 ; i < 8 - leftover_bits ; i ++)
    {
      temp = pow ( 2 , i ) + temp;
    }

    return temp;
  }();

  std::fstream file;
  file.open( argv[1] , std::ios::in | std::ios::binary );

  if( symbol_bits >= 8 )
  {
    while ( file.peek() != EOF)
    {
      partial_symbol = 0;
      file.read( (char*)&partial_symbol , full_symbol_bytes );
      symbol = partial_symbol << leftover_bits;

      partial_symbol = 0;
      file.read( (char*)&partial_symbol , 1 );
      carry_over_bits = partial_symbol & carry_over_bitmask;
      symbol = symbol + ( partial_symbol >> ( 8 - leftover_bits ) );

      try
      {
        symbol_map [ symbol ] ++;
      }

      catch ( std::out_of_range )
      {
        symbol_map.insert ( std::pair < unsigned int , unsigned int > ( symbol , 1 ) );
      }

      if( file.eof() )
      {
        break;
      }

      if ( symbol_bits == 8 )
      {
        symbol = carry_over_bits;
      }

      else
      {
        partial_symbol = 0;
        file.read( (char*)&partial_symbol , full_symbol_bytes );
        symbol = carry_over_bits + partial_symbol;
      }

      try
      {
        symbol_map [ symbol ] ++;
      }

      catch ( std::out_of_range )
      {
        symbol_map.insert( std::pair < unsigned int , unsigned int > ( symbol , 1 ) );
      }

    }
  }

  else
  {
    //need to do this
  }

  std::map < unsigned int , unsigned int > :: iterator p;

  for ( p = symbol_map.begin() ; p != symbol_map.end() ; p ++ )
  {
    std::cout << p -> first << '\t' << p -> second << '\n';
  }

  return 1;
}
