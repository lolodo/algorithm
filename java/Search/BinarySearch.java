import java.util.Scanner;
public class BinarySearch {
    public static int[] a = {1, 3, 4, 5, 5, 8, 9, 11, 12, 13, 14, 15, 17, 18, 23, 25, 25, 25, 26, 27, 31, 32, 34, 34, 35, 38, 41, 41, 49, 49, 51, 51, 53, 54, 55, 56, 62, 64, 65, 76, 78, 97, 98, 99, 112, 113, 135, 137, 137, 148, 163, 165, 172, 181, 215, 217, 235, 253, 262, 277, 314, 318, 325, 334, 335, 338, 344, 364, 364, 399, 439, 442, 491, 491, 511, 526, 531, 533, 534, 536, 544, 614, 615, 622, 625, 644, 655, 716, 718, 746, 783, 917, 937, 938, 958, 1252, 1721, 1822, 1843, 2313, 2847, 3634, 3994, 4974, 5124, 5326, 6422, 7456, 7638, 9128, 9219, 9457};

    public static void binarySearch(int[] array, int number) {
        int low = 0;
        int high = array.length - 1;
        int mid;

        while(low <= high) {
            mid = (low + high) / 2;
            if (array[mid] > number) {
                if (mid == 0) {
                    System.out.println("No result 1");
                    break;
                }
                high = mid - 1;
            } else if(array[mid] < number) {
                if (mid == array.length - 1) {
                    System.out.println("No result 2");
                    break;
                }
                low = mid + 1;
            } else {
                System.out.println("We found it at the " + mid + "th position.");
                break;
            }
        }

        if (low > high) {
            System.out.println("NO result 3");
        }
    }
    public static void main(String[] args) {
        System.out.println("Please input number:");
        Scanner input = new Scanner(System.in);
        int number = input.nextInt();

        binarySearch(a, number);
    }
}