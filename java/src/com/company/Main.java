package com.company;
import edu.princeton.cs.algs4.Accumulator;
import edu.princeton.cs.algs4.Quick;
import edu.princeton.cs.algs4.QuickFindUF;

public class Main {

    public static void main(String[] args) {
       //j System.out.println("hello!");
        QuickFindUF uf0 = new QuickFindUF(10);
        int num;

        for (int i = 0; i < 10; i++) {
            num = uf0.find(i);
            System.out.println("num is " + num);
        }

        uf0.union(0, 3);
        uf0.union(2,5);
        uf0.union(6, 1);

        boolean ret = uf0.connected(0,3);
        System.out.println("ret is " + ret);
    }
}
