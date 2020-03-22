#include <iostream>
#include <fstream>
#include <math.h>
#include <limits.h>
#include <cstring>
#include <unordered_map>
#include <iterator>
#include <regex>
#include <vector>
#include <array>

unsigned int cstring_to_int ( char* s , bool check = 0 )
{
  unsigned int temp = 0;
  unsigned int num = 0;
  unsigned int len = strlen ( s );

  if ( check )
  {
    for ( int i = 0 ; s[i] != '\0' ; i++)
    {
      temp = int(s[i] - '0');

      if( temp < 0 || temp > 9 )
      {
        throw 0;
      }

    }
  }

  for ( int i = 0 ; s[i] != '\0' ; i ++ )
  {
    temp = int(s[i] - '0');
    num = temp * pow ( 10 , len - i - 1 ) + num;
  }

  return num;

}

inline unsigned int carry_over_bitmask ( int leftover_bits )
{
  unsigned int temp = 0;
  for ( int i = 0 ; i < leftover_bits ; i ++ )
  {
    temp = pow ( 2 , i ) + temp;
  }

  return temp;
}

int main( int argc , char* argv[] )
{
  switch ( argc )
  {
    case 3:
    {
      std::fstream file;
      file.open( argv [1] , std::ios::in );

      if ( file.peek() == EOF )
      {
        std::cout << "file is empty or does not exist\n";

        return -1;
      }

      try
      {
        cstring_to_int ( argv [2] , 1 );
      }

      catch (...)
      {
        std::cout << "second argument is not a positive non-zero integer\n";

        return -1;
      }

    } break;

    case 4:
    {
      std::fstream file;
      file.open( argv [1] , std::ios::in );

      if ( file.peek() == EOF )
      {
        std::cout << "file is empty or does not exist\n";

        return -1;
      }

      try
      {
        cstring_to_int ( argv [2] , 1 );
      }

      catch (...)
      {
        std::cout << "second argument is not a positive non-zero integer\n";

        return -1;
      }

      std::regex reg ( "(?:\\d+\\s\\d+\\s\\d+\\s\\d+\\s)*" );

      if ( ! std::regex_match ( argv [3] , reg ) )
      {
        std::cout << "third argument is not well formed\nformat: ''starting decimal value to map from' 'ending decimal value to map from' 'starting decimal value to map to' 'ending decimal value to map to' '\nexample '1 1 1 1 '\nseperate multiple ranges by whitespace\nranges should be the same\nto map an ascending range to a descending range enter the last index first in the mapping to part\n";

        return -1;
      }

    } break;

    default:
    {
      std::cout << "first argument: filename\n" << "second argument: no. of bits per symbol\n" << "third argument: (leave empty if none) list of symbols to map to other symbols and be treated the same as them\n" << "format: ''starting decimal value to map from' 'ending decimal value to map from' 'starting decimal value to map to' 'ending decimal value to map to' ' seperate multiple ranges by whitespace\nranges should be the same\nto map an ascending range to a descending range enter the last index first in the mapping to part\n";

      return -1;
    } break;

  }

  //std::cout << "starting\n";

  unsigned int symbol_bits;
  symbol_bits = cstring_to_int ( argv [2] , 0 );

  if ( symbol_bits == 0 )
  {
    std::cout << "second argument is not a positive non-zero integer\n";

    return -1;

  }

  std::unordered_map < unsigned int , unsigned int > symbol_map;

  int leftover_bits = symbol_bits % 8;
  unsigned int full_symbol_bytes = symbol_bits / 8;
  unsigned int partial_symbol = 0;
  unsigned int symbol = 0;
  unsigned int carry_over_bits = 0;
  bool early_eof = 0;
  bool mod8_flag = ! bool ( leftover_bits );

  std::fstream file;
  file.open( argv[1] , std::ios::in | std::ios::binary );

  if ( argc == 4 )
  {
    std::unordered_map < unsigned int , unsigned int > cross_map;

    std::vector < std::array < int , 4 > > map_range;
    std::array < int , 4 > range_temp;

    unsigned int arg_len = strlen ( argv [3] );
    unsigned int temp;

    char *num_temp;

    while ( arg_len > 0 )
    {
      for ( int i = 0 ; i < 4 ; i ++ )
      {
        temp = strcspn ( argv [3] , " " );
        num_temp = new char [ temp + 1 ];
        strncpy ( num_temp , argv [3] , temp );
        num_temp [ temp ] = '\0';

        range_temp [i] = cstring_to_int ( num_temp );

        arg_len -= temp + 1;
        argv [3] += temp + 1;
        delete [] num_temp;
      }

      map_range.push_back ( range_temp );

    }

    int range_num = 1;

    while ( !map_range.empty() )
    {
      range_temp = map_range.back();

      if ( range_temp [1] - range_temp [0] == range_temp [3] - range_temp [2] )
      {
        unsigned int range = range_temp [1] - range_temp [0];
        unsigned int f_temp = range_temp [0];
        unsigned int b_temp = range_temp [2];

        for ( int i = 0 ; i <= range ; i ++ )
        {
          cross_map.insert ( std::pair < unsigned int , unsigned int > ( i + f_temp , i + b_temp ) );
        }

      }

      else if ( range_temp [1] - range_temp [0] == - range_temp [3] + range_temp [2] && range_temp [1] > range_temp [0] )
      {
        unsigned int range = range_temp [1] - range_temp [0];
        unsigned int f_temp = range_temp [0];
        unsigned int b_temp = range_temp [2];

        for ( int i = 0 ; i <= range ; i ++ )
        {
          cross_map.insert ( std::pair < unsigned int , unsigned int > ( i + f_temp , b_temp - i ) );
        }

      }

      else
      {
        std::cout << "recheck range no. " << range_num << " it is not in the right format\n";
        return 1;
      }

      map_range.pop_back();
      range_num ++;

    }

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
          cross_map.at ( symbol );
          symbol_map [ cross_map [ symbol ] ] ++;
        }

        catch ( std::out_of_range )
        {
          symbol_map [ symbol ] ++;
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
          cross_map.at ( symbol );
          symbol_map [ cross_map [ symbol ] ] ++;
        }

        catch ( std::out_of_range )
        {
          symbol_map [ symbol ] ++;
        }

      }

      if ( ! early_eof )
      {
        symbol = carry_over_bits;

        symbol_map [ symbol ] ++;
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
          cross_map.at ( symbol );
          symbol_map [ cross_map [ symbol ] ] ++;
        }

        catch ( std::out_of_range )
        {
          symbol_map [ symbol ] ++;
        }

      }

    }

  }

  else
  {
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

        symbol_map [ symbol ] ++;

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

        symbol_map [ symbol ] ++;

      }

      if ( ! early_eof )
      {
        symbol = carry_over_bits;

        symbol_map [ symbol ] ++;

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

        symbol_map [ symbol ] ++;

      }

    }
  }

  std::unordered_map < unsigned int , unsigned int > :: iterator i;

  for ( i = symbol_map.begin() ; i != symbol_map.end() ; i ++ )
  {
    std::cout << char ( i -> first ) << '\t' << i -> second << '\n';
  }

  file.close();

  return 1;
}
