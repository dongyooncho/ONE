/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd. All Rights Reserved
 * Copyright 2019 The TensorFlow Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Relu.h"

#include <string>
#include "Util.h"
#include "GpuOperation.h"
#include "absl/strings/str_cat.h"
#include "open_cl/Precision.h"

namespace onert
{
namespace backend
{
namespace gpu_cl
{

GPUOperation CreateReLU(const OperationDef &definition, const ReLUAttributes &attr)
{
  GPUOperation op(definition);
  op.elementwise_ = true;

  std::string min_func;
  if (attr.alpha != 0.0f)
  {
    min_func = "min(in_out_value * args.alpha, (FLT)(0.0f))";
    if (definition.precision == CalculationsPrecision::F32)
    {
      op.args_.AddFloat("alpha", attr.alpha);
    }
    else
    {
#ifdef FIXME_PORTING_HALF_REQIRED
      op.args_.AddHalf("alpha", half(attr.alpha));
#endif
    }
  }
  else
  {
    min_func = "(FLT)(0.0f)";
  }
  if (attr.clip != 0.0f)
  {
    if (definition.precision == CalculationsPrecision::F32)
    {
      op.args_.AddFloat("clip", attr.clip);
    }
    else
    {
#ifdef FIXME_PORTING_HALF_REQIRED
      op.args_.AddHalf("clip", half(attr.clip));
#endif
    }
    op.code_ = absl::StrCat("in_out_value = clamp(in_out_value, " + min_func + ", args.clip);");
  }
  else
  {
    op.code_ = absl::StrCat("in_out_value = max(in_out_value, ", min_func, ");");
  }
  return op;
}

} // namespace gpu_cl
} // namespace backend
} // namespace onert
