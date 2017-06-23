public class SelectSort {
    public static void selectSort(int[] a) {
        int min;

        for (int i = 0; i < a.length; i++) {
            min = a[i];
            for (int j = i + 1; j < a.length; j++) {
                if (a[j] < min) {
                    min = a[j];
                    a[j] = a[i];
                    a[i] = min;
                }
            }
        }
    }
    public static void main(String[] args) {
        int[] a = ShellSort.array;

        if(a.length <= 1) {
            return;
        }

        selectSort(a);

        ShellSort.printArray(a);
    }
}