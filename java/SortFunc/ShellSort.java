public class ShellSort {
    public static int[] array = {49,38,65,97,76,13,27,49,78,34,12,64,5,4,62,99,98,54,56,17,18,23,34,15,35,25,53,51,
    41,338,625,937,746,163,277,491,783,344,172,364,25,14,262,3994,958,544,526,137,148,253,364,165,325,215,3,1,
9,8,655,9457,7456,1843,2847,4974,7638,3634,1252,644,511,442,6422,9219,9128,5124,5326,1721,181,26,32,11,55,25,5,31,
439,318,615,917,716,113,217,491,718,314,112,614,51,41,622,399,938,534,536,137,1822,2313,334,135,335,235,533,531};
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
        // int[] a = {49,38,65,97,76,13,27,49,78,34,12,64,5,4,62,99,98,54,56,17,18,23,34,15,35,25,53,51};

        shellsort(array);
        // printArray(a);
    }
}