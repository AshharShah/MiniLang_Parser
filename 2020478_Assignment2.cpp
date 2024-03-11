#include <iostream>
#include <string>
#include <vector>
#include <cctype>

using namespace std;

// define the token types
enum class TokenType {
    INTEGER,
    BOOLEAN,
    IDENTIFIER,
    OPERATOR,
    KEYWORD,
    COMMENT,
    END_OF_FILE
};

// create a structure for the token, each token can have type and its value
struct Token {
    TokenType type;
    string value;
};

// this is the implementation of our scanner for the MiniLang programming language
class Scanner {
private:
    string sourceCode;
    size_t currentPos = 0;

    // function to check if the provided character is a digit
    bool isDigit(char c) {
        return isdigit(static_cast<unsigned char>(c));
    }

    // function to check if the provided character is a alphabet
    bool isAlpha(char c) {
        return isalpha(static_cast<unsigned char>(c)) || c == '_';
    }

    // function that will skip the white spaces in the sting by incrementing the pointer forward
    void skipWhitespace() {
        while (currentPos < sourceCode.size() && isspace(sourceCode[currentPos])) {
            currentPos++;
        }
    }

    // function to check if we have an identifier or a keyword
    Token readIdentifierOrKeyword() {
        size_t startPos = currentPos;
        // get the whole word from the input string
        while (currentPos < sourceCode.size() && (isAlpha(sourceCode[currentPos]) || isDigit(sourceCode[currentPos]))) {
            currentPos++;
        }
        string identifier = sourceCode.substr(startPos, currentPos - startPos);

        // check if the identifier is a keyword
        if (identifier == "if" || identifier == "else" || identifier == "print" || identifier == "true" || identifier == "false") {
            return {TokenType::KEYWORD, identifier};
        } else {
            return {TokenType::IDENTIFIER, identifier};
        }
    }

    // this function processes any numbers in the string
    Token readNumber() {
        size_t startPos = currentPos;
        while (currentPos < sourceCode.size() && isDigit(sourceCode[currentPos])) {
            currentPos++;
        }
        string number = sourceCode.substr(startPos, currentPos - startPos);
        return {TokenType::INTEGER, number};
    }

    // this function checks if we have an operator
    Token readOperator() {
        char op = sourceCode[currentPos];
        currentPos++;
        return {TokenType::OPERATOR, string(1, op)};
    }

    // this function reads comments
    Token readComment() {
        size_t startPos = currentPos;
        while (currentPos < sourceCode.size() && sourceCode[currentPos] != '\n') {
            currentPos++;
        }
        return {TokenType::COMMENT, sourceCode.substr(startPos, currentPos - startPos)};
    }

public:
    Scanner(const string& code) : sourceCode(code) {}

    // this is the function that generates the tokens for our scanner
    Token getNextToken() {
        // we skip any white spaces
        skipWhitespace();
        
        // check if we are at the end of a file
        if (currentPos >= sourceCode.size()) {
            return {TokenType::END_OF_FILE, ""};
        }
        
        // get the head of the pointer
        char currentChar = sourceCode[currentPos];

        // check for the multiple token possibilies and call the desired function
        if (isAlpha(currentChar)) {
            return readIdentifierOrKeyword();
        } else if (isDigit(currentChar)) {
            return readNumber();
        } else if (currentChar == '+' || currentChar == '-' || currentChar == '*' || currentChar == '/') {
            return readOperator();
        } else if (currentChar == '(' || currentChar == ')') {
            currentPos++;
            return {TokenType::OPERATOR, string(1, currentChar)};
        } else if (currentChar == '=') {
            currentPos++;
            return {TokenType::OPERATOR, "="};
        } else if (currentChar == ';') {
            currentPos++;
            return {TokenType::OPERATOR, ";"};
        } else if (currentChar == '#') {
            return readComment();
        } else {
            // unexpected character
            currentPos++;
            return {TokenType::OPERATOR, string(1, currentChar)};
        }
    }
};

