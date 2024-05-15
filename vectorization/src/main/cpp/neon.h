/*
 * Copyright (C) 2024 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#if defined(__arm__) || defined(__aarch64__)

#include <arm_neon.h>

#include "matrix.h"

namespace samples::vectorization {

/**
 * Multiplies two compatible matrices and returns the result.
 *
 * Don't do this. See the README for an explanation of why doing so would be a
 * bad idea. This is only present for illustrative purposes.
 *
 * @tparam T The type of each matrix cell.
 * @tparam M The number of rows in the left operand and the result.
 * @tparam N The number of columns in the left operand, and the rows in the
 * right operand.
 * @tparam P The number of columns in the right operand and the result.
 * @param lhs The left operand.
 * @param rhs The right operand.
 * @return The result of lhs * rhs.
 */
template <typename T, size_t M, size_t N, size_t P>
Matrix<M, P, T> MultiplyWithNeon(const Matrix<M, N, T>& lhs,
                                 const Matrix<N, P, T>& rhs)
  requires(M == 4)
{
  typedef float32x4_t Vec;
  Matrix<M, P, T> result;
  for (auto result_column_index = 0U; result_column_index < P;
       result_column_index++) {
    Vec result_column = {};
    for (auto lhs_column_index = 0U; lhs_column_index < N; lhs_column_index++) {
      auto c = lhs.column(lhs_column_index);
      Vec lhs_column = *reinterpret_cast<const Vec*>(c.data());
      result_column += lhs_column * rhs[lhs_column_index, result_column_index];
    }
    *reinterpret_cast<Vec*>(result.column(result_column_index).data()) =
        result_column;
  }
  return result;
}

}  // namespace samples::vectorization

#endif
