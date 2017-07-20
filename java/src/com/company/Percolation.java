package com.company;
import edu.princeton.cs.algs4.StdRandom;
import edu.princeton.cs.algs4.StdStats;
import edu.princeton.cs.algs4.WeightedQuickUnionUF;

public class Percolation {
    WeightedQuickUnionUF uf;
    private boolean[] NN;
    private int N;

    public Percolation(int n){
        this.N = n;
        NN = new boolean[n * n + 2];
        uf = new WeightedQuickUnionUF(n * n + 4);

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                NN[i * n + j] = false;
            }
        }

        NN[n * n] = true;
        NN[n * n + 1] = true;
    }         // create n-by-n grid, with all sites blocked

    public void open(int row, int col){
        if (row < 1 || row > N || col < 1 || col > N) {
            throw new java.lang.IndexOutOfBoundsException();
        }

        int i = row - 1;
        int j = col - 1;
        int up;
        int left;
        int right;
        int below;
        int self = i * N + j;

        if (NN[self]) {
            return;
        }

        NN[self] = true;

        if (row == 1) {
            up = N * N;
            below = j;
        } else if ( row == N) {
            up = self - N;
            below = N * N + 1;
        } else {
            up = self - N;
            below = self + N;
        }

        if ( col == 1) {
            left = self;
            right = self + 1;
        } else if (col == N) {
            left = self - 1;
            right = self;
        } else {
            left = self - 1;
            right = self + 1;
        }

        if (NN[left]) {
            uf.union(left, self);
        }

        if (NN[right]) {
            uf.union(self, right);
        }

        if (NN[up]) {
            uf.union(up, self);
        }

        if (NN[below]){
            uf.union(self, below);
        }

    }    // open site (row, col) if it is not open already
    public boolean isOpen(int row, int col){
        if (row < 1 || row > N || col < 1 || col > N){
            throw new java.lang.IndexOutOfBoundsException();
        }

        return NN[(row - 1) * N + (col - 1)];
    }  // is site (row, col) open?
    public boolean isFull(int row, int col){
        if (row < 1 || row > N || col < 1 || col > N){
            throw new java.lang.IndexOutOfBoundsException();
        }

        return NN[(row - 1) * N + col - 1] && uf.connected((row - 1) * N + col - 1, N * N);
    }  // is site (row, col) full?
    public int numberOfOpenSites(){
        int i;
        int count = 0;

        for (i = 0; i < N * N; i++){
            if (NN[i]){
                count++;
            }
        }

        return count;
    }    // number of open sites
    public boolean percolates(){
        return uf.connected(N * N, N * N + 1);
    }         // does the system percolate?

    public static void main(String[] args){
        System.out.println("just test!");
    } // test client (optional)
}
