import java.util.Calendar;
import java.util.Date;
import java.text.*;

public class LearnDate {
    public static void mySleep(int ms) {
        try {
            // Date date = new Date();
            System.out.println(new Date());
            Thread.sleep(ms);
            System.out.println(new Date());
        } catch (Exception e) {
            System.out.println("Got a exception!");
        }
    }

    public static void calendarLearn () {
        Calendar c1 = Calendar.getInstance();
        // c1.set(2020, 5, 12);

        System.out.println("hour:" + c1.HOUR);
    }
    public static void main(String[] args) {
        // Date date = new Date();
        // System.out.println(date.toString());
        // System.out.println(date);

        // long current = date.getTime();

        // System.out.println(current);
        long start = System.currentTimeMillis();
        // mySleep(3000);
        calendarLearn();
        long end = System.currentTimeMillis();

        System.out.println("diff is " + (end - start));
    }
}