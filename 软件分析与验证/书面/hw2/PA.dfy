/////////////////////////////////////////////////////////////////////////////
// Peano Arithmetic
/////////////////////////////////////////////////////////////////////////////

// Important note: DO NOT CHANGE the definitions of Nat, one(), add(x, y), or mult(x, y).
// Turned-in solutions that differ in the definitions will receive no credit!
datatype Nat = Zero | Succ(n: Nat)

function one(): Nat
{
	Succ(Zero)
}

function add(x: Nat, y: Nat): Nat
{
	match(x) {
		case Zero => y
		case Succ(n) => Succ(add(n, y))
	}
}

function mult(x: Nat, y: Nat): Nat
{
	match(x) {
		case Zero => Zero
		case Succ(n) => add(mult(n, y), y)
	}
}

// 2-1
// Add an ensures annotation sufficient
// to prove the statement:
//   forall x . ~(x + 1 = 0)
// Then prove the lemma by providing a body
// sufficient to establish your ensures annotation.
// Note: your ensures annotation should make use
// of the argument passed to the lemma.
lemma {:induction false} axm_zero(x: Nat)
	ensures add(x, one()) != Zero
{
	// calc {
	// 	add(x, one());
	// 		{ add_lemmatwo(x); }
	// 	Succ(x);
	// }
}

// 2-2
// This is provided as an example, you do not
// need to do anything.
// This lemma proves the statement:
//   forall x . x + 0 == x
// Note that it uses proof by contradiction,
// by assuming that its postcondition is false
// using an if statement, and deriving the
// negation of this assumption on all branches
// within the body.
lemma {:induction false} axm_pluszero(x: Nat)
	ensures add(x, Zero) == x
{
	if (add(x, Zero) != x) {
		match(x) {
			case Zero =>
				calc == {
					add(x, Zero);
					add(Zero, Zero);
					Zero;
					x;
				}
			case Succ(n) =>
				calc == {
					add(x, Zero);
					add(Succ(n), Zero);
					Succ(add(n, Zero));
						{ axm_pluszero(n); } // inductive step
					Succ(n);
					x;
				}
		}
	}
}

// 2-3
// Add an ensures annotation sufficient
// to prove the statement:
//   forall x . x * 0 = 0
// Then prove the lemma by providing a body
// sufficient to establish your ensures annotation.
// Note: your ensures annotation should make use
// of the arguments passed to the lemma.
// Note: do not remove the {:induction false} attribute.
// Solutions that do not verify with this attribute
// will recieve no credit!
lemma {:induction false} axm_timeszero(x: Nat)
	ensures mult(x, Zero) == Zero
{
	match (x) {
		case Zero =>
			calc == {
				mult(Zero, Zero);
				Zero;
			}
		case Succ(n) =>
			calc == {
				mult(Succ(n), Zero);
				add(mult(n, Zero), Zero);
					{ axm_pluszero(mult(n, Zero)); }
				mult(n, Zero);
					{ axm_timeszero(n); }
				Zero;
			}
	}
}

// 2-4
// Add an ensures annotation sufficient
// to prove the statement:
//   forall x, y . x + 1 = y + 1 ==> x = y
// Then prove the lemma by providing a body
// sufficient to establish your ensures annotation.
// Note: your ensures annotation should make use
// of the arguments passed to the lemma.
lemma {:induction false} axm_successor(x: Nat, y: Nat)
	ensures add(x, one()) == add(y, one()) ==> x == y
{
	calc == {
		add(x, one());
			{ add_lemmatwo(x); }
		Succ(x);
	}
	calc == {
		add(y, one());
			{ add_lemmatwo(y); }
		Succ(y);
	}
}

// 2-5
// Add an ensures annotation sufficient
// to prove the statement:
//   forall x, y . x + (y + 1) = (x + y) + 1
// Then prove the lemma by providing a body
// sufficient to establish your ensures annotation.
// Note: your ensures annotation should make use
// of the arguments passed to the lemma.
lemma axm_plussuccessor(x: Nat, y: Nat)
	ensures add(x, add(y, one())) == add(add(x, y), one())
{
  calc == {
		add(x, add(y, one()));
			{ add_associative(x, y, one()); }
		add(add(x, y), one());
	}
}

// This lemma is provided for your benefit.
// It might come in handy in a subsequent proof.
lemma add_lemmatwo(x: Nat)
	ensures add(x, one()) == Succ(x)
{}

lemma add_associative(x: Nat, y: Nat, z: Nat)
	ensures add(x, add(y, z)) == add(add(x, y), z)
{
	match (x) {
		case Zero =>
			calc == {
				add(Zero, add(y, z));
				add(y, z);
			}
			calc == {
				add(add(Zero, y), z);
				add(y, z);
			}
		case Succ(n) =>
			calc == {
				add(Succ(n), add(y, z));
				Succ(add(n, add(y, z)));
					{ add_associative(n, y, z); }
				Succ(add(add(n, y), z));
			}
			calc == {
				add(add(Succ(n), y), z);
				add(Succ(add(n, y)), z);
				Succ(add(add(n, y), z));
			}
	}
}

lemma add_commutative(x: Nat, y: Nat)
	ensures add(x, y) == add(y, x)
{
	match (x) {
		case Zero =>
			calc == {
				add(Zero, y);
				y;
			}
			calc == {
				add(y, Zero);
					{ axm_pluszero(y); }
				y;
			}
		case Succ(n) =>
			calc == {
				add(Succ(n), y);
				Succ(add(n, y));
					{ add_lemmatwo(add(y, n)); }
				add(add(n, y), one());
			}
			calc == {
				add(y, Succ(n));
					{ add_lemmatwo(n); }
				add(y, add(n, one()));
				  { add_associative(y, n, one()); }
				add(add(y, n), one());
					{ add_commutative(n, y); }
				add(add(n, y), one());
			}
	}
}

// 2-6
// Prove the following lemma by providing a body
// sufficient to establish the ensures annotation.
// Hint: if you get stuck, you should consider
// making use of existing lemmas, or defining a
// new one.
// Note: do not change the provided postcondition,
// you will not receive credit if you do
lemma axm_timessuccessor(x: Nat, y: Nat)
	ensures mult(x, add(y, one())) == add(mult(x, y), x)
{
	match (x) {
		case Zero =>
			calc == {
				mult(Zero, add(y, one()));
				Zero;
			}
			calc == {
				add(mult(Zero, y), Zero);
					{ axm_pluszero(mult(Zero, y)); }
				mult(Zero, y);
				Zero;
			}
		case Succ(n) =>
			calc == {
				mult(Succ(n), add(y, one()));
				add(mult(n, add(y, one())), add(y, one()));
					{ axm_timessuccessor(n, add(y, one())); }
				add(add(mult(n, y), n), add(y, one()));	
					{ add_associative(mult(n, y), n, add(y, one())); }
				add(mult(n, y), add(n, add(y, one())));
					{ add_associative(n, y, one()); }
				add(mult(n, y), add(add(n, y), one()));
					{ add_commutative(n, y); }
				add(mult(n, y), add(add(y, n), one()));
			}
			calc == {
				add(mult(Succ(n), y), Succ(n));
				add(add(mult(n, y), y), Succ(n));
					{ add_lemmatwo(n); }
				add(add(mult(n, y), y), add(n, one()));
					{ add_associative(mult(n, y), y, add(n, one())); }
				add(mult(n, y), add(y, add(n, one())));
					{ add_associative(y, n, one()); }
				add(mult(n, y), add(add(y, n), one()));
			}
	}	
}
