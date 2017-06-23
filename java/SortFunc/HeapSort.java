import java.util.Arrays;

public class HeapSort {

    public static void Swap(int[] array, int i, int j) {
        int max;

        max = array[i];
        array[i] = array[j];
        array[j] = max;
    }

    public static void buildHeap(int[] array, int last) {
        int i;

        for (i = (last - 1) / 2; i >= 0; i--) {
            int k = i;

            while (k * 2 + 1 <= last) {
                int biggerest = k * 2 + 1;

                if (biggerest < last) {
                    if (array[biggerest] < array[biggerest + 1]) {
                        biggerest++;
                    }
                }

                if (array[biggerest] > array[k]) {
                    Swap(array, biggerest, k);
                    k = biggerest;
                } else {
                    break;
                }
            }
        }
    }

    public static void heapSort(int[] array) {
        int i;
        int max;

        for (i = 0; i < array.length - 1; i++) {
            int last = array.length - 1 - i;
            buildHeap(array, last);
            Swap(array, 0, last);
        }
    }

    public static void main(String[] args) {
        int[] a = ShellSort.array;

        if (a.length <= 1) {
            return;
        }

        System.out.println(Arrays.toString(a));
        heapSort(a);

        System.out.println(Arrays.toString(a));
        // ShellSort.printArray(a);
    }
}