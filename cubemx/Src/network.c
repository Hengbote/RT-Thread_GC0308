/**
  ******************************************************************************
  * @file    network.c
  * @author  AST Embedded Analytics Research Platform
  * @date    Fri Sep  8 09:50:25 2023
  * @brief   AI Tool Automatic Code Generator for Embedded NN computing
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */


#include "network.h"
#include "network_data.h"

#include "ai_platform.h"
#include "ai_platform_interface.h"
#include "ai_math_helpers.h"

#include "core_common.h"
#include "core_convert.h"

#include "layers.h"



#undef AI_NET_OBJ_INSTANCE
#define AI_NET_OBJ_INSTANCE g_network
 
#undef AI_NETWORK_MODEL_SIGNATURE
#define AI_NETWORK_MODEL_SIGNATURE     "bafef44858ba2469967e55213ca47eb7"

#ifndef AI_TOOLS_REVISION_ID
#define AI_TOOLS_REVISION_ID     ""
#endif

#undef AI_TOOLS_DATE_TIME
#define AI_TOOLS_DATE_TIME   "Fri Sep  8 09:50:25 2023"

#undef AI_TOOLS_COMPILE_TIME
#define AI_TOOLS_COMPILE_TIME    __DATE__ " " __TIME__

#undef AI_NETWORK_N_BATCHES
#define AI_NETWORK_N_BATCHES         (1)

static ai_ptr g_network_activations_map[1] = AI_C_ARRAY_INIT;
static ai_ptr g_network_weights_map[1] = AI_C_ARRAY_INIT;



/**  Array declarations section  **********************************************/
/* Array#0 */
AI_ARRAY_OBJ_DECLARE(
  eltwise_4_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 64, AI_STATIC)
/* Array#1 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_7_weights_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 73728, AI_STATIC)
/* Array#2 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_7_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 128, AI_STATIC)
/* Array#3 */
AI_ARRAY_OBJ_DECLARE(
  eltwise_9_scale_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 128, AI_STATIC)
/* Array#4 */
AI_ARRAY_OBJ_DECLARE(
  eltwise_9_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 128, AI_STATIC)
/* Array#5 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_12_weights_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 73728, AI_STATIC)
/* Array#6 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_12_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 64, AI_STATIC)
/* Array#7 */
AI_ARRAY_OBJ_DECLARE(
  eltwise_14_scale_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 64, AI_STATIC)
/* Array#8 */
AI_ARRAY_OBJ_DECLARE(
  eltwise_14_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 64, AI_STATIC)
/* Array#9 */
AI_ARRAY_OBJ_DECLARE(
  gemm_18_weights_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 131072, AI_STATIC)
/* Array#10 */
AI_ARRAY_OBJ_DECLARE(
  gemm_18_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 256, AI_STATIC)
/* Array#11 */
AI_ARRAY_OBJ_DECLARE(
  gemm_20_weights_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 16384, AI_STATIC)
/* Array#12 */
AI_ARRAY_OBJ_DECLARE(
  gemm_20_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 64, AI_STATIC)
/* Array#13 */
AI_ARRAY_OBJ_DECLARE(
  gemm_22_weights_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 640, AI_STATIC)
/* Array#14 */
AI_ARRAY_OBJ_DECLARE(
  gemm_22_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 10, AI_STATIC)
/* Array#15 */
AI_ARRAY_OBJ_DECLARE(
  serving_default_input_10_output_array, AI_ARRAY_FORMAT_FLOAT|AI_FMT_FLAG_IS_IO,
  NULL, NULL, 1920, AI_STATIC)
/* Array#16 */
AI_ARRAY_OBJ_DECLARE(
  eltwise_0_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 1920, AI_STATIC)
/* Array#17 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_2_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 40960, AI_STATIC)
/* Array#18 */
AI_ARRAY_OBJ_DECLARE(
  nl_3_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 40960, AI_STATIC)
/* Array#19 */
AI_ARRAY_OBJ_DECLARE(
  eltwise_4_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 40960, AI_STATIC)
/* Array#20 */
AI_ARRAY_OBJ_DECLARE(
  pool_6_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 10240, AI_STATIC)
/* Array#21 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_7_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 20480, AI_STATIC)
/* Array#22 */
AI_ARRAY_OBJ_DECLARE(
  nl_8_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 20480, AI_STATIC)
/* Array#23 */
AI_ARRAY_OBJ_DECLARE(
  eltwise_9_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 20480, AI_STATIC)
/* Array#24 */
AI_ARRAY_OBJ_DECLARE(
  pool_11_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 5120, AI_STATIC)
/* Array#25 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_12_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 2560, AI_STATIC)
/* Array#26 */
AI_ARRAY_OBJ_DECLARE(
  nl_13_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 2560, AI_STATIC)
/* Array#27 */
AI_ARRAY_OBJ_DECLARE(
  eltwise_14_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 2560, AI_STATIC)
/* Array#28 */
AI_ARRAY_OBJ_DECLARE(
  pool_16_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 512, AI_STATIC)
/* Array#29 */
AI_ARRAY_OBJ_DECLARE(
  gemm_18_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 256, AI_STATIC)
/* Array#30 */
AI_ARRAY_OBJ_DECLARE(
  nl_19_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 256, AI_STATIC)
/* Array#31 */
AI_ARRAY_OBJ_DECLARE(
  gemm_20_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 64, AI_STATIC)
/* Array#32 */
AI_ARRAY_OBJ_DECLARE(
  nl_21_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 64, AI_STATIC)
/* Array#33 */
AI_ARRAY_OBJ_DECLARE(
  gemm_22_output_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 10, AI_STATIC)
