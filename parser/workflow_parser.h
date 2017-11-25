//
//  workflow_parser.h
//  Workflow
//
//  Created by Кирилл on 25.11.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#ifndef WORKFLOW_PARSER_H_
#define WORKFLOW_PARSER_H_

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC void pushArg(const char* arg);

EXTERNC void pushCommand(const char* cmd);

EXTERNC void pushNumber(int num);

EXTERNC void pushDescEnd(void);

EXTERNC void pushError(const char* msg);

#undef EXTERNC

#endif /* WORKFLOW_PARSER_H_ */
