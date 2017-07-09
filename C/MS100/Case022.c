/*
有4 张红色的牌和4 张蓝色的牌，主持人先拿任意两张，再分别在A、B、C 三人额头上贴任意两张牌，A、B、C 三人都可以看见其余两人额头上的牌，
看完后让他们猜自己额头上是什么颜色的牌，A 说不知道，B 说不知道，C 说不知道，然后A 说知道了。
请教如何推理，A 是怎么知道的。如果用程序，又怎么实现呢？
ANSWER
I dont’ like brain teaser. As an AI problem, it seems impossible to write the solution in 20 min...

It seems that a brute-force edge cutting strategy could do. Enumerate all possibilities, 
then for each guy delete the permutation that could be reduced if failed (for A, B, C at 1st round), 
Then there should be only one or one group of choices left.

But who uses this as an interview question?
*/

