# Vectorization

This sample shows how to implement matrix multiplication using various
vectorization approaches.

Note: You should not reuse this matrix library in your application. It was not
written to be useful beyond the scope of this demo. If you're looking for a
matrix library, you probably want [GLM] for graphics applications, or a linear
algebra library for compute applications.

The sample app will benchmark each implementation and display the average run
time over 1,000,000 runs. The goal of this sample is to illustrate the trade-
offs of each implementation in terms of flexibility, readability, and
performance.

Given the relatively small problem size used here (4x4 matrices and vec4s), the
best performing implementations in this sample are the ones that can best
improve over the naive implementation without large set up costs. You should not
take the results of this sample as authoritative: if performance is important to
you, you **must** benchmark your code for workloads realistic for your app.

If you're not familiar with it [Godbolt] is an invaluable tool for examining
compiler optimizer behavior. You could also use `llvm-objdump` from the command
line, but that's just Godbolt with extra steps.

## Implementations

This sample contains the following implementations. Each of their trade-offs are
discussed briefly, but as mentioned above, you should not rely on the
performance results measured here to make a decision for your app.

### Auto-vectorization

See [auto_vectorization.h] for the implementation.

This implementation is written in generic C++ and contains no explicit SIMD. The
only vectorization that will be performed is Clang's auto-vectorization. This
makes for the most portable code and readable code, but at the cost of
performance.

See https://llvm.org/docs/Vectorizers.html for Clang's docs about
auto-vectorization.

### std::simd

This isn't actually available yet. It's an experimental part of the C++ standard
and is in development in libc++, but NDK r27 happened to catch it right in the
middle of a rewrite, so it's not currently usable.

See https://en.cppreference.com/w/cpp/experimental/simd/simd.

### Clang vectors

See [clang_vector.h] for the implementation.

This implementation uses Clang's generic vector types. This code is mostly as
portable as the auto-vectorization implementation, with the only caveat being
that it is limited by the width of the vector registers for the target hardware.
To deal with problems that don't fit in the target's vector registers, you would
need to either alter the algorithm to tile the operations, or use Scalable
Vector Extensions (AKA [SVE]).

However, the benefit of the portability trade-off is that this does outperform
the auto-vectorization implementation.

See
https://clang.llvm.org/docs/LanguageExtensions.html#vectors-and-extended-vectors.

### Neon

See [neon.h] for the implementation.

This implementation uses the Arm-specific Neon vector types. The implementation
is nearly identical to the one in [clang_vector.h]. The only differences how the
vector type is specified, and the restriction to only working with matrices of 4
rows, which was done for the sake of brevity. It doesn't use the Neon intrinsics
from `arm_neon.h`. Why not? If you look at how the intrinsics in that file are
defined, all they actually do (for a little endian system, and Android does not
support big endian, so we can ignore that caveat) is use the `*` operator and
leave the correct instruction selection up to Clang.

In other words, you should probably never use this. The generated code should be
identical to code written with Clang's arch-generic vectors. These benchmarks
show nearly identical results when run on a Pixel 4A (5G), and the differences
are actually a problem with the benchmark harness itself: Neon sometimes runs
marginally faster, unless you swap the order of the benchmarks, then the Clang
Vector implementation sometimes runs marginally faster.

See
https://developer.arm.com/documentation/dht0002/a/Introducing-NEON/Developing-for-NEON/Intrinsics
for more information.

### Clang matrices

See [matrix.h] for the implementation. This is the default implementation for
`Matrix::operator*`, so unlike the others that file contains the rest of the
`Matrix` class as well.

This implementation uses Clang's built-in matrix type. This is an experimental
feature in Clang, but it has the simplest code (because some kind Clang person
wrote the hard part) and performs the best by a wide margin. There are
implementation defined limits on the size of the matrix, but within those limits
the code is as portable as the auto-vectorization implementation. The docs say
the feature is still under development and subject to change, so be wary of
using this in production, and definitely don't use these types as part of your
ABI.

See https://clang.llvm.org/docs/LanguageExtensions.html#matrix-types for more
details.

### OpenMP SIMD

See [omp_simd.h] for the implementation.

This implementation uses OpenMP's SIMD directive. For some reason this
under-performs even the auto-vectorized implementation. There are a lot of
additional specifiers that can be added to the simd directive that would maybe
improve this implementation. Patches welcome :)

See https://www.openmp.org/spec-html/5.0/openmpsu42.html for more information.

## Alternatives not shown here

There are other approaches that could be used that aren't shown here.

[SVE] scales SIMD to arbitrarily sized vectors, and the C extensions, while
making for less concise code than is needed for a constrained vector size like
we have here, handle windowing of data to fit the hardware vector size for you.
For problems like the small matrix multiply we do here, it's overkill. For
portability to a wide variety of (Arm) CPUs, it can reduce the difficulty of
writing SIMD code.

GPU acceleration is a better fit for large data sets. That approach isn't shown
here because it's substantially more code to set up the GPU for this
computation, and our data size is so small that the cost of GPU initialization
and streaming the data to the GPU is likely to make that a net-loss. If you want
to learn more about GPU compute, see https://vulkan-tutorial.com/Compute_Shader,
https://www.khronos.org/opengl/wiki/Compute_Shader, and
https://www.khronos.org/opencl/ (while OpenCL is not guaranteed to be available
for all Android devices, it is a very common OEM extension).

[auto_vectorization.h]: src/main/cpp/auto_vectorization.h
[clang_vector.h]: src/main/cpp/clang_vector.h
[GLM]: https://github.com/g-truc/glm
[Gobolt]: https://godbolt.org/
[matrix.h]: src/main/cpp/matrix.h
[neon.h]: src/main/cpp/neon.h
[omp_simd.h]: src/main/cpp/omp_simd.h
[SVE]: https://developer.arm.com/Architectures/Scalable%20Vector%20Extensions
