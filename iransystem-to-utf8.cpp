#include <cstdio>
#include <cstdint>
#include <cstring>

#define MAX_SIZE (1<<24)
#define REVERSE 1		// Are characters of Right-to-Left parts of input in reverse order?

static const char* ZWNJ = "\xE2\x80\x8C";
static const size_t ZWNJ_LENGTH = strlen(ZWNJ);

static const char* const digits_fa[10] =
{
	"\xDB\xB0",		// ۰
	"\xDB\xB1",		// ۱
	"\xDB\xB2",		// ۲
	"\xDB\xB3",		// ۳
	"\xDB\xB4",		// ۴
	"\xDB\xB5",		// ۵
	"\xDB\xB6",		// ۶
	"\xDB\xB7",		// ۷
	"\xDB\xB8",		// ۸
	"\xDB\xB9",		// ۹
};

enum CharJoining		// What a char does while it shouldn't
{
	JOINS_NONE = 0,		// Leave it alone. It doesn't join badly.
	JOINS_PREV = 1,		// If previous has JOINS_NEXT, it accepts
	JOINS_NEXT = 2,		// Tries to join the next character, while it shouldn't
	JOINS_BOTH = JOINS_PREV | JOINS_NEXT,
};

enum CharOrdering
{
	ORDER_AGNOSTIC = 0,
	ORDER_CHECKPOINT,
	ORDER_LTR,
	ORDER_RTL,
};

static char ascii[256];
static const char* map[256] = { nullptr };
static uint8_t map_size[256] = { 0 };
CharJoining map_joining[256] = { JOINS_NONE };
static CharOrdering map_ordering[256] = { ORDER_AGNOSTIC };

CharJoining print_byte(uint8_t byte, CharJoining prev_joining)
{
	CharJoining my_joining = map_joining[byte];
	const char* to = map[byte];
	uint8_t to_size = map_size[byte];
	if (to_size)
	{
		if ((prev_joining & JOINS_NEXT) && (my_joining & JOINS_PREV))
			fwrite(ZWNJ, 1, ZWNJ_LENGTH, stdout);		// Prevent their joining using a zero-width non-joiner
		fwrite(to, 1, to_size, stdout);
	}
	else if (!to)
	{
		printf("<!-- unknown byte [%#.2x] -->", byte);
		fprintf(stderr, "Warning: unknown byte %#.2x\n", byte);
	}
	return my_joining;
}