/* Array#34 */
AI_ARRAY_OBJ_DECLARE(
  nl_23_output_array, AI_ARRAY_FORMAT_FLOAT|AI_FMT_FLAG_IS_IO,
  NULL, NULL, 10, AI_STATIC)
/* Array#35 */
AI_ARRAY_OBJ_DECLARE(
  eltwise_0_scale_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 3, AI_STATIC)
/* Array#36 */
AI_ARRAY_OBJ_DECLARE(
  eltwise_0_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 3, AI_STATIC)
/* Array#37 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_2_weights_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 1728, AI_STATIC)
/* Array#38 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_2_bias_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 64, AI_STATIC)
/* Array#39 */
AI_ARRAY_OBJ_DECLARE(
  eltwise_4_scale_array, AI_ARRAY_FORMAT_FLOAT,
  NULL, NULL, 64, AI_STATIC)
/**  Tensor declarations section  *********************************************/
/* Tensor #0 */
AI_TENSOR_OBJ_DECLARE(
  eltwise_4_bias, AI_STATIC,
  0, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 1, 1), AI_STRIDE_INIT(4, 4, 4, 256, 256),
  1, &eltwise_4_bias_array, NULL)

/* Tensor #1 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_7_weights, AI_STATIC,
  1, 0x0,
  AI_SHAPE_INIT(4, 64, 3, 3, 128), AI_STRIDE_INIT(4, 4, 256, 32768, 98304),
  1, &conv2d_7_weights_array, NULL)

/* Tensor #2 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_7_bias, AI_STATIC,
  2, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &conv2d_7_bias_array, NULL)

/* Tensor #3 */
AI_TENSOR_OBJ_DECLARE(
  eltwise_9_scale, AI_STATIC,
  3, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &eltwise_9_scale_array, NULL)

/* Tensor #4 */
AI_TENSOR_OBJ_DECLARE(
  eltwise_9_bias, AI_STATIC,
  4, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 1, 1), AI_STRIDE_INIT(4, 4, 4, 512, 512),
  1, &eltwise_9_bias_array, NULL)

/* Tensor #5 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_12_weights, AI_STATIC,
  5, 0x0,
  AI_SHAPE_INIT(4, 128, 3, 3, 64), AI_STRIDE_INIT(4, 4, 512, 32768, 98304),
  1, &conv2d_12_weights_array, NULL)

/* Tensor #6 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_12_bias, AI_STATIC,
  6, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 1, 1), AI_STRIDE_INIT(4, 4, 4, 256, 256),
  1, &conv2d_12_bias_array, NULL)

/* Tensor #7 */
AI_TENSOR_OBJ_DECLARE(
  eltwise_14_scale, AI_STATIC,
  7, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 1, 1), AI_STRIDE_INIT(4, 4, 4, 256, 256),
  1, &eltwise_14_scale_array, NULL)

/* Tensor #8 */
AI_TENSOR_OBJ_DECLARE(
  eltwise_14_bias, AI_STATIC,
  8, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 1, 1), AI_STRIDE_INIT(4, 4, 4, 256, 256),
  1, &eltwise_14_bias_array, NULL)

/* Tensor #9 */
AI_TENSOR_OBJ_DECLARE(
  gemm_18_weights, AI_STATIC,
  9, 0x0,
  AI_SHAPE_INIT(4, 512, 256, 1, 1), AI_STRIDE_INIT(4, 4, 2048, 524288, 524288),
  1, &gemm_18_weights_array, NULL)

/* Tensor #10 */
AI_TENSOR_OBJ_DECLARE(
  gemm_18_bias, AI_STATIC,
  10, 0x0,
  AI_SHAPE_INIT(4, 1, 256, 1, 1), AI_STRIDE_INIT(4, 4, 4, 1024, 1024),
  1, &gemm_18_bias_array, NULL)

/* Tensor #11 */
AI_TENSOR_OBJ_DECLARE(
  gemm_20_weights, AI_STATIC,
  11, 0x0,
  AI_SHAPE_INIT(4, 256, 64, 1, 1), AI_STRIDE_INIT(4, 4, 1024, 65536, 65536),
  1, &gemm_20_weights_array, NULL)

/* Tensor #12 */
AI_TENSOR_OBJ_DECLARE(
  gemm_20_bias, AI_STATIC,
  12, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 1, 1), AI_STRIDE_INIT(4, 4, 4, 256, 256),
  1, &gemm_20_bias_array, NULL)

/* Tensor #13 */
AI_TENSOR_OBJ_DECLARE(
  gemm_22_weights, AI_STATIC,
  13, 0x0,
  AI_SHAPE_INIT(4, 64, 10, 1, 1), AI_STRIDE_INIT(4, 4, 256, 2560, 2560),
  1, &gemm_22_weights_array, NULL)

/* Tensor #14 */
AI_TENSOR_OBJ_DECLARE(
  gemm_22_bias, AI_STATIC,
  14, 0x0,
  AI_SHAPE_INIT(4, 1, 10, 1, 1), AI_STRIDE_INIT(4, 4, 4, 40, 40),
  1, &gemm_22_bias_array, NULL)

/* Tensor #15 */
AI_TENSOR_OBJ_DECLARE(
  serving_default_input_10_output, AI_STATIC,
  15, 0x0,
  AI_SHAPE_INIT(4, 1, 3, 20, 32), AI_STRIDE_INIT(4, 4, 4, 12, 240),
  1, &serving_default_input_10_output_array, NULL)

