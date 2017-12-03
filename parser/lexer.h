//
//  lexer.h
//  Workflow
//
//  Created by Кирилл on 03.12.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#ifndef LEXER_H
#define LEXER_H

#if ! defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer wkfw_FlexLexer
#include <FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL wkfw::BisonWorkflowParser::symbol_type wkfw::FlexWorkflowLexer::nextLexem()

#include "parser.hpp"

namespace wkfw {
  
class FlexWorkflowLexer : public yyFlexLexer {
 public:
  FlexWorkflowLexer() {}
  wkfw::BisonWorkflowParser::symbol_type nextLexem();
};

} // namespace wkfw

#endif /* LEXER_H */
