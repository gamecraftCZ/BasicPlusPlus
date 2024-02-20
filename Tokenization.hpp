#ifndef BASICPLUSPLUS_TOKENIZATION_HPP
#define BASICPLUSPLUS_TOKENIZATION_HPP

#include <string>
#include <variant>
#include <format>
#include <vector>
#include <memory>
#include <map>
#include <istream>

namespace Tokenization {
    using Literal = std::variant<std::string, double, bool>;
    
    enum TokenType {
        // One character
        LEFT_PAREN, RIGHT_PAREN, COMMA,
        MINUS, PLUS, SLASH, STAR,

        // Potentially more characters
        EQUAL, EQUAL_EQUAL,
        GREATER, GREATER_EQUAL,
        LESS, LESS_EQUAL, NOT_EQUAL,

        // Literals
        IDENTIFIER, STRING, NUMBER, BOOLEAN,

        // Keywords
        REM, LET, INPUT, PRINT, TONUM, TOSTR,
        IF, THEN, ELSE, END, WHILE, DO, BREAK, CONTINUE,
        NOT, AND, OR,
        
        EOF_TOKEN
    };
    
    class Token {
    public:
        const TokenType type;
        const std::string lexeme;
        const std::optional<Literal> literal;
        const u_int32_t line;
    };
    
    class TokenizationError : public std::exception {};

    class Tokenizer {
    private:
        static const std::map<std::string, TokenType> keywords;
        
        std::istream &inputStream;
        bool isEnd = false;
        char lastChar = 0;
        char curChar = 0;
        char nextChar = 0;
        std::unique_ptr<std::vector<Token>> tokens = std::make_unique<std::vector<Token>>();
        u_int32_t lineNumber = 1;  // Current line
        std::string errorMessage;  // Error message is stored here if error occurred
        
        void scanToken();
        
        void scanString();
        
        void scanNumber();
        
        void scanIdentifier();

        char advance();
        
        char prev();
        
        char cur();
        
        char peek();
        
        bool isAtEnd();
        
        static bool isDigit(char c);
        
        static bool isAlpha(char c);
        
        bool isAlphaNum(char c);
        
        // Match next char with c
        bool matchNext(char c);

        void addToken(TokenType type, std::string &&lexeme);

        void addToken(TokenType type, char lexeme);

        void addToken(TokenType type, std::string &&lexeme, Literal &&literal);
        
        void throwError(std::string &&message);

    public:
        explicit Tokenizer(std::istream &inputStream) : inputStream(inputStream) {
            if (!inputStream.get(nextChar)) {
                nextChar = 0;
                isEnd = true;
            }
        }

        void scanTokens();
        
        std::unique_ptr<std::vector<Token>> getTokens();

        std::string& getErrorMessage();
        
        uint32_t getErrorLine() const;
    };
}


#endif //BASICPLUSPLUS_TOKENIZATION_HPP