/* Tensor #16 */
AI_TENSOR_OBJ_DECLARE(
  eltwise_0_output, AI_STATIC,
  16, 0x0,
  AI_SHAPE_INIT(4, 1, 3, 20, 32), AI_STRIDE_INIT(4, 4, 4, 12, 240),
  1, &eltwise_0_output_array, NULL)

/* Tensor #17 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_2_output, AI_STATIC,
  17, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 20, 32), AI_STRIDE_INIT(4, 4, 4, 256, 5120),
  1, &conv2d_2_output_array, NULL)

/* Tensor #18 */
AI_TENSOR_OBJ_DECLARE(
  nl_3_output, AI_STATIC,
  18, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 20, 32), AI_STRIDE_INIT(4, 4, 4, 256, 5120),
  1, &nl_3_output_array, NULL)

/* Tensor #19 */
AI_TENSOR_OBJ_DECLARE(
  eltwise_4_output, AI_STATIC,
  19, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 20, 32), AI_STRIDE_INIT(4, 4, 4, 256, 5120),
  1, &eltwise_4_output_array, NULL)

/* Tensor #20 */
AI_TENSOR_OBJ_DECLARE(
  pool_6_output, AI_STATIC,
  20, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 10, 16), AI_STRIDE_INIT(4, 4, 4, 256, 2560),
  1, &pool_6_output_array, NULL)

/* Tensor #21 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_7_output, AI_STATIC,
  21, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 10, 16), AI_STRIDE_INIT(4, 4, 4, 512, 5120),
  1, &conv2d_7_output_array, NULL)

/* Tensor #22 */
AI_TENSOR_OBJ_DECLARE(
  nl_8_output, AI_STATIC,
  22, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 10, 16), AI_STRIDE_INIT(4, 4, 4, 512, 5120),
  1, &nl_8_output_array, NULL)

/* Tensor #23 */
AI_TENSOR_OBJ_DECLARE(
  eltwise_9_output, AI_STATIC,
  23, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 10, 16), AI_STRIDE_INIT(4, 4, 4, 512, 5120),
  1, &eltwise_9_output_array, NULL)

/* Tensor #24 */
AI_TENSOR_OBJ_DECLARE(
  pool_11_output, AI_STATIC,
  24, 0x0,
  AI_SHAPE_INIT(4, 1, 128, 5, 8), AI_STRIDE_INIT(4, 4, 4, 512, 2560),
  1, &pool_11_output_array, NULL)

/* Tensor #25 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_12_output, AI_STATIC,
  25, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 5, 8), AI_STRIDE_INIT(4, 4, 4, 256, 1280),
  1, &conv2d_12_output_array, NULL)

/* Tensor #26 */
AI_TENSOR_OBJ_DECLARE(
  nl_13_output, AI_STATIC,
  26, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 5, 8), AI_STRIDE_INIT(4, 4, 4, 256, 1280),
  1, &nl_13_output_array, NULL)

/* Tensor #27 */
AI_TENSOR_OBJ_DECLARE(
  eltwise_14_output, AI_STATIC,
  27, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 5, 8), AI_STRIDE_INIT(4, 4, 4, 256, 1280),
  1, &eltwise_14_output_array, NULL)

/* Tensor #28 */
AI_TENSOR_OBJ_DECLARE(
  pool_16_output, AI_STATIC,
  28, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 2, 4), AI_STRIDE_INIT(4, 4, 4, 256, 512),
  1, &pool_16_output_array, NULL)

/* Tensor #29 */
AI_TENSOR_OBJ_DECLARE(
  pool_16_output0, AI_STATIC,
  29, 0x0,
  AI_SHAPE_INIT(4, 1, 512, 1, 1), AI_STRIDE_INIT(4, 4, 4, 2048, 2048),
  1, &pool_16_output_array, NULL)

/* Tensor #30 */
AI_TENSOR_OBJ_DECLARE(
  gemm_18_output, AI_STATIC,
  30, 0x0,
  AI_SHAPE_INIT(4, 1, 256, 1, 1), AI_STRIDE_INIT(4, 4, 4, 1024, 1024),
  1, &gemm_18_output_array, NULL)

/* Tensor #31 */
AI_TENSOR_OBJ_DECLARE(
  nl_19_output, AI_STATIC,
  31, 0x0,
  AI_SHAPE_INIT(4, 1, 256, 1, 1), AI_STRIDE_INIT(4, 4, 4, 1024, 1024),
  1, &nl_19_output_array, NULL)

/* Tensor #32 */
AI_TENSOR_OBJ_DECLARE(
  gemm_20_output, AI_STATIC,
  32, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 1, 1), AI_STRIDE_INIT(4, 4, 4, 256, 256),
  1, &gemm_20_output_array, NULL)

/* Tensor #33 */
AI_TENSOR_OBJ_DECLARE(
  nl_21_output, AI_STATIC,
  33, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 1, 1), AI_STRIDE_INIT(4, 4, 4, 256, 256),
  1, &nl_21_output_array, NULL)

/* Tensor #34 */
AI_TENSOR_OBJ_DECLARE(
  gemm_22_output, AI_STATIC,
  34, 0x0,
  AI_SHAPE_INIT(4, 1, 10, 1, 1), AI_STRIDE_INIT(4, 4, 4, 40, 40),
  1, &gemm_22_output_array, NULL)

/* Tensor #35 */
AI_TENSOR_OBJ_DECLARE(
  nl_23_output, AI_STATIC,
  35, 0x0,
  AI_SHAPE_INIT(4, 1, 10, 1, 1), AI_STRIDE_INIT(4, 4, 4, 40, 40),
  1, &nl_23_output_array, NULL)

