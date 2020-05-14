import pickle as pk
import numpy as np
from typing import Iterable, Tuple, Dict

seq_len = 600
embed_dim = 300
num_class = 8
tot_train_len = 2342
train_len = 2048
validate_len = tot_train_len - train_len
test_len = 2228


def word2vec2pk():
    data = {}

    with open('data/sgns.merge.word', 'r') as f:
        for ln in f.readlines():
            sp = ln.split()
            data[sp[0]] = np.array(sp[1:], dtype='float32')

    pk.dump(data, open('word2vec.pk', 'wb'))


def news2bin():
    print('begin load word2vec')
    with open('word2vec.pk', 'rb') as f:
        word2vec: Dict[str, np.array] = pk.load(f)
    print(f'loaded word2vec, words count = {len(word2vec)}')

    def impl(len_, file):
        in_ = np.ndarray((len_, seq_len, embed_dim), dtype='float32')
        out = np.zeros((len_, num_class), dtype='float32')
        out_val = np.ndarray((len_, num_class), dtype='float32')
        cur_in = np.ndarray((seq_len, embed_dim), dtype='float32')

        with open(file, 'r') as f:
            for (i, ln) in enumerate(f.readlines()):
                sp = ln.split()
                out_val[i] = np.array(list(map(lambda x: int(x.split(':')[1]), sp[2:10])))
                out[i][np.argmax(out_val[i])] = 1  # one hot
                cur_in.fill(0)
                idx = 0
                for (j, ch) in enumerate(sp[10:]):
                    if ch in word2vec:
                        cur_in[idx] = word2vec[ch]
                        idx += 1
                        if idx >= seq_len:
                            break
                in_[i] = cur_in

        return in_, out, out_val

    train_x, train_y, _ = impl(tot_train_len, 'data/sinanews.train')
    train_x[0:train_len].tofile('data/train_x.bin')
    train_y[0:train_len].tofile('data/train_y.bin')
    train_x[train_len:].tofile('data/validate_x.bin')
    train_y[train_len:].tofile('data/validate_y.bin')
    train_x[0:train_len, 0:200, 0:300].tofile('data/train_x_small.bin')
    train_x[train_len:, 0:200, 0:300].tofile('data/validate_x_small.bin')

    test_x, test_y, test_y_val = impl(test_len, 'data/sinanews.test')
    test_x.tofile('data/test_x.bin')
    test_y.tofile('data/test_y.bin')
    test_y_val.tofile('data/test_y_val.bin')
    test_x[:, 0:200, 0:300].tofile('data/test_x_small.bin')


def select_batch(x: np.ndarray, y: np.ndarray, batch_size: int) -> Iterable[Tuple[np.ndarray, np.ndarray]]:
    assert len(x) == len(y)
    num_batch = (len(x) - 1) // batch_size + 1

    indices = np.random.permutation(np.arange(len(x)))
    shuffle_x = x[indices]
    shuffle_y = y[indices]

    for i in range(num_batch):
        start_id = i * batch_size
        end_id = min((i + 1) * batch_size, len(x))
        yield shuffle_x[start_id:end_id], shuffle_y[start_id:end_id]


def sequence_batch(x: np.ndarray, y: np.ndarray, batch_size: int) -> Iterable[Tuple[np.ndarray, np.ndarray]]:
    assert len(x) == len(y)
    num_batch = len(x) // batch_size
    for i in range(num_batch):
        start_id = i * batch_size
        end_id = (i + 1) * batch_size if i != num_batch - 1 else len(x)
        yield x[start_id:end_id], y[start_id:end_id]


def select(x: np.ndarray, y: np.ndarray, batch_size: int) -> Tuple[np.ndarray, np.ndarray]:
    assert len(x) == len(y)
    indices = np.random.permutation(np.arange(len(x)))
    shuffle_x = x[indices]
    shuffle_y = y[indices]
    return shuffle_x[0:batch_size], shuffle_y[0:batch_size]


if __name__ == '__main__':
    news2bin()
