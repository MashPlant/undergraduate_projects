template<typename It, typename T>
It lower_bound(It first, It last, const T &val)
{
   // loop invariant:
   // *(first - 1) is the biggest which < val (already found)
   // *last is the smallest which >= val (already found)
   while (first < last)
   {
       It mid = first + (last - first) / 2;
       if (*mid < val)
           first = mid + 1;
       else
           last = mid;
   }
   return first;
}

template<typename It, typename T>
It upper_bound(It first, It last, const T &val)
{
   // loop invariant:
   // *(first - 1) is the biggest which <= val (already found)
   // *last is the smallest which > val (already found)
   while (first < last)
   {
       It mid = first + (last - first) / 2;
       if (val < *mid)
           last = mid;
       else
           first = mid + 1;
   }
   return last;
}