/* Tensor #36 */
AI_TENSOR_OBJ_DECLARE(
  eltwise_0_scale, AI_STATIC,
  36, 0x0,
  AI_SHAPE_INIT(4, 1, 3, 1, 1), AI_STRIDE_INIT(4, 4, 4, 12, 12),
  1, &eltwise_0_scale_array, NULL)

/* Tensor #37 */
AI_TENSOR_OBJ_DECLARE(
  eltwise_0_bias, AI_STATIC,
  37, 0x0,
  AI_SHAPE_INIT(4, 1, 3, 1, 1), AI_STRIDE_INIT(4, 4, 4, 12, 12),
  1, &eltwise_0_bias_array, NULL)

/* Tensor #38 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_2_weights, AI_STATIC,
  38, 0x0,
  AI_SHAPE_INIT(4, 3, 3, 3, 64), AI_STRIDE_INIT(4, 4, 12, 768, 2304),
  1, &conv2d_2_weights_array, NULL)

/* Tensor #39 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_2_bias, AI_STATIC,
  39, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 1, 1), AI_STRIDE_INIT(4, 4, 4, 256, 256),
  1, &conv2d_2_bias_array, NULL)

/* Tensor #40 */
AI_TENSOR_OBJ_DECLARE(
  eltwise_4_scale, AI_STATIC,
  40, 0x0,
  AI_SHAPE_INIT(4, 1, 64, 1, 1), AI_STRIDE_INIT(4, 4, 4, 256, 256),
  1, &eltwise_4_scale_array, NULL)



/**  Layer declarations section  **********************************************/


AI_TENSOR_CHAIN_OBJ_DECLARE(
  nl_23_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gemm_22_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &nl_23_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  nl_23_layer, 23,
  NL_TYPE, 0x0, NULL,
  nl, forward_sm,
  &nl_23_chain,
  NULL, &nl_23_layer, AI_STATIC, 
  .nl_params = NULL, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  gemm_22_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &nl_21_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gemm_22_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &gemm_22_weights, &gemm_22_bias),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  gemm_22_layer, 22,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense,
  &gemm_22_chain,
  NULL, &nl_23_layer, AI_STATIC, 
)


AI_STATIC_CONST ai_float nl_21_nl_params_data[] = { 0.0, 0.30000001192092896, -1 };
AI_ARRAY_OBJ_DECLARE(
    nl_21_nl_params, AI_ARRAY_FORMAT_FLOAT,
    nl_21_nl_params_data, nl_21_nl_params_data, 3, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  nl_21_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gemm_20_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &nl_21_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  nl_21_layer, 21,
  NL_TYPE, 0x0, NULL,
  nl, forward_relu,
  &nl_21_chain,
  NULL, &gemm_22_layer, AI_STATIC, 
  .nl_params = &nl_21_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  gemm_20_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &nl_19_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gemm_20_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &gemm_20_weights, &gemm_20_bias),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  gemm_20_layer, 20,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense,
  &gemm_20_chain,
  NULL, &nl_21_layer, AI_STATIC, 
)


AI_STATIC_CONST ai_float nl_19_nl_params_data[] = { 0.0, 0.30000001192092896, -1 };
AI_ARRAY_OBJ_DECLARE(
    nl_19_nl_params, AI_ARRAY_FORMAT_FLOAT,
    nl_19_nl_params_data, nl_19_nl_params_data, 3, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  nl_19_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gemm_18_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &nl_19_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  nl_19_layer, 19,
  NL_TYPE, 0x0, NULL,
  nl, forward_relu,
  &nl_19_chain,
  NULL, &gemm_20_layer, AI_STATIC, 
  .nl_params = &nl_19_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  gemm_18_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &pool_16_output0),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gemm_18_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &gemm_18_weights, &gemm_18_bias),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  gemm_18_layer, 18,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense,
  &gemm_18_chain,
  NULL, &nl_19_layer, AI_STATIC, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  pool_16_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &eltwise_14_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &pool_16_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  pool_16_layer, 16,
  POOL_TYPE, 0x0, NULL,
  pool, forward_mp,
  &pool_16_chain,
  NULL, &gemm_18_layer, AI_STATIC, 
  .pool_size = AI_SHAPE_2D_INIT(2, 2), 
  .pool_stride = AI_SHAPE_2D_INIT(2, 2), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  eltwise_14_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &nl_13_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &eltwise_14_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &eltwise_14_scale, &eltwise_14_bias),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  eltwise_14_layer, 15,
  BN_TYPE, 0x0, NULL,
  bn, forward_bn,
  &eltwise_14_chain,
  NULL, &pool_16_layer, AI_STATIC, 
)


AI_STATIC_CONST ai_float nl_13_nl_params_data[] = { 0.0, 0.30000001192092896, -1 };
AI_ARRAY_OBJ_DECLARE(
    nl_13_nl_params, AI_ARRAY_FORMAT_FLOAT,
    nl_13_nl_params_data, nl_13_nl_params_data, 3, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  nl_13_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_12_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &nl_13_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  nl_13_layer, 13,
  NL_TYPE, 0x0, NULL,
  nl, forward_relu,
  &nl_13_chain,
  NULL, &eltwise_14_layer, AI_STATIC, 
  .nl_params = &nl_13_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  conv2d_12_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &pool_11_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_12_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &conv2d_12_weights, &conv2d_12_bias, NULL),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  conv2d_12_layer, 12,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_conv2d_if32of32wf32,
  &conv2d_12_chain,
  NULL, &nl_13_layer, AI_STATIC, 
  .groups = 1, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 1, 1, 1, 1), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  pool_11_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &eltwise_9_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &pool_11_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  pool_11_layer, 11,
  POOL_TYPE, 0x0, NULL,
  pool, forward_mp,
  &pool_11_chain,
  NULL, &conv2d_12_layer, AI_STATIC, 
  .pool_size = AI_SHAPE_2D_INIT(2, 2), 
  .pool_stride = AI_SHAPE_2D_INIT(2, 2), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  eltwise_9_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &nl_8_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &eltwise_9_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &eltwise_9_scale, &eltwise_9_bias),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  eltwise_9_layer, 10,
  BN_TYPE, 0x0, NULL,
  bn, forward_bn,
  &eltwise_9_chain,
  NULL, &pool_11_layer, AI_STATIC, 
)


