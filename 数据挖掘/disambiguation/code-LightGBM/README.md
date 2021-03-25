# WhoIsWho-DataMining-Competition
WhoIsWho -- AMiner manually-labeled name disambiguation competition 2020 solution

```
python -m src.pre
python -m src.embed
python -m src.datasplit
python -m src.feature
python -m src.train
python -m src.predict
```

Expected data folder structure:

```
📦data
 ┣ 📂sna_data    # Data last year
 ┃ ┣ 📜sna_valid_author_ground_truth.json
 ┃ ┣ 📜sna_valid_author_raw.json
 ┃ ┣ 📜sna_valid_example_evaluation_scratch.json
 ┃ ┗ 📜sna_valid_pub.json
 ┣ 📂train       # Data last year
 ┃ ┣ 📜train_author.json
 ┃ ┗ 📜train_pub.json
 ┣ 📂v2          # Data this year
 ┃ ┣ 📂valid
 ┃ ┃ ┣ 📜sna_valid_author_raw.json
 ┃ ┃ ┣ 📜sna_valid_example_evaluation_scratch.json
 ┃ ┃ ┗ 📜sna_valid_pub.json
 ┃ ┣ 📜train_author.json
 ┃ ┗ 📜train_pub.json
 ┗ 📜.gitkeep
```
