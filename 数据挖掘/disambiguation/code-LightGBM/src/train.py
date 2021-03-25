import lightgbm as lgm
import pickle as pkl


class Data:
    def __init__(self, path):
        data = pkl.load(open(path, "rb"))
        self.data = data[:, :-1]
        self.label = data[:, -1]


param = {
    "boosting_type": "gbdt",
    "num_leaves": 200,
    "max_depth": 21,
    "learning_rate": 0.05,
    # 'boost_from_average': False,
    "objective": "binary",
    "min_split_gain": 1,
    "subsample": 0.8,
    # 'pos_bagging_fraction': 0.8808,
    # 'neg_bagging_fraction': 1.0,
    "subsample_freq": 5,
    "feature_fraction": 0.7,
    "reg_lambda": 5,
    "n_jobs": 4,
    # "min_data_in_leaf": 50,
    "metric": ["binary_logloss"],
    # 'feature_contri': [1.0]*34 + [1.0]*5 + [1, 1] + [1.0]*5
}

save_path = "./lgm_model/model.txt"

if __name__ == "__main__":
    train = Data("./final_train_and_valid_set/train_training.pkl")
    valid = Data("./final_train_and_valid_set/train_validating.pkl")
    train = lgm.Dataset(train.data, label=train.label)
    valid = lgm.Dataset(valid.data, label=valid.label, reference=train)

    lgm.train(
        param,
        train_set=train,
        valid_sets=[valid, train],
        valid_names=["valid", "train"],
        num_boost_round=2000,
        early_stopping_rounds=20,
    ).save_model(save_path)
