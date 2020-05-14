from typing import List, Tuple
from collections import Counter


class ParseError(Exception):
    pass


ops = {'AND', 'OR', 'BEFORE', 'AFTER'}


def get_result(query: str, gen_fn, before_fn, after_fn) -> Tuple[Counter, List[str]]:
    query = query.replace('(', ' ( ')
    query = query.replace(')', ' ) ')
    tokens = query.split()
    search_words: List[str] = []
    i = 0

    def parse_query():
        return parse_right(parse_left())

    def parse_left():
        nonlocal i
        if tokens[i] == '(':
            i += 1
            q = parse_query()
            if tokens[i] != ')':
                raise ParseError()
            i += 1
            return q
        else:
            i += 1
            res, words = gen_fn(tokens[i - 1])
            if words:
                search_words.extend(words)
            return res

    def parse_right(l):
        nonlocal i
        if i < len(tokens) and tokens[i] in ops:
            op = tokens[i]
            i += 1
            r = parse_left()
            if op == 'AND':
                l &= r
            elif op == 'OR':
                l += r  # merge two counter
            elif op == 'BEFORE':
                before_fn(l, r)
            elif op == 'AFTER':
                after_fn(l, r)
            return parse_right(l)
        else:
            return l

    ret = parse_query()
    if i != len(tokens):
        raise ParseError()
    return ret, search_words
