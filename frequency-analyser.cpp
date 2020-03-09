#include <iostream>
#include <fstream>

double cstring_to_int ( char* s , bool check_only )
{
  double num;

  for ( int i = 0 ; s[i] != '\0' ; i++)
  {
    num = int(s[i] - '0');
    if( num < 0 || num > 9 )
    {
      throw 0;
    }
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
      if ( file.peek() == std::ifstream::traits_type::eof() )
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

  std::fstream file;
  file.open( argv[1] , std::ios::in );

  int c;
  int frequencies[256]; // file read in 8 bit blocks
  file.read((char*)&c , sizeof(char));
  std::cout << c << "\n";
}
