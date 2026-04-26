# AI 的加速技術

深度學習的興起帶來了巨大的計算需求。從 GPT-4 的數萬 GPU 小時訓練到即時推理，硬體加速成為 AI 發展的關鍵推手。本章探討 AI 加速的硬體架構、軟體優化和系統設計。

## AI 計算的特點

### 運算類型

神經網路的運算以矩陣乘法為主：

```
全連接層：  y = Wx + b
           ┌───┐   ┌───┐
     x ──→ │ W │──→│ + │──→ y
           └───┘   └───┘
卷積層：    大量小矩陣乘法（im2col）
注意力：    Q、K、V 矩陣乘法
```

### 運算特性

- **密集運算**：大量矩陣乘法
- **高記憶體頻寬需求**：權重、反向傳播
- **低精度友好**：fp16、int8、int4 足夠
- **並行度高**：適合 SIMD/SIMT

## CPU 加速：向量化和 SIMD

### AVX-512

Intel AVX-512 提供 512 位元向量：

```c
#include <immintrin.h>

void matmul_avx512(float *C, const float *A, const float *B, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j += 16) {  // 16 floats = 512 bits
            __m512 c_reg = _mm512_setzero_ps();
            
            for (int k = 0; k < N; k++) {
                __m512 a_reg = _mm512_set1_ps(A[i * N + k]);
                __m512 b_reg = _mm512_loadu_ps(&B[k * N + j]);
                c_reg = _mm512_fmadd_ps(a_reg, b_reg, c_reg);
            }
            
            _mm512_storeu_ps(&C[i * N + j], c_reg);
        }
    }
}
```

### ARM NEON

行動裝置上的 SIMD 擴展：

```c
#include <arm_neon.h>

void dot_product_neon(float *result, float *a, float *b, int n) {
    float32x4_t sum = vdupq_n_f32(0);
    
    int i = 0;
    for (; i + 4 <= n; i += 4) {
        float32x4_t va = vld1q_f32(&a[i]);
        float32x4_t vb = vld1q_f32(&b[i]);
        sum = vfmaq_f32(sum, va, vb);
    }
    
    *result = vgetq_lane_f32(sum, 0) + vgetq_lane_f32(sum, 1) +
              vgetq_lane_f32(sum, 2) + vgetq_lane_f32(sum, 3);
}
```

## GPU 加速：CUDA 深度學習

### cuDNN

NVIDIA 針對深度學習的優化庫：

```cuda
#include <cudnn.h>

void conv2d_cudnn(cudnnHandle_t handle, float *input, float *output,
                  float *kernel, int N, int C, int H, int W,
                  int K, int R, int S) {
    cudnnTensorDescriptor_t input_desc, output_desc;
    cudnnFilterDescriptor_t kernel_desc;
    cudnnConvolutionDescriptor_t conv_desc;
    
    cudnnCreateTensorDescriptor(&input_desc);
    cudnnSetTensor4dDescriptor(input_desc, CUDNN_TENSOR_NCHW,
                                CUDNN_DATA_FLOAT, N, C, H, W);
    
    cudnnCreateConvolutionDescriptor(&conv_desc);
    cudnnSetConvolution2dDescriptor(conv_desc, R/2, S/2,
                                     R, S, 1, 1, CUDNN_CROSS_CORRELATION,
                                     CUDNN_DATA_FLOAT);
    
    cudnnConvolutionForward(handle,
                           &alpha, input_desc, input,
                           kernel_desc, kernel,
                           conv_desc, CUDNN_CONVOLUTION_FWD_SPECIFY_WORKSPACE,
                           workspace, workspace_size,
                           &beta, output_desc, output);
}
```

### cuBLAS

BLAS 矩陣運算庫：

```cuda
#include <cublas_v2.h>

void gemm_cublas(cublasHandle_t handle, float *C, float *A, float *B,
                 int m, int n, int k, float alpha, float beta) {
    const float *alpha_ptr = &alpha;
    const float *beta_ptr = &beta;
    
    cublasSgemm(handle,
                CUBLAS_OP_N, CUBLAS_OP_N,  // 不轉置
                n, m, k,                    // C(m,n) = A(m,k) * B(k,n)
                alpha_ptr,
                B, n,                       // B(k,n)
                A, k,                       // A(m,k)
                beta_ptr,
                C, n);                      // C(m,n)
}
```

