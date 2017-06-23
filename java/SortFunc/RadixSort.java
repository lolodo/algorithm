import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import javax.swing.GroupLayout.Alignment;

public class RadixSort {

    public static void radixSort(int[] a) {
        int max = a[0];
        int time = 0;

        for (int i = 1; i < a.length; i++) {
            if(a[i] > max) {
                max = a[i];
            }
        }

        while(max > 0) {
            max /= 10;
            time++;
        }

        List<ArrayList>  queue = new ArrayList<ArrayList>();

        for (int i = 0; i < 10; i++) {
            ArrayList<Integer>  queue1 = new ArrayList<Integer>();
            queue.add(queue1);
        }

        for (int i=0; i < time; i++){
            for (int j = 0; j < a.length; j++) {
                int x = (a[j] % (int)Math.pow(10, i+1))/(int)Math.pow(10, i);
                ArrayList<Integer> queue2 = queue.get(x);
                queue2.add(a[j]);
                queue.set(x, queue2);
            }

            int count = 0;

            for (int k = 0; k < 10; k++) {
                while (queue.get(k).size() > 0) {
                    ArrayList<Integer> queue3 = queue.get(k);
                    a[count] = queue3.get(0);
                    queue3.remove(0);
                    count++;
                }
            }
        }
    }
    public static void main(String[] args) {
        int[] a = ShellSort.array;
        radixSort(a);
        System.out.println(Arrays.toString(a));
    }


}