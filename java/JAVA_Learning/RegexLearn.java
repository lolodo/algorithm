import java.util.regex.*;
// import java.util.regex.Pattern;

public class RegexLearn {

    int number;
    public static void firstHandle() {
        String content = "I am noob from runoob.com";
        String pattern = ".*runoob.*";

        boolean isMatch = Pattern.matches(pattern, content);
        System.out.println("match:" + isMatch);
    }

    public static void regexMatches() {
        String line = "This order was placed for QT3000! OK?";
        String pattern = "(\\D*)(\\d+)(.*)";

        Pattern r = Pattern.compile(pattern);

        Matcher m = r.matcher(line);

        if(m.find()) {
            for(int i = 0; i < m.groupCount() + 1; i++) {
                System.out.println("The "+ i + "th==>>" +"Found value:" + m.group(i));
            }
        }
    }

    RegexLearn(int input) {
        number = input * input;
    }

    public static void printMax(double... numbers) {
        if (numbers.length == 0) {
            System.out.println("No argument passsed");
            return;
        }

        double result = numbers[0];

        for (double tmp:numbers){
            if (tmp > result) {
                result = tmp;
            }
        }

        System.out.println("The max value is " + result);
    }
    public static void main(String[] args) {
        // firstHandle();
        // regexMatches();

        for (int i = 0; i < args.length; i++) {
            System.out.println("args[" + i + "]:" + args[i]);
        }

        RegexLearn c1 = new RegexLearn(2);
        System.out.println("number is:" + c1.number);

        double[] ll1 = {2.3, 4.4, 3.6, 2.4};

        printMax(ll1);
    }
}