### PyTorch CUDA

Python 介面：

```python
import torch

# 使用 GPU
device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

# 模型移至 GPU
model = MyModel().to(device)

# 資料移至 GPU
x = torch.randn(32, 3, 224, 224).to(device)
y = model(x)

# 混合精度訓練
with torch.cuda.amp.autocast():
    output = model(input)
    loss = criterion(output, target)
    scaler.scale(loss).backward()
```

## TPU：張量處理器

Google TPU 是專為深度學習設計的 ASIC：

### TPU 架構

```
┌─────────────────────────────────────────┐
│          TPU Chip                        │
├─────────────────────────────────────────┤
│  ┌───────────────────────────────────┐ │
│  │      Matrix Multiply Unit         │ │
│  │   (128x128 脈動陣列)              │ │
│  │   峰值: 92 TFLOPs/s (int8)       │ │
│  └───────────────────────────────────┘ │
│                                         │
│  ┌─────────┐ ┌─────────┐ ┌─────────┐  │
│  │ Accumul │ │ Accumul │ │ Accumul │  │
│  │  (32MB) │ │  (32MB) │ │  (32MB) │  │
│  └─────────┘ └─────────┘ └─────────┘  │
│                                         │
│  ┌───────────────────────────────────┐ │
│  │      Unified Buffer (8MB)         │ │
│  └───────────────────────────────────┘ │
└─────────────────────────────────────────┘
```

### TPU 程式設計（TFLite）

```python
import tensorflow as tf

# 轉換模型為 TFLite 格式
converter = tf.lite.TFLiteConverter.from_saved_model(saved_model_dir)
converter.optimizations = [tf.lite.Optimize.DEFAULT]

# 量化為 int8
converter.representative_dataset = representative_data_gen
converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]

tflite_model = converter.convert()

# 儲存模型
with open('model.tflite', 'wb') as f:
    f.write(tflite_model)

# 在 Android 上執行
interpreter = tf.lite.Interpreter(model_path='model.tflite')
interpreter.allocate_tensors()

input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

interpreter.set_tensor(input_details[0]['index'], input_data)
interpreter.invoke()
result = interpreter.get_tensor(output_details[0]['index'])
```

## FPGA 加速

FPGA 提供硬體級的自定義：

### Vitis AI

Xilinx 的深度學習加速框架：

```c++
#include <vart/tensor_buffer.hpp>
#include <vart/runner.hpp>

// 載入量化模型
auto runner = vart::Runner::create_runner("resnet50.xmodel");

// 準備輸入
auto input_tensor = runner->get_input_tensors()[0];
auto input_buffer = vart::alloc_tensor_buffer(input_tensor);
auto input_map = runner->get_inputs();
input_map[0] = input_buffer;

// 執行推論
runner->execute(input_map, runner->get_outputs());

// 讀取輸出
auto output_map = runner->get_outputs();
auto output_buffer = output_map[0];
```

### HLS 高層次合成

用 C++ 直接生成硬體：

```cpp
#include <hls_stream.h>
#include <ap_fixed.h>

// 定點數類型
typedef ap_fixed<16, 4> fixed_t;

void conv_layer(hls::stream<fixed_t> &input,
                hls::stream<fixed_t> &output,
                const fixed_t weight[3][3][64][64]) {
    
    fixed_t line_buffer[3][1920];
    
    for (int row = 0; row < 1080; row++) {
        for (int col = 0; col < 1920; col++) {
            fixed_t mac = 0;
            
            // 3x3 卷積
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    mac += line_buffer[2-i][col+j] * weight[i][j][ch][ch];
                }
            }
            
            output << mac;
        }
    }
}
```

## 模型優化技術

### 量化（Quantization）

降低權重和 activation 的精度：

```python
import torch.quantization

# 動態量化（簡單）
model_int8 = torch.quantization.quantize_dynamic(
    model, {torch.nn.Linear}, dtype=torch.qint8
)

# 靜態量化（需要校正）
model.qconfig = torch.quantization.get_default_qconfig('fbgemm')
torch.quantization.prepare(model, inplace=True)
# 校正資料通過模型
torch.quantization.convert(model, inplace=True)
```

