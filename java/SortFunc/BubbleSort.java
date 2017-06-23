import java.util.Arrays;

public class BubbleSort {
    public static void bubblesort (int[] number) {
        int max;
        int flag;

        if(number.length <= 1) {
            System.out.println("number.length is " + number.length);
            return;
        }

        for(int i = number.length - 1; i > 0 ; i--) {
            flag = 0;
            for (int j = 0; j < i; j++) {
                if (number[j] > number[j + 1]) {
                    max = number[j];
                    number[j] = number[j + 1];
                    number[j + 1] = max;
                    flag = 1;
                }
            }

            if(flag == 0) {
                break;
            }
        }
    }
    public static void main(String[] args) {
        int[] array = ShellSort.array;
        bubblesort(array);
        System.out.println(Arrays.toString(array));
    }
}