// this is the implemenation for the parser class
class Parser {
private:
    // we maintain the list of tokens that we have recieved from the scanner
    vector<Token> tokens;
    // we maintain a pointer which points to the current token being processed
    size_t currentTokenIndex = 0;

    // this function retrieves the current token that is being pointed to
    Token getCurrentToken() {
        if (currentTokenIndex < tokens.size()) {
            return tokens[currentTokenIndex];
        } else {
            return {TokenType::END_OF_FILE, ""};
        }
    }

    // this function is used when we have completely processed a token
    void consumeToken() {
        currentTokenIndex++;
    }

    // this is the start of our CFG where program -> statement
    void program() {
        if (getCurrentToken().type != TokenType::END_OF_FILE) {
            statement();
        }
    }

    // this is the statement rule of our CFG
    // <statement> --> <assignment> | <conditional> | <printStatement>
    void statement() {
        Token currentToken = getCurrentToken();
        if (currentToken.type == TokenType::IDENTIFIER) {
            assignment();
        } else if (currentToken.type == TokenType::KEYWORD && currentToken.value == "if") {
            conditional();
        } else if (currentToken.type == TokenType::KEYWORD && currentToken.value == "print") {
            printStatement();
        } else {
            // Handle syntax error
            cout << "Syntax error: Unexpected token " << currentToken.value << endl;
            consumeToken(); // Consume the unexpected token and try to recover
        }
    }

    // this is the assignment rule of our CFG
    // <assignment> --> = <expression> ; 
    void assignment() {
        // Parse variable assignment
        Token identifier = getCurrentToken();
        consumeToken(); // Consume identifier
        if (getCurrentToken().value == "=") {
            consumeToken(); // Consume "="
            expression(); // Parse expression
            if (getCurrentToken().value == ";") {
                consumeToken(); // Consume ";"
            } else {
                // Handle syntax error
                cout << "Syntax error: Expected ';', found " << getCurrentToken().value << endl;
                consumeToken(); // Consume the unexpected token and try to recover
            }
        } else {
            // Handle syntax error
            cout << "Syntax error: Expected '=', found " << getCurrentToken().value << endl;
            consumeToken(); // Consume the unexpected token and try to recover
        }
    }

    // this is the conditional rule of our CFG
    // <conditional> --> if ( <expression> ) { <program> } | ( <expression> ) { <program> } else { <program> }
    void conditional() {
        // Parse if-else statement
        consumeToken(); // Consume "if"
        if (getCurrentToken().value == "(") {
            consumeToken(); // Consume "("
            expression(); // Parse condition
            if (getCurrentToken().value == ")") {
                consumeToken(); // Consume ")"
                if (getCurrentToken().value == "{") {
                    // Parse true block
                    consumeToken(); // Consume "{"
                    program(); // Parse true block
                    if (getCurrentToken().value == "}") {
                        consumeToken(); // Consume "}"
                        if (getCurrentToken().value == "else") {
                            // Parse else block
                            consumeToken(); // Consume "else"
                            if (getCurrentToken().value == "{") {
                                consumeToken(); // Consume "{"
                                program(); // Parse else block
                                if (getCurrentToken().value == "}") {
                                    consumeToken(); // Consume "}"
                                } else {
                                    // Handle syntax error
                                    cout << "Syntax error: Expected '}', found " << getCurrentToken().value << endl;
                                    consumeToken(); // Consume the unexpected token and try to recover
                                }
                            } else {
                                // Handle syntax error
                                cout << "Syntax error: Expected '{' after else, found " << getCurrentToken().value << endl;
                                consumeToken(); // Consume the unexpected token and try to recover
                            }
                        }
                    } else {
                        // Handle syntax error
                        cout << "Syntax error: Expected '}', found " << getCurrentToken().value << endl;
                        consumeToken(); // Consume the unexpected token and try to recover
                    }
                } else {
                    // Handle syntax error
                    cout << "Syntax error: Expected '{' after if condition, found " << getCurrentToken().value << endl;
                    consumeToken(); // Consume the unexpected token and try to recover
                }
            } else {
                // Handle syntax error
                cout << "Syntax error: Expected ')', found " << getCurrentToken().value << endl;
                consumeToken(); // Consume the unexpected token and try to recover
            }
        } else {
            // Handle syntax error
            cout << "Syntax error: Expected '(' after if, found " << getCurrentToken().value << endl;
            consumeToken(); // Consume the unexpected token and try to recover
        }
    }

