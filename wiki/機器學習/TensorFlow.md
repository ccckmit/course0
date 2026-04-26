# TensorFlow

TensorFlow 是 Google 開發的開源深度學習框架，於 2015 年首次發布。TensorFlow 廣泛應用於機器學習和深度神經網路的研究與生產環境，提供從資料處理、模型建立、訓練、部署到推理的完整工具鏈。TensorFlow 的核心設計理念是使用資料流圖（Data Flow Graph）來表示計算，其中節點代表數學運算，邊代表在節點間流動的資料（稱為張量）。這種設計使得 TensorFlow 能夠高效地利用硬體資源，特別適合大規模分散式訓練和部署。

## TensorFlow 的發展歷程

```
┌─────────────────────────────────────────────────────────────┐
│                  TensorFlow 發展時間線                      │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   2015.11    TensorFlow 1.0 發布                           │
│             開源深度學習框架正式推出                         │
│                                                             │
│   2017.02    TensorFlow 1.0 Release                         │
│             Keras 整合、 eager execution (實驗性)            │
│                                                             │
│   2018.03    TensorFlow 2.0 預覽                            │
│             Keras 成為官方高層 API                          │
│                                                             │
│   2019.09    TensorFlow 2.0 正式發布                        │
│             Eager Execution 預設啟用                         │
│             Keras 完全整合                                  │
│                                                             │
│   2021.05    TensorFlow 2.5                                 │
│             支援 TPU v3, 混合精度訓練                        │
│                                                             │
│   2023.06    TensorFlow 2.14                                │
│             Keras 3.0 預覽                                  │
│                                                             │
│   2024       TensorFlow Lite MLIR 遷移                     │
│             行動/邊緣部署優化                                │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

## TensorFlow 核心概念

### 張量 (Tensor)

```python
import tensorflow as tf

# 建立張量
# 0 維張量 (標量)
scalar = tf.constant(42)
print(f"Scalar: {scalar}, shape: {scalar.shape}")

# 1 維張量 (向量)
vector = tf.constant([1.0, 2.0, 3.0])
print(f"Vector: {vector}, shape: {vector.shape}")

# 2 維張量 (矩陣)
matrix = tf.constant([[1, 2], [3, 4]], dtype=tf.float32)
print(f"Matrix:\n{matrix}\nShape: {matrix.shape}")

# 3 維張量
tensor_3d = tf.constant([[[1, 2], [3, 4]], [[5, 6], [7, 8]]])
print(f"3D Tensor shape: {tensor_3d.shape}")

# 常用建立方法
zeros = tf.zeros([3, 3])           # 全零
ones = tf.ones([2, 4])              # 全一
eye_matrix = tf.eye(3)              # 單位矩陣
range_tensor = tf.range(10)         # 範圍
linspace = tf.linspace(0, 1, 5)     # 線性空間
random_normal = tf.random.normal([3, 3])  # 常態分佈隨機
random_uniform = tf.random.uniform([3, 3], minval=0, maxval=1)  # 均勻分佈

# 張量屬性
print(f"dtype: {matrix.dtype}")
print(f"device: {matrix.device}")
print(f"numpy: {matrix.numpy()}")
```

### 運算操作

```python
import tensorflow as tf

a = tf.constant([[1, 2], [3, 4]], dtype=tf.float32)
b = tf.constant([[5, 6], [7, 8]], dtype=tf.float32)

# 基本數學運算
c_add = tf.add(a, b)              # 加法
c_sub = tf.subtract(a, b)        # 減法
c_mul = tf.multiply(a, b)        # 乘法
c_div = tf.divide(a, b)          # 除法
c_matmul = tf.matmul(a, b)       # 矩陣乘法

# 元素級運算
c_pow = tf.pow(a, 2)             # 次方
c_sqrt = tf.sqrt(a)              # 平方根
c_log = tf.log(a)                # 對數
c_exp = tf.exp(a)                # 指數

# 聚合操作
sum_val = tf.reduce_sum(a)       # 總和
mean_val = tf.reduce_mean(a)     # 平均
max_val = tf.reduce_max(a)       # 最大
min_val = tf.reduce_min(a)       # 最小

# 維度操作
transposed = tf.transpose(a)     # 轉置
reshaped = tf.reshape(a, [1, 4])  # 形狀變更
sliced = a[0:2, :]              # 切片

# 資料型態轉換
a_float = tf.cast(a, tf.float32)
a_int = tf.cast(a, tf.int32)
```

### 自動微分 (GradientTape)

```python
import tensorflow as tf

# 基本梯度計算
x = tf.Variable(3.0)

with tf.GradientTape() as tape:
    y = x ** 2

grad = tape.gradient(y, x)
print(f"dy/dx = {grad.numpy()}")  # 6.0

