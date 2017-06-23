import java.util.Arrays;

public class QuickSort {

    public static int partition(int[] array, int left, int right) {
        int pivot = array[left];
        int i, j;

        i = left;
        j = right;
        while (i < j) {
            while ((i < j) && (array[i] <= pivot)) {
                i++;
            }

            while ((j > i) &&(array[j] >= pivot)) {
                j--;
            }

            if (i < j) {
                HeapSort.Swap(array, i, j);
            }
        }

        HeapSort.Swap(array, left, j);

        return j;
    }

    public static void quickSort(int[] array, int left, int right) {
        // int i;
        int mid;

        if (left >= right)
            return;

        mid = partition(array, left, right);
        quickSort(array, left, mid - 1);
        quickSort(array, mid + 1, right);
        
        // System.out.println(Arrays.toString(array));s
    }

    public static void main(String[] args) {
        int[] a = ShellSort.array;

        System.out.println(Arrays.toString(a));

        quickSort(a, 0, a.length - 1);

        System.out.println(Arrays.toString(a));
    }
}