////////////////////////////////////////////////////////////////////////////////////
// PA 2
// Problem 1: A Verified Heap 70%
////////////////////////////////////////////////////////////////////////////////////

datatype Heap = H(a: array<int>, size: int, capacity: int)

predicate valid_heap(h: Heap)
  reads h.a
{
  h.a != null && h.a.Length == h.capacity + 1 && 0 <= h.size <= h.capacity &&
      forall i :: 1 < i <= h.size ==> h.a[i] <= h.a[i / 2]
}

method init_heap(cap: int) returns (h: Heap)
  requires 0 <= cap
  ensures valid_heap(h)
  ensures h.size == 0 && h.capacity == cap
  ensures fresh(h.a)
{
  // it seems that H(new int[cap + 1], 0, cap) will result in parser error
  var a := new int[cap + 1];
  return H(a, 0, cap);
}

method insert(x: int, h: Heap) returns (h': Heap)
  requires valid_heap(h)
  requires h.size < h.capacity
  ensures valid_heap(h')
  ensures h'.size == h.size + 1 && h'.capacity == h.capacity
  ensures h.a == h'.a
  modifies h.a
{
  var n := h.size + 1;
  while n != 1 && h.a[n / 2] < x
    invariant 1 <= n <= h.size + 1
    invariant valid_heap(h)
    invariant n * 2 <= h.size ==> h.a[n * 2] < x
    invariant n * 2 + 1 <= h.size ==> h.a[n * 2 + 1] < x
    invariant n != h.size + 1 ==> forall i :: 1 < i <= h.size + 1 ==> h.a[i] <= h.a[i / 2]
    invariant n != h.size + 1 ==> n * 2 <= h.size + 1 ==> h.a[n * 2] < x
    invariant n != h.size + 1 ==> n * 2 + 1 <= h.size + 1 ==> h.a[n * 2 + 1] < x
  {
    h.a[n] := h.a[n / 2];
    n := n / 2;
  }
  h.a[n] := x;
  return H(h.a, h.size + 1, h.capacity);
}

method extract(h: Heap) returns (x: int, h': Heap)
  requires valid_heap(h)
  requires 0 < h.size
  ensures valid_heap(h')
  ensures h'.size == h.size - 1
  ensures h.a == h'.a
  // I add these 2 postconditions in order to prove the correctness of heap-sort
  ensures x == old(h.a[1])
  ensures forall i :: 1 <= i <= h'.size ==> x >= h'.a[i]
  modifies h.a
{
  var n := h.size;
  var p := 1;
  var ch := 2;
  x := h.a[1];
  h.a[1] := h.a[n];
  while true
  {
    if ch + 1 <= n && h.a[ch] < h.a[ch + 1] { ch := ch + 1; }
    if ch <= n && x < h.a[ch]
    {
      h.a[p] := h.a[ch];
      p := ch;
      ch := p * 2;
    }
    else { break; }
  }
  h.a[p] := x;
  h' := H(h.a, h.size - 1, h.capacity);
  if h'.size > 0
  {
    assert x >= h'.a[1];
    heap_max(h');
    assert forall i :: 1 <= i <= h'.size ==> x >= h'.a[i];
  }
}

////////////////////////////////////////////////////////////////////////////////////
// PA 2
// Problem 2: Heapsort 30%
////////////////////////////////////////////////////////////////////////////////////

lemma heap_max(h: Heap)
  requires valid_heap(h) && h.size > 0
  ensures forall i :: 2 <= i <= h.size ==> h.a[i] <= h.a[1] 
{
  forall i | 2 <= i <= h.size
    ensures h.a[i] <= h.a[1]
  {
    var n := i;
    while n != 1
      invariant 1 <= n <= h.size
      invariant h.a[i] <= h.a[n]
    {
      n := n / 2;  
    }
  }
}

class Heapsort
{
  var h: Heap;

  method sort(a: array<int>)
    requires a != null && 0 <= a.Length
    ensures forall i :: 0 <= i < a.Length - 1 ==> a[i] >= a[i + 1]
    modifies this, h.a, a
  {
    // I tried to use Heap, but it reports "Call may violate context's modifies clause" due to reasons that I can't understand
    // so I am using raw array now, and only create a Heap when needed 
    var h := new int[a.Length + 1];
    var i := 0;
    while i < a.Length
      invariant valid_heap(H(h, i, a.Length))
    {
      var _ := insert(a[i], H(h, i, a.Length));
      i := i + 1;
    }
    i := 0;
    while i < a.Length
      invariant 0 <= i <= a.Length
      invariant valid_heap(H(h, a.Length - i, a.Length))
      invariant forall j :: 0 <= j < i - 1 ==> a[j] >= a[j + 1]
      invariant i != 0 ==> forall j :: 1 <= j <= a.Length - i ==> a[i - 1] >= h[j]
    {
      var x, _ := extract(H(h, a.Length - i, a.Length));
      a[i] := x;
      assert i > 0 ==> a[i - 1] >= a[i];
      i := i + 1;
    }
  }
}