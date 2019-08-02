/*
A final version of the calculator program from PPP chapters 6/7, there are still some bugs
but after over a week on these chapters and this program its time to move on.
*/

#include "D:\Visual_Studio_Projects_Cpp_book\std_lib_facilities.h"

//------------------------------------------------------------------------------

const char NUMBER = 'n';
const char PRINT = ';';
const char QUIT = 'q';
const char NAME = 'a';
const char LET = 'l';
const string DECLARATION = "let";
const string PROMPT = ":> ";
const string RESULT = "= ";

class Token
{
public:
	char kind;        // what kind of token
	double value;     // for numbers: a value
	string name;

	Token() : kind{ 0 } {}
	Token(char ch) : kind{ ch } {}
	Token(char ch, double val) : kind{ ch }, value{ val }{}
	Token(char ch, string n) : kind{ ch }, name{ n }{}


	//Token(char ch)    // make a Token from a char
	//	:kind(ch), value(0) { }
	//Token(char ch, double val)     // make a Token from a char and a double
	//	:kind(ch), value(val) { }
	//Token()
};

//------------------------------------------------------------------------------

class Token_stream
{
public:
	Token_stream();   // make a Token_stream that reads from cin
	Token get();      // get a Token (get() is defined elsewhere)
	void putback(Token token);    // put a Token back
	void ignore(char c);	// Discard characters up to and including a c;
private:
	bool full;        // is there a Token in the buffer?
	Token buffer;     // here is where we keep a Token put back using putback()
};

//------------------------------------------------------------------------------

class Variable
{
public:
	string name;
	double value;
	Variable(string name, double value)
		:name(name), value(value) {}
};

//------------------------------------------------------------------------------

vector<Variable> varTable;

//------------------------------------------------------------------------------

double getValue(string s)
{
	// return the value of the Variable named s.

	for (const Variable& v : varTable)
	{
		if (v.name == s)
		{
			return v.value;
		}
	}

	error("get: undefined variable ", s);
}

//------------------------------------------------------------------------------

void setValue(string s, double d)
{
	// Set the Variable named s to d.

	for (Variable& v : varTable)
	{
		if (v.name == s)
		{
			v.value = d;
			return;
		}
	}

	error("set: undefined variable ", s);
}

//------------------------------------------------------------------------------


// The constructor just sets full to indicate that the buffer is empty:
Token_stream::Token_stream()
	:full(false), buffer(0)    // no Token in buffer
{
}

//------------------------------------------------------------------------------

// The putback() member function puts its argument back into the Token_stream's buffer:
void Token_stream::putback(Token token)
{
	//cout << "Putting back token kind: " << token.kind << '\n';
	//cout << "Putting back token value: " << token.value << '\n';

	if (full)
	{
		error("putback() into a full buffer");
	}

	buffer = token;       // copy t to buffer
	full = true;      // buffer is now full
}

//------------------------------------------------------------------------------

