import java.util.Arrays;

public class MergeSort {

    public static void merge(int[] a, int start, int mid, int end) {
        int i = mid + 1;
        int[] tmp = new int[a.length];
        int third = start;
        int pos = start;

        while(start <= mid && i <= end) {
            if (a[start] <= a[i]) {
                tmp[third++] = a[start++];
            } else {
                tmp[third++] = a[i++];
            }
        }

        while(i <= end) {
            tmp[third++] = a[i++];
        }

        while(start <= mid) {
            tmp[third++] = a[start++];
        }

        while(pos <= end) {
            a[pos] = tmp[pos];
            pos++;
        }

        // System.out.println(Arrays.toString(a));
    }

    public static void sort(int[] a, int left, int right){
        if(left < right) {
            int center = (left + right) / 2;

            sort(a, left, center);
            sort(a, center + 1, right);

            merge(a, left, center, right);
        }
    }
    public static void mergeSort(int[] a) {
        int mid = a.length / 2;
        sort(a, 0, a.length - 1);
    }
    public static void main(String[] args) {
        int[] a = ShellSort.array;
        mergeSort(a);

        System.out.println(Arrays.toString(a));
    }
}