### 剪枝（Pruning）

移除不重要的連接：

```python
import torch.nn.utils.prune as prune

# 結構化剪枝（移除整個神經元）
prune.l1_unstructured(model.fc1, name='weight', amount=0.3)

# 非結構化剪枝
prune.global_unstructured(
    parameters_to_prune=[(model.fc1, 'weight'), (model.fc2, 'weight')],
    pruning_method=prune.L1Unstructured,
    amount=0.2,
)

# LAMP 剪枝
prune.LampPrune(model, name='weight', amount=0.3)
```

### 知識蒸餾（Knowledge Distillation）

用大模型指導小模型學習：

```python
def distillation_loss(student_logits, teacher_logits, labels, T=4, alpha=0.7):
    # 軟目標蒸餾
    soft_loss = F.kl_div(
        F.log_softmax(student_logits / T),
        F.softmax(teacher_logits / T),
        reduction='batchmean'
    ) * (T * T)
    
    # 硬目標交叉熵
    hard_loss = F.cross_entropy(student_logits, labels)
    
    return alpha * soft_loss + (1 - alpha) * hard_loss
```

## AI 加速框架

### TensorRT

NVIDIA 高效能推論引擎：

```python
import tensorrt as trt

# 建立 builder
builder = trt.Builder(trt.Logger())
network = builder.create_network(1 << int(trt.NetworkDefinitionCreationFlag.EXPLICIT_BATCH))
config = builder.create_builder_config()
config.max_workspace_size = 1 << 30  # 1GB

# 解析 ONNX 模型
import onnx_tensorrt.backend as backend
engine = backend.prepare(onnx_model, device='CUDA:0')

# 執行推論
results = engine.run(input_data)
```

### ONNX Runtime

跨平臺推論引擎：

```python
import onnxruntime as ort

session = ort.InferenceSession("model.onnx")

# 推論
input_name = session.get_inputs()[0].name
output_name = session.get_outputs()[0].name

results = session.run([output_name], {input_name: input_data})
```

### 系統軟體棧

```
┌─────────────────────────────────────────┐
│         應用層 (PyTorch, TensorFlow)     │
├─────────────────────────────────────────┤
│       模型轉換 (ONNX, TFLite)           │
├─────────────────────────────────────────┤
│     推理引擎 (TensorRT, ONNX Runtime)    │
├─────────────────────────────────────────┤
│       硬體抽象 (CUDA, OpenCL)           │
├─────────────────────────────────────────┤
│    硬體 (GPU, TPU, NPU, FPGA, CPU)      │
└─────────────────────────────────────────┘
```

## Edge AI

在邊緣裝置上執行 AI：

### 設備端訓練

```python
# Federated Learning（聯邦學習）
import torch federated as fl

@fl.client
class FlowerClient(fl.client.NumPyClient):
    def get_parameters(self):
        return [val.cpu().numpy() for val in model.parameters()]
    
    def fit(self, parameters, config):
        set_model_params(model, parameters)
        train(model)
        return get_model_params(model), len(train_loader), {}
```

### 模型壓縮

```python
# 知識蒸餾壓縮
teacher = torchvision.models.resnet50(pretrained=True)
student = torchvision.models.mobilenet_v2(pretrained=False)

# Student 學習 Teacher 的中間表示
teacher_features = teacher.features(input)
student_features = student.features(input)

# 特徵蒸餾損失
distillation_loss = F.mse_loss(student_features, teacher_features)
```

## 未來趨勢

### 光子計算

用光子替代電子進行矩陣運算，理論上可達到極低延遲和能耗。

### 類比 AI

直接在類比電路上進行計算，進一步降低能耗。

### 記憶體內計算

在記憶體陣列中直接執行矩陣乘法，避免資料搬移。

## 相關主題

- [並行計算與異構平台](並行計算與異構平台.md) - 硬體並行基礎
- [虛擬記憶體](虛擬記憶體.md) - AI 加速器的記憶體架構
- [編譯器](編譯器.md) - AI 編譯器技術
