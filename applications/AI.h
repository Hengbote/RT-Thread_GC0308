#ifndef APPLICATIONS_AI_H_
#define APPLICATIONS_AI_H_

#include <rtthread.h>
#include <stdio.h>
#include "main.h"

/* 模型头文件 */
#include "ai_datatypes_defines.h"
#include "ai_platform.h"
#include "network_data.h"
#include "network.h"

#include "image_recognition.h"

int AI_Init(void);
int AI_Run(const void *in_data, void *out_data);

void AI_Reponse_Callback(void *parameter);
void AI_Reponse(void);

#endif /* APPLICATIONS_AI_H_ */
