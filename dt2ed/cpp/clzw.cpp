
#include "precomp.h"

/********************************************************************
**
** Copyright (c) 1989 Mark R. Nelson
**
** LZW data compression/expansion demonstration program.
**
** April 13, 1989
**
** Minor mods made 7/19/2006 to conform with ANSI-C - prototypes, casting, 
** and argument agreement.
**
*****************************************************************************/

/********************************************************************
**
** This program gets a file name from the command line.  It compresses the
** file, placing its output in a file named test.lzw.  It then expands
** test.lzw into test.out.  Test.out should then be an exact duplicate of
** the input file.
**
*************************************************************************/

CLzw::CLzw(BOOL fFailOnGrowSize)
	: m_fFailOnGrowSize( fFailOnGrowSize )
{

}

void CLzw::flush()
{
	code_value.clear();
	prefix_code.clear();
	append_character.clear();
	decode_stack.clear();
	ZeroMemory(&bits, sizeof(bits));

	code_value.insert(code_value.end(), TABLE_SIZE, -1);
	prefix_code.insert(prefix_code.end(), TABLE_SIZE, 0);
	append_character.insert(append_character.end(), TABLE_SIZE, 0);
	decode_stack.insert(decode_stack.end(), 4000, 0);
}

CLzw::~CLzw()
{
	flush();
}

/*
** This is the compression routine.  The code should be a fairly close
** match to the algorithm accompanying the article.
**
*/

int CLzw::deflate(PBYTE pInput, int inpLen, PBYTE & pOutput)
{
	std::vector<BYTE> vOut;
	deflate(pInput, inpLen, vOut);
	if ( vOut.size() )
	{
		pOutput = new BYTE[vOut.size()];
		CopyMemory(pOutput, &vOut[0], vOut.size());
	}

	return vOut.size();
}

int CLzw::inflate(PBYTE pInput, int inpLen, PBYTE & pOutput)
{
	std::vector<BYTE> vOut;
	inflate(pInput, inpLen, vOut);
	if ( vOut.size() )
	{
		pOutput = new BYTE[vOut.size()];
		CopyMemory(pOutput, &vOut[0], vOut.size());
	}

	return vOut.size();
}


void CLzw::deflate(PBYTE pInput, int inpLen, std::vector<BYTE> & pOutput)
{
	if ( !pInput || IsBadReadPtr(pInput, inpLen) )
		return;

	flush();

	std::vector<int> vInput, vOutput;

	int i = 0, chr = 0;
	while(i < inpLen)
		vInput.push_back(pInput[i++]);

	
	unsigned int next_code;
	unsigned int character;
	unsigned int string_code;
	unsigned int index;

	next_code=256;              /* Next code is the next available string code*/

	string_code=vInput[chr++];    /* Get the first code                         */
/*
** This is the main loop where it all happens.  This loop runs util all of
** the input has been exhausted.  Note that it stops adding codes to the
** table after all of the possible codes have been defined.
*/
	while ( chr < int(vInput.size()) )
	{
		character = vInput[chr++];
			
		index=find_match(string_code,character);/* See if the string is in */
		if (code_value[index] != -1)            /* the table.  If it is,   */
			string_code=code_value[index];        /* get the code value.  If */
		else                                    /* the string is not in the*/
		{                                       /* table, try to add it.   */
			if (next_code <= MAX_CODE)
			{
				code_value[index]=next_code++;
				prefix_code[index]=string_code;
				append_character[index]=character;
			}
			output_code(vOutput,string_code);  /* When a string is found  */
			string_code=character;            /* that is not in the table*/
		}                                   /* I output the last string*/
	}                                     /* after adding the new one*/
/*
** End of the main loop.
*/
	output_code(vOutput,string_code); /* Output the last code               */
	output_code(vOutput,MAX_VALUE);   /* Output the end of buffer code      */
	output_code(vOutput,0);           /* This code flushes the output buffer*/

	if ( vOutput.size() > 0 )
	{
	  pOutput.clear();
	  if ( !m_fFailOnGrowSize || vOutput.size() <= vInput.size() )
		  pOutput.insert(pOutput.end(), vOutput.begin(), vOutput.end());
	}
}

/*
** This is the hashing routine.  It tries to find a match for the prefix+char
** string in the string table.  If it finds it, the index is returned.  If
** the string is not found, the first available index in the string table is
** returned instead.
*/

