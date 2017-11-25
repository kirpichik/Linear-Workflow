//
//  yacc_parser.h
//  Workflow
//
//  Created by Кирилл on 25.11.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#ifndef YACC_PARSER_H_
#define YACC_PARSER_H_

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC int yyparse(void);

#undef EXTERNC

#endif /* YACC_PARSER_H_ */