# 多變數梯度
w = tf.Variable([1.0, 2.0])
b = tf.Variable(3.0)

with tf.GradientTape() as tape:
    y = w[0] * 5 + w[1] * 10 + b

dw, db = tape.gradient(y, [w, b])
print(f"dw = {dw.numpy()}, db = {db.numpy()}")  # dw=[5, 10], db=1.0

# 持續追蹤
with tf.GradientTape(persistent=True) as tape:
    y1 = w[0] ** 2
    y2 = w[1] ** 3

grad1 = tape.gradient(y1, w)
grad2 = tape.gradient(y2, w)
print(f"grad1 = {grad1.numpy()}, grad2 = {grad2.numpy()}")  # [2, 0], [0, 12]

# 高階導數
x = tf.Variable(2.0)

with tf.GradientTape() as tape2:
    with tf.GradientTape() as tape1:
        y = x ** 4
    first_grad = tape1.gradient(y, x)
second_grad = tape2.gradient(first_grad, x)

print(f"first_grad: {first_grad.numpy()}")   # 32
print(f"second_grad: {second_grad.numpy()}")  # 96
```

## Keras 高層 API

```python
from tensorflow import keras
from tensorflow.keras import layers

# -----------------------------------------------------------
# Sequential 模型 (最簡單)
# -----------------------------------------------------------
model = keras.Sequential([
    # 輸入層
    layers.Dense(256, activation='relu', input_shape=(784,)),
    layers.Dropout(0.3),
    
    # 隱藏層
    layers.Dense(128, activation='relu'),
    layers.Dropout(0.2),
    
    # 輸出層
    layers.Dense(10, activation='softmax')
])

# 模型摘要
model.summary()

# -----------------------------------------------------------
# Functional API (更靈活)
# -----------------------------------------------------------
# 多輸入
inputs_input = keras.Input(shape=(784,))
labels_input = keras.Input(shape=(10,))

# 共享層
x = layers.Dense(64, activation='relu')(inputs_input)
x = layers.Dense(10)(x)

# 多輸出
output = layers.Dense(10, activation='softmax')(x)
output_labels = layers.Dense(10, activation='sigmoid')(output)

model = keras.Model(
    inputs=[inputs_input, labels_input],
    outputs=[output, output_labels]
)

# -----------------------------------------------------------
# Subclassing API (最大彈性)
# -----------------------------------------------------------
class MyModel(keras.Model):
    def __init__(self):
        super().__init__()
        self.dense1 = layers.Dense(64, activation='relu')
        self.dense2 = layers.Dense(10, activation='softmax')
    
    def call(self, inputs, training=False):
        x = self.dense1(inputs)
        if training:
            x = tf.nn.dropout(x, 0.3)
        return self.dense2(x)

model = MyModel()
```

## 模型訓練

```python
from tensorflow import keras

# -----------------------------------------------------------
# 編譯模型
# -----------------------------------------------------------
model = keras.Sequential([
    layers.Dense(64, activation='relu', input_shape=(784,)),
    layers.Dense(10, activation='softmax')
])

model.compile(
    # 優化器
    optimizer='adam',  # 或 keras.optimizers.Adam(learning_rate=0.001)
    # 損失函數
    loss='sparse_categorical_crossentropy',  # 整數標籤
    # 評估指標
    metrics=['accuracy', keras.metrics.Precision(), keras.metrics.Recall()]
)

# -----------------------------------------------------------
# 訓練模型
# -----------------------------------------------------------
# 方式1: 使用 NumPy 資料
history = model.fit(
    x_train, y_train,
    epochs=10,
    batch_size=32,
    validation_split=0.2,
    callbacks=[
        keras.callbacks.EarlyStopping(patience=3, restore_best_weights=True),
        keras.callbacks.ModelCheckpoint('best_model.h5', save_best_only=True),
        keras.callbacks.TensorBoard(log_dir='./logs')
    ]
)

# 方式2: 使用 tf.data.Dataset
train_dataset = tf.data.Dataset.from_tensor_slices((x_train, y_train))
train_dataset = train_dataset.shuffle(10000).batch(32).prefetch(tf.data.AUTOTUNE)

history = model.fit(train_dataset, epochs=10)

# 方式3: 自訂訓練循環
optimizer = keras.optimizers.Adam(learning_rate=0.001)
loss_fn = keras.losses.SparseCategoricalCrossentropy()

train_loss = keras.metrics.Mean(name='train_loss')
train_accuracy = keras.metrics.SparseCategoricalAccuracy(name='train_accuracy')

@tf.function
def train_step(images, labels):
    with tf.GradientTape() as tape:
        predictions = model(images, training=True)
        loss = loss_fn(labels, predictions)
    
    gradients = tape.gradient(loss, model.trainable_variables)
    optimizer.apply_gradients(zip(gradients, model.trainable_variables))
    
    train_loss(loss)
    train_accuracy(labels, predictions)