bool isDeclared(string var)
{
	// Is a var already in the varTable?

	//cout << "Checking if variable name already exists\n";

	for (const Variable& v : varTable)
	{
		if (v.name == var)
		{
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------------

Token Token_stream::get()
{
	//cout << "Calling token stream get()\n";

	if (full)
	{			// do we already have a Token ready?
						// remove token from buffer
		full = false;
		return buffer;
	}

	char ch;
	cin >> ch;    // note that >> skips whitespace (space, newline, tab, etc.)

	//cout << "The value read at this point(ch) was: " << ch << "\n";

	switch (ch)
	{
		case PRINT:    // for "print"
		case QUIT:    // for "quit"
		case '(':
		case ')':
		case '+':
		case '-':
		case '*':
		case '/':
		case '%':
		case '=':

			//cout << "The token kind read at this point(val) was: " << ch << "\n";
			return Token(ch);        // let each character represent itself

		case '.':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			{
				cin.putback(ch);         // put digit back into the input stream

				//cout << "About to read the number that was put back in to the input stream: \n";

				double val;
				cin >> val;              // read a floating-point number directly from the input stream we just put our 'ch' value back in to.

				//cout << "The token value read at this point(val) was: " << val << "\n";

				return Token(NUMBER, val);   // let 'n' represent "a number"
			}
		default:
			if (isalpha(ch))
			{
				//cout << "isalpha(ch) = " << ch << '\n';


				string s;
				s += ch;

				while (cin.get(ch) && (isalpha(ch) || isdigit(ch)))
				{
					s += ch;
				}

				//cout << "Putting back in to cin.putback " << ch << '\n';
				cin.putback(ch);

				//cout << "declaration = " << s << '\n';
				//cout << "s = " << s << '\n';

				if (isDeclared(s))
				{
					return Token{ NUMBER, getValue(s) };
				}

				if (s == DECLARATION)
				{
					//cout << "Returning LET\n";
					return Token{ LET };
				}

				//cout << "name = " << s << '\n';
				//cout << "s = " << s << '\n';

				return Token{ NAME, s };
			}

		error("Bad token");
	}
}

//------------------------------------------------------------------------------

Token_stream tokenStream;        // provides get() and putback() 

//------------------------------------------------------------------------------

double expression();    // declaration so that primary() can call expression()

//------------------------------------------------------------------------------

// deal with numbers and parentheses
double primary()
{
	//cout << "Primary token stream get() call\n";

	Token token = tokenStream.get();
	switch (token.kind)
	{
		case '(':					// handle '(' expression ')'
		{
			double d = expression();

			//cout << "Case '(' token stream get() call\n";

			token = tokenStream.get();
			if (token.kind != ')')
			{
				error("')' expected");
			}

			return d;
		}
		case NUMBER:
			//cout << "Case 'NUMBER'\n";
			return token.value;		// return the number's value
		case '-':
			//cout << "Case unary '-', return - primary() ntoken stream get() call\n";
			return - primary();
		case '+':
			//cout << "Case unary '+', return + primary() ntoken stream get() call\n";
			return +primary();
		default:
			error("primary expected");
	}
}

//------------------------------------------------------------------------------

// deal with *, /, and %
double term()
{
	double left = primary();

	//cout << "Left = " << left << '\n';

	//cout << "Term token stream get() call\n";

	Token token = tokenStream.get();        // get the next token from token stream

	while (true)
	{
		switch (token.kind)
		{
			case '*':
			{
				left *= primary();

				//cout << "Case '*' token stream get() call\n";

				token = tokenStream.get();
				break;
			}
			case '/':
			{
				double d = primary();
				if (d == 0) error("divide by zero");
				left /= d;

				//cout << "Case '/' token stream get() call\n";

				token = tokenStream.get();
				break;
			}
			case '%':
			{
				double d = primary();
				if (d == 0)
				{
					error("%: divide by zero!");
				}
				left = fmod(left, d);

				//cout << "Case '%' token stream get() call\n";

				token = tokenStream.get();
				break;
			}
			default:
			{
				tokenStream.putback(token);     // put t back into the token stream
				return left;
			}
		}
	}
}

//------------------------------------------------------------------------------

// deal with + and -
double expression()
{
	double left = term();      // read and evaluate a Term

	//cout << "Left = " << left << '\n';

	//cout << "Expression token stream get() call\n";

	Token token = tokenStream.get();        // get the next token from token stream

	while (true)
	{
		switch (token.kind)
		{
		case '+':
			left += term();    // evaluate Term and add

			//cout << "Case '+' token stream get() call\n";

			token = tokenStream.get();
			break;
		case '-':
			left -= term();    // evaluate Term and subtract

			//cout << "Case '-' token stream get() call\n";

			token = tokenStream.get();
			break;
		default:
			tokenStream.putback(token);     // put t back into the token stream
			return left;       // finally: no more + or -: return the answer
		}
	}
}

//------------------------------------------------------------------------------



//------------------------------------------------------------------------------

double defineName(string varName, double val)
{
	// add {var, val} to the varTable.

	//cout << "Defining declared variable\n";

	if (isDeclared(varName))
	{
		error(varName, " declared twice");
	}

	varTable.push_back(Variable(varName, val));

	return val;
}

//------------------------------------------------------------------------------

double declaration()
{
	// Assume we have seen "let".
	// Handle: name = expression.
	// Declare a variable called "name" with the initial value "expression"

	Token token = tokenStream.get();
	if (token.kind != NAME)
	{
		error("name expected in declaration");
	}

	string varName = token.name;

	Token token2 = tokenStream.get();
	if (token2.kind != '=')
	{
		error("= missing in declaration of ", varName);
	}

	double d = expression();

	//cout << "d = " << d << '\n';

	defineName(varName, d);
	return d;
}

double statement()
{
	//cout << "Calling statement()\n";

	Token token = tokenStream.get();

	switch (token.kind)
	{
		case LET:
			return declaration();
		default:
			tokenStream.putback(token);
			return expression();
	}

	//cout << "Leaving statement()\n";
}

//------------------------------------------------------------------------------


void cleanUpMess()
{
	tokenStream.ignore(PRINT);
}

//------------------------------------------------------------------------------

void calculate()
{
	//double val = 0;
	while (cin)
	{
		try
		{
			cout << PROMPT;

			//cout << "Main token stream get() call\n";

			Token token = tokenStream.get();

			while (token.kind == PRINT)
			{
				//cout << "Case ';' token stream get() call\n";

				token = tokenStream.get();
			}

			if (token.kind == QUIT)
			{
				return;
			}

			//cout << "Putting token back in token stream, value: " << token.value << '\n';
			//cout << "Putting token back in token stream, value: " << token.name << '\n';
			//cout << "Putting token back in token stream, value: " << token.kind << '\n';


			tokenStream.putback(token);

			cout << RESULT << statement() << '\n';
		}
		catch (exception& e)
		{
			cerr << e.what() << '\n';
			cleanUpMess();
		}
		
	}
}

//------------------------------------------------------------------------------

void Token_stream::ignore(char c)
{
	// c represents the kind of Token

	// First look in the buffer

	if (full && c == buffer.kind)
	{
		full = false;
		return;
	}

	full = false;

	// Now search input:

	char ch = 0;
	while (cin >> ch)
	{
		if (ch == c)
		{
			return;
		}
	}
}

//------------------------------------------------------------------------------



//------------------------------------------------------------------------------

int main()
try
{	
	// Predefined names
	defineName("pi", 3.1415926535);
	defineName("e", 2.7182818284);


	calculate();
	keep_window_open();
	return 0;
}
catch (exception& e) {
	cerr << "error: " << e.what() << '\n';

	keep_window_open("~~");

	//// Keep window open.
	//cout << "Please enter the character '~' to close the window\n";

	//// Keep reading until we find a tild.
	//for (char ch; cin >> ch;)
	//{
	//	if (ch == '~')
	//		return 1;
	//}
	return 1;
}
catch (...) {
	cerr << "Oops: unknown exception!\n";

	keep_window_open("~~");

	return 2;
}

//------------------------------------------------------------------------------
