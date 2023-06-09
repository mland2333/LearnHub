import numpy as np
import pandas as pd
from keras.models import Sequential
from keras.layers import Dense

# 导入数据集
dataset = pd.read_csv('铸件数据集.csv')
X = dataset.iloc[:, 0:8].values
y = dataset.iloc[:, 8:11].values

# 初始化模型
model = Sequential()

# 添加输入层和第一个隐藏层
model.add(Dense(units = 8, kernel_initializer = 'uniform', activation = 'relu', input_dim = 8))

# 添加第二个隐藏层
model.add(Dense(units = 6, kernel_initializer = 'uniform', activation = 'relu'))

# 添加输出层
model.add(Dense(units = 3, kernel_initializer = 'uniform', activation = 'sigmoid'))

# 编译模型
model.compile(optimizer = 'adam', loss = 'binary_crossentropy', metrics = ['accuracy'])

# 训练模型
model.fit(X, y, batch_size = 10, epochs = 100)

# 对测试集进行预测
y_pred = model.predict(X)
y_pred = (y_pred > 0.5)

# 计算准确率
from sklearn.metrics import accuracy_score
accuracy = accuracy_score(y, y_pred)
print('准确率：', accuracy)

