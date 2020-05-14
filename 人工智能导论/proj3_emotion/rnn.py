import util
import os
import time
import tensorflow as tf
import numpy as np
import sys

num_layers = 4
hidden_size = 256
dropout_keep = 0.5
learn_rate = 1e-3
batch_size = 128
num_epochs = 200
save_path = 'rnn_ckps/best'


class RNN:
    __slots__ = ('input_x', 'input_y', 'keep_prob', 'loss', 'opt', 'acc', 'val')

    def __init__(self):
        self.input_x = tf.placeholder(tf.float32, [None, util.seq_len, util.embed_dim], name='input_x')
        self.input_y = tf.placeholder(tf.float32, [None, util.num_class], name='input_y')
        self.keep_prob = tf.placeholder(tf.float32, name='keep_prob')
        l2_reg = tf.contrib.layers.l2_regularizer(scale=0.01)

        with tf.name_scope("rnn"):
            rnn_cell = tf.contrib.cudnn_rnn.CudnnGRU(num_layers, hidden_size)
            outputs, output_states = rnn_cell(self.input_x)
            last = outputs[:, -1, :]

        with tf.name_scope("dense"):
            fc = tf.layers.dense(last, hidden_size, name='fc1', kernel_regularizer=l2_reg)
            fc = tf.contrib.layers.dropout(fc, self.keep_prob)
            fc = tf.nn.relu(fc)

            logits = tf.layers.dense(fc, util.num_class, name='fc2', kernel_regularizer=l2_reg)
            self.val = tf.nn.softmax(logits)
            pred_y = tf.argmax(self.val, 1)

        with tf.variable_scope("err"):
            cross_entropy = tf.nn.softmax_cross_entropy_with_logits(logits=logits, labels=self.input_y)
            l2_loss = tf.losses.get_regularization_loss()
            self.loss = tf.reduce_mean(cross_entropy) + l2_loss

        with tf.variable_scope("opt"):
            self.opt = tf.train.AdamOptimizer(learning_rate=learn_rate).minimize(self.loss)
            self.acc = tf.reduce_mean(tf.cast(tf.equal(tf.argmax(self.input_y, 1), pred_y), tf.float32))


def evaluate(rnn: RNN, sess, x, y):
    data_len = len(x)
    total_loss = 0.0
    total_acc = 0.0
    for batch_x, batch_y in util.select_batch(x, y, batch_size):
        batch_len = len(batch_x)
        loss, acc = sess.run([rnn.loss, rnn.acc],
                             feed_dict={rnn.input_x: batch_x, rnn.input_y: batch_y, rnn.keep_prob: 1})
        total_loss += loss * batch_len
        total_acc += acc * batch_len

    return total_loss / data_len, total_acc / data_len


def train(rnn: RNN):
    print("Configuring TensorBoard and Saver...")

    tensorboard_dir = 'tensorboard/rnn'
    if not os.path.exists(tensorboard_dir):
        os.makedirs(tensorboard_dir)

    with tf.name_scope('train'):
        tf.summary.scalar("train_loss", rnn.loss)
        tf.summary.scalar("train_acc", rnn.acc)
        train_summary = tf.summary.merge_all(scope='train')

    with tf.name_scope('validate'):
        tf.summary.scalar("validate_loss", rnn.loss)
        tf.summary.scalar("validate_acc", rnn.acc)
        validate_summary = tf.summary.merge_all(scope='validate')

    writer = tf.summary.FileWriter(tensorboard_dir)
    saver = tf.train.Saver()

    beg = time.time()
    train_x = np.fromfile('data/train_x.bin', dtype='float32').reshape(util.train_len, util.seq_len, util.embed_dim)
    train_y = np.fromfile('data/train_y.bin', dtype='float32').reshape(util.train_len, util.num_class)
    validate_x = np.fromfile('data/validate_x.bin', dtype='float32').reshape(util.validate_len, util.seq_len,
                                                                             util.embed_dim)
    validate_y = np.fromfile('data/validate_y.bin', dtype='float32').reshape(util.validate_len, util.num_class)
    print(f"Load train & validate: {time.time() - beg}s")

    sess = tf.Session()
    sess.run(tf.global_variables_initializer())
    writer.add_graph(sess.graph)

    print('Training and evaluating...')
    beg = time.time()

    best_validate_acc = 0.0
    for epoch in range(num_epochs):
        print(f'Epoch: {epoch}, {time.time() - beg}s')
        if epoch % 5 == 0:
            small_train_x, small_train_y = util.select(train_x, train_y, batch_size)
            s = sess.run(train_summary,
                         feed_dict={rnn.input_x: small_train_x, rnn.input_y: small_train_y, rnn.keep_prob: 1})
            writer.add_summary(s, epoch)
            s = tf.Summary().FromString(s)
            train_loss, train_acc = s.value[0].simple_value, s.value[1].simple_value

            small_test_x, small_test_y = util.select(validate_x, validate_y, batch_size)
            s: tf.Summary = sess.run(validate_summary,
                                     feed_dict={rnn.input_x: small_test_x, rnn.input_y: small_test_y, rnn.keep_prob: 1})
            writer.add_summary(s, epoch)
            s = tf.Summary().FromString(s)
            validate_loss, validate_acc = s.value[0].simple_value, s.value[1].simple_value

            if validate_acc > best_validate_acc:
                best_validate_acc = validate_acc
                print(f'update result, best_validate_acc = {best_validate_acc}')
                saver.save(sess=sess, save_path=save_path)

            print(f'train_loss: {train_loss}, train_acc: {train_acc}; '
                  f'validate_loss: {validate_loss}, validate_acc: {validate_acc}')
        for batch_x, batch_y in util.select_batch(train_x, train_y, batch_size):
            sess.run(rnn.opt, feed_dict={rnn.input_x: batch_x, rnn.input_y: batch_y, rnn.keep_prob: dropout_keep})


if __name__ == '__main__':
    rnn = RNN()
    if len(sys.argv) == 2 and sys.argv[1] == 'i':
        saver = tf.train.Saver()
        with tf.Session() as sess:
            saver.restore(sess, save_path)
            print("Model restored.")
            test_x = np.fromfile('data/test_x.bin', dtype='float32').reshape(util.test_len, util.seq_len,
                                                                             util.embed_dim)
            test_y = np.fromfile('data/test_y.bin', dtype='float32').reshape(util.test_len, util.num_class)
            print(f'Test length {len(test_x)}')
            total_len = 0
            total_loss = 0.0
            total_acc = 0.0
            total_val = np.ndarray([util.test_len, util.num_class], dtype='float32')
            for batch_x, batch_y in util.select_batch(test_x, test_y, batch_size):
                loss, acc, val = sess.run([rnn.loss, rnn.acc, rnn.val],
                                          feed_dict={rnn.input_x: batch_x, rnn.input_y: batch_y, rnn.keep_prob: 1})
                total_val[total_len:total_len + len(batch_x)] = val
                total_len += len(batch_x)
                total_loss += loss * len(batch_x)
                total_acc += acc * len(batch_x)

            print(total_acc / total_len)
            total_val.tofile('output/rnn_val.bin')
    else:
        train(rnn)
