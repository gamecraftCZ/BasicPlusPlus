#include <optional>
#include <string>
#include <istream>
#include "Tokenization.hpp"

namespace Tokenization {    
    void Tokenizer::scanToken() {
        char c = advance();
        switch (c) {
            case '(': addToken(LEFT_PAREN, c); break;
            case ')': addToken(RIGHT_PAREN, c); break;
            case ',': addToken(COMMA, c); break;
            case '-': addToken(MINUS, c); break;
            case '+': addToken(PLUS, c); break;
            case '*': addToken(STAR, c); break;
            case '/': addToken(SLASH, c); break;
            case '=':
                if (matchNext('=')) {
                    addToken(EQUAL_EQUAL, "==");
                    advance();
                } else {
                    addToken(EQUAL, "=");
                }
                break;
                
            case '<':
                if (matchNext('=')) {
                    addToken(LESS_EQUAL, "<=");
                    advance();
                } else if (matchNext('>')) {
                    addToken(NOT_EQUAL, "<>");
                    advance();
                } else {
                    addToken(LESS, "<");
                }
                break;
                
            case '>':
                if (matchNext('=')) {
                    addToken(GREATER_EQUAL, ">=");
                    advance();
                } else{
                    addToken(GREATER, ">");
                }
                break;
            case ' ':
            case '\r':
            case '\t':
                // Ignore whitespace
                break;
            case '\n': lineNumber++; break;
            case '"': scanString(); break;
                
            default:
                if (isDigit(c)) {
                    scanNumber();
                } else if (isAlpha(c)) {
                    scanIdentifier();
                } else {
                    throwError("Unexpected character.");
                }
        }
    }

    void Tokenizer::scanString() {
        std::string str;

        while (peek() != '"' && !isAtEnd()) {
            if (peek() == '\n') lineNumber ++;
            str.push_back(advance());
        }

        if (isAtEnd()) {
            throwError("Unterminated string.");
            return;
        }
        
        advance();  // Closing "

        std::string literal(str);
        addToken(STRING, '"' + str + '"', std::move(literal));
    }

    void Tokenizer::scanNumber() {
        std::string numStr;
        numStr.push_back(cur());


        while ((isDigit(peek()) || peek() == '.') && !isAtEnd()) {
            numStr.push_back(advance());
        }
        
        Literal num = stod(numStr);
        addToken(NUMBER, std::move(numStr), std::move(num));
    }
    
    const std::map<std::string, TokenType> Tokenizer::keywords = {
        {"rem", REM},
        {"let", LET},
        {"input", INPUT},
        {"print", PRINT},
        {"tonum", TONUM},
        {"tostr", TOSTR},
        {"rnd", RND},
        {"if", IF},
        {"then", THEN},
        {"else", ELSE},
        {"end", END},
        {"while", WHILE},
        {"do", DO},
        {"break", BREAK},
        {"continue", CONTINUE},
        {"not", NOT},
        {"and", AND},
        {"or", OR},
    };

    void Tokenizer::scanIdentifier() {
        std::string word;
        std::string wordLower;
        word.push_back(cur());
        wordLower.push_back(tolower(cur()));

        while ((isAlphaNum(peek())) && !isAtEnd()) {
            char c = advance();
            word.push_back(c);
            wordLower.push_back(tolower(c));
        }
        
        auto type = keywords.find(wordLower);
        if (type != keywords.end()) {
            if (type->second == REM) {
                // Ignore everything after REM
                std::string comment;
                while (advance() != '\n' && !isAtEnd()) {};  //comment.push_back(advance());
//                addToken(type->second, std::move(word), std::move(comment));
                lineNumber++;
            } else {
                addToken(type->second, std::move(word));
            }
        } else {
            if (wordLower == "true") addToken(BOOLEAN, "true", true);
            else if (wordLower == "false") addToken(BOOLEAN, "false", false);
            else addToken(IDENTIFIER, std::move(word), std::string(word));
        }
    }

    void Tokenizer::scanTokens() {
        while (!isAtEnd()) {
            scanToken();
        }

        addToken(EOF_TOKEN, "");
    }
    
    char Tokenizer::advance() {
         lastChar = curChar;
         curChar = nextChar;
        
         if (inputStream.eof() || !inputStream.get(nextChar) || nextChar == '\0') {
             isEnd = true;
             nextChar = 0;
         }
         
         return curChar;
    }
    
    char Tokenizer::peek() {
        return nextChar;
    }
    
    char Tokenizer::cur() {
        return curChar;
    }

    char Tokenizer::prev() {
        return lastChar;
    }
    
    bool Tokenizer::matchNext(char c) {
        if (isEnd) return false;
        return c == peek();
    }

    void Tokenizer::addToken(TokenType type, std::string&& lexeme) {
        tokens->emplace_back(type, std::move(lexeme), std::nullopt, lineNumber);
    }

    void Tokenizer::addToken(TokenType type, char lexeme) {
        std::string lexemeStr{lexeme};
        addToken(type, std::move(lexemeStr));
    }

    void Tokenizer::addToken(TokenType type, std::string&& lexeme, Literal &&literal) {
        tokens->emplace_back(type, std::move(lexeme), std::move(literal), lineNumber);
    }

    std::unique_ptr<std::vector<Token>> Tokenizer::getTokens() {
        return std::move(tokens);
    }

    void Tokenizer::throwError(std::string &&message) {
        errorMessage = std::move(message);
        throw TokenizationError();
    }

    std::string &Tokenizer::getErrorMessage() {
        return errorMessage;
    }

    uint32_t Tokenizer::getErrorLine() const {
        return lineNumber; 
    }

    bool Tokenizer::isAtEnd() {
        return nextChar == '\0' && isEnd;
    }

    bool Tokenizer::isDigit(char c) {
        return c >= '0' && c <= '9';
    }

    bool Tokenizer::isAlpha(char c) {
        return (c >= 'a' && c <= 'z') ||
               (c >= 'A' && c <= 'Z') ||
               c == '_';
    }

    bool Tokenizer::isAlphaNum(char c) {
        return isAlpha(c) || isDigit(c);
    }
}
 