AI_STATIC_CONST ai_float nl_8_nl_params_data[] = { 0.0, 0.30000001192092896, -1 };
AI_ARRAY_OBJ_DECLARE(
    nl_8_nl_params, AI_ARRAY_FORMAT_FLOAT,
    nl_8_nl_params_data, nl_8_nl_params_data, 3, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  nl_8_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_7_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &nl_8_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  nl_8_layer, 8,
  NL_TYPE, 0x0, NULL,
  nl, forward_relu,
  &nl_8_chain,
  NULL, &eltwise_9_layer, AI_STATIC, 
  .nl_params = &nl_8_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  conv2d_7_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &pool_6_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_7_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &conv2d_7_weights, &conv2d_7_bias, NULL),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  conv2d_7_layer, 7,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_conv2d_if32of32wf32,
  &conv2d_7_chain,
  NULL, &nl_8_layer, AI_STATIC, 
  .groups = 1, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 1, 1, 1, 1), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  pool_6_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &eltwise_4_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &pool_6_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  pool_6_layer, 6,
  POOL_TYPE, 0x0, NULL,
  pool, forward_mp,
  &pool_6_chain,
  NULL, &conv2d_7_layer, AI_STATIC, 
  .pool_size = AI_SHAPE_2D_INIT(2, 2), 
  .pool_stride = AI_SHAPE_2D_INIT(2, 2), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  eltwise_4_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &nl_3_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &eltwise_4_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &eltwise_4_scale, &eltwise_4_bias),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  eltwise_4_layer, 5,
  BN_TYPE, 0x0, NULL,
  bn, forward_bn,
  &eltwise_4_chain,
  NULL, &pool_6_layer, AI_STATIC, 
)


AI_STATIC_CONST ai_float nl_3_nl_params_data[] = { 0.0, 0.30000001192092896, -1 };
AI_ARRAY_OBJ_DECLARE(
    nl_3_nl_params, AI_ARRAY_FORMAT_FLOAT,
    nl_3_nl_params_data, nl_3_nl_params_data, 3, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  nl_3_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_2_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &nl_3_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  nl_3_layer, 3,
  NL_TYPE, 0x0, NULL,
  nl, forward_relu,
  &nl_3_chain,
  NULL, &eltwise_4_layer, AI_STATIC, 
  .nl_params = &nl_3_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  conv2d_2_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &eltwise_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_2_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &conv2d_2_weights, &conv2d_2_bias, NULL),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  conv2d_2_layer, 2,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_conv2d_if32of32wf32,
  &conv2d_2_chain,
  NULL, &nl_3_layer, AI_STATIC, 
  .groups = 1, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 1, 1, 1, 1), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  eltwise_0_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &serving_default_input_10_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &eltwise_0_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &eltwise_0_scale, &eltwise_0_bias),
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  eltwise_0_layer, 1,
  BN_TYPE, 0x0, NULL,
  bn, forward_bn,
  &eltwise_0_chain,
  NULL, &conv2d_2_layer, AI_STATIC, 
)


#if (AI_TOOLS_API_VERSION < AI_TOOLS_API_VERSION_1_5)

AI_NETWORK_OBJ_DECLARE(
  AI_NET_OBJ_INSTANCE, AI_STATIC,
  AI_BUFFER_INIT(AI_FLAG_NONE,  AI_BUFFER_FORMAT_U8,
    AI_BUFFER_SHAPE_INIT(AI_SHAPE_BCWH, 4, 1, 1193536, 1, 1),
    1193536, NULL, NULL),
  AI_BUFFER_INIT(AI_FLAG_NONE,  AI_BUFFER_FORMAT_U8,
    AI_BUFFER_SHAPE_INIT(AI_SHAPE_BCWH, 4, 1, 171520, 1, 1),
    171520, NULL, NULL),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_NETWORK_IN_NUM, &serving_default_input_10_output),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_NETWORK_OUT_NUM, &nl_23_output),
  &eltwise_0_layer, 0, NULL)

#else

AI_NETWORK_OBJ_DECLARE(
  AI_NET_OBJ_INSTANCE, AI_STATIC,
  AI_BUFFER_ARRAY_OBJ_INIT_STATIC(
  	AI_FLAG_NONE, 1,
    AI_BUFFER_INIT(AI_FLAG_NONE,  AI_BUFFER_FORMAT_U8,
      AI_BUFFER_SHAPE_INIT(AI_SHAPE_BCWH, 4, 1, 1193536, 1, 1),
      1193536, NULL, NULL)
  ),
  AI_BUFFER_ARRAY_OBJ_INIT_STATIC(
  	AI_FLAG_NONE, 1,
    AI_BUFFER_INIT(AI_FLAG_NONE,  AI_BUFFER_FORMAT_U8,
      AI_BUFFER_SHAPE_INIT(AI_SHAPE_BCWH, 4, 1, 171520, 1, 1),
      171520, NULL, NULL)
  ),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_NETWORK_IN_NUM, &serving_default_input_10_output),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_NETWORK_OUT_NUM, &nl_23_output),
  &eltwise_0_layer, 0, NULL)