int CLzw::find_match(int hash_prefix,unsigned int hash_character)
{
	int index;
	int offset;

  index = (hash_character << HASHING_SHIFT) ^ hash_prefix;
  if (index == 0)
    offset = 1;
  else
    offset = TABLE_SIZE - index;
  while (1)
  {
    if (code_value[index] == -1)
      return(index);
    if (prefix_code[index] == hash_prefix && 
        append_character[index] == hash_character)
      return(index);
    index -= offset;
    if (index < 0)
      index += TABLE_SIZE;
  }
}

/*
**  This is the expansion routine.  It takes an LZW format file, and expands
**  it to an output file.  The code here should be a fairly close match to
**  the algorithm in the accompanying article.
*/

void CLzw::inflate(PBYTE pInput, int inpLen, std::vector<BYTE> & pOutput)
{
	if ( !pInput || IsBadReadPtr(pInput, inpLen) )
		return;

	std::vector<int> vInput, vOutput;

	int i = 0, chr = 0, ds = 0;
	while(i < inpLen)
		vInput.push_back(pInput[i++]);

	flush();

	unsigned int next_code;
	unsigned int new_code;
	unsigned int old_code;
	int character;

	next_code=256;           /* This is the next available code to define */

	old_code=input_code(vInput, chr);  /* Read in the first code, initialize the */
	character=old_code;          /* character variable, and send the first */
	vOutput.push_back(old_code); /* code to the output file                */
/*
**  This is the main expansion loop.  It reads in characters from the LZW file
**  until it sees the special code used to inidicate the end of the data.
*/
  while ((new_code=input_code(vInput, chr)) != (MAX_VALUE))
  {
/*
** This code checks for the special STRING+CHARACTER+STRING+CHARACTER+STRING
** case which generates an undefined code.  It handles it by decoding
** the last code, and adding a single character to the end of the decode string.
*/
    if (new_code>=next_code)
    {
      decode_stack[ds++]=character;
      decode_string(decode_stack, ds, old_code);
    }
/*
** Otherwise we do a straight decode of the new code.
*/
    else
      decode_string(decode_stack, ds, new_code);
/*
** Now we output the decoded string in reverse order.
*/
	character = decode_stack[ds];
    while (ds >= 0)
		vOutput.push_back(decode_stack[ds--]);
	ds = 0;

	/*
** Finally, if possible, add a new code to the string table.
*/
    if (next_code <= MAX_CODE)
    {
      prefix_code[next_code]=old_code;
      append_character[next_code]=character;
      next_code++;
    }
    old_code=new_code;
  }

  if ( vOutput.size() > 0 )
  {
		pOutput.clear();
		pOutput.insert(pOutput.end(), vOutput.begin(), vOutput.end());
  }
}

/*
** This routine simply decodes a string from the string table, storing
** it in a buffer.  The buffer can then be output in reverse order by
** the expansion program.
*/

BOOL CLzw::decode_string(std::vector<BYTE> & buffer, int & ds, unsigned int code)
{
	int i = 0;
	while (code > 255)
	{
		buffer[ds++] = append_character[code];
		code=prefix_code[code];
		if (i++>=MAX_CODE)
		{
			_tprintf(TEXT("Fatal error during code expansion.\n"));
			return NULL;
		}
	}
	buffer[ds]=code;
	return TRUE;
}

/*
** The following two routines are used to output variable length
** codes.  They are written strictly for clarity, and are not
** particularyl efficient.
*/

unsigned int CLzw::input_code(std::vector<int> & vInput, int & chr)
{
	unsigned int return_value;

	while (bits.input_bit_count <= 24 && chr < int(vInput.size()) )
	{
		bits.input_bit_buffer |= 
			(unsigned long) vInput[chr++] << (24-bits.input_bit_count);
		
		bits.input_bit_count += 8;
	}

	return_value=bits.input_bit_buffer >> (32-BITS);
	bits.input_bit_buffer <<= BITS;
	bits.input_bit_count -= BITS;
	return(return_value);
}

void CLzw::output_code(std::vector<int> & pOutput, int code)
{
	bits.output_bit_buffer |= (unsigned long) code << (32-BITS-bits.output_bit_count);
	bits.output_bit_count += BITS;
	while (bits.output_bit_count >= 8)
	{
		pOutput.push_back( bits.output_bit_buffer >> 24 );
		bits.output_bit_buffer <<= 8;
		bits.output_bit_count -= 8;
	}
}