for epoch in range(10):
    train_loss.reset_states()
    train_accuracy.reset_states()
    
    for images, labels in train_dataset:
        train_step(images, labels)
    
    print(f'Epoch {epoch + 1}: Loss={train_loss.result():.4f}, Accuracy={train_accuracy.result():.4f}')
```

## 模型評估與預測

```python
# -----------------------------------------------------------
# 評估
# -----------------------------------------------------------
test_loss, test_acc = model.evaluate(x_test, y_test, verbose=2)
print(f'Test accuracy: {test_acc:.4f}')

# 交叉驗證
from sklearn.model_selection import cross_val_score
# 注意：sklearn 需要先 predict，然後計算分數

# -----------------------------------------------------------
# 預測
# -----------------------------------------------------------
# 單一預測
predictions = model.predict(x_new_sample)
predicted_class = tf.argmax(predictions, axis=1).numpy()

# 批量預測
batch_predictions = model.predict(x_test_batch)

# -----------------------------------------------------------
# 保存與載入
# -----------------------------------------------------------
# 保存整個模型
model.save('my_model.keras')  # Keras 格式 (TF 2.14+)
model.save('my_model.h5')     # HDF5 格式

# 保存權重
model.save_weights('my_weights.weights.h5')

# 載入模型
loaded_model = keras.models.load_model('my_model.keras')
loaded_model.load_weights('my_weights.weights.h5')

# SavedModel 格式
model.save('saved_model/')
loaded_model = keras.models.load_model('saved_model/')
```

## 資料管道 (tf.data)

```python
import tensorflow as tf

# -----------------------------------------------------------
# 從 NumPy 建立
# -----------------------------------------------------------
dataset = tf.data.Dataset.from_tensor_slices((x_train, y_train))

# -----------------------------------------------------------
# 轉換操作
# -----------------------------------------------------------
dataset = (
    tf.data.Dataset.from_tensor_slices((x_train, y_train))
    .shuffle(10000)           # 打亂
    .batch(32)                # 批次
    .map(lambda x, y: (x / 255.0, y))  # 預處理
    .cache()                  # 快取
    .prefetch(tf.data.AUTOTUNE)  # 預取
)

# -----------------------------------------------------------
# 從檔案讀取
# -----------------------------------------------------------
# CSV
dataset = tf.data.experimental.make_csv_dataset(
    'data.csv',
    batch_size=32,
    label_name='label',
    num_epochs=1
)

# 圖片
image_generator = tf.keras.preprocessing.image.ImageDataGenerator(
    rescale=1./255,
    validation_split=0.2
)

train_data = image_generator.flow_from_directory(
    'train_dir',
    target_size=(224, 224),
    batch_size=32,
    class_mode='categorical'
)

# -----------------------------------------------------------
# TFRecord
# -----------------------------------------------------------
def parse_example(serialized):
    feature_description = {
        'image': tf.io.FixedLenFeature([], tf.string),
        'label': tf.io.FixedLenFeature([], tf.int64)
    }
    example = tf.io.parse_single_example(serialized, feature_description)
    image = tf.io.decode_jpeg(example['image'], channels=3)
    return image, example['label']

dataset = tf.data.TFRecordDataset('data.tfrecord')
dataset = dataset.map(parse_example)
```

## TensorFlow Lite（行動/邊緣部署）

```python
import tensorflow as tf

# -----------------------------------------------------------
# 轉換模型
# -----------------------------------------------------------

# 方式1: 從 Keras 模型轉換
converter = tf.lite.TFLiteConverter.from_keras_model(keras_model)
tflite_model = converter.convert()

# 方式2: 從 SavedModel 轉換
converter = tf.lite.TFLiteConverter.from_saved_model('saved_model/')
tflite_model = converter.convert()

# 方式3: 從 Concrete Function (TF 2 圖) 轉換
@tf.function
def inference(x):
    return model(x)

converter = tf.lite.TFLiteConverter.from_concrete_functions(
    inference.get_concrete_function(x_sample)
)
tflite_model = converter.convert()

# -----------------------------------------------------------
# 量化優化
# -----------------------------------------------------------
converter.optimizations = [tf.lite.Optimize.DEFAULT]

# 動態範圍量化
converter.target_spec.supported_types = [tf.float16]  # Float16 量化

# 完整整數量化 (需要代表性資料)
converter.optimizations = [tf.lite.Optimize.DEFAULT]
converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
converter.representative_dataset = representative_data_generator

tflite_model = converter.convert()

# -----------------------------------------------------------
# 使用 TFLite 模型
# -----------------------------------------------------------
interpreter = tf.lite.Interpreter(model_content=tflite_model)
interpreter.allocate_tensors()