#endif	/*(AI_TOOLS_API_VERSION < AI_TOOLS_API_VERSION_1_5)*/


/******************************************************************************/
AI_DECLARE_STATIC
ai_bool network_configure_activations(
  ai_network* net_ctx, const ai_network_params* params)
{
  AI_ASSERT(net_ctx)

  if (ai_platform_get_activations_map(g_network_activations_map, 1, params)) {
    /* Updating activations (byte) offsets */
    
    serving_default_input_10_output_array.data = AI_PTR(g_network_activations_map[0] + 0);
    serving_default_input_10_output_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    
    eltwise_0_output_array.data = AI_PTR(g_network_activations_map[0] + 0);
    eltwise_0_output_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    
    conv2d_2_output_array.data = AI_PTR(g_network_activations_map[0] + 7680);
    conv2d_2_output_array.data_start = AI_PTR(g_network_activations_map[0] + 7680);
    
    nl_3_output_array.data = AI_PTR(g_network_activations_map[0] + 7680);
    nl_3_output_array.data_start = AI_PTR(g_network_activations_map[0] + 7680);
    
    eltwise_4_output_array.data = AI_PTR(g_network_activations_map[0] + 7680);
    eltwise_4_output_array.data_start = AI_PTR(g_network_activations_map[0] + 7680);
    
    pool_6_output_array.data = AI_PTR(g_network_activations_map[0] + 7680);
    pool_6_output_array.data_start = AI_PTR(g_network_activations_map[0] + 7680);
    
    conv2d_7_output_array.data = AI_PTR(g_network_activations_map[0] + 48640);
    conv2d_7_output_array.data_start = AI_PTR(g_network_activations_map[0] + 48640);
    
    nl_8_output_array.data = AI_PTR(g_network_activations_map[0] + 48640);
    nl_8_output_array.data_start = AI_PTR(g_network_activations_map[0] + 48640);
    
    eltwise_9_output_array.data = AI_PTR(g_network_activations_map[0] + 48640);
    eltwise_9_output_array.data_start = AI_PTR(g_network_activations_map[0] + 48640);
    
    pool_11_output_array.data = AI_PTR(g_network_activations_map[0] + 0);
    pool_11_output_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    
    conv2d_12_output_array.data = AI_PTR(g_network_activations_map[0] + 20480);
    conv2d_12_output_array.data_start = AI_PTR(g_network_activations_map[0] + 20480);
    
    nl_13_output_array.data = AI_PTR(g_network_activations_map[0] + 0);
    nl_13_output_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    
    eltwise_14_output_array.data = AI_PTR(g_network_activations_map[0] + 10240);
    eltwise_14_output_array.data_start = AI_PTR(g_network_activations_map[0] + 10240);
    
    pool_16_output_array.data = AI_PTR(g_network_activations_map[0] + 0);
    pool_16_output_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    
    gemm_18_output_array.data = AI_PTR(g_network_activations_map[0] + 2048);
    gemm_18_output_array.data_start = AI_PTR(g_network_activations_map[0] + 2048);
    
    nl_19_output_array.data = AI_PTR(g_network_activations_map[0] + 0);
    nl_19_output_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    
    gemm_20_output_array.data = AI_PTR(g_network_activations_map[0] + 1024);
    gemm_20_output_array.data_start = AI_PTR(g_network_activations_map[0] + 1024);
    
    nl_21_output_array.data = AI_PTR(g_network_activations_map[0] + 0);
    nl_21_output_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    
    gemm_22_output_array.data = AI_PTR(g_network_activations_map[0] + 256);
    gemm_22_output_array.data_start = AI_PTR(g_network_activations_map[0] + 256);
    
    nl_23_output_array.data = AI_PTR(g_network_activations_map[0] + 0);
    nl_23_output_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    
    return true;
  }
  AI_ERROR_TRAP(net_ctx, INIT_FAILED, NETWORK_ACTIVATIONS);
  return false;
}



