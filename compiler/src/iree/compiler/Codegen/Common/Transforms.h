// Copyright 2021 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef IREE_COMPILER_CODEGEN_COMMON_TRANSFORMS_H_
#define IREE_COMPILER_CODEGEN_COMMON_TRANSFORMS_H_

#include "iree-dialects/Dialect/LinalgExt/Transforms/Transforms.h"
#include "iree/compiler/Codegen/Passes.h"
#include "iree/compiler/Dialect/HAL/IR/HALOps.h"
#include "mlir/Dialect/Linalg/Transforms/Transforms.h"

namespace mlir {
namespace bufferization {
struct OneShotBufferizationOptions;
}  // namespace bufferization

namespace iree_compiler {

/// Eliminates tensor.empty ops to avoid buffer allocations.
LogicalResult eliminateEmptyTensors(
    RewriterBase &rewriter, Operation *op,
    const bufferization::OneShotBufferizationOptions &options);

/// Bufferizes the given op with One-Shot Bufferize.
LogicalResult runIREEOneShotBufferize(
    Operation *op, const bufferization::OneShotBufferizationOptions &options);

/// For a given operation within a dispatch, tile and distribute the operation
/// to workgroups as well as tile + fuse its producers. Returns the
/// generated tiled and fused ops, as well as the loops used for distribution.
struct IREETileAndFuseResult {
  SmallVector<Operation *> tiledAndFusedOps;
  SmallVector<scf::ForOp> loops;
};

FailureOr<IREETileAndFuseResult> tileAndFuseDispatchUsingSCFForOp(
    RewriterBase &rewriter, TilingInterface op,
    linalg::LinalgTilingOptions tilingOptions);

FailureOr<scf::ForOp> pipelineSharedMemoryCopy(
    RewriterBase &rewriter, scf::ForOp forOp,
    PipeliningSchedulingStrategy startegy, bool peelEpilogue, int64_t depth);

/// Populate patterns related to clean up the IR after tile and distribute to
/// workgroups.
void populateTileAndDistributeToWorkgroupsCleanupPatterns(
    RewritePatternSet &patterns, linalg::LinalgTilingOptions options);

}  // namespace iree_compiler
}  // namespace mlir

#endif  // IREE_COMPILER_CODEGEN_COMMON_TRANSFORMS_H_
