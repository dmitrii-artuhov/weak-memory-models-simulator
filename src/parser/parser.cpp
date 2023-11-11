#include "parser.h"

#include <vector>
#include <memory>
#include <algorithm>
#include <stdexcept>

#include "lexer/token.h"
#include "exceptions/exceptions.h"
#include "lang/keywords.h"

#include "ast/node.h"
#include "ast/end/end.h"
#include "ast/number/number.h"
#include "ast/assignment/assignment.h"
#include "ast/binop/binop.h"
#include "ast/condition/condition.h"
#include "ast/load/load.h"
#include "ast/store/store.h"
#include "ast/fai/fai.h"
#include "ast/cas/cas.h"
#include "ast/fence/fence.h"

namespace wmm_simulator {

Parser::Parser(std::vector <Token> tokens): m_tokens(std::move(tokens)) {}

std::shared_ptr<AstNode> Parser::parse() {
    m_current_token = 0;

    auto it = std::find_if(m_tokens.begin(), m_tokens.end(), [] (const Token& token) {
        return token.is(Token::Type::INVALID);
    });
    if (it != m_tokens.end()) {
        throw exceptions::invalid_token(*it);
    }

    std::vector <std::shared_ptr<StatementNode>> statements;

    while (m_current_token < m_tokens.size()) {
        std::shared_ptr<StatementNode> statement = parse_statement();
        statements.push_back(statement);
    }

    return std::shared_ptr<AstNode> (new ProgramNode(statements));
}

const Token& Parser::peek() const {
    if (m_current_token >= m_tokens.size()) {
        throw std::out_of_range("Token index out of bound: m_current_token=" + std::to_string(m_current_token) + ", m_tokens.size()=" + std::to_string(m_tokens.size()));
    }

    return m_tokens[m_current_token];
}

void Parser::advance() {
    m_current_token++;
}

bool Parser::is_labeled_statement_next() const {
    if (m_current_token + 1 >= m_tokens.size()) {
        return false;
    }

    return (
        m_tokens[m_current_token].is(Token::Type::IDENTIFIER) &&
        m_tokens[m_current_token + 1].is(Token::Type::SEMICOLON)
    );
}

BinOpNode::Type Parser::to_binop_type(Token::Type type) {
    switch (type) {
        case Token::Type::PLUS: return BinOpNode::Type::PLUS;
        case Token::Type::MINUS: return BinOpNode::Type::MINUS;
        case Token::Type::MULT: return BinOpNode::Type::MULT;
        case Token::Type::DIV: return BinOpNode::Type::DIV;
        default: throw std::logic_error("Parser::to_binop_type called on token type: " + Token::to_readable_type(type));
    }
}

MemoryOrder Parser::to_memory_order(std::string_view lexeme) {
    if (lexeme == Keyword::SEQ_CST) {
        return MemoryOrder::SEQUENTIALLY_CONSISTENT;
    }
    else if (lexeme == Keyword::REL) {
        return MemoryOrder::RELEASE;
    }
    else if (lexeme == Keyword::ACQ) {
        return MemoryOrder::ACQUIRE;
    }
    else if (lexeme == Keyword::REL_ACQ) {
        return MemoryOrder::RELEASE_ACQUIRE;
    }
    else if (lexeme == Keyword::RLX) {
        return MemoryOrder::RELAXED;
    }

    throw std::logic_error("Parser::to_memory_order called on token lexeme: " + std::string(lexeme));
}


std::shared_ptr<StatementNode> Parser::parse_statement() {
    std::string label = "";
    std::shared_ptr<AstNode> statement;

    if (is_labeled_statement_next()) {
        label = peek().get_lexeme();
        advance(); advance(); // skip label name and semicolon
        std::cout << "Labeled: " << label << std::endl;
    }

    const Token& t = peek();

    switch (t.get_type()) {
        case Token::Type::IDENTIFIER: {
            // assignment
            std::cout << "parse assignment" << std::endl;
            statement = parse_assignment();
            break;
        }
        case Token::Type::CONDITION: {
            // condition
            std::cout << "parse condition" << std::endl;
            statement = parse_condition();
            break;
        }
        case Token::Type::LOAD: {
            // load
            std::cout << "parse load" << std::endl;
            statement = parse_double_argument_call<LoadNode, Token::Type::LOAD>();
            break;
        }
        case Token::Type::STORE: {
            // store
            std::cout << "parse store" << std::endl;
            statement = parse_double_argument_call<StoreNode, Token::Type::STORE>();
            break;
        }
        case Token::Type::FAI: {
            // fai
            std::cout << "parse fai" << std::endl;
            statement = parse_double_argument_call<FaiNode, Token::Type::FAI>();
            break;
        }
        case Token::Type::CAS: {
            // cas
            std::cout << "parse cas" << std::endl;
            statement = parse_cas_call();
            break;
        }
        case Token::Type::FENCE: {
            // fence
            std::cout << "parse fence" << std::endl;
            statement = parse_fence_call();
            break;
        }
        case Token::Type::END: {
            // end of execution
            std::cout << "parse end" << std::endl;
            statement = std::shared_ptr<AstNode>(new EndNode());
            advance();
            break;
        }
        default: throw exceptions::unexpected_token(peek(), {
            Token::Type::IDENTIFIER, Token::Type::CONDITION, Token::Type::LOAD, Token::Type::STORE, Token::Type::FAI, Token::Type::CAS, Token::Type::FENCE, Token::Type::END
        });
    }

    return std::make_shared<StatementNode>(
        statement,
        label
    );
}

std::shared_ptr<AstNode> Parser::parse_assignment() {
    // register
    if (!peek().is(Token::Type::IDENTIFIER)) {
        throw exceptions::unexpected_token(peek(), { Token::Type::IDENTIFIER });
    }

    std::string_view register_name = peek().get_lexeme();
    std::shared_ptr<AstNode> expr;

    advance();

    // equals
    if (!peek().is(Token::Type::EQ)) {
        throw exceptions::unexpected_token(peek(), { Token::Type::EQ });
    }
    advance();


    // binop | number
    const Token& t = peek();
    
    if (t.is(Token::Type::IDENTIFIER)) {
        // binop
        expr = parse_binop();
    }
    else if (t.is(Token::Type::NUMBER)) {
        // number
        int value = std::stoi(std::string(t.get_lexeme()));
        expr = std::shared_ptr<AstNode>(
            new NumberNode(value)
        );
        advance();
    }
    else {
        throw exceptions::unexpected_token(t, { Token::Type::IDENTIFIER, Token::Type::NUMBER });
    }

    return std::shared_ptr<AstNode>(new AssignmentNode(
        std::string(register_name),
        expr
    ));
}

std::shared_ptr<AstNode> Parser::parse_binop() {
    std::string_view left_register, right_register;
    BinOpNode::Type operation;

    // left register
    if (!peek().is(Token::Type::IDENTIFIER)) {
        throw exceptions::unexpected_token(peek(), { Token::Type::IDENTIFIER });
    }

    left_register = peek().get_lexeme();
    advance();

    // binary operation
    if (!peek().is_one_of(Token::Type::PLUS, Token::Type::MINUS, Token::Type::MULT, Token::Type::DIV)) {
        throw exceptions::unexpected_token(peek(), { Token::Type::PLUS, Token::Type::MINUS, Token::Type::MULT, Token::Type::DIV });
    }

    operation = Parser::to_binop_type(peek().get_type());
    advance();

    // right register
    if (!peek().is(Token::Type::IDENTIFIER)) {
        throw exceptions::unexpected_token(peek(), { Token::Type::IDENTIFIER });
    }

    right_register = peek().get_lexeme();
    advance();
    
    return std::shared_ptr<AstNode>(new BinOpNode(
        std::string(left_register),
        operation,
        std::string(right_register)
    ));
}

std::shared_ptr<AstNode> Parser::parse_condition() {
    // if
    if (!peek().is(Token::Type::CONDITION)) {
        throw exceptions::unexpected_token(peek(), { Token::Type::CONDITION });
    }
    advance();

    // register
    if (!peek().is(Token::Type::IDENTIFIER)) {
        throw exceptions::unexpected_token(peek(), { Token::Type::IDENTIFIER });
    }
    std::string_view register_name = peek().get_lexeme();
    advance();

    // goto
    if (!peek().is(Token::Type::GOTO)) {
        throw exceptions::unexpected_token(peek(), { Token::Type::GOTO });
    }
    advance();

    // label
    if (!peek().is(Token::Type::IDENTIFIER)) {
        throw exceptions::unexpected_token(peek(), { Token::Type::IDENTIFIER });
    }
    std::string_view goto_label = peek().get_lexeme();
    advance();

    return std::shared_ptr<AstNode>(
        new ConditionNode(std::string(register_name), std::string(goto_label))
    );
}

std::shared_ptr<AstNode> Parser::parse_cas_call() {
    // function
    if (!peek().is(Token::Type::CAS)) {
        throw exceptions::unexpected_token(peek(), { Token::Type::CAS });
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

    // register expected
    if (!peek().is(Token::Type::IDENTIFIER)) {
        throw exceptions::unexpected_token(peek(), { Token::Type::IDENTIFIER });
    }
    std::string_view register_expected = peek().get_lexeme();
    advance();

    // register desired
    if (!peek().is(Token::Type::IDENTIFIER)) {
        throw exceptions::unexpected_token(peek(), { Token::Type::IDENTIFIER });
    }
    std::string_view register_desired = peek().get_lexeme();
    advance();

    return std::shared_ptr<AstNode> (
        new CasNode(
            memory_order,
            std::string(location_name),
            std::string(register_expected),
            std::string(register_desired)
        )
    );
}

std::shared_ptr<AstNode> Parser::parse_fence_call() {
    // function
    if (!peek().is(Token::Type::FENCE)) {
        throw exceptions::unexpected_token(peek(), { Token::Type::FENCE });
    }
    advance();

    // memory order
    if (!peek().is(Token::Type::MEMORY_ORDER)) {
        throw exceptions::unexpected_token(peek(), { Token::Type::MEMORY_ORDER });
    }
    MemoryOrder memory_order = Parser::to_memory_order(peek().get_lexeme());
    advance();

    return std::shared_ptr<AstNode> (
        new FenceNode(
            memory_order
        )
    );
}
}