#include "inference.h"
#include "main_function.h"

tflite::MicroInterpreter *interpreter;
TfLiteTensor *input;
TfLiteTensor *output;

static const tflite::Model *model;
static tflite::MicroMutableOpResolver<11> micro_op_resolver;
static constexpr int kTensorArenaSize = 25 * 1024;
static uint8_t tensor_arena[kTensorArenaSize];

void f_setupInference(void)
{
    model = tflite::GetModel(detect_ppg_model_tflite);
    if (model->version() != TFLITE_SCHEMA_VERSION)
    {
        Serial.println("Model schema mismatch!");
        while (true)
            ;
    }

    // 1 - AddFullyConnected
    if (micro_op_resolver.AddFullyConnected() != kTfLiteOk)
    {
        Serial.println("AddFullyConnected failed!");
        return;
    }

    // 2 - AddTanh
    if (micro_op_resolver.AddTanh() != kTfLiteOk)
    {
        Serial.println("AddTanh failed!");
        return;
    }

    // 3 - AddQuantize
    if (micro_op_resolver.AddQuantize() != kTfLiteOk)
    {
        Serial.println("AddQuantize failed!");
        return;
    }

    // 4 - AddDequantize
    if (micro_op_resolver.AddDequantize() != kTfLiteOk)
    {
        Serial.println("AddDequantize failed!");
        return;
    }

    // 5 - AddRelu6
    if (micro_op_resolver.AddRelu6() != kTfLiteOk)
    {
        Serial.println("AddRelu6 failed!");
        return;
    }

    // 6 - AddReshape
    if (micro_op_resolver.AddReshape() != kTfLiteOk)
    {
        Serial.println("AddReshape failed!");
        return;
    }

    // 7 - AddConv2D
    if (micro_op_resolver.AddConv2D() != kTfLiteOk)
    {
        Serial.println("AddConv2D failed!");
        return;
    }

    // 8 - AddDepthwiseConv2D
    if (micro_op_resolver.AddDepthwiseConv2D() != kTfLiteOk)
    {
        Serial.println("AddDepthwiseConv2D failed!");
        return;
    }

    // 9 - AddMean
    if (micro_op_resolver.AddMean() != kTfLiteOk)
    {
        Serial.println("AddMean failed!");
        return;
    }

    // 10 - AddExpandDims
    if (micro_op_resolver.AddExpandDims() != kTfLiteOk)
    {
        Serial.println("AddExpandDims failed!");
        return;
    }

    // 11 - AddConcatenation
    if (micro_op_resolver.AddConcatenation() != kTfLiteOk)
    {
        Serial.println("AddConcatenation failed!");
        return;
    }

    interpreter = new tflite::MicroInterpreter(model, micro_op_resolver, tensor_arena, kTensorArenaSize);
    if (interpreter->AllocateTensors() != kTfLiteOk)
    {
        Serial.println("AllocateTensors failed!");
        while (true)
            ;
    }

    input = interpreter->input(0);
    output = interpreter->output(0);
}