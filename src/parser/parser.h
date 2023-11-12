#pragma once

#include <vector>
#include <memory>

#include "lexer/token.h"
#include "exceptions/exceptions.h"
#include "lang/memory-order.h"

#include "ast/node.h"
#include "ast/program/program.h"
#include "ast/binop/binop.h"

namespace wmm_simulator {

class Parser {
public:
    Parser(std::vector<Token> tokens);
    std::shared_ptr<AstNode> parse();

private:
    std::vector <Token> m_tokens;
    size_t m_current_token = 0;

    const Token& peek() const;
    void advance();
    bool is_labeled_statement_next() const;
    static BinOpNode::Type to_binop_type(Token::Type type);
    static MemoryOrder to_memory_order(std::string_view lexeme);

    std::shared_ptr<StatementNode> parse_statement();
    std::shared_ptr<AstNode> parse_assignment();
    std::shared_ptr<AstNode> parse_binop();
    std::shared_ptr<AstNode> parse_condition();
    std::shared_ptr<AstNode> parse_goto();
    std::shared_ptr<AstNode> parse_thread_goto();
    template<class NodeType, Token::Type token_type>
    std::shared_ptr<AstNode> parse_double_argument_call() {
        // function
        if (!peek().is(token_type)) {
            throw exceptions::unexpected_token(peek(), { token_type });
        }
        advance();

        // memory order
        if (!peek().is(Token::Type::MEMORY_ORDER)) {
            throw exceptions::unexpected_token(peek(), { Token::Type::MEMORY_ORDER });
        }
        MemoryOrder memory_order = Parser::to_memory_order(peek().get_lexeme());
        advance();

        // location
        if (!peek().is(Token::Type::LOCATION)) {
            throw exceptions::unexpected_token(peek(), { Token::Type::LOCATION });
        }
        std::string_view location_name = peek().get_lexeme();
        advance();

        // register
        if (!peek().is(Token::Type::IDENTIFIER)) {
            throw exceptions::unexpected_token(peek(), { Token::Type::IDENTIFIER });
        }
        std::string_view register_name = peek().get_lexeme();
        advance();

        return std::shared_ptr<AstNode> (
            new NodeType(
                memory_order,
                std::string(location_name),
                std::string(register_name)
            )
        );
    }
    std::shared_ptr<AstNode> parse_cas_call();
    std::shared_ptr<AstNode> parse_fence_call();
};

}