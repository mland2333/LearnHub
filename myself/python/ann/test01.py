import pandas as pd
import numpy as np
from sklearn.preprocessing import OneHotEncoder

# 导入数据
df = pd.read_csv('data.csv')

# 将数据拆分为输入和输出
X = df.iloc[:, :-1].values  # 输入：铸造工艺参数
y = df.iloc[:, -1:].values  # 输出：铸件缺陷等级

# 将输出进行One-hot编码
enc = OneHotEncoder()
y = enc.fit_transform(y).toarray()

from sklearn.preprocessing import StandardScaler

# 对输入数据进行标准化
sc = StandardScaler()
X = sc.fit_transform(X)
from keras.models import Sequential
from keras.layers import Dense

# 构建神经网络模型
model = Sequential()
model.add(Dense(8, input_dim=2, activation='relu'))
model.add(Dense(4, activation='softmax'))
# 编译模型
model.compile(loss='categorical_crossentropy', optimizer='adam', metrics=['accuracy'])

# 训练模型
model.fit(X, y, epochs=100, batch_size=10)
# 预测新数据
X_new = np.array([[1,1]])  # 新的铸造工艺参数
X_new = sc.transform(X_new)  # 标准化
y_new = model.predict(X_new)  # 预测缺陷等级的概率分布
print(y_new)

