// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "core/providers/cuda/cuda_common.h"
#include "contrib_ops/cpu/bert/embed_layer_norm_helper.h"
#include "embed_layer_norm.h"
#include "embed_layer_norm_impl.h"
#include "core/providers/cuda/tensor/scatter_nd_impl.h"

namespace onnxruntime {
namespace contrib {
namespace cuda {

#define REGISTER_KERNEL_TYPED(T)                                  \
  ONNX_OPERATOR_TYPED_KERNEL_EX(                                  \
      EmbedLayerNormalization,                                    \
      kMSDomain,                                                  \
      1,                                                          \
      T,                                                          \
      kCudaExecutionProvider,                                     \
      (*KernelDefBuilder::Create())                               \
          .TypeConstraint("T", DataTypeImpl::GetTensorType<T>()), \
      EmbedLayerNorm<T>);

REGISTER_KERNEL_TYPED(float)
REGISTER_KERNEL_TYPED(MLFloat16)

using namespace ONNX_NAMESPACE;

template <typename T>
EmbedLayerNorm<T>::EmbedLayerNorm(const OpKernelInfo& op_kernel_info) : CudaKernel(op_kernel_info) {
  ORT_ENFORCE(op_kernel_info.GetAttr<float>("epsilon", &epsilon_).IsOK());
  ORT_ENFORCE(epsilon_ >= 0);
}

template <typename T>
Status EmbedLayerNorm<T>::ComputeInternal(OpKernelContext* context) const {
  ORT_RETURN_IF_ERROR(embed_layer_norm::CheckInputs(context));

  const Tensor* input_ids = context->Input<Tensor>(0);
  const Tensor* segment_ids = context->Input<Tensor>(1);  // optional. nullptr if it's distill-bert
  const Tensor* word_embedding = context->Input<Tensor>(2);
  const Tensor* position_embedding = context->Input<Tensor>(3);
  const Tensor* segment_embedding = context->Input<Tensor>(4);  // optional. nullptr if it's distill-bert
  const Tensor* gamma = context->Input<Tensor>(5);
  const Tensor* beta = context->Input<Tensor>(6);
  const Tensor* mask = context->Input<Tensor>(7);          // optional. nullptr if not provided
  const Tensor* position_ids = context->Input<Tensor>(8);  // optional. nullptr if not provided

  const auto& input_dims = input_ids->Shape().GetDims();
  int64_t hidden_size = word_embedding->Shape()[1];

  TensorShape output_shape({input_dims[0], input_dims[1], hidden_size});
  Tensor* output = context->Output(0, output_shape);

  TensorShape mask_index_shape({input_dims[0]});
  Tensor* mask_index = context->Output(1, mask_index_shape);

  Tensor* embedding_sum = context->Output(2, output_shape);

  int batch_size = static_cast<int>(input_dims[0]);
  int sequence_length = static_cast<int>(input_dims[1]);
  size_t element_size = sizeof(T);

  auto status = LaunchEmbedLayerNormKernel(
      Stream(),
      output->MutableData<T>(),
      mask_index->MutableData<int32_t>(),
      input_ids->Data<int32_t>(),
      nullptr == segment_ids ? nullptr : segment_ids->Data<int32_t>(),
      nullptr == mask ? nullptr : mask->Data<int32_t>(),
      gamma->Data<T>(),
      beta->Data<T>(),
      word_embedding->Data<T>(),
      position_embedding->Data<T>(),
      nullptr == segment_embedding ? nullptr : segment_embedding->Data<T>(),
      epsilon_,
      static_cast<int>(hidden_size),
      batch_size,
      sequence_length,
      element_size,
      embedding_sum == nullptr ? nullptr : embedding_sum->MutableData<T>(),
      position_ids == nullptr ? nullptr : position_ids->Data<int32_t>());

  ORT_IGNORE_RETURN_VALUE(status);

  if (should_randomize_ && random_data_ == nullptr) {
    std::cout << "Generating random input" << std::endl;
    cudaMalloc(&random_data_, output->SizeInBytes());
  }

  if (should_randomize_) {
    std::vector<uint16_t> host_data(output->SizeInBytes(), 0);
    cudaMemcpyAsync(host_data.data(), random_data_, output->SizeInBytes(), cudaMemcpyDeviceToHost);

    for (size_t i = 0; i < static_cast<size_t>(output->Shape().Size()); ++i) {
      if (host_data[i] != 0 && ((host_data[i] & 0x7C00) == 0)) {
        ORT_THROW("Sub-normal found 3: ", host_data[i]);
      }
    }

    //cudaMemcpyAsync(output->MutableDataRaw(), random_data_, output->SizeInBytes(), cudaMemcpyDeviceToDevice);
  }

  return Status::OK();
}

template <typename T>
EmbedLayerNorm<T>::~EmbedLayerNorm() {
  if (random_data_ != nullptr) {
    cudaFree(random_data_);
  }
}

}  // namespace cuda
}  // namespace contrib
}  // namespace onnxruntime
