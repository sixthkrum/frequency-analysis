#include <iostream>
#include <fstream>
#include <math.h>
#include <limits.h>
#include <cstring>

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

  unsigned int size_of_symbol_map;
  size_of_symbol_map = pow ( 2 , symbol_bits );
  std::cout << "size of symbol map " << size_of_symbol_map << '\n';

  if ( size_of_symbol_map == UINT_MAX )
  {
    std::cout << "only first " << UINT_MAX + 1 << " combinations considered, too many symbols\n";
  }

  unsigned int* symbol_map = new unsigned int [ size_of_symbol_map ];
  for( int i = 0 ; i < size_of_symbol_map ; i++ )
  {
    symbol_map[i] = 0;
  }

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
      file.read( (char*)&partial_symbol , full_symbol_bytes );
      symbol = partial_symbol << leftover_bits;
      file.read( (char*)&partial_symbol , 1 );
      carry_over_bits = partial_symbol & carry_over_bitmask;
      symbol = symbol + ( partial_symbol >> ( 8 - leftover_bits ) );

      //std::cout << symbol << '\n';
      symbol_map [ symbol ] ++;

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
        file.read( (char*)&partial_symbol , full_symbol_bytes );
        symbol = carry_over_bits + partial_symbol;
      }

      //std::cout << symbol << '\n';
      symbol_map [ symbol ] ++;
    }
  }

  else
  {
    //need to do this
  }

  for ( unsigned int i = 0 ; i < size_of_symbol_map ; i ++ )
  {
    if ( symbol_map[i] > 0 )
    {
      std::cout << symbol_map[i] << '\t' << i << '\t' << char(i) << '\n';
    }
  }

  return 1;
}
