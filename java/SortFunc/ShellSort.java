public class ShellSort {
    public static void shellsort(int[] array) {
        int gap;
        int max;
        int cur;
        int i,j;

        if (array.length <= 1) {
            return;
        }

        gap = array.length / 2;
        while(gap > 0) {
            for (i = gap; i < array.length; i++) {
                cur = array[i];
                for(j = i - gap; j >= 0 && array[j] > cur;j -= gap){
                    array[j+gap] = array[j];
                }

                array[j+gap] = cur;
            }
            System.out.println("gap is " + gap);
            printArray(array);
            gap = gap / 2;
        }
    }

    public static void printArray(int[] array) {
        for(int num:array) {
            System.out.print(num + " ");
        }
        System.out.println(" ");
    }
    public static void main(String[] args) {
        int[] a = {49,38,65,97,76,13,27,49,78,34,12,64,5,4,62,99,98,54,56,17,18,23,34,15,35,25,53,51};

        shellsort(a);
        // printArray(a);
    }
}