int main(int argc, const char* argv[])
{
	if (argc > 1 && freopen(argv[1], "r", stdin) == nullptr)
	{
		fputs("Error: Failed to open input file\n", stderr);
		return 1;
	}

	for (int i = 0; i < 256; ++i)
		ascii[i] = i;
	for (int i = 0; i < 256; ++i)
		switch (i)
		{
			case '(':
				map[i] = &ascii[')'];
				map_size[i] = 1;
				break;
			case ')':
				map[i] = &ascii['('];
				map_size[i] = 1;
				break;
			case '[':
				map[i] = &ascii[']'];
				map_size[i] = 1;
				break;
			case ']':
				map[i] = &ascii['['];
				map_size[i] = 1;
				break;
			case '{':
				map[i] = &ascii['}'];
				map_size[i] = 1;
				break;
			case '}':
				map[i] = &ascii['{'];
				map_size[i] = 1;
				break;
			case 0x80:		// ۰
			case 0x81:		// ۱
			case 0x82:		// ۲
			case 0x83:		// ۳
			case 0x84:		// ۴
			case 0x85:		// ۵
			case 0x86:		// ۶
			case 0x87:		// ۷
			case 0x88:		// ۸
			case 0x89:		// ۹
				map[i] = digits_fa[i - 0x80];
				map_size[i] = 2;
				map_ordering[i] = ORDER_LTR;
				break;
			case 0x8A:		// ، (ویرگول)
				map[i] = "\xD8\x8C";
				map_size[i] = 2;
				break;
			case 0x8B:		// ـ
				map[i] = "\xD9\x80";
				map_size[i] = 2;
				break;
			case 0x8C:		// ؟
				map[i] = "\xD8\x9F";
				map_size[i] = 2;
				break;
			case 0x8D:		// آ جدا و آخر
				map[i] = "\xD8\xA2";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0x8E:		// ئ اول و وسط
				map[i] = "\xD8\xA6";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0x8F:		// همزه جدا
				map[i] = "\xD8\xA1";
				map_size[i] = 2;
				break;
			case 0x90:		// الف جدا
				map_joining[i] = JOINS_PREV;
			case 0x91:		// الف آخر
				map[i] = "\xD8\xA7";
				map_size[i] = 2;
				break;
			case 0x92:		// ب آخر
				map[i] = "\xD8\xA8";
				map_size[i] = 2;
				map_joining[i] = JOINS_BOTH;
				break;
			case 0x93:		// ب اول
				map[i] = "\xD8\xA8";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0x94:		// پ آخر
				map[i] = "\xD9\xBE";
				map_size[i] = 2;
				map_joining[i] = JOINS_BOTH;
				break;
			case 0x95:		// پ اول
				map[i] = "\xD9\xBE";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0x96:		// ت آخر
				map[i] = "\xD8\xAA";
				map_size[i] = 2;
				map_joining[i] = JOINS_BOTH;
				break;
			case 0x97:		// ت اول
				map[i] = "\xD8\xAA";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0x98:		// ث آخر
				map[i] = "\xD8\xAB";
				map_size[i] = 2;
				map_joining[i] = JOINS_BOTH;
				break;
			case 0x99:		// ث اول
				map[i] = "\xD8\xAB";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0x9A:		// جیم آخر
				map[i] = "\xD8\xAC";
				map_size[i] = 2;
				map_joining[i] = JOINS_BOTH;
				break;
			case 0x9B:		// جیم اول
				map[i] = "\xD8\xAC";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0x9C:		// چ آخر
				map[i] = "\xDA\x86";
				map_size[i] = 2;
				map_joining[i] = JOINS_BOTH;
				break;
			case 0x9D:		// چ اول
				map[i] = "\xDA\x86";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0x9E:		// ح آخر
				map[i] = "\xD8\xAD";
				map_size[i] = 2;
				map_joining[i] = JOINS_BOTH;
				break;
			case 0x9F:		// ح اول
				map[i] = "\xD8\xAD";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xA0:		// خ آخر
				map[i] = "\xD8\xAE";
				map_size[i] = 2;
				map_joining[i] = JOINS_BOTH;
				break;
			case 0xA1:		// خ اول
				map[i] = "\xD8\xAE";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xA2:		// دال جدا و آخر
				map[i] = "\xD8\xAF";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xA3:		// ذال جدا و آخر
				map[i] = "\xD8\xB0";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xA4:		// ر جدا و آخر
				map[i] = "\xD8\xB1";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xA5:		// ز جدا و آخر
				map[i] = "\xD8\xB2";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xA6:		// ژ جدا و آخر
				map[i] = "\xDA\x98";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xA7:		// سین آخر
				map[i] = "\xD8\xB3";
				map_size[i] = 2;
				map_joining[i] = JOINS_BOTH;
				break;
			case 0xA8:		// سین اول
				map[i] = "\xD8\xB3";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xA9:		// شین جدا
				map[i] = "\xD8\xB4";
				map_size[i] = 2;
				map_joining[i] = JOINS_BOTH;
				break;
			case 0xAA:		// شین اول
				map[i] = "\xD8\xB4";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xAB:		// صاد آخر
				map[i] = "\xD8\xB5";
				map_size[i] = 2;
				map_joining[i] = JOINS_BOTH;
				break;
			case 0xAC:		// صاد اول
				map[i] = "\xD8\xB5";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xAD:		// ضاد آخر
				map[i] = "\xD8\xB6";
				map_size[i] = 2;
				map_joining[i] = JOINS_BOTH;
				break;
			case 0xAE:		// ضاد اول
				map[i] = "\xD8\xB6";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xAF:		// طای اول
				map[i] = "\xD8\xB7";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xB0:
			case 0xB1:
			case 0xB2:
			case 0xB3:
			case 0xB4:
			case 0xB5:
			case 0xB6:
			case 0xB7:
			case 0xB8:
			case 0xB9:
			case 0xBA:
			case 0xBB:
			case 0xBC:
			case 0xBD:
			case 0xBE:
			case 0xBF:
			case 0xC0:
			case 0xC1:
			case 0xC2:
			case 0xC3:
			case 0xC4:
			case 0xC5:
			case 0xC6:
			case 0xC7:
			case 0xC8:
			case 0xC9:
			case 0xCA:
			case 0xCB:
			case 0xCC:
			case 0xCD:
			case 0xCE:
			case 0xCF:
			case 0xD0:
			case 0xD1:
			case 0xD2:
			case 0xD3:
			case 0xD4:
			case 0xD5:
			case 0xD6:
			case 0xD7:
			case 0xD8:
			case 0xD9:
			case 0xDA:
			case 0xDB:
			case 0xDC:
			case 0xDD:
			case 0xDE:
			case 0xDF:
				// TODO: Add table drawing shapes
				break;
			case 0xE0:		// ظای اول
				map[i] = "\xD8\xB8";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xE1:		// عین جدا
			case 0xE2:		// عین آخر
				map[i] = "\xD8\xB9";
				map_size[i] = 2;
				map_joining[i] = JOINS_BOTH;
				break;
			case 0xE3:		// عین وسط
			case 0xE4:		// عین اول
				map[i] = "\xD8\xB9";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xE5:		// غین جدا
			case 0xE6:		// غین آخر
				map[i] = "\xD8\xBA";
				map_size[i] = 2;
				map_joining[i] = JOINS_BOTH;
				break;
			case 0xE7:		// غین وسط
			case 0xE8:		// غین اول
				map[i] = "\xD8\xBA";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xE9:		// ف آخر
				map[i] = "\xD9\x81";
				map_size[i] = 2;
				map_joining[i] = JOINS_BOTH;
				break;
			case 0xEA:		// ف وسط
				map[i] = "\xD9\x81";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xEB:		// قاف جدا
				map[i] = "\xD9\x82";
				map_size[i] = 2;
				map_joining[i] = JOINS_BOTH;
				break;
			case 0xEC:		// قاف اول
				map[i] = "\xD9\x82";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xED:		// کاف آخر
				map[i] = "\xDA\xA9";
				map_size[i] = 2;
				map_joining[i] = JOINS_BOTH;
				break;
			case 0xEE:		// کاف وسط
				map[i] = "\xDA\xA9";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xEF:		// گاف آخر
				map[i] = "\xDA\xAF";
				map_size[i] = 2;
				map_joining[i] = JOINS_BOTH;
				break;
			case 0xF0:		// گاف اول
				map[i] = "\xDA\xAF";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xF1:		// لام جدا
				map[i] = "\xD9\x84";
				map_size[i] = 2;
				map_joining[i] = JOINS_BOTH;
				break;
			case 0xF2:		// لا
				map[i] = "\xD9\x84\xD8\xA7";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xF3:		// لام اول و وسط
				map[i] = "\xD9\x84";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xF4:		// میم جدا
				map[i] = "\xD9\x85";
				map_size[i] = 2;
				map_joining[i] = JOINS_BOTH;
				break;
			case 0xF5:		// میم اول
				map[i] = "\xD9\x85";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xF6:		// نون جدا
				map[i] = "\xD9\x86";
				map_size[i] = 2;
				map_joining[i] = JOINS_BOTH;
				break;
			case 0xF7:		// نون اول
				map[i] = "\xD9\x86";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xF8:		// واو آخر و جدا
				map[i] = "\xD9\x88";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xF9:		// ه آخر و جدا
				map[i] = "\xD9\x87";
				map_size[i] = 2;
				map_joining[i] = JOINS_BOTH;
				break;
			case 0xFA:		// ه وسط
			case 0xFB:		// ه اول
				map[i] = "\xD9\x87";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			case 0xFC:		// ی آخر
			case 0xFD:		// ی جدا
				map[i] = "\xDB\x8C";
				map_size[i] = 2;
				map_joining[i] = JOINS_BOTH;
				break;
			case 0xFE:		// ی اول و وسط
				map[i] = "\xDB\x8C";
				map_size[i] = 2;
				map_joining[i] = JOINS_PREV;
				break;
			default:
				map[i] = &ascii[i];		// Default to the same ASCII character
				map_size[i] = 1;
				break;
		}

	for (int c = '0'; c < '9'; ++c)
		map_ordering[c] = ORDER_LTR;
	for (int c = 'A'; c < 'Z'; ++c)
		map_ordering[c] = ORDER_LTR;
	for (int c = 'a'; c < 'z'; ++c)
		map_ordering[c] = ORDER_LTR;
	for (int i = 0; i < 256; ++i)
		if (map_size[i] > 1 && map_ordering[i] == ORDER_AGNOSTIC)
			map_ordering[i] = ORDER_RTL;
	map_ordering['\n'] = ORDER_CHECKPOINT;

	uint8_t* buf = new uint8_t[MAX_SIZE];
	size_t size = fread(buf, 1, MAX_SIZE, stdin);

	CharJoining prev_joining = JOINS_NONE;
	uint8_t* data = buf;
#if REVERSE
	uint8_t* context_start = data;
	while (size >= 0)
	{
		uint8_t byte = *data;
		CharOrdering my_ordering = map_ordering[byte];
		if (my_ordering == ORDER_CHECKPOINT || size == 0)
		{
			uint8_t* context_end = data - 1;
			// Reverse the context
			for (int i = 0; context_start + i < context_end - i; ++i)
			{
				uint8_t temp = context_end[-i];
				context_end[-i] = context_start[i];
				context_start[i] = temp;
			}
			const uint8_t* reverse_start = nullptr;
			for (const uint8_t* ptr = context_start; ptr <= data; ++ptr)
			{
				if (map_ordering[*ptr] != ORDER_LTR)
				{
					if (reverse_start)
					{
						for (const uint8_t* reverse_ptr = ptr - 1; reverse_ptr >= reverse_start; --reverse_ptr)
							prev_joining = print_byte(*reverse_ptr, prev_joining);
						reverse_start = nullptr;
					}
					prev_joining = print_byte(*ptr, prev_joining);
				}
				else if (!reverse_start)
					reverse_start = ptr;
			}
			if (size == 0)
				break;
			context_start = data + 1;
		}

		++data;
		--size;
	}
#else
	while (size > 0)
	{
		uint8_t byte = *data;
		prev_joining = print_byte(byte, prev_joining);
		++data;
		--size;
	}
#endif
	
	delete[] buf;
	return 0;
}
