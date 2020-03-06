import tensorflow as tf
import numpy as np

def loss_func(predicted_y, target_y):
  return tf.reduce_mean(tf.square(predicted_y - target_y))

dataset_x = np.array([
    [1, 2, 3, 4],
    [2, 3, 4, 5],
    [3, 4, 5, 6],
    [4, 5, 6, 6],
    [1, 3, 2, 4],
]).astype(np.float32)

np.save("/tmp/simple_model/r_dataset", dataset_x)

dataset_y = np.array([10.0, 20.0, 30.0, 30.0, 12.0])

model = tf.keras.models.Sequential([
  tf.keras.layers.Input(shape=(4,)),
  tf.keras.layers.Dense(8, activation='tanh'),
  tf.keras.layers.Dense(1)
])

model.compile(optimizer='adam',
              loss=loss_func,
              metrics=['accuracy'])

model.fit(dataset_x, dataset_y, epochs=10000)

model.summary()

tf.saved_model.save(model, "/tmp/simple_model")

predictions = model(dataset_x[:1]).numpy()
print(f"Predication: {predictions}")
