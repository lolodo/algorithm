public class InsertSort {
    public InsertSort() {
        int[] a = {49,38,65,97,76,13,27,49,78,34,12,64,5,4,62,99,98,54,56,17,18,23,34,15,35,25,53,51};
        int cur = 0;
        int i,j;

        for (i = 1; i < a.length - 1; i++) {
            cur = a[i];
            j = i -1;
            for (; (j >= 0) && (cur < a[j]); j--) {
                a[j+1] = a[j];
            }

            a[j+1] = cur;
        }

        for (int num:a) {
            System.out.print(num + " ");
        }
    }

    public static void main(String[] args) {
        InsertSort s1 = new InsertSort();
    }
}