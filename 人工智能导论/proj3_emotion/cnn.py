import util
import time
import tensorflow as tf
import numpy as np
import sys

num_filter = 256
kernel_size = 5
hidden_size = 256
dropout_keep = 0.5
learn_rate = 1e-3
num_epochs = 200
save_path = 'cnn_ckps/best'
tb_dir = 'tensorboard/cnn'


class CNN:
    __slots__ = ('input_x', 'input_y', 'keep_prob', 'loss', 'opt', 'acc', 'val')

    def __init__(self):
        self.input_x = tf.placeholder(tf.float32, [None, util.seq_len, util.embed_dim], name='input_x')
        self.input_y = tf.placeholder(tf.float32, [None, util.num_class], name='input_y')

        self.keep_prob = tf.placeholder(tf.float32, name='keep_prob')
        l2_reg = tf.contrib.layers.l2_regularizer(scale=0.01)

        with tf.variable_scope("cnn"):
            conv = tf.layers.conv1d(self.input_x, num_filter, kernel_size, name='conv',
                                    kernel_regularizer=l2_reg)
            gmp = tf.reduce_max(conv, reduction_indices=[1], name='gmp')

        with tf.variable_scope("dense"):
            fc = tf.layers.dense(gmp, hidden_size, name='fc1', kernel_regularizer=l2_reg)
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


def train(cnn: CNN):
    with tf.name_scope('train'):
        tf.summary.scalar("train_loss", cnn.loss)
        tf.summary.scalar("train_acc", cnn.acc)
        train_summary = tf.summary.merge_all(scope='train')

    with tf.name_scope('validate'):
        tf.summary.scalar("validate_loss", cnn.loss)
        tf.summary.scalar("validate_acc", cnn.acc)
        validate_summary = tf.summary.merge_all(scope='validate')

    writer = tf.summary.FileWriter(tb_dir)
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

    beg = time.time()

    best_validate_acc = 0
    for epoch in range(num_epochs):
        print(f'Epoch: {epoch}, elapsed {time.time() - beg}s')
        if epoch % 5 == 0:
            s = sess.run(train_summary, feed_dict={cnn.input_x: train_x, cnn.input_y: train_y, cnn.keep_prob: 1})
            writer.add_summary(s, epoch)
            s = tf.Summary().FromString(s)
            train_loss, train_acc = s.value[0].simple_value, s.value[1].simple_value

            s: tf.Summary = sess.run(validate_summary,
                                     feed_dict={cnn.input_x: validate_x, cnn.input_y: validate_y, cnn.keep_prob: 1})
            writer.add_summary(s, epoch)
            s = tf.Summary().FromString(s)
            validate_loss, validate_acc = s.value[0].simple_value, s.value[1].simple_value

            if best_validate_acc < validate_acc:
                best_validate_acc = validate_acc
                print(f'update result, best_validate_acc = {best_validate_acc}')
                saver.save(sess=sess, save_path=save_path)

            print(f'train_loss: {train_loss}, train_acc: {train_acc}; '
                  f'validate_loss: {validate_loss}, validate_acc: {validate_acc}')
        sess.run(cnn.opt, feed_dict={cnn.input_x: train_x, cnn.input_y: train_y, cnn.keep_prob: dropout_keep})


if __name__ == '__main__':
    cnn = CNN()
    if len(sys.argv) == 2 and sys.argv[1] == 'i':
        saver = tf.train.Saver()
        with tf.Session() as sess:
            saver.restore(sess, save_path)
            print("Model restored.")
            test_x = np.fromfile('data/test_x.bin', dtype='float32').reshape(util.test_len, util.seq_len,
                                                                             util.embed_dim)
            test_y = np.fromfile('data/test_y.bin', dtype='float32').reshape(util.test_len, util.num_class)
            print(f'Test length {len(test_x)}')
            loss, acc, val = sess.run([cnn.loss, cnn.acc, cnn.val],
                                      feed_dict={cnn.input_x: test_x, cnn.input_y: test_y, cnn.keep_prob: 1})
            print(acc)
            val.tofile('output/cnn_val.bin')
    else:
        train(cnn)
