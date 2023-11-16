#include "doctest.h"

#include <iostream>
#include <string>
#include <memory>
#include <algorithm>
#include <unordered_map>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "ast/node.h"
#include "visitor/visitor.h"

namespace doctest {
using namespace wmm_simulator;

// class TestVisitor final : public Visitor {
// public:
//     void visit(const AstNode*) const override {
//         std::cout << "AstNode visitor (not supported node type)" << std::endl;
//     }
//     void visit(const ProgramNode* node) const override {
//         std::cout << "ProgramNode visitor" << std::endl;
//         const auto& statements = node->get_statements();
//         for (const auto& statement : statements) {
//             statement->accept(this);
//         }
//     }
//     void visit(const StatementNode* node) const override {
//         std::cout << "StatementNode visitor" << std::endl;
//         if (!node->get_label().empty()) {
//             std::cout << "Label: " << node->get_label() << std::endl;
//         }
//     }
// };

// class TestAst {
    
// };


TEST_CASE("Parser: assignment") {
    std::string code =
        "r = 1\n"
        "goto L1\n"
        "L1: store SEQ_CST #x r\n";

    Lexer lexer(code.c_str());
    Parser parser(lexer.get_tokens());

    std::pair<
        std::shared_ptr<ProgramNode>,
        std::unordered_map<std::string_view, int>
    > ast = parser.parse();
    // TestVisitor visitor;
    // ast->accept(&visitor);
    // TODO: write tests
}

}