input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

# 執行推論
interpreter.set_tensor(input_details[0]['index'], input_data)
interpreter.invoke()
output_data = interpreter.get_tensor(output_details[0]['index'])
```

## 分散式訓練

```python
import tensorflow as tf

# -----------------------------------------------------------
# MirroredStrategy (單機多 GPU)
# -----------------------------------------------------------
strategy = tf.distribute.MirroredStrategy()
print(f'Number of devices: {strategy.num_replicas_in_sync}')

with strategy.scope():
    model = keras.Sequential([
        layers.Dense(64, activation='relu', input_shape=(784,)),
        layers.Dense(10, activation='softmax')
    ])
    model.compile(
        optimizer='adam',
        loss='sparse_categorical_crossentropy',
        metrics=['accuracy']
    )

model.fit(x_train, y_train, epochs=10, batch_size=32)

# -----------------------------------------------------------
# MultiWorkerMirroredStrategy (多機多 GPU)
# -----------------------------------------------------------
# 環境變數
os.environ['TF_CONFIG'] = '''
{
    "cluster": {
        "worker": ["host1:port", "host2:port", "host3:port"]
    },
    "task": {"type": "worker", "index": 0}
}
'''

strategy = tf.distribute.MultiWorkerMirroredStrategy()

with strategy.scope():
    model = build_model()
    model.compile(...)

model.fit(dataset)

# -----------------------------------------------------------
# TPUStrategy (Google Cloud TPU)
# -----------------------------------------------------------
resolver = tf.distribute.cluster_resolver.TPUClusterResolver(tpu='')
tf.config.experimental_connect_to_cluster(resolver)
tf.tpu.experimental.initialize_tpu_system(resolver)
strategy = tf.distribute.TPUStrategy(resolver)

with strategy.scope():
    model = build_model()
```

## 模型服務

```python
# -----------------------------------------------------------
# TensorFlow Serving (REST API)
# -----------------------------------------------------------
# 建立 SavedModel
model.save('1')

# 啟動服務
# tensorflow_model_server --port=8501 --model_name=my_model --model_base_path=/tmp/models'

import requests
import json

# REST API 預測
payload = {
    "instances": [[0.1, 0.2, 0.3] for _ in range(10)]
}
response = requests.post(
    'http://localhost:8501/v1/models/my_model:predict',
    json=payload
)
predictions = response.json()['predictions']

# -----------------------------------------------------------
# TensorFlow.js
# -----------------------------------------------------------
# 匯出為 TensorFlow.js 格式
!tensorflowjs_converter \
    --input_format=keras \
    model.h5 \
    web_model/

# -----------------------------------------------------------
# TensorFlow Serving gRPC
# -----------------------------------------------------------
from tensorflow_serving.apis import predict_pb2
from tensorflow_serving.apis import prediction_service_pb2_grpc
import grpc

channel = grpc.insecure_channel('localhost:8500')
stub = prediction_service_pb2_grpc.PredictionServiceStub(channel)

request = predict_pb2.PredictRequest()
request.model_spec.name = 'my_model'
request.model_spec.signature_name = 'serving_default'
request.inputs['input'].CopyFrom(tf.make_tensor_proto(input_data))
response = stub.Predict(request)
```

## 模型效能優化

```python
# -----------------------------------------------------------
# 混合精度訓練
# -----------------------------------------------------------
from tensorflow.keras import mixed_precision
mixed_precision.set_global_policy('mixed_float16')

# 對特定層設定
policy = mixed_precision.Policy('mixed_float16')
layer = layers.Dense(64, dtype='float32')  # 保持 float32

# -----------------------------------------------------------
# XLA 編譯
# -----------------------------------------------------------
@tf.function(jit_compile=True)  # 啟用 XLA
def optimized_function(x, y):
    return tf.matmul(x, y) + x

# -----------------------------------------------------------
# Profiling
# -----------------------------------------------------------
@tf.function
def train_step(images, labels):
    with tf.profiler.experimental.Trace('train_step'):
        predictions = model(images, training=True)
        loss = loss_fn(labels, predictions)
    
    gradients = tape.gradient(loss, model.trainable_variables)
    optimizer.apply_gradients(zip(gradients, model.trainable_variables))
    return loss

# TensorBoard Profiler
profiler_callback = tf.keras.callbacks.TensorBoard(
    log_dir='./logs',
    profile_batch=(5, 10)
)
model.fit(train_dataset, callbacks=[profiler_callback])
```

## 相關主題

- [深度學習](深度學習.md) - 深度學習基礎
- [PyTorch](PyTorch.md) - 另一深度學習框架
- [卷積神經網路](卷積神經網路.md) - CNN 實作
- [大型語言模型](大型語言模型.md) - LLM 部署