    // this is the CFG rule for our print statement
    // <printStatement> -- > print <expression> ;
    void printStatement() {
        // Parse print statement
        consumeToken(); // Consume "print"
        expression(); // Parse expression
        if (getCurrentToken().value == ";") {
            consumeToken(); // Consume ";"
        } else {
            // Handle syntax error
            cout << "Syntax error: Expected ';', found " << getCurrentToken().value << endl;
            consumeToken(); // Consume the unexpected token and try to recover
        }
    }

    // this is the CFG rule for our expression
    // <expression> --> <term> + <term> | <term> - <term> | <term>
    void expression() {
        // Parse arithmetic expression
        term(); // Parse the first term
        while (getCurrentToken().value == "+" || getCurrentToken().value == "-") {
            // Parse the operator
            consumeToken(); // Consume operator
            term(); // Parse the next term
        }
    }
    
    // this is the CFG rule for our term 
    // <term> --> <factor> * <factor> | <factor> / <factor> | <factor>
    void term() {
        // Parse term
        factor(); // Parse the first factor
        while (getCurrentToken().value == "*" || getCurrentToken().value == "/") {
            // Parse the operator
            consumeToken(); // Consume operator
            factor(); // Parse the next factor
        }
    }
    
    // this is the CFG for our factor 
    // <factor> --> INTEGER | IDENTIFIER | ( <expression> )
    void factor() {
        // Parse factor
        Token currentToken = getCurrentToken();
        if (currentToken.type == TokenType::INTEGER || currentToken.type == TokenType::IDENTIFIER) {
            consumeToken(); // Consume number or identifier
        } else if (currentToken.value == "(") {
            // Parse parentheses
            consumeToken(); // Consume "("
            expression(); // Parse expression within parentheses
            if (getCurrentToken().value == ")") {
                consumeToken(); // Consume ")"
            } else {
                // Handle syntax error
                cout << "Syntax error: Expected ')', found " << getCurrentToken().value << endl;
                consumeToken(); // Consume the unexpected token and try to recover
            }
        } else {
            // Handle syntax error
            cout << "Syntax error: Unexpected token " << currentToken.value << endl;
            consumeToken(); // Consume the unexpected token and try to recover
        }
    }
    
public:
    Parser(const vector<Token>& tokens) : tokens(tokens) {}

    void parse() {
        program();
    }
};

int main(int argc, char* argv[]) {

    if(argc < 2){
        printf("Provide MiniLang Code As Argument!\n");
        printf("./assign print(x)\n");
        return -1;
    }
    // Example source code
    string sourceCode = argv[1];

    // Tokenize source code
    Scanner scanner(sourceCode);
    vector<Token> tokens;
    Token token = scanner.getNextToken();
    // Continue tokenizing till all of the input string is processed
    while (token.type != TokenType::END_OF_FILE) {
        tokens.push_back(token);
        token = scanner.getNextToken();
    }


    // Print the tokens that our generated by our scanner
    for (const auto& token : tokens) {
        cout << "Type: ";
        switch (token.type) {
            case TokenType::INTEGER:
                cout << "INTEGER";
                break;
            case TokenType::BOOLEAN:
                cout << "BOOLEAN";
                break;
            case TokenType::IDENTIFIER:
                cout << "IDENTIFIER";
                break;
            case TokenType::OPERATOR:
                cout << "OPERATOR";
                break;
            case TokenType::KEYWORD:
                cout << "KEYWORD";
                break;
            case TokenType::COMMENT:
                cout << "COMMENT";
                break;
            case TokenType::END_OF_FILE:
                cout << "END_OF_FILE";
                break;
        }
        cout << ", Value: " << token.value << endl;
    }

    // Parser the tokens by running them through the CFG defined in the parser class
    Parser parser(tokens);
    parser.parse();

    return 0;
}