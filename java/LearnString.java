public class LearnString {
    public static void main(String []args) {
        String site = "www.google.com";
        int len = site.length();

        System.out.println("The site's length is " + len + ".");

        String fs;
        float f1 = 1.3f;
        int i1 = 4;
        String s1 = "newest";

        fs = String.format("The float number is %f, the integer is %d, and the string is %s", f1, i1, s1);
        System.out.println("fs is " + fs);

        StringBuffer sBuffer = new StringBuffer("The offical website:");
        sBuffer.append("www");
        sBuffer.append(".google");
        sBuffer.append(".com");

        System.out.println(sBuffer);
        sBuffer.reverse();
        System.out.println(sBuffer);
    }
}