/******************************************************************************/
AI_DECLARE_STATIC
ai_bool network_configure_weights(
  ai_network* net_ctx, const ai_network_params* params)
{
  AI_ASSERT(net_ctx)

  if (ai_platform_get_weights_map(g_network_weights_map, 1, params)) {
    /* Updating weights (byte) offsets */
    
    eltwise_4_bias_array.format |= AI_FMT_FLAG_CONST;
    eltwise_4_bias_array.data = AI_PTR(g_network_weights_map[0] + 0);
    eltwise_4_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 0);
    
    conv2d_7_weights_array.format |= AI_FMT_FLAG_CONST;
    conv2d_7_weights_array.data = AI_PTR(g_network_weights_map[0] + 256);
    conv2d_7_weights_array.data_start = AI_PTR(g_network_weights_map[0] + 256);
    
    conv2d_7_bias_array.format |= AI_FMT_FLAG_CONST;
    conv2d_7_bias_array.data = AI_PTR(g_network_weights_map[0] + 295168);
    conv2d_7_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 295168);
    
    eltwise_9_scale_array.format |= AI_FMT_FLAG_CONST;
    eltwise_9_scale_array.data = AI_PTR(g_network_weights_map[0] + 295680);
    eltwise_9_scale_array.data_start = AI_PTR(g_network_weights_map[0] + 295680);
    
    eltwise_9_bias_array.format |= AI_FMT_FLAG_CONST;
    eltwise_9_bias_array.data = AI_PTR(g_network_weights_map[0] + 296192);
    eltwise_9_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 296192);
    
    conv2d_12_weights_array.format |= AI_FMT_FLAG_CONST;
    conv2d_12_weights_array.data = AI_PTR(g_network_weights_map[0] + 296704);
    conv2d_12_weights_array.data_start = AI_PTR(g_network_weights_map[0] + 296704);
    
    conv2d_12_bias_array.format |= AI_FMT_FLAG_CONST;
    conv2d_12_bias_array.data = AI_PTR(g_network_weights_map[0] + 591616);
    conv2d_12_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 591616);
    
    eltwise_14_scale_array.format |= AI_FMT_FLAG_CONST;
    eltwise_14_scale_array.data = AI_PTR(g_network_weights_map[0] + 591872);
    eltwise_14_scale_array.data_start = AI_PTR(g_network_weights_map[0] + 591872);
    
    eltwise_14_bias_array.format |= AI_FMT_FLAG_CONST;
    eltwise_14_bias_array.data = AI_PTR(g_network_weights_map[0] + 592128);
    eltwise_14_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 592128);
    
    gemm_18_weights_array.format |= AI_FMT_FLAG_CONST;
    gemm_18_weights_array.data = AI_PTR(g_network_weights_map[0] + 592384);
    gemm_18_weights_array.data_start = AI_PTR(g_network_weights_map[0] + 592384);
    
    gemm_18_bias_array.format |= AI_FMT_FLAG_CONST;
    gemm_18_bias_array.data = AI_PTR(g_network_weights_map[0] + 1116672);
    gemm_18_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 1116672);
    
    gemm_20_weights_array.format |= AI_FMT_FLAG_CONST;
    gemm_20_weights_array.data = AI_PTR(g_network_weights_map[0] + 1117696);
    gemm_20_weights_array.data_start = AI_PTR(g_network_weights_map[0] + 1117696);
    
    gemm_20_bias_array.format |= AI_FMT_FLAG_CONST;
    gemm_20_bias_array.data = AI_PTR(g_network_weights_map[0] + 1183232);
    gemm_20_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 1183232);
    
    gemm_22_weights_array.format |= AI_FMT_FLAG_CONST;
    gemm_22_weights_array.data = AI_PTR(g_network_weights_map[0] + 1183488);
    gemm_22_weights_array.data_start = AI_PTR(g_network_weights_map[0] + 1183488);
    
    gemm_22_bias_array.format |= AI_FMT_FLAG_CONST;
    gemm_22_bias_array.data = AI_PTR(g_network_weights_map[0] + 1186048);
    gemm_22_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 1186048);
    
    eltwise_0_scale_array.format |= AI_FMT_FLAG_CONST;
    eltwise_0_scale_array.data = AI_PTR(g_network_weights_map[0] + 1186088);
    eltwise_0_scale_array.data_start = AI_PTR(g_network_weights_map[0] + 1186088);
    
    eltwise_0_bias_array.format |= AI_FMT_FLAG_CONST;
    eltwise_0_bias_array.data = AI_PTR(g_network_weights_map[0] + 1186100);
    eltwise_0_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 1186100);
    
    conv2d_2_weights_array.format |= AI_FMT_FLAG_CONST;
    conv2d_2_weights_array.data = AI_PTR(g_network_weights_map[0] + 1186112);
    conv2d_2_weights_array.data_start = AI_PTR(g_network_weights_map[0] + 1186112);
    
    conv2d_2_bias_array.format |= AI_FMT_FLAG_CONST;
    conv2d_2_bias_array.data = AI_PTR(g_network_weights_map[0] + 1193024);
    conv2d_2_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 1193024);
    
    eltwise_4_scale_array.format |= AI_FMT_FLAG_CONST;
    eltwise_4_scale_array.data = AI_PTR(g_network_weights_map[0] + 1193280);
    eltwise_4_scale_array.data_start = AI_PTR(g_network_weights_map[0] + 1193280);
    
    return true;
  }
  AI_ERROR_TRAP(net_ctx, INIT_FAILED, NETWORK_WEIGHTS);
  return false;
}


/**  PUBLIC APIs SECTION  *****************************************************/


AI_DEPRECATED
AI_API_ENTRY
ai_bool ai_network_get_info(
  ai_handle network, ai_network_report* report)
{
  ai_network* net_ctx = AI_NETWORK_ACQUIRE_CTX(network);

  if (report && net_ctx)
  {
    ai_network_report r = {
      .model_name        = AI_NETWORK_MODEL_NAME,
      .model_signature   = AI_NETWORK_MODEL_SIGNATURE,
      .model_datetime    = AI_TOOLS_DATE_TIME,
      
      .compile_datetime  = AI_TOOLS_COMPILE_TIME,
      
      .runtime_revision  = ai_platform_runtime_get_revision(),
      .runtime_version   = ai_platform_runtime_get_version(),

      .tool_revision     = AI_TOOLS_REVISION_ID,
      .tool_version      = {AI_TOOLS_VERSION_MAJOR, AI_TOOLS_VERSION_MINOR,
                            AI_TOOLS_VERSION_MICRO, 0x0},
      .tool_api_version  = AI_STRUCT_INIT,

      .api_version            = ai_platform_api_get_version(),
      .interface_api_version  = ai_platform_interface_api_get_version(),
      
      .n_macc            = 16388640,
      .n_inputs          = 0,
      .inputs            = NULL,
      .n_outputs         = 0,
      .outputs           = NULL,
      .params            = AI_STRUCT_INIT,
      .activations       = AI_STRUCT_INIT,
      .n_nodes           = 0,
      .signature         = 0x0,
    };

    if (!ai_platform_api_get_network_report(network, &r)) return false;

    *report = r;
    return true;
  }
  return false;
}


