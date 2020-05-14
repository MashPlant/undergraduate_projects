import numpy as np
from scipy.stats import pearsonr
from sklearn.metrics import f1_score
import util

test_y_val = np.fromfile('data/test_y_val.bin', dtype='float32').reshape(util.test_len, util.num_class)
test_y = np.argmax(test_y_val, axis=1)
cnn_val = np.fromfile('output/cnn_val.bin', dtype='float32').reshape(util.test_len, util.num_class)
cnn_y = np.argmax(cnn_val, axis=1)
rnn_val = np.fromfile('output/rnn_val.bin', dtype='float32').reshape(util.test_len, util.num_class)
rnn_y = np.argmax(rnn_val, axis=1)
dnn_val = np.fromfile('output/dnn_val.bin', dtype='float32').reshape(util.test_len, util.num_class)
dnn_y = np.argmax(dnn_val, axis=1)


def stat(val: np.ndarray, y: np.ndarray, name: str):
    print(name)
    print(f'acc = {np.mean(test_y == y)}')
    correlation = sum(pearsonr(test_y_val_i, val_i)[0] for (test_y_val_i, val_i) in zip(test_y_val, val)) / len(
        test_y_val)
    print(f'correlation = {correlation}')
    print(f"f1 score = {f1_score(test_y, y, average='macro')}")
    print()


stat(cnn_val, cnn_y, 'cnn')
stat(rnn_val, rnn_y, 'rnn')
stat(dnn_val, dnn_y, 'dnn')
# print(pearsonr(test_y_val[0], cnn_val[0]))

# print(pearsonr(test_y_val[0], cnn_val[0]))
# print(f1_score(test_y, cnn_y, average='macro'))
# print(np.mean(test_y == cnn_y))

# print(pearsonr(test_y_val[0], cnn_val[0]))
# print(f1_score(test_y, cnn_y, average='macro'))
# print(np.mean(test_y == cnn_y))
