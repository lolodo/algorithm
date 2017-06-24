public class Array {

    public static void printArray(double[] array) {
        for (double number:array) {
            System.out.println(number);
        }
    }

    public static void main(String []args) {
        int size = 2;
        double[] myList = new double[size];

        myList[0] = 5.6;
        myList[1] = 3.2;

        double total = 0;
        for (int i = 0; i < size; i++) {
            total += myList[i];
        }

        System.out.println("The total is " + total + ".");

        double[] theList = {1.1, 2.3, 5.3, 1.5, 2.5, 6.7, 2.3};

        for (int i = 0; i < theList.length; i++) {
            System.out.println(theList[i] + " ");
        }

        double total1 = 0;

        for (int i = 0; i < theList.length; i++) {
            total1 += theList[i];
        }

        System.out.println("The total is " + total1);

        double max = theList[0];

        for (int i = 1; i < theList.length; i++) {
            if (theList[i] > max) {
                max = theList[i];
            }
        }

        System.out.println("The max number is " + max);

        for (double element:theList) {
            System.out.println(element);
        }

        // printArray(theList);
        System.out.println("the next!");
        double[] newlist;
        newlist = reverse(theList);
        for (double list:newlist) {
            System.out.println(list);
        }

        // Arrays.sort(theList);
        java.util.Arrays.sort(theList);
        
        printArray(theList);
    }

    public static double[] reverse(double[] list) {
        double tmp;

        for(int i = 0; i < list.length / 2 ; i++) {
            tmp = list[i];
            list[i] = list[list.length - i - 1];
            list[list.length - i - 1] = tmp;
        }

        return list;
    }
}