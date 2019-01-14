
#define BITS 12                   /* Setting the number of bits to 12, 13*/
#define HASHING_SHIFT (BITS-8)    /* or 14 affects several constants.    */
#define MAX_VALUE (1 << BITS) - 1 /* Note that MS-DOS machines need to   */
#define MAX_CODE MAX_VALUE - 1    /* compile their code in large model if*/
                                  /* 14 bits are selected.               */
#if BITS == 14
  #define TABLE_SIZE 18041        /* The string table size needs to be a */
#endif                            /* prime number that is somewhat larger*/
#if BITS == 13                    /* than 2**BITS.                       */
  #define TABLE_SIZE 9029
#endif
#if BITS <= 12
  #define TABLE_SIZE 5021
#endif

class CLzw
{
	BOOL							m_fFailOnGrowSize;

	struct
	{
		int output_bit_count;
		unsigned long output_bit_buffer;
		int input_bit_count;
		unsigned long input_bit_buffer;
	} bits;

	std::vector<int>				code_value;
	std::vector<UINT>				prefix_code;
	std::vector<BYTE>				append_character;
	std::vector<BYTE>				decode_stack;
		
	void							output_code(std::vector<int> & pOutput, int code);
	unsigned int					input_code(std::vector<int> & vInput, int & chr);
	BOOL							decode_string(std::vector<BYTE> & buffer, int & ds, unsigned int code);
	int								find_match(int hash_prefix,unsigned int hash_character);

	void							flush();

public:
	CLzw(BOOL fFailOnGrowSize = TRUE);
	~CLzw();

	int			deflate(PBYTE pInput, int inpLen, PBYTE & pOutput);
	int			inflate(PBYTE pInput, int inpLen, PBYTE & pOutput);
	void		deflate(PBYTE pInput, int inpLen, std::vector<BYTE> & pOutput);
	void		inflate(PBYTE pInput, int inpLen, std::vector<BYTE> & pOutput);

};