AI_API_ENTRY
ai_bool ai_network_get_report(
  ai_handle network, ai_network_report* report)
{
  ai_network* net_ctx = AI_NETWORK_ACQUIRE_CTX(network);

  if (report && net_ctx)
  {
    ai_network_report r = {
      .model_name        = AI_NETWORK_MODEL_NAME,
      .model_signature   = AI_NETWORK_MODEL_SIGNATURE,
      .model_datetime    = AI_TOOLS_DATE_TIME,
      
      .compile_datetime  = AI_TOOLS_COMPILE_TIME,
      
      .runtime_revision  = ai_platform_runtime_get_revision(),
      .runtime_version   = ai_platform_runtime_get_version(),

      .tool_revision     = AI_TOOLS_REVISION_ID,
      .tool_version      = {AI_TOOLS_VERSION_MAJOR, AI_TOOLS_VERSION_MINOR,
                            AI_TOOLS_VERSION_MICRO, 0x0},
      .tool_api_version  = AI_STRUCT_INIT,

      .api_version            = ai_platform_api_get_version(),
      .interface_api_version  = ai_platform_interface_api_get_version(),
      
      .n_macc            = 16388640,
      .n_inputs          = 0,
      .inputs            = NULL,
      .n_outputs         = 0,
      .outputs           = NULL,
      .map_signature     = AI_MAGIC_SIGNATURE,
      .map_weights       = AI_STRUCT_INIT,
      .map_activations   = AI_STRUCT_INIT,
      .n_nodes           = 0,
      .signature         = 0x0,
    };

    if (!ai_platform_api_get_network_report(network, &r)) return false;

    *report = r;
    return true;
  }
  return false;
}

AI_API_ENTRY
ai_error ai_network_get_error(ai_handle network)
{
  return ai_platform_network_get_error(network);
}

AI_API_ENTRY
ai_error ai_network_create(
  ai_handle* network, const ai_buffer* network_config)
{
  return ai_platform_network_create(
    network, network_config, 
    &AI_NET_OBJ_INSTANCE,
    AI_TOOLS_API_VERSION_MAJOR, AI_TOOLS_API_VERSION_MINOR, AI_TOOLS_API_VERSION_MICRO);
}

AI_API_ENTRY
ai_error ai_network_create_and_init(
  ai_handle* network, const ai_handle activations[], const ai_handle weights[])
{
    ai_error err;
    ai_network_params params;

    err = ai_network_create(network, AI_NETWORK_DATA_CONFIG);
    if (err.type != AI_ERROR_NONE)
        return err;
    if (ai_network_data_params_get(&params) != true) {
        err = ai_network_get_error(*network);
        return err;
    }
#if defined(AI_NETWORK_DATA_ACTIVATIONS_COUNT)
    if (activations) {
        /* set the addresses of the activations buffers */
        for (int idx=0;idx<params.map_activations.size;idx++)
            AI_BUFFER_ARRAY_ITEM_SET_ADDRESS(&params.map_activations, idx, activations[idx]);
    }
#endif
#if defined(AI_NETWORK_DATA_WEIGHTS_COUNT)
    if (weights) {
        /* set the addresses of the weight buffers */
        for (int idx=0;idx<params.map_weights.size;idx++)
            AI_BUFFER_ARRAY_ITEM_SET_ADDRESS(&params.map_weights, idx, weights[idx]);
    }
#endif
    if (ai_network_init(*network, &params) != true) {
        err = ai_network_get_error(*network);
    }
    return err;
}

AI_API_ENTRY
ai_buffer* ai_network_inputs_get(ai_handle network, ai_u16 *n_buffer)
{
  if (network == AI_HANDLE_NULL) {
    network = (ai_handle)&AI_NET_OBJ_INSTANCE;
    ((ai_network *)network)->magic = AI_MAGIC_CONTEXT_TOKEN;
  }
  return ai_platform_inputs_get(network, n_buffer);
}

AI_API_ENTRY
ai_buffer* ai_network_outputs_get(ai_handle network, ai_u16 *n_buffer)
{
  if (network == AI_HANDLE_NULL) {
    network = (ai_handle)&AI_NET_OBJ_INSTANCE;
    ((ai_network *)network)->magic = AI_MAGIC_CONTEXT_TOKEN;
  }
  return ai_platform_outputs_get(network, n_buffer);
}

AI_API_ENTRY
ai_handle ai_network_destroy(ai_handle network)
{
  return ai_platform_network_destroy(network);
}

AI_API_ENTRY
ai_bool ai_network_init(
  ai_handle network, const ai_network_params* params)
{
  ai_network* net_ctx = ai_platform_network_init(network, params);
  if (!net_ctx) return false;

  ai_bool ok = true;
  ok &= network_configure_weights(net_ctx, params);
  ok &= network_configure_activations(net_ctx, params);

  ok &= ai_platform_network_post_init(network);

  return ok;
}


AI_API_ENTRY
ai_i32 ai_network_run(
  ai_handle network, const ai_buffer* input, ai_buffer* output)
{
  return ai_platform_network_process(network, input, output);
}

AI_API_ENTRY
ai_i32 ai_network_forward(ai_handle network, const ai_buffer* input)
{
  return ai_platform_network_process(network, input, NULL);
}



#undef AI_NETWORK_MODEL_SIGNATURE
#undef AI_NET_OBJ_INSTANCE
#undef AI_TOOLS_DATE_TIME
#undef AI_TOOLS_COMPILE_TIME

