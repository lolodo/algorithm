import java.util.Arrays;
public class HeapSort {
    public static void buildHeap(int[] array, int last) {
        int i;
        int max;

        for(i = (last - 1) / 2; i >= 0; i--) {
            int k = 2 * i + 1;//left
            int biggerest = k;

            if (k < last) {
                if(array[k] < array[k + 1]) {
                    biggerest++;
                }
            }

            if(array[biggerest] > array[i]) {
                max = array[biggerest];
                array[biggerest] = array[i];
                array[i] = max;
            }
        }
    }
    public static void heapSort(int[] array) {
        int i;

        for (i = 0; i < array.length; i++)
        buildHeap(array, array.length - 1)
    }
    public static void main(String[] args) {
        int[] a = ShellSort.array;

        if(a.length <= 1) {
            return;
        }

        heapSort(a);

        System.out.println(Arrays.toString(a));
        // ShellSort.printArray(a);
    }
}