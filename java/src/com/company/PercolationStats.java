
import edu.princeton.cs.algs4.*;
import edu.princeton.cs.algs4.StdRandom;

public class PercolationStats {

    private double mean;
    private double stddev;
    private double conflow;
    private double confup;
    
    public PercolationStats(int n, int trials) {
        if (n <= 0 || trials <= 0) {
            throw new java.lang.IllegalArgumentException();
        }

        double times[] =  new double[trials];
        for (int t = 0; t < trials; t++) {
            Percolation perc = new Percolation(n);

            for (int a = 0; a < n * n; a++) {
                int pt = (int)(Math.random() * n * n);
                int i = pt / n + 1;
                int j = pt % n + 1;
                while(perc.isOpen(i, j)) {
                    pt = (int)(Math.random() * n * n);
                    i = pt / n + 1;
                    j = pt % n + 1;
                }

                perc.open(i, j);
                if (perc.percolates()) {
                    times[t] = ((double)a) / (n * n);
                    break;
                }
            }
        }

        for (int t = 0; t < trials; t++) {
            mean += times[t];
        }

        mean /= trials;

        for (int t = 0; t < trials; t++) {
            stddev += (times[t] - mean) * (times[t] - mean);
        }

        stddev /= (trials - 1);
        stddev = Math.sqrt(stddev);

        conflow = mean - 1.96 * stddev / Math.sqrt((double)trials);
        confup = mean + 1.96 * stddev / Math.sqrt((double)trials);
    }    // perform trials independent experiments on an n-by-n grid
    public double mean(){
        return mean;
    }               // sample mean of percolation threshold
    public double stddev() {
        return stddev;
    }                  // sample standard deviation of percolation threshold
    public double confidenceLo() {
        return conflow;
    }               // low  endpoint of 95% confidence interval
    public double confidenceHi(){
        return confup;
    }                // high endpoint of 95% confidence interval

    public static void main(String[] args){
        int N = StdIn.readInt();
        int T = StdIn.readInt();

        System.out.println("N is " + N + " T is " + T);
        PercolationStats pers = new PercolationStats(N, T);

        StdOut.println("mean = " + pers.mean());
        StdOut.println("stddev = " + pers.stddev());
        StdOut.println("95% confidence interval = " + pers.confidenceLo() + ", " + pers.confidenceHi());
    }      // test client (described below)
}