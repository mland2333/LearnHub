import numpy as np

def affine_forward(x, w, b):
    out = None                       
    N = x.shape[0]
    x_row = x.reshape(N, -1)         
    out = np.dot(x_row, w) + b       
    cache = (x, w, b)                
    return out,cache

def affine_backward(dout, cache):   
    x, w, b = cache                              
    dx, dw, db = None, None, None                
    dx = np.dot(dout, w.T)                       
    dx = np.reshape(dx, x.shape)                 
    x_row = x.reshape(x.shape[0], -1)            
    dw = np.dot(x_row.T, dout)                   
    db = np.sum(dout, axis=0, keepdims=True)     
    return dx, dw, db

x = np.array([[[1,1],[-1,1],[-1,-1],[1,-1]],
            [[100,1],[-100,1],[-100,-1],[100,-1]],
              [[1,100],[-1,100],[-1,-100],[1,-100]],
              [[100,100],[-100,100],[-100,-100],[100,-100]],
              [[1000, 1], [-1000, 1], [-1000, -1], [1000, -1]],
              [[1,1000],[-1,1000],[-1,-1000],[1,-1000]]])
t = np.array([0,1,2,3])   
np.random.seed(1)         

# 一些初始化参数
input_dim = x.shape[2]
num_classes = t.shape[0]  
hidden_dim = 50           
reg = 0.001               
epsilon = 0.001           
# 初始化W1，W2，b1，b2
W1 = np.random.randn(input_dim, hidden_dim)     
W2 = np.random.randn(hidden_dim, num_classes)   
b1 = np.zeros((1, hidden_dim))                  
b2 = np.zeros((1, num_classes))

for X in x:
    for j in range(10000):
     # 前向传播
        H,fc_cache = affine_forward(X,W1,b1)
        H = np.maximum(0, H)
        relu_cache = H
        Y,cachey = affine_forward(H,W2,b2)
     # Softmax层计算
        probs = np.exp(Y - np.max(Y, axis=1, keepdims=True))
        probs /= np.sum(probs, axis=1, keepdims=True)
     # 计算loss值
        N = Y.shape[0]
        #print(probs[np.arange(N), t])
        loss = -np.sum(np.log(probs[np.arange(N), t]+1e-5)) / N
        #print(loss)
     # 反向传播
        dx = probs.copy()
        dx[np.arange(N), t] -= 1
        dx /= N
        dh1, dW2, db2 = affine_backward(dx, cachey)
        dh1[relu_cache <= 0] = 0
        dX, dW1, db1 = affine_backward(dh1, fc_cache)
    # 参数更新
        dW2 += reg * W2
        dW1 += reg * W1
        W2 += -epsilon * dW2
        b2 += -epsilon * db2
        W1 += -epsilon * dW1
        b1 += -epsilon * db1

test = np.array([[10,2000],[-20000,1],[-5,-19000],[5000,-1]])
H,fc_cache = affine_forward(test,W1,b1)               
H = np.maximum(0, H)                                 
relu_cache = H
Y,cachey = affine_forward(H,W2,b2)  
 # Softmax
probs = np.exp(Y - np.max(Y, axis=1, keepdims=True))    
probs /= np.sum(probs, axis=1, keepdims=True) 
print(probs)
for k in range(4):
    print(test[k,:],"所在的象限为",np.argmax(probs[k,:])+1)
