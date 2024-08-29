#ifndef _INFERENCE_H_
#define _INFERENCE_H_

#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include "model.h"

extern tflite::MicroInterpreter *interpreter;
extern TfLiteTensor *input;
extern TfLiteTensor *output;

void f_setupInference(void);

#endif // _INFERENCE_H_