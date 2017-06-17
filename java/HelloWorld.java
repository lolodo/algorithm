public class HelloWorld {
    public static void main(String[] args) {
        System.out.println("Hello World");
        Character ch = '4';

        System.out.println("get a Character:" + ch);

        if (Character.isLetter(ch)) {
            System.out.println("ch is a character!");
        }

        if (Character.isDigit(ch)) {
            System.out.println("ch is a digit!");
        }
    }
}
