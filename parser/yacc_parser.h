//
//  yacc_parser.h
//  Workflow
//
//  Created by Кирилл on 25.11.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#ifndef YACC_PARSER_H_
#define YACC_PARSER_H_

#include <stdio.h>

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC int yyparse(void);

EXTERNC void scanBuffer(const char* buff, size_t size);

#undef EXTERNC

#endif /* YACC_PARSER_H_ */
