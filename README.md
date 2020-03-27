ONGOING ONLY OUTPUTS FREQUENCIES FOR ALL BIT SIZES, now checks if frequency table is valid

analyses the frequency of symbols of given bit size ( < (UINT_MAX + 1) corresponding combinations) in a file and compares them to the frequency of alphabets in the english language

to build: g++ frequency-analyser.cpp

first argument: filename

second argument: no. of bits per symbol

third argument: (leave empty if none) list of symbols to map to other symbols and be treated the same as them

format: ''starting decimal value to map from' 'ending decimal value to map from' 'starting decimal value to map to' 'ending decimal value to map to' ' seperate multiple ranges by whitespace

ranges should be the same ex: '1 4 7 10 ' is a valid range

to map an ascending range to a descending range enter the last index first in the mapping to part ex: '1 12 25 14 '

to map a range to a single index enter the same numbers in the mapping to part ex: '10 65 97 97 '

frequency table format:

'symbol index in decimal' 'frequency of symbol in percent' with entries in seperate lines

frequency total should not be more than 100 and there should not be any duplicate entries in the file

entries should be arranged in decreasing order of frequency

for example check 'english_frequency_table.txt' in source
