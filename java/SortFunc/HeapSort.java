public class HeapSort {

    public static void heapSort(int[] array) {
        
    }
    public static void main(String[] args) {
        int[] a = ShellSort.array;

        if(a.length <= 1) {
            return;
        }

        heapSort(a);

        ShellSort.printArray(a);
    }
}